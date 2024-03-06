//============================================================
//
//	雷処理 [thunder.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "thunder.h"
#include "orbit.h"
#include "scene.h"
#include "stage.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXCOLOR COL[] =	// 軌跡の色
	{
		D3DXCOLOR(1.0f, 1.0f, 0.1f, 1.0f),
		D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f),
	};
	const D3DXVECTOR3 OFFSET[] =	// 軌跡のオフセット
	{
		D3DXVECTOR3(10.0f, 0.0f, 0.0f),
		D3DXVECTOR3(32.0f, 0.0f, 0.0f),
	};
	const int DIV_DIRRAND[] = { 6, 36 };	// 方向の剰余算の値
	const int ADD_DIRRAND[] = { 10, 80 };	// 方向の減算の値

	const int	PRIORITY		= 4;		// 雷の優先順位
	const int	PART			= 20;		// 軌跡の分割数
	const int	MAX_SHIFT_POSY	= 120;		// 軌跡のY座標ずらし量の最大値
	const float	DOWN_MOVE		= 200.0f;	// 雷の下移動量
	const float	SPAWN_POSY		= 2500.0f;	// 雷の生成Y座標
}

//************************************************************
//	子クラス [CThunder] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CThunder::CThunder() : CObject(CObject::LABEL_THUNDER, DIM_3D, PRIORITY),
	m_posOrbit	(VEC3_ZERO),	// 軌跡の生成位置
	m_bLand		(false)			// 軌跡の着地判定
{
	// メンバ変数をクリア
	memset(&m_aOrbit[0], 0, sizeof(m_aOrbit));	// 軌跡の情報
}

//============================================================
//	デストラクタ
//============================================================
CThunder::~CThunder()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CThunder::Init(void)
{
	// メンバ変数を初期化
	memset(&m_aOrbit[0], 0, sizeof(m_aOrbit));	// 軌跡の情報
	m_posOrbit	= VEC3_ZERO;	// 軌跡の生成位置
	m_bLand		= false;		// 軌跡の着地判定

	for (int nCntOrbit = 0; nCntOrbit < thunder::NUM_ORBIT; nCntOrbit++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の生成
		m_aOrbit[nCntOrbit].pOrbit = COrbit::Create
		( // 引数
			&m_aOrbit[nCntOrbit].mtxWorld,									// 親マトリックス
			COrbit::SOffset(VEC3_ZERO, OFFSET[nCntOrbit], COL[nCntOrbit]),	// オフセット情報
			PART															// 分割数
		);
		if (m_aOrbit[nCntOrbit].pOrbit == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_aOrbit[nCntOrbit].pOrbit->SetPriority(PRIORITY);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CThunder::Uninit(void)
{
	for (int nCntOrbit = 0; nCntOrbit < thunder::NUM_ORBIT; nCntOrbit++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の終了
		SAFE_UNINIT(m_aOrbit[nCntOrbit].pOrbit);
	}

	// 雷オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CThunder::Update(void)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージの情報
	assert(pStage != nullptr);	// ステージ未使用

	if (!m_bLand)
	{ // 地面に到達していない場合

		// 位置を下げる
		m_posOrbit.y -= DOWN_MOVE;

		float fLandPosY = pStage->GetStageLimit().fField;
		if (m_posOrbit.y <= fLandPosY)
		{ // 地面に到達した場合

			// 位置を補正
			m_posOrbit.y = fLandPosY;

			// 着地済みにする
			m_bLand = true;
		}
	}

	D3DXMATRIX  mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXVECTOR3 rotRand;			// ランダム向き
	bool bRelease = true;			// 破棄するかどうか
	for (int nCntOrbit = 0; nCntOrbit < thunder::NUM_ORBIT; nCntOrbit++)
	{ // 軌跡の生成数分繰り返す

		if (m_bLand)
		{ // 地面に到達している場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&m_aOrbit[nCntOrbit].mtxWorld);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, m_posOrbit.x, m_posOrbit.y, m_posOrbit.z);
			D3DXMatrixMultiply(&m_aOrbit[nCntOrbit].mtxWorld, &m_aOrbit[nCntOrbit].mtxWorld, &mtxTrans);

			// 軌跡を消失状態にする
			m_aOrbit[nCntOrbit].pOrbit->SetState(COrbit::STATE_VANISH);
		}
		else
		{ // 地面に到達していない場合

			// ランダムに向きを設定
			rotRand.x = useful::RandomRot();
			rotRand.y = useful::RandomRot();
			rotRand.z = useful::RandomRot();

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&m_aOrbit[nCntOrbit].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, rotRand.y, rotRand.x, rotRand.z);
			D3DXMatrixMultiply(&m_aOrbit[nCntOrbit].mtxWorld, &m_aOrbit[nCntOrbit].mtxWorld, &mtxRot);

			// 軌跡の位置ずらし量を設定
			float fRandShiftLength = 0.0f;	// 位置ずらしの長さ
			fRandShiftLength  = (float)((rand() % DIV_DIRRAND[nCntOrbit] + ADD_DIRRAND[nCntOrbit]));	// ずらす長さの絶対値を求める
			fRandShiftLength *= (rand() % 2 == 0) ? +1.0f : -1.0f;										// どちらの方向にずらすかを設定

			// 位置を反映
			D3DXMatrixTranslation
			( // 引数
				&mtxTrans,
				m_posOrbit.x + sinf(rotRand.y) * fRandShiftLength,
				m_posOrbit.y + (float)(rand() % MAX_SHIFT_POSY),
				m_posOrbit.z + cosf(rotRand.y) * fRandShiftLength
			);
			D3DXMatrixMultiply(&m_aOrbit[nCntOrbit].mtxWorld, &m_aOrbit[nCntOrbit].mtxWorld, &mtxTrans);
		}

		// 軌跡の更新
		m_aOrbit[nCntOrbit].pOrbit->Update();

		if (bRelease)
		{ // 現状破棄する設定の場合

			if (m_aOrbit[nCntOrbit].pOrbit->GetState() != COrbit::STATE_NONE)
			{ // 軌跡が消失していない場合

				// 破棄できないようにする
				bRelease = false;
			}
		}
	}

	if (bRelease)
	{ // 破棄可能な場合

		// 自身を終了
		Uninit();

		// 処理を抜ける
		return;
	}
}

//============================================================
//	描画処理
//============================================================
void CThunder::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	位置の設定処理
//============================================================
void CThunder::SetVec3Position(const D3DXVECTOR3 &rPos)
{
	// 位置を設定
	m_posOrbit = D3DXVECTOR3(rPos.x, SPAWN_POSY, rPos.z);

	// ワールドマトリックスを初期化
	D3DXMATRIX  mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXVECTOR3 rotRand;			// ランダム向き
	for (int nCntOrbit = 0; nCntOrbit < thunder::NUM_ORBIT; nCntOrbit++)
	{ // 軌跡の生成数分繰り返す

		// 向きをランダム回転
		rotRand.x = useful::RandomRot();
		rotRand.y = useful::RandomRot();
		rotRand.z = useful::RandomRot();

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_aOrbit[nCntOrbit].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, rotRand.y, rotRand.x, rotRand.z);
		D3DXMatrixMultiply(&m_aOrbit[nCntOrbit].mtxWorld, &m_aOrbit[nCntOrbit].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_posOrbit.x, m_posOrbit.y, m_posOrbit.z);
		D3DXMatrixMultiply(&m_aOrbit[nCntOrbit].mtxWorld, &m_aOrbit[nCntOrbit].mtxWorld, &mtxTrans);
	}
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CThunder::GetVec3Position(void) const
{
	// 軌跡の生成位置を返す
	return m_posOrbit;
}

//============================================================
//	生成処理
//============================================================
CThunder *CThunder::Create(const D3DXVECTOR3 &rPos)
{
	// 雷の生成
	CThunder *pThunder = new CThunder;
	if (pThunder == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 雷の初期化
		if (FAILED(pThunder->Init()))
		{ // 初期化に失敗した場合

			// 雷の破棄
			SAFE_DELETE(pThunder);
			return nullptr;
		}

		// 位置を設定
		pThunder->SetVec3Position(rPos);

		// 確保したアドレスを返す
		return pThunder;
	}
}

//============================================================
//	破棄処理
//============================================================
void CThunder::Release(void)
{
	// オブジェクトの破棄
	CObject::Release();
}
