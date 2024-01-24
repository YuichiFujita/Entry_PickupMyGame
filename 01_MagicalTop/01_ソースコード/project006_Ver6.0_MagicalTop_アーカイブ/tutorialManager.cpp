//============================================================
//
//	チュートリアルマネージャー処理 [tutorialManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "tutorialManager.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "letterManager.h"
#include "object2D.h"
#include "objectGauge2D.h"
#include "player.h"
#include "enemy.h"

//************************************************************
//	マクロ定義
//************************************************************
#define TUTORIAL_PRIO	(6)	// チュートリアルの優先順位

#define LETTER_WAIT_CNT	(90)	// 待機状態の待機フレーム
#define TITLE_WAIT_CNT	(30)	// タイトル遷移の余韻フレーム

#define POS_GUIDE		(D3DXVECTOR3(1020.0f, 585.0f, 0.0f))	// 小説明表示の位置
#define POS_CONTROL		(D3DXVECTOR3(260.0f, 585.0f, 0.0f))		// 操作方法表示の位置

#define SIZE_PROGRESSION_UI		(D3DXVECTOR3(500.0f, 250.0f, 0.0f))	// チュートリアル進行時のUI表示の大きさ
#define COL_PROGRESSION_UI_BG	(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.6f))	// チュートリアル進行時のUI表示の背景色

#define FRAME_COUNTER		(10)	// ゲージの変動フレーム
#define SIZE_COUNTER		(D3DXVECTOR3(SIZE_PROGRESSION_UI.x * 0.5f, 8.0f, 0.0f))	// ゲージの大きさ
#define POS_COUNTER			(D3DXVECTOR3(POS_GUIDE.x, POS_GUIDE.y - (SIZE_PROGRESSION_UI.y * 0.5f) - SIZE_COUNTER.y, 0.0f))	// ゲージの位置
#define COL_FRONT_COUNTER	(XCOL_WHITE)	// ゲージの表ゲージ色
#define COL_BACK_COUNTER	(XCOL_AWHITE)	// ゲージの裏ゲージ色

#define COL_FADE_MIN	(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f))	// フェードのα値の最小
#define COL_FADE_MAX	(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.8f))	// フェードのα値の最大
#define FADE_LEVEL		(0.025f)	// フェードのα値の加減量

#define POS_EXPLAIN		(D3DXVECTOR3(SCREEN_CENT.x, 315.0f, 0.0f))	// 説明表示の位置
#define SIZE_EXPLAIN	(D3DXVECTOR3(1000.0f, 597.5f, 0.0f))		// 説明表示の大きさ
#define SET_SCALE		(0.05f)		// 拡大率の初期値
#define ADD_SCALE		(0.1f)		// 拡大率の加算量

#define POS_CLOSE	(D3DXVECTOR3(SCREEN_CENT.x, 665.0f, 0.0f))	// 操作方法表示の位置
#define SIZE_CLOSE	(D3DXVECTOR3(SCREEN_WIDTH, 100.0f, 0.0f))	// 操作方法表示の大きさ
#define ADD_ROT		(0.02f)		// 点滅向きの加算量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CTutorialManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\letter001.png",	// 操作方法表示のテクスチャ
	"data\\TEXTURE\\letter002.png",	// 開始時の便箋テクスチャ
	"data\\TEXTURE\\letter003.png",	// 終了時の便箋テクスチャ
};

const char *CTutorialManager::mc_apLessonTextureFile[] =	// レッスンテクスチャ定数
{
	NULL,	// レッスン00：テクスチャなし
	"data\\TEXTURE\\lesson000.png",	// レッスン01：吸い込まれる	説明テクスチャ
	"data\\TEXTURE\\lesson001.png",	// レッスン02：前後加速		説明テクスチャ
	"data\\TEXTURE\\lesson002.png",	// レッスン03：左右加速		説明テクスチャ
	"data\\TEXTURE\\lesson003.png",	// レッスン04：攻撃			説明テクスチャ
	"data\\TEXTURE\\lesson004.png",	// レッスン05：マナ回復		説明テクスチャ
	"data\\TEXTURE\\lesson005.png",	// レッスン06：敵への攻撃	説明テクスチャ
};

const char *CTutorialManager::mc_apGuideTextureFile[] =	// 小説明テクスチャ定数
{
	NULL,	// レッスン00：テクスチャなし
	"data\\TEXTURE\\guide000.png",	// レッスン01：吸い込まれる	小説明のテクスチャ
	"data\\TEXTURE\\guide001.png",	// レッスン02：前後加速		小説明のテクスチャ
	"data\\TEXTURE\\guide002.png",	// レッスン03：左右加速		小説明のテクスチャ
	"data\\TEXTURE\\guide003.png",	// レッスン04：攻撃			小説明のテクスチャ
	"data\\TEXTURE\\guide004.png",	// レッスン05：マナ回復		小説明のテクスチャ
	"data\\TEXTURE\\guide005.png",	// レッスン06：敵への攻撃	小説明のテクスチャ
};

const char *CTutorialManager::mc_apControlTextureFile[] =	// 操作方法テクスチャ定数
{
	NULL,	// レッスン00：テクスチャなし
	NULL,	// レッスン01：テクスチャなし
	"data\\TEXTURE\\control000.png",	// レッスン02：前後加速	操作方法のテクスチャ
	"data\\TEXTURE\\control001.png",	// レッスン03：左右加速	操作方法のテクスチャ
	"data\\TEXTURE\\control002.png",	// レッスン04：攻撃		操作方法のテクスチャ
	NULL,	// レッスン05：テクスチャなし
	NULL,	// レッスン06：テクスチャなし
};

const int CTutorialManager::mc_aNextLesson[] =	// レッスン移行カウント
{
	0,		// レッスンなし
	30,		// レッスン01：吸い込まれる	終了カウント
	320,	// レッスン02：前後加速		終了カウント
	320,	// レッスン03：左右加速		終了カウント
	240,	// レッスン04：攻撃			終了カウント
	70,		// レッスン05：マナ回復		終了カウント
	3,		// レッスン06：敵への攻撃	終了カウント
};

const int CTutorialManager::mc_aNextLessonWait[] =	// 次レッスン余韻カウント
{
	0,		// レッスンなし
	60,		// レッスン01：吸い込まれる	終了時の余韻カウント
	90,		// レッスン02：前後加速		終了時の余韻カウント
	90,		// レッスン03：左右加速		終了時の余韻カウント
	90,		// レッスン04：攻撃			終了時の余韻カウント
	120,	// レッスン05：マナ回復		終了時の余韻カウント
	90,		// レッスン06：敵への攻撃	終了時の余韻カウント
};

//************************************************************
//	親クラス [CTutorialManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTutorialManager::CTutorialManager()
{
	// メンバ変数をクリア
	m_pLetterManager	= NULL;			// 手紙マネージャーの情報
	m_pCounterLesson	= NULL;			// レッスン管理カウンターの情報
	m_pFade				= NULL;			// フェードの情報
	m_pExplain			= NULL;			// 説明表示の情報
	m_pClose			= NULL;			// 説明終了表示の情報
	m_pGuide			= NULL;			// 小説明表示の情報
	m_pGuideBG			= NULL;			// 小説明表示の背景情報
	m_pControl			= NULL;			// 操作方法表示の情報
	m_pControlBG		= NULL;			// 操作方法表示の背景情報
	m_state				= STATE_NONE;	// 状態
	m_nLesson			= 0;			// レッスン
	m_fScale			= 0.0f;			// ポリゴン拡大率
	m_fSinRot			= 0.0f;			// ポリゴン点滅向き
	m_nCounterState		= 0;			// 状態管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CTutorialManager::~CTutorialManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CTutorialManager::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_pLetterManager	= NULL;			// 手紙マネージャーの情報
	m_pCounterLesson	= NULL;			// レッスン管理カウンターの情報
	m_pFade				= NULL;			// フェードの情報
	m_pExplain			= NULL;			// 説明表示の情報
	m_pClose			= NULL;			// 説明終了表示の情報
	m_pGuide			= NULL;			// 小説明表示の情報
	m_pGuideBG			= NULL;			// 小説明表示の背景情報
	m_pControl			= NULL;			// 操作方法表示の情報
	m_pControlBG		= NULL;			// 操作方法表示の背景情報
	m_state				= STATE_WAIT;	// 状態
	m_nLesson			= LESSON_NONE;	// レッスン
	m_fScale			= 0.0f;			// ポリゴン拡大率
	m_fSinRot			= -HALF_PI;		// ポリゴン点滅向き
	m_nCounterState		= 0;			// 状態管理カウンター

	//--------------------------------------------------------
	//	レッスン管理カウンターの生成・設定
	//--------------------------------------------------------
	// レッスン管理カウンターの生成
	m_pCounterLesson = CObjectGauge2D::Create
	( // 引数
		CObject::LABEL_GAUGE,	// オブジェクトラベル
		mc_aNextLesson[0],		// 最大表示値
		FRAME_COUNTER,			// 表示値変動フレーム
		POS_COUNTER,			// 位置
		SIZE_COUNTER,			// ゲージ大きさ
		COL_FRONT_COUNTER,		// 表ゲージ色
		COL_BACK_COUNTER		// 裏ゲージ色
	);
	if (UNUSED(m_pCounterLesson))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pCounterLesson->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	小説明表示の背景生成・設定
	//--------------------------------------------------------
	// 小説明表示の背景生成
	m_pGuideBG = CObject2D::Create
	( // 引数
		POS_GUIDE,				// 位置
		SIZE_PROGRESSION_UI,	// 大きさ
		VEC3_ZERO,				// 向き
		COL_PROGRESSION_UI_BG	// 色
	);
	if (UNUSED(m_pGuideBG))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pGuideBG->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	小説明表示の生成・設定
	//--------------------------------------------------------
	// 小説明表示の生成
	m_pGuide = CObject2D::Create
	( // 引数
		POS_GUIDE,			// 位置
		SIZE_PROGRESSION_UI	// 大きさ
	);
	if (UNUSED(m_pGuide))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pGuide->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	操作方法表示の背景生成・設定
	//--------------------------------------------------------
	// 操作方法表示の背景生成
	m_pControlBG = CObject2D::Create
	( // 引数
		POS_CONTROL,			// 位置
		SIZE_PROGRESSION_UI,	// 大きさ
		VEC3_ZERO,				// 向き
		COL_PROGRESSION_UI_BG	// 色
	);
	if (UNUSED(m_pControlBG))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pControlBG->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	操作方法表示の生成・設定
	//--------------------------------------------------------
	// 操作方法表示の生成
	m_pControl = CObject2D::Create
	( // 引数
		POS_CONTROL,		// 位置
		SIZE_PROGRESSION_UI	// 大きさ
	);
	if (UNUSED(m_pControl))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pControl->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	フェードの生成・設定
	//--------------------------------------------------------
	// フェードの生成
	m_pFade = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		COL_FADE_MIN	// 色
	);
	if (UNUSED(m_pFade))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFade->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	選択背景の生成・設定
	//--------------------------------------------------------
	// 選択背景の生成
	m_pExplain = CObject2D::Create
	( // 引数
		POS_EXPLAIN,	// 位置
		SIZE_EXPLAIN * SET_SCALE	// 大きさ
	);
	if (UNUSED(m_pExplain))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pExplain->SetPriority(TUTORIAL_PRIO);

	// 描画を停止
	m_pExplain->SetEnableDraw(false);

	//--------------------------------------------------------
	//	説明終了表示の生成・設定
	//--------------------------------------------------------
	// 説明終了表示の生成
	m_pClose = CObject2D::Create
	( // 引数
		POS_CLOSE,	// 位置
		SIZE_CLOSE,	// 大きさ
		VEC3_ZERO,	// 向き
		XCOL_AWHITE	// 色
	);
	if (UNUSED(m_pClose))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pClose->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_CONTROL]));

	// 優先順位を設定
	m_pClose->SetPriority(TUTORIAL_PRIO);

	//--------------------------------------------------------
	//	手紙マネージャーの生成・設定
	//--------------------------------------------------------
	// 手紙マネージャーの生成
	m_pLetterManager = CLetterManager::Create();
	if (UNUSED(m_pLetterManager))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 進行状態時に表示するUIの描画を停止
	SetEnableProgressionDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
HRESULT CTutorialManager::Uninit(void)
{
	// 手紙マネージャーの破棄
	if (FAILED(CLetterManager::Release(m_pLetterManager)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 小説明表示の背景終了
	m_pGuideBG->Uninit();

	// 小説明表示の終了
	m_pGuide->Uninit();

	// 操作方法表示の背景終了
	m_pControlBG->Uninit();

	// 操作方法表示の終了
	m_pControl->Uninit();

	// フェードの終了
	m_pFade->Uninit();

	// 選択背景の終了
	m_pExplain->Uninit();

	// 説明終了の終了
	m_pClose->Uninit();

	// 成功を返す
	return S_OK;
}

//============================================================
//	更新処理
//============================================================
void CTutorialManager::Update(void)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:		// 何もしない状態

		// 無し

		break;

	case STATE_WAIT:		// 待機状態

		// 待機の更新
		UpdateWait();

		break;

	case STATE_LETTER:		// 手紙表示状態

		// 手紙表示の更新
		UpdateLetter();

		break;

	case STATE_FADEIN:		// フェードイン状態

		// フェードインの更新
		UpdateFadeIn();

		break;

	case STATE_EXPLAIN:		// 説明表示状態

		// 説明表示の更新
		UpdateExplain();

		break;

	case STATE_FADEWAIT:	// フェード待機状態

		// フェード待機の更新
		UpdateFadeWait();

		break;

	case STATE_FADEOUT:		// フェードアウト状態

		// フェードアウトの更新
		UpdateFadeOut();

		break;

	case STATE_PROGRESSION:	// 進行状態

		// 無し

		break;

	case STATE_NEXTWAIT:	// 次レッスン待機状態

		// 次レッスン待機の更新
		UpdateNextWait();

		break;

	case STATE_END:			// 終了状態

		// シーンの設定
		CManager::SetScene(CScene::MODE_TITLE, TITLE_WAIT_CNT);	// タイトル画面

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (USED(m_pLetterManager))
	{ // 使用中の場合

		// 手紙マネージャーの更新
		m_pLetterManager->Update();
	}
	else { assert(false); }	// 非使用中

	// 小説明表示の背景更新
	m_pGuideBG->Update();

	// 小説明表示の更新
	m_pGuide->Update();

	// 操作方法表示の背景更新
	m_pControlBG->Update();

	// 操作方法表示の更新
	m_pControl->Update();

	// フェードの更新
	m_pFade->Update();

	// 選択背景の更新
	m_pExplain->Update();

	// 説明終了の更新
	m_pClose->Update();
}

//============================================================
//	レッスンカウンターの加算処理
//============================================================
void CTutorialManager::AddLessonCounter(void)
{
	if (m_nLesson < LESSON_MAX && m_state != STATE_NEXTWAIT)
	{ // レッスンがまだある且つ、次レッスンの待機中ではない場合

		// レッスンカウンターを加算
		m_pCounterLesson->AddNum(1);

		if (m_pCounterLesson->GetNum() >= mc_aNextLesson[m_nLesson])
		{ // レッスンを次に進めるカウントまで到達した場合

			// 状態を設定
			m_state = STATE_NEXTWAIT;	// 次レッスン待機状態

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
	}
}

//============================================================
//	レッスン取得処理
//============================================================
int CTutorialManager::GetLesson(void) const
{
	// 現在のレッスンを返す
	return m_nLesson;
}

//============================================================
//	状態取得処理
//============================================================
CTutorialManager::STATE CTutorialManager::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	生成処理
//============================================================
CTutorialManager *CTutorialManager::Create(void)
{
	// ポインタを宣言
	CTutorialManager *pTutorialManager = NULL;	// チュートリアルマネージャー生成用

	if (UNUSED(pTutorialManager))
	{ // 使用されていない場合

		// メモリ確保
		pTutorialManager = new CTutorialManager;	// チュートリアルマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pTutorialManager))
	{ // 使用されている場合
		
		// チュートリアルマネージャーの初期化
		if (FAILED(pTutorialManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pTutorialManager;
			pTutorialManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pTutorialManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CTutorialManager::Release(CTutorialManager *&prTutorialManager)
{
	if (USED(prTutorialManager))
	{ // 使用中の場合

		// チュートリアルマネージャーの終了
		if (FAILED(prTutorialManager->Uninit()))
		{ // 破棄に失敗した場合

			// メモリ開放
			delete prTutorialManager;
			prTutorialManager = NULL;

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// メモリ開放
		delete prTutorialManager;
		prTutorialManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	レッスン移行カウント取得処理
//============================================================
int CTutorialManager::GetNextLessonCounter(const int nID)
{
	if (nID < LESSON_MAX)
	{ // インデックスが範囲内の場合

		// 引数のインデックスのレッスン移行カウントを返す
		return mc_aNextLesson[nID];
	}
	else { assert(false); return NONE_IDX; }	// 範囲外
}

//============================================================
//	進行状態時に表示するUIの描画状況の設定処理
//============================================================
void CTutorialManager::SetEnableProgressionDraw(const bool bDraw)
{
	//--------------------------------------------------------
	//	引数の描画状況を設定
	//--------------------------------------------------------
	// レッスン管理カウンターの描画状況を設定
	m_pCounterLesson->SetEnableDraw(bDraw);

	// 小説明表示の背景描画状況を設定
	m_pGuideBG->SetEnableDraw(bDraw);

	// 小説明表示の描画状況を設定
	m_pGuide->SetEnableDraw(bDraw);

	//--------------------------------------------------------
	//	描画状況を状況に応じて変更して設定
	//--------------------------------------------------------
	// 変数を宣言
	bool bDrawControl = bDraw;	// 操作方法の表示状況

	if (m_nLesson != LESSON_02	// レッスン02：前後加速
	&&  m_nLesson != LESSON_03	// レッスン03：左右加速
	&&  m_nLesson != LESSON_04)	// レッスン04：攻撃
	{ // 上記のレッスンのいずれでもない場合

		// 操作方法を表示しない状態にする
		bDrawControl = false;
	}

	// 操作方法表示の背景描画状況を設定
	m_pControlBG->SetEnableDraw(bDrawControl);

	// 操作方法表示の描画状況を設定
	m_pControl->SetEnableDraw(bDrawControl);
}

//============================================================
//	待機の更新処理
//============================================================
void CTutorialManager::UpdateWait(void)
{
	if (m_nCounterState < LETTER_WAIT_CNT)
	{ // カウンターが待機時間未満の場合

		// カウンターを加算
		m_nCounterState++;
	}
	else
	{ // 待機時間が終了した場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 手紙表示の開始
		m_pLetterManager->SetLook(mc_apTextureFile[TEXTURE_START]);

		// 状態を設定
		m_state = STATE_LETTER;	// 手紙表示状態
	}
}

//============================================================
//	手紙表示の更新処理
//============================================================
void CTutorialManager::UpdateLetter(void)
{
	if (m_pLetterManager->GetState() == CLetterManager::STATE_END)
	{ // 手紙を読み終えた場合

		if (m_nLesson == LESSON_NONE)
		{ // レッスン開始前の場合

			// 次レッスンへの移行
			NextLesson();
		}
		else
		{ // レッスン終了後の場合

			// 状態を設定
			m_state = STATE_END;	// 終了状態
		}
	}
}

//============================================================
//	フェードインの更新処理
//============================================================
void CTutorialManager::UpdateFadeIn(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	if (colFade.a < COL_FADE_MAX.a)
	{ // 透明量が設定値未満の場合

		// 透明度を加算
		colFade.a += FADE_LEVEL;
	}
	else
	{ // 透明量が設定値以上の場合

		// 透明度を補正
		colFade.a = COL_FADE_MAX.a;

		// 説明表示の描画を再開
		m_pExplain->SetEnableDraw(true);

		// 説明表示の初期ポリゴン拡大率を設定
		m_fScale = SET_SCALE;

		// 状態を設定
		m_state = STATE_EXPLAIN;	// 説明表示状態

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_PAPER);	// 便箋めくり音
	}

	// 透明度を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	説明表示の更新処理
//============================================================
void CTutorialManager::UpdateExplain(void)
{
	if (m_fScale < 1.0f)
	{ // 拡大率が最小値未満の場合

		// 拡大率を加算
		m_fScale += ADD_SCALE;

		// 説明表示の大きさを設定
		m_pExplain->SetScaling(SIZE_EXPLAIN * m_fScale);
	}
	else
	{ // 拡大率が最小値以下の場合

		// 説明表示の大きさを設定
		m_pExplain->SetScaling(SIZE_EXPLAIN);

		// 状態を設定
		m_state = STATE_FADEWAIT;	// フェード待機状態
	}
}

//============================================================
//	フェード待機の更新処理
//============================================================
void CTutorialManager::UpdateFadeWait(void)
{
	// 変数を宣言
	D3DXCOLOR colControl = m_pClose->GetColor();	// 説明終了表示の色

	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetPad();		// パッド

	if (pKeyboard->GetTrigger(DIK_RETURN)
	||  pKeyboard->GetTrigger(DIK_SPACE)
	||  pPad->GetTrigger(CInputPad::KEY_A)
	||  pPad->GetTrigger(CInputPad::KEY_B)
	||  pPad->GetTrigger(CInputPad::KEY_X)
	||  pPad->GetTrigger(CInputPad::KEY_Y)
	||  pPad->GetTrigger(CInputPad::KEY_START))
	{
		// ポリゴン点滅向きを初期化
		m_fSinRot = -HALF_PI;

		// 説明表示の初期ポリゴン拡大率を設定
		m_fScale = SET_SCALE;

		// 説明表示の大きさを設定
		m_pExplain->SetScaling(SIZE_EXPLAIN * m_fScale);

		// 説明表示の描画を停止
		m_pExplain->SetEnableDraw(false);

		// 状態を設定
		m_state = STATE_FADEOUT;	// フェードアウト状態
	}

	// 説明終了表示の点滅向きを加算
	m_fSinRot += ADD_ROT;
	useful::NormalizeRot(m_fSinRot);	// 向きを補正

	// 説明終了表示のα値を変更
	colControl.a = (1.0f / 2.0f) * (sinf(m_fSinRot) + 1.0f);

	// 説明終了表示の色を反映
	m_pClose->SetColor(colControl);
}

//============================================================
//	フェードアウトの更新処理
//============================================================
void CTutorialManager::UpdateFadeOut(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	if (colFade.a > COL_FADE_MIN.a)
	{ // 透明量が設定値より大きい場合

		// 透明度を減算
		colFade.a -= FADE_LEVEL;
	}
	else
	{ // 透明量が設定値以下の場合

		// 透明度を補正
		colFade.a = COL_FADE_MIN.a;

		// プレイヤーを再出現させる
		CScene::GetPlayer()->SetRespawn(PLAY_SPAWN_POS);

		switch (m_nLesson)
		{ // レッスンごとの処理
		case LESSON_05:	// レッスン05：マナ回復

			// マナの回復をできるように変更
			CScene::GetPlayer()->SetEnableHealMana(true);

			// マナを空にする
			CScene::GetPlayer()->SetMana(0);

			break;

		case LESSON_06:	// レッスン06：敵への攻撃

			// 敵ランダム生成
			CEnemy::RandomSpawn(mc_aNextLesson[LESSON_06], CEnemy::TYPE_HUMAN);

			break;
		}

		// 進行状態時に表示するUIの描画を再開
		SetEnableProgressionDraw(true);

		// 状態を設定
		m_state = STATE_PROGRESSION;	// 進行状態
	}

	// 透明度を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	次レッスン待機の更新処理
//============================================================
void CTutorialManager::UpdateNextWait(void)
{
	if (m_nCounterState < mc_aNextLessonWait[m_nLesson])
	{ // カウンターが余韻時間未満の場合

		// カウンターを加算
		m_nCounterState++;
	}
	else
	{ // 余韻時間が終了した場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 次レッスンへの移行
		NextLesson();

		// 進行状態時に表示するUIの描画を停止
		SetEnableProgressionDraw(false);
	}
}

//============================================================
//	次レッスンへの移行処理
//============================================================
void CTutorialManager::NextLesson(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	//--------------------------------------------------------
	//	レッスン・カウンターを更新
	//--------------------------------------------------------
	// レッスンを次に進める
	m_nLesson++;

	// レッスンカウンターの最大値を設定
	m_pCounterLesson->SetMaxNum(mc_aNextLesson[m_nLesson]);

	// レッスンカウンターを初期化
	m_pCounterLesson->SetNum(0);

	//--------------------------------------------------------
	//	レッスン表示を更新
	//--------------------------------------------------------
	// レッスン説明テクスチャを登録・割当
	m_pExplain->BindTexture(pTexture->Regist(mc_apLessonTextureFile[m_nLesson]));

	// レッスン小説明テクスチャを登録・割当
	m_pGuide->BindTexture(pTexture->Regist(mc_apGuideTextureFile[m_nLesson]));

	// レッスン操作方法テクスチャを登録・割当
	m_pControl->BindTexture(pTexture->Regist(mc_apControlTextureFile[m_nLesson]));

	//--------------------------------------------------------
	//	状態を更新
	//--------------------------------------------------------
	// プレイヤーを見えなくする
	CScene::GetPlayer()->SetDisp(false);

	if (m_nLesson < LESSON_MAX)
	{ // レッスンがまだある場合

		// 状態を設定
		m_state = STATE_FADEIN;	// フェードイン状態
	}
	else
	{ // レッスンがもうない場合

		// 手紙表示の開始
		m_pLetterManager->SetLook(mc_apTextureFile[TEXTURE_END]);

		// 状態を設定
		m_state = STATE_LETTER;	// 手紙表示状態
	}
}
