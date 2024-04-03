//============================================================
//
//	魔法陣処理 [magicCircle.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "magicCircle.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "objectMeshTube.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\magicCircle000.png",	// 通常
	};

	const int	PRIORITY		= 2;		// 魔法陣の優先順位
	const float	CROP_LENGTH		= -1000.0f;	// 透明の長さ
	const POSGRID2 PART_CIRCLE	= POSGRID2(32, 2);	// 魔法陣の分割数
	const POSGRID2 PART_CROP	= POSGRID2(32, 2);	// 魔法陣の分割数
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CMagicCircle::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CListManager<CMagicCircle> *CMagicCircle::m_pList = nullptr;	// オブジェクトリスト

//************************************************************
//	子クラス [CMagicCircle] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMagicCircle::CMagicCircle() : CObjectMeshCircle(CObject::LABEL_MAGIC_CIRCLE, CObject::DIM_3D, PRIORITY),
	m_pCropTube	(nullptr)	// 切り抜き用チューブの情報
{

}

//============================================================
//	デストラクタ
//============================================================
CMagicCircle::~CMagicCircle()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMagicCircle::Init(void)
{
	// メンバ変数を初期化
	m_pCropTube = nullptr;	// 切り抜き用チューブの情報

	//--------------------------------------------------------
	//	基底クラスの初期化・設定
	//--------------------------------------------------------
	{
		// オブジェクトメッシュサークルの初期化
		if (FAILED(CObjectMeshCircle::Init()))
		{ // 初期化に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 分割数の設定
		if (FAILED(SetPattern(PART_CIRCLE)))
		{ // 分割数の設定に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// レンダーステートの情報を取得
		CRenderState *pRenderState = GetRenderState();

		// Zテストを絶対成功にする
		pRenderState->SetZFunc(D3DCMP_ALWAYS);

		// ポリゴンの両面を表示状態にする
		pRenderState->SetCulling(D3DCULL_NONE);

		// ライティングをOFFにする
		pRenderState->SetLighting(false);
	}

	//--------------------------------------------------------
	//	切り抜き用チューブの初期化・設定
	//--------------------------------------------------------
	{
		// 切り抜き用チューブの生成
		m_pCropTube = CObjectMeshTube::Create
		( // 引数
			VEC3_ZERO,	// 位置
			VEC3_ZERO,	// 向き
			XCOL_WHITE,	// 色
			PART_CROP,	// 分割数
			GRID2_ONE,	// テクスチャ分割数
			0.0f,		// 半径
			CROP_LENGTH	// 縦幅
		);
		if (m_pCropTube == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 自動描画をOFFにする
		m_pCropTube->SetEnableDraw(false);

		// レンダーステートの情報を取得
		CRenderState renderState = m_pCropTube->GetRenderState();

		// ポリゴンの両面を表示状態にする
		renderState.SetCulling(D3DCULL_NONE);

		// ライティングをOFFにする
		renderState.SetLighting(false);

		// レンダーステートの情報を反映
		m_pCropTube->SetRenderState(renderState);
	}

	if (m_pList == nullptr)
	{ // リストマネージャーが存在しない場合

		// リストマネージャーの生成
		m_pList = CListManager<CMagicCircle>::Create();
		if (m_pList == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// リストに自身のオブジェクトを追加・イテレーターを取得
	m_iterator = m_pList->AddList(this);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CMagicCircle::Uninit(void)
{
	// 切り抜き用チューブの終了
	SAFE_UNINIT(m_pCropTube);

	// リストから自身のオブジェクトを削除
	m_pList->DeleteList(m_iterator);

	if (m_pList->GetNumAll() == 0)
	{ // オブジェクトが一つもない場合

		// リストマネージャーの破棄
		m_pList->Release(m_pList);
	}

	// オブジェクトメッシュサークルの終了
	CObjectMeshCircle::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CMagicCircle::Update(void)
{
	// オブジェクトメッシュサークルの更新
	CObjectMeshCircle::Update();
}

//============================================================
//	描画処理
//============================================================
void CMagicCircle::Draw(CShader *pShader)
{
	// オブジェクトメッシュサークルの描画
	CObjectMeshCircle::Draw(pShader);
}

//============================================================
//	位置の設定処理
//============================================================
void CMagicCircle::SetVec3Position(const D3DXVECTOR3 &rPos)
{
	// 自身の位置を設定
	CObjectMeshCircle::SetVec3Position(rPos);

	// 切り抜き半径の位置を設定
	m_pCropTube->SetVec3Position(rPos);
}

//============================================================
//	向きの設定処理
//============================================================
void CMagicCircle::SetVec3Rotation(const D3DXVECTOR3 &rRot)
{
	// 自身の向きを設定
	CObjectMeshCircle::SetVec3Rotation(rRot);

	// 切り抜き半径の向きを設定
	m_pCropTube->SetVec3Rotation(rRot);
}

//============================================================
//	生成処理
//============================================================
CMagicCircle *CMagicCircle::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const float fRadius,		// 半径
	const float fCropRadius		// 切り抜き半径
)
{
	// 魔法陣の生成
	CMagicCircle *pMagicCircle = new CMagicCircle;
	if (pMagicCircle == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 魔法陣の初期化
		if (FAILED(pMagicCircle->Init()))
		{ // 初期化に失敗した場合

			// 魔法陣の破棄
			SAFE_DELETE(pMagicCircle);
			return nullptr;
		}

		// テクスチャを登録・割当
		pMagicCircle->BindTexture(GET_MANAGER->GetTexture()->Regist(TEXTURE_FILE[TEXTURE_NORMAL]));

		// 位置を設定
		pMagicCircle->SetVec3Position(rPos);

		// 向きを設定
		pMagicCircle->SetVec3Rotation(rRot);

		// 半径を設定
		pMagicCircle->SetRadius(fRadius);

		// 切り抜き半径を設定
		pMagicCircle->SetCropRadius(fCropRadius);

		// 確保したアドレスを返す
		return pMagicCircle;
	}
}

//============================================================
//	リスト取得処理
//============================================================
CListManager<CMagicCircle>* CMagicCircle::GetList(void)
{
	// オブジェクトリストを返す
	return m_pList;
}

//============================================================
//	切り抜き用の描画処理
//============================================================
void CMagicCircle::DrawCrop(void)
{
	// 自動描画がOFFの場合抜ける
	if (!IsDraw()) { return; }

	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイス情報
	CMonoShader	*pMonoShader  = CMonoShader::GetInstance();	// 単色描画シェーダー情報
	if (pMonoShader->IsEffectOK())
	{ // エフェクトが使用可能な場合

		// ピクセル描画色を白に設定
		pMonoShader->SetColor(XCOL_WHITE);

		// ステンシルテストを有効にする
		pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

		// 比較参照値を設定する
		pDevice->SetRenderState(D3DRS_STENCILREF, 1);

		// ステンシルマスクを指定する 
		pDevice->SetRenderState(D3DRS_STENCILMASK, 255);

		// ステンシル比較関数を指定する
		pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

		// ステンシル結果に対しての反映設定
		pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);	// Zテスト・ステンシルテスト成功
		pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);		// Zテスト・ステンシルテスト失敗
		pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);		// Zテスト失敗・ステンシルテスト成功

		// 切り抜き用チューブの描画
		m_pCropTube->Draw(pMonoShader);

		// ステンシルテストを無効にする
		pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	else { assert(false); }
}

//============================================================
//	切り抜き半径の設定処理
//============================================================
void CMagicCircle::SetCropRadius(const float fRadius)
{
	// 切り抜き用チューブの半径を設定
	m_pCropTube->SetRadius(fRadius);
}
