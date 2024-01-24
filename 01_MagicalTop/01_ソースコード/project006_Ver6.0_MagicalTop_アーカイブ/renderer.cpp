//============================================================
//
//	レンダラー処理 [renderer.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "renderer.h"
#include "object.h"
#include "manager.h"
#include "camera.h"

//************************************************************
//	親クラス [CRenderer] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CRenderer::CRenderer()
{
	// メンバ変数をクリア
	m_pD3D			= NULL;		// Direct3Dオブジェクト
	m_pD3DDevice	= NULL;		// Direct3Dデバイス
}

//============================================================
//	デストラクタ
//============================================================
CRenderer::~CRenderer()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	// メンバ変数を初期化
	m_pD3D			= NULL;		// Direct3Dオブジェクト
	m_pD3DDevice	= NULL;		// Direct3Dデバイス

	// 変数を宣言
	D3DDISPLAYMODE			d3ddm;	// ディスプレイモード
	D3DPRESENT_PARAMETERS	d3dpp;	// プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (UNUSED(m_pD3D))
	{ // オブジェクトの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{ // ディスプレイモードの取得に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// パラメータのゼロクリア
	d3dpp.BackBufferWidth	= SCREEN_WIDTH;		// ゲーム画面サイズ (幅)
	d3dpp.BackBufferHeight	= SCREEN_HEIGHT;	// ゲーム画面サイズ (高さ)
	d3dpp.BackBufferFormat	= d3ddm.Format;		// バックバッファの形式
	d3dpp.BackBufferCount	= 1;				// バックバッファの数
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;	// ダブルバッファの切り替え (映像信号に同期)
	d3dpp.EnableAutoDepthStencil	= TRUE;				// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;		// デプスバッファとして 16bit を使う
	d3dpp.Windowed					= bWindow;			// ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// Direct3Dデバイスの生成
	if (FAILED(CreateDevice(hWnd, d3dpp)))
	{ // デバイスの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定 (テクスチャの拡縮補間の設定)
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定 (テクスチャのアルファブレンドの設定)
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CRenderer::Uninit(void)
{
	// Direct3Dデバイスの破棄
	if (USED(m_pD3DDevice))
	{ // Direct3Dデバイスが使用中の場合

		// メモリ開放
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの破棄
	if (USED(m_pD3D))
	{ // Direct3Dオブジェクトが使用中の場合

		// メモリ開放
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//============================================================
//	更新処理
//============================================================
void CRenderer::Update(void)
{
	// オブジェクトの全更新
	CObject::UpdateAll();
}

//============================================================
//	描画処理
//============================================================
void CRenderer::Draw(void)
{
	// 変数を宣言
	HRESULT			hr;				// 異常終了の確認用
	D3DVIEWPORT9	viewportDef;	// カメラのビューポート保存用

	// バックバッファとZバッファのクリア
	hr = m_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
	if (FAILED(hr)) { assert(false); }

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{ // 描画開始が成功した場合

		// 現在のビューポートを取得
		m_pD3DDevice->GetViewport(&viewportDef);

		// カメラの設定
		CManager::GetCamera()->SetCamera(CCamera::TYPE_MAIN);

		// オブジェクトの全描画
		CObject::DrawAll();

		// デバッグ表示の描画
		CManager::GetDebugProc()->Draw();

		// ビューポートを元に戻す
		m_pD3DDevice->SetViewport(&viewportDef);

		// 描画終了
		hr = m_pD3DDevice->EndScene();
		if (FAILED(hr)) { assert(false); }
	}

	// バックバッファとフロントバッファの入れ替え
	hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	if (FAILED(hr)) { assert(false); }
}

//============================================================
//	デバイス取得処理
//============================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void) const
{
	// デバイスのポインタを返す
	return m_pD3DDevice;
}

//============================================================
//	生成処理
//============================================================
CRenderer *CRenderer::Create(HWND hWnd)
{
	// ポインタを宣言
	CRenderer *pRenderer = NULL;	// レンダラー生成用

	if (UNUSED(pRenderer))
	{ // 使用されていない場合

		// メモリを確保
		pRenderer = new CRenderer;	// レンダラー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pRenderer))
	{ // 確保に成功している場合

		// レンダラーの初期化
		if (FAILED(pRenderer->Init(hWnd, FALSE)))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pRenderer;
			pRenderer = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pRenderer;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CRenderer::Release(CRenderer *&prRenderer)
{
	if (USED(prRenderer))
	{ // 使用中の場合

		// レンダラーの終了
		prRenderer->Uninit();

		// メモリ開放
		delete prRenderer;
		prRenderer = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	デバイス生成処理
//============================================================
HRESULT CRenderer::CreateDevice(HWND hWnd, D3DPRESENT_PARAMETERS d3dpp)
{
	// 描画処理と頂点処理をハードウェアで行う設定にする
	if (FAILED(m_pD3D->CreateDevice
	( // 引数
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice
	)))
	{ // CPUの性能が低かった場合

		// 描画処理はハードウェア、頂点処理はCPUが行う設定にする
		if (FAILED(m_pD3D->CreateDevice
		( // 引数
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice
		)))
		{ // ハードウェアの性能が低かった場合

			// 描画処理と頂点処理をCPUで行う設定にする
			if (FAILED(m_pD3D->CreateDevice
			( // 引数
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice
			)))
			{ // CPUとハードウェアの性能が使い物にならなかった場合

				// 失敗を返す
				assert(false);
				return E_FAIL;
			}
		}
	}

	// 成功を返す
	return S_OK;
}
