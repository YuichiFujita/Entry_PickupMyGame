//============================================================
//
//	球体判定デバッグ表示処理 [debugCollSphere.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "debugCollSphere.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int PRIORITY = 7;	// 球体判定デバッグ表示の優先順位
}

//************************************************************
//	子クラス [CDebugCollSphere] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CDebugCollSphere::CDebugCollSphere() : CObjectMeshSphere(CObject::LABEL_DEBUG, CObject::DIM_3D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CDebugCollSphere::~CDebugCollSphere()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CDebugCollSphere::Init(void)
{
	// オブジェクトメッシュスフィアの初期化
	if (FAILED(CObjectMeshSphere::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CDebugCollSphere::Uninit(void)
{
	// オブジェクトメッシュスフィアの終了
	CObjectMeshSphere::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CDebugCollSphere::Update(void)
{
	// オブジェクトメッシュスフィアの更新
	CObjectMeshSphere::Update();
}

//============================================================
//	描画処理
//============================================================
void CDebugCollSphere::Draw(CShader *pShader)
{
	// オブジェクトメッシュスフィアの描画
	CObjectMeshSphere::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CDebugCollSphere *CDebugCollSphere::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const float fRadius,		// 半径
	const D3DXCOLOR& rCol,		// 色
	const D3DXVECTOR3& rRot,	// 向き
	const POSGRID2& rPart,		// 分割数
	const POSGRID2& rTexPart	// テクスチャ分割数
)
{
	// 球体判定デバッグ表示の生成
	CDebugCollSphere *pDebugCollSphere = new CDebugCollSphere;
	if (pDebugCollSphere == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 球体判定デバッグ表示の初期化
		if (FAILED(pDebugCollSphere->Init()))
		{ // 初期化に失敗した場合

			// 球体判定デバッグ表示の破棄
			SAFE_DELETE(pDebugCollSphere);
			return nullptr;
		}

		// 位置を設定
		pDebugCollSphere->SetVec3Position(rPos);

		// 向きを設定
		pDebugCollSphere->SetVec3Rotation(rRot);

		// 色を設定
		pDebugCollSphere->SetColor(rCol);

		// 半径を設定
		pDebugCollSphere->SetRadius(fRadius);

		// 分割数を設定
		if (FAILED(pDebugCollSphere->SetPattern(rPart)))
		{ // 分割数の設定に失敗した場合

			// 球体判定デバッグ表示の破棄
			SAFE_DELETE(pDebugCollSphere);
			return nullptr;
		}

		// テクスチャ分割数を設定
		pDebugCollSphere->SetTexPattern(rTexPart);

		// 確保したアドレスを返す
		return pDebugCollSphere;
	}
}
