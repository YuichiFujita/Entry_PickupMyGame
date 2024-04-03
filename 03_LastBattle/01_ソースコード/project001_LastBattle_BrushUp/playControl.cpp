//============================================================
//
//	プレイ操作処理 [playControl.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "playControl.h"
#include "renderer.h"
#include "manager.h"
#include "sound.h"
#include "fade.h"
#include "texture.h"
#include "anim2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEX_CONTROL_FILE[] =	// 操作テクスチャファイル
	{
		nullptr,								// 操作なし
		"data\\TEXTURE\\playControl000.png",	// 騎乗操作テクスチャ
		"data\\TEXTURE\\playControl001.png",	// 連続攻撃操作テクスチャ
	};

	const POSGRID2 PART = POSGRID2(1, 2);	// テクスチャ分割数
	const int	PRIORITY		= 6;		// プレイ操作表示の優先順位
	const int	SCALEIN_FRAME	= 8;		// 表示開始から完了までのフレーム数
	const int	BLINK_FRAME		= 12;		// 点滅フレーム
	const float	MUL_SCALEPLUS	= 0.5f;		// 拡大率加算量の倍率
	const float	ORIGIN_SCALE	= 1.0f;		// 基礎拡大率
	const float	ADD_SINROT		= 0.035f;	// 拡大率を上下させるサインカーブ向き加算量
	const float	MAX_ADD_SCALE	= 0.08f;	// 拡大率の最大加算量
	const float	FADE_LEVEL		= 0.05f;	// 透明度のフレーム変動量
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEX_CONTROL_FILE) == CPlayControl::CONTROL_MAX, "ERROR : ControlTexture Count Mismatch");

//************************************************************
//	子クラス [CPlayControl] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayControl::CPlayControl() : 
	m_pControl		(nullptr),		// 操作の情報
	m_originSize	(VEC3_ZERO),	// 操作の元の大きさ
	m_contType		(CONTROL_NONE),	// 表示操作
	m_dispType		(DISP_NORMAL),	// 表示形式
	m_state			(STATE_NONE),	// 状態
	m_nCounter		(0),			// 状態管理カウンター
	m_fSinScale		(0.0f),			// 拡縮向き
	m_fScale		(0.0f)			// 拡大率
{

}

//============================================================
//	デストラクタ
//============================================================
CPlayControl::~CPlayControl()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayControl::Init(void)
{
	// メンバ変数を初期化
	m_pControl		= nullptr;		// 操作の情報
	m_originSize	= VEC3_ZERO;	// 操作の元の大きさ
	m_contType		= CONTROL_NONE;	// 表示操作
	m_dispType		= DISP_NORMAL;	// 表示形式
	m_state			= STATE_NONE;	// 状態
	m_nCounter		= 0;			// 状態管理カウンター
	m_fSinScale		= 0.0f;			// 拡縮向き
	m_fScale		= 0.0f;			// 拡大率

	// 操作情報の生成
	m_pControl = CAnim2D::Create
	( // 引数
		PART.x,		// テクスチャ横分割数
		PART.y,		// テクスチャ縦分割数
		VEC3_ZERO,	// 位置
		VEC3_ZERO,	// 大きさ
		VEC3_ZERO,	// 向き
		XCOL_AWHITE	// 色
	);
	if (m_pControl == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pControl->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayControl::Uninit(void)
{
	// 操作情報の終了
	SAFE_UNINIT(m_pControl);
}

//============================================================
//	更新処理
//============================================================
void CPlayControl::Update(void)
{
	if (m_state != STATE_NONE)
	{ // 何もしない状態ではない場合

		D3DXVECTOR3 sizeCont = m_pControl->GetVec3Sizing();	// 操作表示の大きさ
		D3DXCOLOR colCont = m_pControl->GetColor();			// 操作表示の色

		switch (m_state)
		{ // 状態ごとの処理
		case STATE_SCALEIN:
		{
			// 拡大率を設定
			m_fScale = (easeing::OutCubic((SCALEIN_FRAME - m_nCounter), 0, SCALEIN_FRAME) * MUL_SCALEPLUS) + 1.0f;

			// 操作表示の大きさを設定
			sizeCont = m_originSize * m_fScale;

			// カウンターを加算
			m_nCounter++;
			if (m_nCounter > SCALEIN_FRAME)
			{ // 表示時間を超えた場合

				// カウンター初期化
				m_nCounter = 0;

				// 表示状態にする
				m_state = STATE_DISP;
			}

			break;
		}
		case STATE_DISP:
		{
			float fAddScale = 0.0f;	// 拡大率の加算量

			// サインカーブ向きを回転
			m_fSinScale += ADD_SINROT;
			useful::NormalizeRot(m_fSinScale);	// 向き正規化

			// 拡大率加算量を求める
			fAddScale = (MAX_ADD_SCALE / 2.0f) * (sinf(m_fSinScale) - 1.0f);

			// 拡大率を設定
			m_fScale = ORIGIN_SCALE + fAddScale;

			// 操作表示の大きさを設定
			sizeCont = m_originSize * m_fScale;

			break;
		}
		case STATE_FADEOUT:
		{
			// α値を減算
			colCont.a -= FADE_LEVEL;
			if (colCont.a <= 0.0f)
			{ // 透明になった場合

				// 透明度を補正
				colCont.a = 0.0f;

				// 何もしない状態にする
				m_state = STATE_NONE;
			}

			break;
		}
		default:
			assert(false);
			break;
		}

		// 大きさ・色を反映
		m_pControl->SetVec3Sizing(sizeCont);
		m_pControl->SetColor(colCont);
	}

	// 操作情報の更新
	m_pControl->Update();
}

//============================================================
//	描画処理
//============================================================
void CPlayControl::Draw(void)
{

}

//============================================================
//	表示設定処理
//============================================================
void CPlayControl::SetDisp
(
	EControl contType,	// 表示操作
	EDisp dispType		// 表示形式
)
{
	// 既に表示中の場合抜ける
	if (m_state == STATE_DISP) { return; }

	// 表示操作を設定
	m_contType = contType;
	m_pControl->BindTexture(TEX_CONTROL_FILE[m_contType]);

	// 表示形式を設定
	m_dispType = dispType;
	switch (m_dispType)
	{ // 表示形式ごとの処理
	case DISP_NORMAL:
		m_pControl->SetCounter(0);
		break;

	case DISP_BLINK:
		m_pControl->SetCounter(BLINK_FRAME);
		break;

	default:
		assert(false);
		break;
	}

	// 情報を初期化
	m_nCounter	= 0;
	m_fSinScale	= 0.0f;
	m_fScale	= 1.0f;

	// パターンを初期化
	m_pControl->SetPattern(0);

	// 操作表示の大きさを初期化
	D3DXVECTOR3 sizeCont = m_pControl->GetVec3Sizing();						// 操作表示の大きさ
	m_fScale = easeing::OutCubic(SCALEIN_FRAME, 0, SCALEIN_FRAME) + 1.0f;	// 初期拡大率を設定
	sizeCont = m_originSize * m_fScale;		// 操作表示の大きさを設定
	m_pControl->SetVec3Sizing(sizeCont);	// 大きさ反映

	// 操作表示を不透明にする
	D3DXCOLOR colCont = m_pControl->GetColor();	// 操作表示の色
	colCont.a = 1.0f;				// 不透明にする
	m_pControl->SetColor(colCont);	// 色反映

	// 表示開始状態にする
	m_state = STATE_SCALEIN;
}

//============================================================
//	非表示設定処理
//============================================================
void CPlayControl::SetHide(const bool bFlash)
{
	if (bFlash)
	{ // フラッシュがONの場合

		// パターンを進める
		m_pControl->SetPattern(1);
	}

	// 表示終了状態にする
	m_state = STATE_FADEOUT;
}

//============================================================
//	表示取得処理
//============================================================
bool CPlayControl::IsDisp(void)
{
	// 表示状況を設定
	bool bDisp = (m_state == STATE_DISP || m_state == STATE_SCALEIN);

	// 表示状況を返す
	return bDisp;
}

//============================================================
//	生成処理
//============================================================
CPlayControl *CPlayControl::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize	// 大きさ
)
{
	// プレイ操作の生成
	CPlayControl *pPlayControl = new CPlayControl;
	if (pPlayControl == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// プレイ操作の初期化
		if (FAILED(pPlayControl->Init()))
		{ // 初期化に失敗した場合

			// プレイ操作の破棄
			SAFE_DELETE(pPlayControl);
			return nullptr;
		}

		// 位置を設定
		pPlayControl->m_pControl->SetVec3Position(rPos);

		// 大きさを設定
		pPlayControl->m_pControl->SetVec3Sizing(rSize);
		pPlayControl->m_originSize = rSize;

		// 確保したアドレスを返す
		return pPlayControl;
	}
}

//============================================================
//	破棄処理
//============================================================
void CPlayControl::Release(CPlayControl *&prPlayControl)
{
	// プレイ操作の終了
	assert(prPlayControl != nullptr);
	prPlayControl->Uninit();

	// メモリ開放
	SAFE_DELETE(prPlayControl);
}
