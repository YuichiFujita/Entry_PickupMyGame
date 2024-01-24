//============================================================
//
//	手紙マネージャー処理 [letterManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "letterManager.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "object2D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define LETTER_PRIO	(6)	// 手紙の優先順位

#define SIZE_LETTER		(D3DXVECTOR3(1150.0f, 687.0f, 0.0f))		// 手紙の大きさ
#define COL_LETTER		(XCOL_AWHITE)								// 手紙の色
#define COL_FADE		(XCOL_ABLACK)								// フェードの色
#define POS_PAPER		(D3DXVECTOR3(SCREEN_CENT.x, 1020.0f, 0.0f))	// 便箋の初期座標
#define SIZE_PAPER		(D3DXVECTOR3(1000.0f, 597.5f, 0.0f))		// 便箋の大きさ
#define POS_CONTROL		(D3DXVECTOR3(SCREEN_CENT.x, 665.0f, 0.0f))	// 操作の初期座標
#define SIZE_CONTROL	(D3DXVECTOR3(SCREEN_WIDTH, 100.0f, 0.0f))	// 操作の大きさ
#define COL_CONTROL		(XCOL_AWHITE)								// 操作の色

#define LETTER_CHAN	(0.04f)		// 手紙のα値変更量
#define LETTER_STOP	(1.0f)		// 手紙の最大α値
#define FADE_CHAN	(0.02f)		// フェードのα値変更量
#define FADE_STOP	(0.6f)		// フェードの最大α値
#define PAPER_MOVE	(0.5f)		// 便箋の位置の更新量
#define PAPER_STOP	(315.0f)	// 便箋の停止Y位置

#define CONTROL_ADD_ROT		(0.02f)	// 操作説明の点滅向きの加算量
#define CONTROL_SUB_ALPHA	(0.06f)	// 操作説明のしまい時のα値の変更量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CLetterManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\letter000.png",	// 手紙のテクスチャ相対パス
	"data\\TEXTURE\\letter001.png",	// 操作表示のテクスチャ相対パス
};

//************************************************************
//	親クラス [CLetterManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CLetterManager::CLetterManager()
{
	// メンバ変数をクリア
	m_pLetter	= NULL;			// 手紙の情報
	m_pFade		= NULL;			// フェードの情報
	m_pPaper	= NULL;			// 便箋の情報
	m_pControl	= NULL;			// 操作方法の情報
	m_state		= STATE_NONE;	// 状態
	m_fMove		= 0.0f;			// 便箋の移動量
	m_fSinRot	= 0.0f;			// 操作方法の点滅向き
	m_nCounterState	= 0;		// 状態管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CLetterManager::~CLetterManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CLetterManager::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_pLetter	= NULL;			// 手紙の情報
	m_pFade		= NULL;			// フェードの情報
	m_pPaper	= NULL;			// 便箋の情報
	m_pControl	= NULL;			// 操作方法の情報
	m_state		= STATE_NONE;	// 状態
	m_fMove		= 0.0f;			// 便箋の移動量
	m_fSinRot	= -HALF_PI;		// 操作方法の点滅向き
	m_nCounterState	= 0;		// 状態管理カウンター

	//--------------------------------------------------------
	//	手紙の生成・設定
	//--------------------------------------------------------
	// 手紙の生成
	m_pLetter = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SIZE_LETTER,	// 大きさ
		VEC3_ZERO,		// 向き
		COL_LETTER		// 色
	);
	if (UNUSED(m_pLetter))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pLetter->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_LETTER]));

	// 優先順位を設定
	m_pLetter->SetPriority(LETTER_PRIO);

	//--------------------------------------------------------
	//	フェードの生成・設定
	//--------------------------------------------------------
	// フェードの生成
	m_pFade = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		COL_FADE		// 色
	);
	if (UNUSED(m_pFade))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFade->SetPriority(LETTER_PRIO);

	//--------------------------------------------------------
	//	便箋の生成・設定
	//--------------------------------------------------------
	// 便箋の生成
	m_pPaper = CObject2D::Create
	( // 引数
		POS_PAPER,	// 位置
		SIZE_PAPER	// 大きさ
	);
	if (UNUSED(m_pPaper))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pPaper->SetPriority(LETTER_PRIO);

	//--------------------------------------------------------
	//	操作方法の生成・設定
	//--------------------------------------------------------
	// 操作方法の生成
	m_pControl = CObject2D::Create
	( // 引数
		POS_CONTROL,	// 位置
		SIZE_CONTROL,	// 大きさ
		VEC3_ZERO,		// 向き
		COL_CONTROL		// 色
	);
	if (UNUSED(m_pControl))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pControl->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_CONTROL]));

	// 優先順位を設定
	m_pControl->SetPriority(LETTER_PRIO);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CLetterManager::Uninit(void)
{
	// 手紙の終了
	m_pLetter->Uninit();

	// フェードの終了
	m_pFade->Uninit();

	// 便箋の終了
	m_pPaper->Uninit();

	// 操作方法の終了
	m_pControl->Uninit();
}

//============================================================
//	更新処理
//============================================================
void CLetterManager::Update(void)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:			// 何もしない状態

		// 無し

		// 処理を抜ける
		break;

	case STATE_LETTER:			// 手紙の表示状態

		// 手紙表示の更新
		UpdateLetter();

		// 処理を抜ける
		break;

	case STATE_FADEIN:			// フェードイン状態

		// フェードインの更新
		UpdateFade();

		// 処理を抜ける
		break;

	case STATE_PAPER_TAKE:		// 便箋の取り出し状態

		// 便箋取出の更新
		UpdatePaperTake();

		// 処理を抜ける
		break;

	case STATE_WAIT:			// 待機状態

		// 待機の更新
		UpdateWait();

		// 処理を抜ける
		break;

	case STATE_PAPER_RETURN:	// 便箋のしまい状態

		// 便箋収納の更新
		UpdatePaperReturn();

		// 処理を抜ける
		break;

	case STATE_END:				// 終了状態

		// 無し

		// 処理を抜ける
		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 手紙の更新
	m_pLetter->Update();

	// フェードの更新
	m_pFade->Update();

	// 便箋の更新
	m_pPaper->Update();

	// 操作方法の更新
	m_pControl->Update();
}

//============================================================
//	手紙表示の開始処理
//============================================================
void CLetterManager::SetLook(const char *pPassTex)
{
	// 便箋の設定
	SetPaper(pPassTex);

	// 便箋の位置を初期化
	m_pPaper->SetPosition(POS_PAPER);

	// ポリゴンの色情報を初期化
	m_pLetter->SetColor(COL_LETTER);	// 手紙
	m_pFade->SetColor(COL_FADE);		// フェード
	m_pControl->SetColor(COL_CONTROL);	// 操作方法

	// 情報を初期化
	m_fMove = 0.0f;			// 便箋の移動量
	m_fSinRot = -HALF_PI;	// 操作方法の点滅向き
	m_nCounterState = 0;	// 状態管理カウンター

	// 状態を設定
	m_state = STATE_LETTER;	// 手紙の表示状態
}

//============================================================
//	便箋の設定処理
//============================================================
void CLetterManager::SetPaper(const char *pPassTex)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// 便箋のテクスチャを登録・割当
	m_pPaper->BindTexture(pTexture->Regist(pPassTex));
}

//============================================================
//	状態の設定処理
//============================================================
void CLetterManager::SetState(const STATE state)
{
	// 引数の状態を設定
	m_state = state;
}

//============================================================
//	状態取得処理
//============================================================
CLetterManager::STATE CLetterManager::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	生成処理
//============================================================
CLetterManager *CLetterManager::Create(void)
{
	// ポインタを宣言
	CLetterManager *pLetterManager = NULL;	// 手紙マネージャー生成用

	if (UNUSED(pLetterManager))
	{ // 使用されていない場合

		// メモリ確保
		pLetterManager = new CLetterManager;	// 手紙マネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pLetterManager))
	{ // 使用されている場合
		
		// 手紙マネージャーの初期化
		if (FAILED(pLetterManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pLetterManager;
			pLetterManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pLetterManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CLetterManager::Release(CLetterManager *&prLetterManager)
{
	if (USED(prLetterManager))
	{ // 使用中の場合

		// 手紙マネージャーの終了
		prLetterManager->Uninit();

		// メモリ開放
		delete prLetterManager;
		prLetterManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	手紙表示の更新処理
//============================================================
void CLetterManager::UpdateLetter(void)
{
	// 変数を宣言
	D3DXCOLOR colLetter = m_pLetter->GetColor();	// 手紙の色

	// 手紙のα値を加算
	colLetter.a += LETTER_CHAN;

	if (colLetter.a >= LETTER_STOP)
	{ // 手紙のα値が一定値以上の場合

		// 手紙のα値を補正
		colLetter.a = LETTER_STOP;

		// フェードイン状態にする
		m_state = STATE_FADEIN;
	}

	// 手紙の色を反映
	m_pLetter->SetColor(colLetter);
}

//============================================================
//	フェードインの更新処理
//============================================================
void CLetterManager::UpdateFade(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	// フェードのα値を加算
	colFade.a += FADE_CHAN;
	
	if (colFade.a >= FADE_STOP)
	{ // フェードのα値が一定値以上の場合

		// フェードのα値を補正
		colFade.a = FADE_STOP;

		// 便箋の取り出し状態にする
		m_state = STATE_PAPER_TAKE;

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_PAPER);	// 便箋めくり音
	}

	// フェードの色を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	便箋取出の更新処理
//============================================================
void CLetterManager::UpdatePaperTake(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPaper = m_pPaper->GetPosition();	// 便箋の位置

	// 便箋の位置減算量を設定
	m_fMove += PAPER_MOVE;

	// 便箋の位置を減算
	posPaper.y -= m_fMove;

	if (posPaper.y <= PAPER_STOP)
	{ // 便箋の位置が一定値以下の場合

		// 便箋の位置を補正
		posPaper.y = PAPER_STOP;

		// 便箋の移動量を初期化
		m_fMove = 0.0f;

		// 待機状態にする
		m_state = STATE_WAIT;
	}

	// 便箋の位置を反映
	m_pPaper->SetPosition(posPaper);
}

//============================================================
//	待機の更新処理
//============================================================
void CLetterManager::UpdateWait(void)
{
	// 変数を宣言
	D3DXCOLOR colControl = m_pControl->GetColor();	// 操作方法の色

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
		// 操作方法の点滅向きを初期化
		m_fSinRot = -HALF_PI;

		// 便箋のしまい状態にする
		m_state = STATE_PAPER_RETURN;

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_PAPER);	// 便箋めくり音
	}

	// 操作方法の点滅向きを加算
	m_fSinRot += CONTROL_ADD_ROT;
	useful::NormalizeRot(m_fSinRot);	// 向きを補正

	// 操作説明のα値を変更
	colControl.a = (1.0f / 2.0f) * (sinf(m_fSinRot) + 1.0f);

	// 操作方法の色を反映
	m_pControl->SetColor(colControl);
}

//============================================================
//	便箋収納の更新処理
//============================================================
void CLetterManager::UpdatePaperReturn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPaper = m_pPaper->GetPosition();	// 便箋の位置
	D3DXCOLOR colLetter = m_pLetter->GetColor();	// 手紙の色
	D3DXCOLOR colFade = m_pFade->GetColor();		// フェードの色
	D3DXCOLOR colControl = m_pControl->GetColor();	// 操作方法の色

	//--------------------------------------------------------
	//	手紙の透明化
	//--------------------------------------------------------
	if (colLetter.a > 0.0f)
	{ // 手紙のα値が一定値より大きい場合

		// 手紙のα値を減算
		colLetter.a -= LETTER_CHAN;

		if (colLetter.a <= 0.0f)
		{ // 手紙のα値が一定値以下の場合

			// 手紙のα値を補正
			colLetter.a = 0.0f;
		}

		// 手紙の色を反映
		m_pLetter->SetColor(colLetter);
	}

	//--------------------------------------------------------
	//	フェードの透明化
	//--------------------------------------------------------
	if (colFade.a > 0.0f)
	{ // フェードのα値が一定値より大きい場合

		// フェードのα値を減算
		colFade.a -= FADE_CHAN;

		if (colFade.a <= 0.0f)
		{ // フェードのα値が一定値以下の場合

			// フェードのα値を補正
			colFade.a = 0.0f;
		}

		// フェードの色を反映
		m_pFade->SetColor(colFade);
	}

	//--------------------------------------------------------
	//	操作説明の透明化
	//--------------------------------------------------------
	if (colControl.a > 0.0f)
	{ // 操作説明のα値が一定値より大きい場合

		// 操作説明のα値を減算
		colControl.a -= CONTROL_SUB_ALPHA;

		if (colControl.a <= 0.0f)
		{ // 操作説明のα値が一定値以下の場合

			// 操作説明のα値を補正
			colControl.a = 0.0f;
		}

		// 操作方法の色を反映
		m_pControl->SetColor(colControl);
	}

	//--------------------------------------------------------
	//	便箋の移動
	//--------------------------------------------------------
	if (posPaper.y < POS_PAPER.y)
	{ // 便箋の位置が一定値より小さい場合

		// 便箋の移動量を加算
		m_fMove += PAPER_MOVE;

		// 便箋の位置を加算
		posPaper.y += m_fMove;
	}
	else if (posPaper.y >= POS_PAPER.y)
	{ // 便箋の位置が一定値以上の場合

		// 便箋の位置を補正
		posPaper.y = POS_PAPER.y;

		// 便箋の移動量を初期化
		m_fMove = 0.0f;

		if (colLetter.a	 <= 0.0f
		&&  colFade.a	 <= 0.0f
		&&  colControl.a <= 0.0f)
		{ // 全てのα値が下がり切っている場合

			// 終了状態にする
			m_state = STATE_END;
		}
	}

	// 便箋の位置を反映
	m_pPaper->SetPosition(posPaper);
}
