//============================================================
//
//	マネージャー処理 [manager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "texture.h"
#include "model.h"
#include "gameManager.h"
#include "object.h"

#ifdef _DEBUG	// デバッグ処理

#include "debug.h"

#endif	// _DEBUG

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CRenderer		*CManager::m_pRenderer		= NULL;		// レンダラーオブジェクト
CInputKeyboard	*CManager::m_pKeyboard		= NULL;		// キーボードオブジェクト
CInputMouse		*CManager::m_pMouse			= NULL;		// マウスオブジェクト
CInputPad		*CManager::m_pPad			= NULL;		// パッドオブジェクト
CSound			*CManager::m_pSound			= NULL;		// サウンドオブジェクト
CCamera			*CManager::m_pCamera		= NULL;		// カメラオブジェクト
CLight			*CManager::m_pLight			= NULL;		// ライトオブジェクト
CTexture		*CManager::m_pTexture		= NULL;		// テクスチャオブジェクト
CModel			*CManager::m_pModel			= NULL;		// モデルオブジェクト
CFade			*CManager::m_pFade			= NULL;		// フェードオブジェクト
CScene			*CManager::m_pScene			= NULL;		// シーンオブジェクト
CGameManager	*CManager::m_pGameManager	= NULL;		// ゲームマネージャー
CDebugProc		*CManager::m_pDebugProc		= NULL;		// デバッグ表示オブジェクト

#ifdef _DEBUG	// デバッグ処理

CDebug *CManager::m_pDebug = NULL;	// デバッグオブジェクト

#endif	// _DEBUG

//************************************************************
//	親クラス [CManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CManager::CManager()
{

}

//============================================================
//	デストラクタ
//============================================================
CManager::~CManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//--------------------------------------------------------
	//	システムの生成
	//--------------------------------------------------------
	// レンダラーの生成
	m_pRenderer = CRenderer::Create(hWnd);
	if (UNUSED(m_pRenderer))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// キーボードの生成
	m_pKeyboard = CInputKeyboard::Create(hInstance, hWnd);
	if (UNUSED(m_pKeyboard))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// マウスの生成
	m_pMouse = CInputMouse::Create(hInstance, hWnd);
	if (UNUSED(m_pMouse))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// パッドの生成
	m_pPad = CInputPad::Create();
	if (UNUSED(m_pPad))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// サウンドの生成
	m_pSound = CSound::Create(hWnd);
	if (UNUSED(m_pSound))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// カメラの生成
	m_pCamera = CCamera::Create();
	if (UNUSED(m_pCamera))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ライトの生成
	m_pLight = CLight::Create();
	if (UNUSED(m_pLight))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ゲームマネージャーの生成
	m_pGameManager = CGameManager::Create();
	if (UNUSED(m_pGameManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	情報の読込・設定
	//--------------------------------------------------------
	// テクスチャの生成・読込
	m_pTexture = CTexture::Create();
	if (UNUSED(m_pTexture))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルの生成・読込
	m_pModel = CModel::Create();
	if (UNUSED(m_pModel))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// フェードの生成・シーンの設定
	m_pFade = CFade::Create();
	if (UNUSED(m_pFade))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	デバッグ用
	//--------------------------------------------------------
	// デバッグ表示の生成
	m_pDebugProc = CDebugProc::Create(hWnd);
	if (UNUSED(m_pDebugProc))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

#ifdef _DEBUG	// デバッグ処理

	// デバッグの生成
	m_pDebug = CDebug::Create();
	if (UNUSED(m_pDebug))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

#endif	// _DEBUG

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
HRESULT CManager::Uninit(void)
{
	//--------------------------------------------------------
	//	デバッグ用
	//--------------------------------------------------------
	// デバッグ表示の破棄
	if (FAILED(CDebugProc::Release(m_pDebugProc)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

#ifdef _DEBUG	// デバッグ処理

	// デバッグの破棄
	if (FAILED(CDebug::Release(m_pDebug)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

#endif	// _DEBUG

	//--------------------------------------------------------
	//	情報の破棄
	//--------------------------------------------------------
	// テクスチャの破棄
	if (FAILED(CTexture::Release(m_pTexture)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルの破棄
	if (FAILED(CModel::Release(m_pModel)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	システムの破棄
	//--------------------------------------------------------
	// ゲームマネージャーの破棄
	if (FAILED(CGameManager::Release(m_pGameManager)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シーンの破棄
	if (FAILED(CScene::Release(m_pScene)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// フェードの破棄
	if (FAILED(CFade::Release(m_pFade)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ライトの破棄
	if (FAILED(CLight::Release(m_pLight)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// カメラの破棄
	if (FAILED(CCamera::Release(m_pCamera)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// サウンドの破棄
	if (FAILED(CSound::Release(m_pSound)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// パッドの破棄
	if (FAILED(CInputPad::Release(m_pPad)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// マウスの破棄
	if (FAILED(CInputMouse::Release(m_pMouse)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// キーボードの破棄
	if (FAILED(CInputKeyboard::Release(m_pKeyboard)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// オブジェクトの全破棄
	CObject::ReleaseAll();

	// 例外処理
	assert(CObject::GetNumAll() == 0);	// 破棄の失敗

	// レンダラーの破棄
	if (FAILED(CRenderer::Release(m_pRenderer)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	更新処理
//============================================================
void CManager::Update(void)
{
#ifdef _DEBUG	// デバッグ処理

	// デバッグ表示
	m_pDebugProc->Print("FPS：%d\n", m_pDebug->GetFps());

#endif	// _DEBUG

	if (USED(m_pPad))
	{ // 使用中の場合

		// パッドの更新
		m_pPad->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pMouse))
	{ // 使用中の場合

		// マウスの更新
		m_pMouse->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pKeyboard))
	{ // 使用中の場合

		// キーボードの更新
		m_pKeyboard->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pFade))
	{ // 使用中の場合

		// フェードの更新
		m_pFade->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pScene))
	{ // 使用中の場合

		// シーンの更新
		m_pScene->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pGameManager))
	{ // 使用中の場合

		// ゲームマネージャーの更新
		m_pGameManager->Update();
	}
	else { assert(false); }	// 非使用中

	//--------------------------------------------------------
	//	デバッグ用
	//--------------------------------------------------------
	if (USED(m_pDebugProc))
	{ // 使用中の場合

		// デバッグ表示の更新
		m_pDebugProc->Update();
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	描画処理
//============================================================
void CManager::Draw(void)
{
	if (USED(m_pRenderer))
	{ // 使用中の場合

		// レンダラーの描画
		m_pRenderer->Draw();
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	生成処理
//============================================================
CManager *CManager::Create(HINSTANCE hInstance, HWND hWnd)
{
	// ポインタを宣言
	CManager *pManager = NULL;		// マネージャー生成用

	if (UNUSED(pManager))
	{ // 使用されていない場合

		// メモリを確保
		pManager = new CManager;	// マネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pManager))
	{ // 確保に成功している場合

		// マネージャーの初期化
		if (FAILED(pManager->Init(hInstance, hWnd, TRUE)))
		{ // 初期化に失敗した場合

			// マネージャーの終了
			if (FAILED(pManager->Uninit()))
			{ // マネージャーの終了に失敗した場合

				// メモリ開放
				delete pManager;
				pManager = NULL;

				// 失敗を返す
				assert(false);
				return NULL;
			}

			// メモリ開放
			delete pManager;
			pManager = NULL;

			// 失敗を返す
			assert(false);
			return NULL;
		}

		// 確保したアドレスを返す
		return pManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CManager::Release(CManager *&prManager)
{
	if (USED(prManager))
	{ // 使用中の場合

		// マネージャーの終了
		if (FAILED(prManager->Uninit()))
		{ // マネージャーの終了に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// メモリ開放
		delete prManager;
		prManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	シーンの設定処理
//============================================================
void CManager::SetScene(const CScene::MODE mode, const int nWait)
{
	// 次のシーンを設定
	m_pFade->Set(mode, nWait);
}

//============================================================
//	モードの設定処理
//============================================================
HRESULT CManager::SetMode(const CScene::MODE mode)
{
	// サウンドを停止
	m_pSound->Stop();

	if (USED(m_pScene))
	{ // シーンが使用中の場合

		// シーンの破棄
		if (FAILED(CScene::Release(m_pScene)))
		{ // 破棄に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// オブジェクトの全破棄
	CObject::ReleaseAll();

	if (UNUSED(m_pScene))
	{ // シーンが非使用中の場合

		// シーンの生成
		m_pScene = CScene::Create(mode);
		if (UNUSED(m_pScene))
		{ // 非使用中の場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}
	else { assert(false); return E_FAIL; }	// 使用中

	// 成功を返す
	return S_OK;
}

//============================================================
//	モードの取得処理
//============================================================
CScene::MODE CManager::GetMode(void)
{
	// 現在のモードを返す
	return m_pScene->GetMode();
}

//============================================================
//	レンダラー取得処理
//============================================================
CRenderer *CManager::GetRenderer(void)
{
	// レンダラーのポインタを返す
	return m_pRenderer;
}

//============================================================
//	キーボード取得処理
//============================================================
CInputKeyboard *CManager::GetKeyboard(void)
{
	// キーボードのポインタを返す
	return m_pKeyboard;
}

//============================================================
//	マウス取得処理
//============================================================
CInputMouse *CManager::GetMouse(void)
{
	// マウスのポインタを返す
	return m_pMouse;
}

//============================================================
//	パッド取得処理
//============================================================
CInputPad *CManager::GetPad(void)
{
	// パッドのポインタを返す
	return m_pPad;
}

//============================================================
//	サウンド取得処理
//============================================================
CSound *CManager::GetSound(void)
{
	// サウンドのポインタを返す
	return m_pSound;
}

//============================================================
//	カメラ取得処理
//============================================================
CCamera *CManager::GetCamera(void)
{
	// カメラのポインタを返す
	return m_pCamera;
}

//============================================================
//	ライト取得処理
//============================================================
CLight *CManager::GetLight(void)
{
	// ライトのポインタを返す
	return m_pLight;
}

//============================================================
//	テクスチャ取得処理
//============================================================
CTexture *CManager::GetTexture(void)
{
	// テクスチャのポインタを返す
	return m_pTexture;
}

//============================================================
//	モデル取得処理
//============================================================
CModel *CManager::GetModel(void)
{
	// モデルのポインタを返す
	return m_pModel;
}

//============================================================
//	フェード取得処理
//============================================================
CFade *CManager::GetFade(void)
{
	// フェードのポインタを返す
	return m_pFade;
}

//============================================================
//	シーン取得処理
//============================================================
CScene *CManager::GetScene(void)
{
	// シーンのポインタを返す
	return m_pScene;
}

//============================================================
//	ゲームマネージャー取得処理
//============================================================
CGameManager *CManager::GetGameManager(void)
{
	// ゲームマネージャーを返す
	return m_pGameManager;
}

//============================================================
//	デバッグ表示取得処理
//============================================================
CDebugProc *CManager::GetDebugProc(void)
{
	// デバッグ表示のポインタを返す
	return m_pDebugProc;
}

//============================================================
//	デバッグ取得処理
//============================================================
#ifdef _DEBUG	// デバッグ処理

CDebug *CManager::GetDebug(void)
{
	// デバッグのポインタを返す
	return m_pDebug;
}

#endif	// _DEBUG
