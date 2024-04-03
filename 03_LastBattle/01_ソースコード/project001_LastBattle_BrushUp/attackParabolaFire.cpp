//============================================================
//
//	カーブ攻撃炎処理 [attackParabolaFire.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "attackParabolaFire.h"
#include "player.h"

// TODO
#include "manager.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const float	GRAVITY = -1.0f;	// 重力
}

//************************************************************
//	子クラス [CAttackParabolaFire] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CAttackParabolaFire::CAttackParabolaFire(const D3DXVECTOR3 &rPosDest) :
	m_posDest		(rPosDest),		// 目標位置
	m_posOrigin		(VEC3_ZERO),	// 原点位置
	m_fLength		(0.0f),			// 移動距離
	m_fPhi			(0.0f),			// 方位角
	m_fTheta		(0.0f),			// 仰角
	m_fMove			(0.0f),			// 移動量
	m_nCounterTime	(0)				// 攻撃時間管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CAttackParabolaFire::~CAttackParabolaFire()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CAttackParabolaFire::Init(void)
{
	// メンバ変数を初期化
	m_posOrigin	= VEC3_ZERO;	// 原点位置
	m_fLength	= 0.0f;			// 移動距離
	m_fPhi		= 0.0f;			// 方位角
	m_fTheta	= 0.0f;			// 仰角
	m_fMove		= 0.0f;			// 移動量
	m_nCounterTime = 0;			// 攻撃時間管理カウンター

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
void CAttackParabolaFire::Uninit(void)
{
	// 炎の終了
	CFire::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CAttackParabolaFire::Update(void)
{
	// 炎移動量の更新
	UpdateParabolaMove();

	// 炎の更新
	CFire::Update();
}

//============================================================
//	描画処理
//============================================================
void CAttackParabolaFire::Draw(CShader *pShader)
{
	// 炎の描画
	CFire::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CAttackParabolaFire *CAttackParabolaFire::Create
(
	const D3DXVECTOR3& rCurPos,		// 生成位置
	const D3DXVECTOR3& rDestPos,	// 目標位置
	const float fMove				// 移動量
)
{
	// カーブ攻撃炎の生成
	CAttackParabolaFire *pAttackParabolaFire = new CAttackParabolaFire(rDestPos);
	if (pAttackParabolaFire == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// カーブ攻撃炎の初期化
		if (FAILED(pAttackParabolaFire->Init()))
		{ // 初期化に失敗した場合

			// カーブ攻撃炎の破棄
			SAFE_DELETE(pAttackParabolaFire);
			return nullptr;
		}

		// 生成位置を設定
		pAttackParabolaFire->SetVec3Position(rCurPos);
		pAttackParabolaFire->m_posOrigin = rCurPos;

		// 炎の移動パラメーターを初期化
		pAttackParabolaFire->InitParabolaParam(rCurPos);

		// 移動量を設定
		pAttackParabolaFire->m_fMove = fMove;

		// 確保したアドレスを返す
		return pAttackParabolaFire;
	}
}

//============================================================
//	炎移動パラメーターの初期化処理
//============================================================
void CAttackParabolaFire::InitParabolaParam(const D3DXVECTOR3& rCurPos)
{
	D3DXVECTOR3 vecLength = m_posDest - rCurPos;		// 目標位置へのベクトル
	m_fLength = D3DXVec3Length(&vecLength);				// ベクトルの長さ
	useful::VecToRot(vecLength, &m_fPhi, &m_fTheta);	// ベクトルを向きに変換
}

//============================================================
//	炎移動量の更新処理
//============================================================
void CAttackParabolaFire::UpdateParabolaMove(void)
{
#if 0
#if 0
#if 0
	// 現在の経過時間から炎の位置を求める
	float fMaxTime;	// 経過時間
	D3DXVECTOR2 pos = useful::CalcPosParabola(GRAVITY, m_fMove, m_posDest.x, m_posDest.y, (float)m_nCounterTime, &fMaxTime);

	if ((float)m_nCounterTime <= fMaxTime)
	{ // 最大経過時間に到達していない場合

		// 位置を反映
		SetVec3Position(D3DXVECTOR3(pos.x, pos.y, 0.0f));
	}
#else
	// 現在の経過時間から炎の移動量を求める
	float fMaxTime;	// 経過時間
	D3DXVECTOR2 move = useful::CalcMoveParabola(GRAVITY, m_fMove, m_posDest.x, m_posDest.y, (float)m_nCounterTime, &fMaxTime);

	if ((float)m_nCounterTime <= fMaxTime)
	{ // 最大経過時間に到達していない場合

		// 移動量を反映
		SetMove(D3DXVECTOR3(move.x, move.y, 0.0f));
	}
	else
	{ // 最大経過時間に到達していない場合

		// 移動量を初期化
		SetMove(VEC3_ZERO);
	}
#endif
#else
	// 現在の経過時間から炎の位置を求める
	float fMaxTime;	// 経過時間
	D3DXVECTOR2 posPara = useful::CalcPosParabola(GRAVITY, m_fMove, m_fLength, 1000.0f, (float)m_nCounterTime, &fMaxTime);

	if ((float)m_nCounterTime <= fMaxTime)
	{ // 最大経過時間に到達していない場合

		D3DXVECTOR2 posPara = useful::CalcPosParabola(GRAVITY, m_fMove, m_fLength, 1000.0f, fMaxTime);

		D3DXVECTOR3 pos;	// 位置
		D3DXMATRIX mtxTrans, mtxRot, mtxRoot, mtxWorld;	// マトリックス計算用

		// 初期化
		D3DXMatrixIdentity(&mtxWorld);

		// Yaw（方位角）とPitch（仰角）から回転行列を作成
		//D3DXMatrixRotationYawPitchRoll(&mtxRot, m_fPhi, m_fTheta, 0.0f);
		//D3DXMatrixMultiply(&mtxWorld, &mtxRot, &mtxWorld);

		// 方位角反映
		D3DXMatrixRotationX(&mtxRoot, m_fPhi);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRoot);

		// 仰角反映
		D3DXMatrixRotationY(&mtxRot, m_fTheta);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		// 位置反映
		D3DXMatrixTranslation(&mtxTrans, 0.0f, posPara.y, -posPara.x);
		D3DXMatrixMultiply(&mtxWorld, &mtxTrans, &mtxWorld);

		//D3DXMatrixMultiply(&mtxWorld, &mtxRot, &mtxTrans);

		// 位置を設定
		pos = useful::GetMatrixPosition(mtxWorld);

		// 位置を反映
		SetVec3Position(pos + m_posOrigin);

		GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_CENTER, "%f %f %f\n", pos.x, pos.y, pos.z);
	}
#endif
#endif

	// 攻撃発射からの経過カウンターを加算
	m_nCounterTime++;
}
