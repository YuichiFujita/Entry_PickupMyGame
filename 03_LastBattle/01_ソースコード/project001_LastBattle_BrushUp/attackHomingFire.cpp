//============================================================
//
//	ホーミング攻撃炎処理 [attackHomingFire.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "attackHomingFire.h"
#include "player.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const float	MOVE	= 7.6f;		// 炎の移動量
	const float	REV_ROT	= 0.08f;	// 炎の向き変更補正係数
}

//************************************************************
//	子クラス [CAttackHomingFire] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CAttackHomingFire::CAttackHomingFire()
{

}

//============================================================
//	デストラクタ
//============================================================
CAttackHomingFire::~CAttackHomingFire()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CAttackHomingFire::Init(void)
{
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
void CAttackHomingFire::Uninit(void)
{
	// 炎の終了
	CFire::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CAttackHomingFire::Update(void)
{
	// 炎移動量の更新
	UpdateHomingMove();

	// 炎の更新
	CFire::Update();
}

//============================================================
//	描画処理
//============================================================
void CAttackHomingFire::Draw(CShader *pShader)
{
	// 炎の描画
	CFire::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CAttackHomingFire *CAttackHomingFire::Create(const D3DXVECTOR3& rPos)
{
	// ホーミング攻撃炎の生成
	CAttackHomingFire *pAttackHomingFire = new CAttackHomingFire;
	if (pAttackHomingFire == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// ホーミング攻撃炎の初期化
		if (FAILED(pAttackHomingFire->Init()))
		{ // 初期化に失敗した場合

			// ホーミング攻撃炎の破棄
			SAFE_DELETE(pAttackHomingFire);
			return nullptr;
		}

		// 位置を設定
		pAttackHomingFire->SetVec3Position(rPos);

		// 確保したアドレスを返す
		return pAttackHomingFire;
	}
}

//============================================================
//	炎移動量の更新処理
//============================================================
void CAttackHomingFire::UpdateHomingMove(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();		// プレイヤーリスト
	if (pList == nullptr)		 { assert(false); return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { assert(false); return; }	// プレイヤーが1人じゃない
	auto player = pList->GetList().front();					// プレイヤー情報

	D3DXVECTOR3 posPlayer	 = player->GetVec3Position();	// プレイヤーの中心位置
				posPlayer.y += player->GetHeight() * 0.5f;	// 高さの半分上にあげる

	D3DXVECTOR3 posFire		= GetVec3Position();	// 炎の位置
	D3DXVECTOR3 vecCurMove = GetMove();				// 炎の現在移動ベクトル
	D3DXVECTOR3 vecDestMove = posPlayer - posFire;	// 炎の目標移動ベクトル
	float fCurPhi, fCurTheta, fDestPhi, fDestTheta;	// 炎のホーミング計算用

	// 現在・目標向きを求める
	useful::VecToRot(vecCurMove,  &fCurPhi,  &fCurTheta);
	useful::VecToRot(vecDestMove, &fDestPhi, &fDestTheta);

	// ホーミングの計算
	CalcHoming(&fCurPhi,   fDestPhi);
	CalcHoming(&fCurTheta, fDestTheta);

	// ベクトルを求める
	useful::RotToVec(fCurPhi, fCurTheta, &vecCurMove);

	// 移動量を反映
	SetMove(vecCurMove * MOVE);
}

//============================================================
//	ホーミングの計算処理
//============================================================
void CAttackHomingFire::CalcHoming(float *pCurRot, float fDestRot)
{
	// 変数を宣言
	float fDiffRot = 0.0f;	// 差分向き

	// 目標向きの正規化
	useful::NormalizeRot(fDestRot);

	// 目標向きまでの差分を計算
	fDiffRot = fDestRot - *pCurRot;

	// 差分向きの正規化
	useful::NormalizeRot(fDiffRot);

	// 向きの更新
	*pCurRot += fDiffRot * REV_ROT;

	// 向きの正規化
	useful::NormalizeRot(*pCurRot);
}
