//============================================================
//
//	星ゲージ処理 [gaugeStar.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "gaugeStar.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "texture.h"
#include "sceneGame.h"
#include "objectBillboard.h"
#include "target.h"

//************************************************************
//	マクロ定義
//************************************************************
#define GAUGE_PRIO	(3)	// 星ゲージの優先順位

#define GAUGE_BG_COL	(D3DXCOLOR(0.25f, 0.25f, 0.25f, 1.0f))	// ゲージ背景色
#define GAUGE_OVER_COL	(D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f))		// ゲージオーバーヒート色

#define ADD_ROT	(0.05f)	// 距離加算用向きの加算量
#define REV_ROT	(0.1f)	// 加算向きの補正係数

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CGaugeStar::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\star000.png",	// 星
	"data\\TEXTURE\\star001.png",	// 星枠
};

//************************************************************
//	子クラス [CGaugeStar] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CGaugeStar::CGaugeStar(const int nHealNumGauge, const int nHealWait, const float fMaxRadius) : CObject(CObject::LABEL_GAUGE, GAUGE_PRIO),
m_nHealNumGauge(nHealNumGauge), m_nHealWait(nHealWait), m_fMaxRadius(fMaxRadius)
{
	// メンバ変数をクリア
	memset(&m_apStarFrame[0], 0, sizeof(m_apStarFrame));	// 星の枠情報
	memset(&m_apStarBG[0], 0, sizeof(m_apStarBG));			// 星の背景情報
	memset(&m_aStar[0], 0, sizeof(m_aStar));				// 星の情報

	m_state			= STATE_NORMAL;	// 状態
	m_nMaxNumGauge	= 0;			// 最大ゲージ量
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nCurrentStar	= 0;			// 現在消費中の星
	m_bOverheat		= false;		// オーバーヒート状況
	m_pParentObject = NULL;			// 親オブジェクト
	m_posGap		= VEC3_ZERO;	// 表示位置の加算量
	m_fDistance		= 0.0f;			// 中心からの距離
	m_fFlicker		= 0.0f;			// 揺らめき量
	m_fSinRot		= 0.0f;			// 距離加減量
	m_fAddRot		= 0.0f;			// 消費中の星方向への現在加算量
	m_fDestAddRot	= 0.0f;			// 消費中の星方向への目標加算量
}

//============================================================
//	デストラクタ
//============================================================
CGaugeStar::~CGaugeStar()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CGaugeStar::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apStarFrame[0], 0, sizeof(m_apStarFrame));	// 星の枠情報
	memset(&m_apStarBG[0], 0, sizeof(m_apStarBG));			// 星の背景情報
	memset(&m_aStar[0], 0, sizeof(m_aStar));				// 星の情報

	m_state			= STATE_NORMAL;	// 状態
	m_nMaxNumGauge	= 0;			// 最大ゲージ量
	m_nCounterState = 0;			// 状態管理カウンター
	m_nCurrentStar	= 0;			// 現在消費中の星
	m_bOverheat		= false;		// オーバーヒート状況
	m_pParentObject = NULL;			// 親オブジェクト
	m_posGap		= VEC3_ZERO;	// 表示位置の加算量
	m_fDistance		= 0.0f;			// 中心からの距離
	m_fFlicker		= 0.0f;			// 揺らめき量
	m_fSinRot		= 0.0f;			// 距離加減量
	m_fAddRot		= 0.0f;			// 消費中の星方向への現在加算量
	m_fDestAddRot	= 0.0f;			// 消費中の星方向への目標加算量

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の枠の生成
		m_apStarFrame[nCntStar] = CObjectBillboard::Create
		( // 引数
			VEC3_ZERO,	// 位置
			VEC3_ZERO	// 大きさ
		);
		if (UNUSED(m_apStarFrame[nCntStar]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apStarFrame[nCntStar]->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_FRAME]));

		// 優先順位を設定
		m_apStarFrame[nCntStar]->SetPriority(GAUGE_PRIO);

		// 色を設定
		m_apStarFrame[nCntStar]->SetColor(XCOL_BLACK);

		// Zテストを設定
		m_apStarFrame[nCntStar]->SetFunc(D3DCMP_LESSEQUAL);
	}

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の背景の生成
		m_apStarBG[nCntStar] = CObjectBillboard::Create
		( // 引数
			VEC3_ZERO,	// 位置
			VEC3_ZERO	// 大きさ
		);
		if (UNUSED(m_apStarBG[nCntStar]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apStarBG[nCntStar]->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_STAR]));

		// 優先順位を設定
		m_apStarBG[nCntStar]->SetPriority(GAUGE_PRIO);

		// 色を設定
		m_apStarBG[nCntStar]->SetColor(GAUGE_BG_COL);

		// Zテストを設定
		m_apStarBG[nCntStar]->SetFunc(D3DCMP_LESSEQUAL);

		// 星の生成
		m_aStar[nCntStar].pBillboard = CObjectBillboard::Create
		( // 引数
			VEC3_ZERO,	// 位置
			VEC3_ZERO	// 大きさ
		);
		if (UNUSED(m_aStar[nCntStar].pBillboard))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_aStar[nCntStar].pBillboard->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_STAR]));

		// 優先順位を設定
		m_aStar[nCntStar].pBillboard->SetPriority(GAUGE_PRIO);

		// 色を設定
		m_aStar[nCntStar].pBillboard->SetColor(XCOL_WHITE);

		// Zテストを設定
		m_aStar[nCntStar].pBillboard->SetFunc(D3DCMP_LESSEQUAL);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CGaugeStar::Uninit(void)
{
	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の終了
		m_aStar[nCntStar].pBillboard->Uninit();

		// 星の背景の終了
		m_apStarBG[nCntStar]->Uninit();

		// 星の枠の終了
		m_apStarFrame[nCntStar]->Uninit();
	}

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CGaugeStar::Update(void)
{
	// 変数を宣言
	float fDiffRot;	// 差分向き

	//--------------------------------------------------------
	//	サインカーブ用の向きを加算
	//--------------------------------------------------------
	// 距離加算用の向きを加算
	m_fSinRot += ADD_ROT;

	// 向きを補正
	useful::NormalizeRot(m_fSinRot);

	//--------------------------------------------------------
	//	加算向きの更新
	//--------------------------------------------------------
	// 差分向きを計算
	fDiffRot = m_fDestAddRot - m_fAddRot;

	// 差分向きを補正
	useful::NormalizeRot(fDiffRot);

	// 加算向きの更新
	m_fAddRot += fDiffRot * REV_ROT;

	// 加算向きを補正
	useful::NormalizeRot(m_fAddRot);

	//--------------------------------------------------------
	//	ゲージ消費の更新
	//--------------------------------------------------------
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:	// 通常状態

		if (m_nCurrentStar > 0
		||  m_aStar[m_nCurrentStar].nNumGauge < m_nMaxNumGauge)
		{ // ゲージが消費されている場合

			if (m_nCounterState < m_nHealWait)
			{ // カウンターが待機カウントより小さい場合

				// カウンターを加算
				m_nCounterState++;
			}
			else
			{ // カウンターが待機カウント以上の場合

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を設定
				m_state = STATE_HEAL;	// ゲージ回復状態
			}
		}

		break;

	case STATE_USAGE:	// ゲージ使用状態

		if (m_aStar[MAX_STAR - 1].nNumGauge > 0)
		{ // 最大消費をしていない場合

			// ゲージを減らす
			m_aStar[m_nCurrentStar].nNumGauge--;

			// 現在のゲージ量に応じた星の大きさに変更
			m_aStar[m_nCurrentStar].fRadius = (m_fMaxRadius / (float)m_nMaxNumGauge) * m_aStar[m_nCurrentStar].nNumGauge;

			// 星の大きさを設定
			m_aStar[m_nCurrentStar].pBillboard->SetScaling(D3DXVECTOR3(m_aStar[m_nCurrentStar].fRadius, m_aStar[m_nCurrentStar].fRadius, 0.0f));

			if (m_aStar[m_nCurrentStar].nNumGauge <= 0)
			{ // 現在の星ゲージを使い切った場合

				if (m_nCurrentStar < MAX_STAR - 1)
				{ // 消費星が最大数より小さい場合

					// 次の星に移行
					m_nCurrentStar++;

					// 目標加算向きを設定
					m_fDestAddRot = ((D3DX_PI * 2) / MAX_STAR) * m_nCurrentStar;
					useful::NormalizeRot(m_fDestAddRot);	// 向きを補正
				}
				else
				{ // 消費しきった場合

					// オーバーヒートした状態にする
					m_bOverheat = true;

					for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
					{ // 星の最大数分繰り返す

						// 色を設定
						m_aStar[nCntStar].pBillboard->SetColor(GAUGE_OVER_COL);
					}
				}
			}
		}

		// 通常状態に戻す
		m_state = STATE_NORMAL;

		break;

	case STATE_HEAL:	// ゲージ回復状態

		if (m_aStar[0].nNumGauge < m_nMaxNumGauge)
		{ // 最大回復をしていない場合

			// ゲージを増やす
			m_aStar[m_nCurrentStar].nNumGauge += m_nHealNumGauge;

			// 現在のゲージ量に応じた星の大きさに変更
			m_aStar[m_nCurrentStar].fRadius = (m_fMaxRadius / (float)m_nMaxNumGauge) * m_aStar[m_nCurrentStar].nNumGauge;

			// 星の大きさを設定
			m_aStar[m_nCurrentStar].pBillboard->SetScaling(D3DXVECTOR3(m_aStar[m_nCurrentStar].fRadius, m_aStar[m_nCurrentStar].fRadius, 0.0f));

			if (m_aStar[m_nCurrentStar].nNumGauge >= m_nMaxNumGauge)
			{ // 現在の星ゲージが満タンの場合

				// ゲージを補正
				m_aStar[m_nCurrentStar].nNumGauge = m_nMaxNumGauge;

				if (m_nCurrentStar > 0)
				{ // 消費星が最小数より大きい場合

					// 前の星に移行
					m_nCurrentStar--;

					// 目標加算向きを設定
					m_fDestAddRot = ((D3DX_PI * 2) / MAX_STAR) * m_nCurrentStar;
					useful::NormalizeRot(m_fDestAddRot);	// 向きを補正
				}
				else
				{ // 回復しきった場合

					// 状態を設定
					m_state = STATE_NORMAL;	// 通常状態

					// オーバーヒートしていない状態にする
					m_bOverheat = false;

					for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
					{ // 星の最大数分繰り返す

						// 色を設定
						m_aStar[nCntStar].pBillboard->SetColor(XCOL_WHITE);
					}
				}
			}
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	//--------------------------------------------------------
	//	ゲージの描画設定
	//--------------------------------------------------------
	// 描画情報を設定
	if (FAILED(SetDrawInfo()))
	{ // 描画情報の設定に失敗した場合

		// オブジェクトの終了
		Uninit();

		// 関数を抜ける
		return;
	}

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の更新
		m_aStar[nCntStar].pBillboard->Update();

		// 星の背景の更新
		m_apStarBG[nCntStar]->Update();

		// 星の枠の更新
		m_apStarFrame[nCntStar]->Update();
	}
}

//============================================================
//	描画処理
//============================================================
void CGaugeStar::Draw(void)
{

}

//============================================================
//	更新状況の設定処理
//============================================================
void CGaugeStar::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);	// 自身

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の更新状況を設定
		m_aStar[nCntStar].pBillboard->SetEnableUpdate(bUpdate);

		// 星の背景の更新状況を設定
		m_apStarBG[nCntStar]->SetEnableUpdate(bUpdate);

		// 星の枠の更新状況を設定
		m_apStarFrame[nCntStar]->SetEnableUpdate(bUpdate);
	}
}

//============================================================
//	描画状況の設定処理
//============================================================
void CGaugeStar::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);	// 自身

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星の描画状況を設定
		m_aStar[nCntStar].pBillboard->SetEnableDraw(bDraw);

		// 星の背景の描画状況を設定
		m_apStarBG[nCntStar]->SetEnableDraw(bDraw);

		// 星の枠の描画状況を設定
		m_apStarFrame[nCntStar]->SetEnableDraw(bDraw);
	}
}

//============================================================
//	生成処理
//============================================================
CGaugeStar *CGaugeStar::Create
(
	const int nMaxNumGauge,		// 最大ゲージ量
	const float fMaxRadius,		// 半径
	const int nHealNumGauge,	// ゲージ回復量
	const int nHealWait,		// 回復待機カウント
	CObject *pObject,			// 親オブジェクト
	const D3DXVECTOR3& rGap,	// 表示位置の加算量
	const float fDistance,		// 中心からの距離
	const float fFlicker		// 揺らめき量
)
{
	// ポインタを宣言
	CGaugeStar *pGaugeStar = NULL;	// 星ゲージ生成用

	if (UNUSED(pGaugeStar))
	{ // 使用されていない場合

		// メモリ確保
		pGaugeStar = new CGaugeStar(nHealNumGauge, nHealWait, fMaxRadius);	// 星ゲージ
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pGaugeStar))
	{ // 確保に成功している場合

		// 星ゲージの初期化
		if (FAILED(pGaugeStar->Init()))
		{ // 初期化に失敗した場合

			// 失敗を返す
			return NULL;
		}

		// 親オブジェクトを設定
		pGaugeStar->SetParentObject(pObject);

		// 最大ゲージ量を設定
		pGaugeStar->SetMaxNumGauge(nMaxNumGauge);

		// 表示位置の加算量を設定
		pGaugeStar->SetGapPosition(rGap);

		// 星情報を設定
		pGaugeStar->SetStar(nMaxNumGauge, fMaxRadius);

		// 中心からの距離を設定
		pGaugeStar->SetDistance(fDistance);

		// 揺らめき量を設定
		pGaugeStar->SetFlicker(fFlicker);

		// 確保したアドレスを返す
		return pGaugeStar;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ゲージの使用処理
//============================================================
bool CGaugeStar::UseGauge(void)
{
	if (!m_bOverheat)
	{ // オーバーヒートしていない場合

		if (m_aStar[MAX_STAR - 1].nNumGauge > 0)
		{ // ゲージ残量がある場合

			// カウンターを初期化
			m_nCounterState = 0;

			// 状態を設定
			m_state = STATE_USAGE;	// ゲージ使用状態

			// 使用した状態を返す
			return true;
		}
		else
		{ // ゲージ残量がない場合

			// 使用していない状態を返す
			return false;
		}
	}
	else
	{ // オーバーヒートしていた場合

		// 使用していない状態を返す
		return false;
	}
}

//============================================================
//	ゲージの全回復処理
//============================================================
void CGaugeStar::HealNumGauge(void)
{
	// 目標加算向きを設定
	m_fDestAddRot = ((D3DX_PI * 2) / MAX_STAR) * 0;
	useful::NormalizeRot(m_fDestAddRot);	// 向きを補正

	// 消費星を先頭に設定
	m_nCurrentStar = 0;

	// 状態管理カウンターを初期化
	m_nCounterState = 0;

	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// ゲージを最大値に補正
		m_aStar[nCntStar].nNumGauge = m_nMaxNumGauge;

		// 現在のゲージ量に応じた星の大きさに変更
		m_aStar[nCntStar].fRadius = (m_fMaxRadius / (float)m_nMaxNumGauge) * m_aStar[nCntStar].nNumGauge;

		// 星の大きさを設定
		m_aStar[nCntStar].pBillboard->SetScaling(D3DXVECTOR3(m_aStar[nCntStar].fRadius, m_aStar[nCntStar].fRadius, 0.0f));

		// 色を設定
		m_aStar[nCntStar].pBillboard->SetColor(XCOL_WHITE);
	}

	// 状態を設定
	m_state = STATE_NORMAL;	// 通常状態

	// オーバーヒートしていない状態にする
	m_bOverheat = false;
}

//============================================================
//	最大ゲージ量の設定処理
//============================================================
void CGaugeStar::SetMaxNumGauge(const int nMax)
{
	// 引数の最大ゲージ量を設定
	m_nMaxNumGauge = nMax;

	// ゲージの全回復
	HealNumGauge();
}

//============================================================
//	星情報の設定処理
//============================================================
void CGaugeStar::SetStar(const int nNumGauge, const float fRadius)
{
	for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
	{ // 星の最大数分繰り返す

		// 星のゲージ量を設定
		m_aStar[nCntStar].nNumGauge = nNumGauge;

		// 星の半径を設定
		m_aStar[nCntStar].fRadius = fRadius;

		// 星の大きさを設定
		m_aStar[nCntStar].pBillboard->SetScaling(D3DXVECTOR3(fRadius, fRadius, 0.0f));

		// 星の背景の大きさを設定
		m_apStarBG[nCntStar]->SetScaling(D3DXVECTOR3(fRadius, fRadius, 0.0f));

		// 星の枠の大きさを設定
		m_apStarFrame[nCntStar]->SetScaling(D3DXVECTOR3(fRadius, fRadius, 0.0f));
	}
}

//============================================================
//	表示位置の加算量の設定処理
//============================================================
void CGaugeStar::SetGapPosition(const D3DXVECTOR3& rGap)
{
	// 引数の表示位置の加算量を設定
	m_posGap = rGap;
}

//============================================================
//	中心からの距離の設定処理
//============================================================
void CGaugeStar::SetDistance(const float fDistance)
{
	// 引数の中心からの距離を設定
	m_fDistance = fDistance;
}

//============================================================
//	揺らめき量の設定処理
//============================================================
void CGaugeStar::SetFlicker(const float fFlicker)
{
	// 引数の揺らめき量を設定
	m_fFlicker = fFlicker;
}

//============================================================
//	描画情報の設定処理
//============================================================
HRESULT CGaugeStar::SetDrawInfo(void)
{
	// TODO：ポインタの確認もっときれいに
	if (CObject::CheckUse(m_pParentObject))
	{ // 親オブジェクトが使用されていた場合

		// 変数を宣言
		D3DXVECTOR3 posParent = m_pParentObject->GetPosition();		// 親オブジェクト位置
		D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
		float fPartRot = (D3DX_PI * 2) / MAX_STAR;	// 分割向き

		// 星の描画情報を設定
		for (int nCntStar = 0; nCntStar < MAX_STAR; nCntStar++)
		{ // 星の最大数分繰り返す

			// 変数を宣言
			D3DXVECTOR3 posStar = posParent + m_posGap;	// 表示位置
			float fRot = (fPartRot * nCntStar);			// 表示方向

			// カメラの向きを加算
			fRot += CManager::GetCamera()->GetRotation().y;
			useful::NormalizeRot(fRot);	// 向きを補正

			// 消費中の星を正面にする
			fRot -= m_fAddRot;
			useful::NormalizeRot(fRot);	// 向きを補正

			// 表示位置に加算
			posStar.x += sinf(fRot) * (m_fDistance + sinf(m_fSinRot) * m_fFlicker);
			posStar.z += cosf(fRot) * (m_fDistance + sinf(m_fSinRot) * m_fFlicker);

			// 星の位置設定
			m_aStar[nCntStar].pBillboard->SetPosition(posStar);
			m_apStarBG[nCntStar]->SetPosition(posStar);
			m_apStarFrame[nCntStar]->SetPosition(posStar);
		}

		// 成功を返す
		return S_OK;
	}
	else
	{ // 親オブジェクトが使用されていなかった場合

		// 失敗を返す
		return E_FAIL;
	}
}

//============================================================
//	親オブジェクトの設定処理
//============================================================
void CGaugeStar::SetParentObject(CObject *pObject)
{
	// 引数の親オブジェクトを設定
	m_pParentObject = pObject;
}
