//============================================================
//
//	スキップ操作処理 [skip.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "skip.h"
#include "renderer.h"
#include "manager.h"
#include "sound.h"
#include "fade.h"
#include "texture.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE = "data\\TEXTURE\\skip000.png";	// スキップ操作表示テクスチャ
	const int	PRIORITY	 = 6;		// スキップ操作表示の優先順位
	const int	DISP_FRAME	 = 240;		// 完全不透明フレーム
	const float	FADE_LEVEL	 = 0.05f;	// 透明度のフレーム変動量
}

//************************************************************
//	子クラス [CSkip] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSkip::CSkip() : 
	m_pControl	(nullptr),		// 操作の情報
	m_state		(STATE_NONE),	// 状態
	m_nCounter	(0)				// 状態管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CSkip::~CSkip()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSkip::Init(void)
{
	// メンバ変数を初期化
	m_pControl	= nullptr;		// 操作の情報
	m_state		= STATE_NONE;	// 状態
	m_nCounter	= 0;			// 状態管理カウンター

	// 操作情報の生成
	m_pControl = CObject2D::Create
	( // 引数
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

	// テクスチャを登録・割当
	m_pControl->BindTexture(TEXTURE_FILE);

	// 優先順位を設定
	m_pControl->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSkip::Uninit(void)
{
	// 操作情報の終了
	SAFE_UNINIT(m_pControl);
}

//============================================================
//	更新処理
//============================================================
void CSkip::Update(void)
{
	if (m_state != STATE_NONE)
	{ // 何もしない状態ではない場合

		D3DXCOLOR colCont = m_pControl->GetColor();	// 操作表示の色

		switch (m_state)
		{ // 状態ごとの処理
		case STATE_FADEOUT:

			// α値を加算
			colCont.a += FADE_LEVEL;
			if (colCont.a >= 1.0f)
			{ // 不透明になった場合

				// 透明度を補正
				colCont.a = 1.0f;

				// 表示状態にする
				m_state = STATE_DISP;
			}

			break;

		case STATE_DISP:

			// カウンターを加算
			m_nCounter++;
			if (m_nCounter > DISP_FRAME)
			{ // 表示時間を超えた場合

				// カウンター初期化
				m_nCounter = 0;

				// フェードイン状態にする
				m_state = STATE_FADEIN;
			}

			break;

		case STATE_FADEIN:

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

		default:
			assert(false);
			break;
		}

		// 色を反映
		m_pControl->SetColor(colCont);
	}

	// 操作情報の更新
	m_pControl->Update();
}

//============================================================
//	描画処理
//============================================================
void CSkip::Draw(void)
{

}

//============================================================
//	表示設定処理
//============================================================
void CSkip::SetDisp(void)
{
	// 既に表示中の場合抜ける
	if (m_state == STATE_DISP) { return; }

	// カウンターを初期化
	m_nCounter = 0;

	// フェードアウト状態にする
	m_state = STATE_FADEOUT;
}

//============================================================
//	表示取得処理
//============================================================
bool CSkip::IsDisp(void)
{
	// 表示状況を設定
	bool bDisp = (m_state == STATE_DISP);

	// 表示状況を返す
	return bDisp;
}

//============================================================
//	生成処理
//============================================================
CSkip *CSkip::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize	// 大きさ
)
{
	// スキップ操作の生成
	CSkip *pSkip = new CSkip;
	if (pSkip == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// スキップ操作の初期化
		if (FAILED(pSkip->Init()))
		{ // 初期化に失敗した場合

			// スキップ操作の破棄
			SAFE_DELETE(pSkip);
			return nullptr;
		}

		// 位置を設定
		pSkip->m_pControl->SetVec3Position(rPos);

		// 大きさを設定
		pSkip->m_pControl->SetVec3Sizing(rSize);

		// 確保したアドレスを返す
		return pSkip;
	}
}

//============================================================
//	破棄処理
//============================================================
void CSkip::Release(CSkip *&prSkip)
{
	// スキップ操作の終了
	assert(prSkip != nullptr);
	prSkip->Uninit();

	// メモリ開放
	SAFE_DELETE(prSkip);
}
