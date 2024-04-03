//============================================================
//
//	直進攻撃炎処理 [attackMoveFire.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "attackMoveFire.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{

}

//************************************************************
//	子クラス [CAttackMoveFire] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CAttackMoveFire::CAttackMoveFire() :
	m_dirMove	(VEC3_ZERO),	// 移動方向ベクトル
	m_fMove		(0.0f),			// 移動量
	m_fAddMove	(0.0f)			// 加速量
{

}

//============================================================
//	デストラクタ
//============================================================
CAttackMoveFire::~CAttackMoveFire()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CAttackMoveFire::Init(void)
{
	// メンバ変数を初期化
	m_dirMove	= VEC3_ZERO;	// 移動方向ベクトル
	m_fMove		= 0.0f;			// 移動量
	m_fAddMove	= 0.0f;			// 加速量

	// 炎の初期化
	if (FAILED(CFire::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CAttackMoveFire::Uninit(void)
{
	// 炎の終了
	CFire::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CAttackMoveFire::Update(void)
{
	// 移動の更新
	UpdateMove();

	// 炎の更新
	CFire::Update();
}

//============================================================
//	描画処理
//============================================================
void CAttackMoveFire::Draw(CShader *pShader)
{
	// 炎の描画
	CFire::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CAttackMoveFire *CAttackMoveFire::Create
(
	const D3DXVECTOR3& rPos,		// 生成位置
	const D3DXVECTOR3& rVecMove,	// 移動方向
	const float fMove,				// 移動量
	const float fAddMove,			// 加速量
	const int nLife					// 寿命
)
{
	// 直進攻撃炎の生成
	CAttackMoveFire *pAttackMoveFire = new CAttackMoveFire;
	if (pAttackMoveFire == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 直進攻撃炎の初期化
		if (FAILED(pAttackMoveFire->Init()))
		{ // 初期化に失敗した場合

			// 直進攻撃炎の破棄
			SAFE_DELETE(pAttackMoveFire);
			return nullptr;
		}

		// 位置を設定
		pAttackMoveFire->SetVec3Position(rPos);

		// 移動方向を設定
		pAttackMoveFire->SetMoveDir(rVecMove);

		// 移動量を設定
		pAttackMoveFire->SetMoveValue(fMove);

		// 加速量を設定
		pAttackMoveFire->SetAddMoveValue(fAddMove);

		// 寿命を設定
		pAttackMoveFire->SetLife(nLife);

		// 確保したアドレスを返す
		return pAttackMoveFire;
	}
}

//============================================================
//	移動方向の設定処理
//============================================================
void CAttackMoveFire::SetMoveDir(const D3DXVECTOR3& rMove)
{
	// 移動方向を設定
	m_dirMove = rMove;

	// 移動方向を正規化
	D3DXVec3Normalize(&m_dirMove, &m_dirMove);
}

//============================================================
//	移動の更新処理
//============================================================
void CAttackMoveFire::UpdateMove(void)
{
	// 移動量を反映
	SetMove(m_dirMove * m_fMove);

	// 移動量を加速
	m_fMove += m_fAddMove;
}
