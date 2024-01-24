//============================================================
//
//	スマホマネージャー処理 [phoneManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "phoneManager.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "object2D.h"
#include "anim2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	// ランキング基本情報
	const int PRIORITY = 14;	// スマホの優先順位

	// フェード基本情報
	namespace fade
	{
		const D3DXCOLOR COL	= XCOL_AWHITE;		// フェードの色

		const float CHANGE_ALPHA	= 0.01f;	// フェードのα値変更量
		const float STOP_ALPHA		= 0.65f;	// フェードの最大α値
	}

	// スマホ基本情報
	namespace phone
	{
		const D3DXVECTOR3	POS		= D3DXVECTOR3(SCREEN_CENT.x, 1200.0f, 0.0f);	// スマホの初期位置
		const D3DXVECTOR3	SIZE	= D3DXVECTOR3(394.0f, 653.0f, 0.0f);			// スマホの大きさ
		const D3DXCOLOR		COL		= XCOL_WHITE;	// スマホの色

		const int	CNT_WAIT_SCALE	= 15;				// スマホ拡大の待機フレーム
		const int	CNT_WAIT_RETURN = 80;				// スマホしまいの待機フレーム
		const float ADD_MOVE_TAKE	= 0.5f;				// スマホ取り出し時の移動量
		const float ADD_MOVE_RETURN	= 0.4f;				// スマホしまい時の移動量
		const float STOP_POS_TAKE	= SCREEN_CENT.y;	// スマホの停止Y位置
		const float STOP_POS_RETURN	= 1800.0f;			// スマホの停止Y位置
		const float SET_SCALE	= 2.5f;					// スマホの初期拡大率
		const float ADD_SCALE	= 0.09f;				// スマホの拡大率加算量
	}

	// メッセージ基本情報
	namespace message
	{
		const D3DXVECTOR3	POS		= D3DXVECTOR3(0.0f, -250.0f, 0.0f);		// メッセージの初期位置
		const D3DXVECTOR3	SIZE	= D3DXVECTOR3(802.0f, 180.0f, 0.0f);	// メッセージの大きさ
		const D3DXVECTOR3	SPACE	= D3DXVECTOR3(0.0f, 200.0f, 0.0f);		// メッセージの空白

		const POSGRID2	TEX_PART	= POSGRID2(1, NUM_MESSAGE);	// メッセージのテクスチャ分割数

		const int INIT_CNT	= 40;	// メッセージ表示の待機フレームの初期値
		const int CNT_DISP	= 80;	// メッセージ表示の待機フレーム
	}

	// 強調基本情報
	namespace stress
	{
		const D3DXVECTOR3	POS = D3DXVECTOR3(220.0f, (float)SCREEN_HEIGHT - 220.0f, 0.0f);	// 強調の初期位置
		const D3DXVECTOR3	ROT = D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(-45.0f));	// 強調の初期向き
		const D3DXVECTOR3	SIZE = D3DXVECTOR3(660.0f, 660.0f, 0.0f);	// 強調の大きさ
		const D3DXCOLOR		COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.85f);	// 強調の色

		const float CHANGE_ALPHA = 0.025f;	// 強調のα値変更量
	}

	// 表情基本情報
	namespace face
	{
		const D3DXVECTOR3	POS = D3DXVECTOR3(150.0f, (float)SCREEN_HEIGHT - 150.0f, 0.0f);	// 表情の位置
		const D3DXVECTOR3	SIZE = D3DXVECTOR3(320.0f, 320.0f, 0.0f);	// 表情の大きさ

		const int	CNT_WAIT_SCALE = 55;	// 表情拡大の待機フレーム
		const float	SET_SCALE = 0.0f;		// 表情の初期拡大率
		const float	ADD_SCALE = 0.1f;		// 表情の拡大率加算量
		const float CHANGE_ALPHA = 0.025f;	// 表情のα値変更量
	}

	// 演出基本情報
	namespace staging
	{
		const D3DXVECTOR3 POS	= D3DXVECTOR3(-420.0f, 620.0f, 0.0f);	// 演出の位置
		const D3DXVECTOR3 SIZE	= D3DXVECTOR3(823.0f, 200.0f, 0.0f);	// 演出の大きさ

		const int	CNT_WAIT_END = 95;	// 終了の待機フレーム
		const float	MOVE_POS = 82.0f;	// 移動量
		const float	STOP_POS = 450.0f;	// 停止位置
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CPhoneManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\phone000.png",	// スマホのテクスチャ
	"data\\TEXTURE\\stress000.png",	// 強調のテクスチャ
};
const char *CPhoneManager::mc_apMessageTextureFile[] =	// メッセージテクスチャ定数
{
	"data\\TEXTURE\\message000.png",	// 開始時のメッセージテクスチャ
	"data\\TEXTURE\\message001.png",	// 終了時のメッセージテクスチャ
};
const char *CPhoneManager::mc_apFaceTextureFile[] =	// 表情テクスチャ定数
{
	"data\\TEXTURE\\face000.png",	// 開始時の表情テクスチャ
	"data\\TEXTURE\\face001.png",	// 終了時の表情テクスチャ
};
const char *CPhoneManager::mc_apStagingTextureFile[] =	// 演出テクスチャ定数
{
	"data\\TEXTURE\\staging000.png",	// 開始時の演出テクスチャ
	"data\\TEXTURE\\staging001.png",	// 終了時の演出テクスチャ
};

//************************************************************
//	親クラス [CPhoneManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPhoneManager::CPhoneManager()
{
	// メンバ変数をクリア
	memset(&m_apMessage[0], 0, sizeof(m_apMessage));	// メッセージの情報
	m_pStaging	= NULL;			// 演出の情報
	m_pFace		= NULL;			// 表情の情報
	m_pStress	= NULL;			// 強調の情報
	m_pPhone	= NULL;			// スマホの情報
	m_pFade		= NULL;			// フェードの情報
	m_state		= STATE_NONE;	// 状態
	m_fMove		= 0.0f;			// スマホの移動量
	m_fScale	= 0.0f;			// スマホの拡大率
	m_bDisp		= false;		// 表示状況
	m_nCounterState	= 0;		// 状態管理カウンター
	m_nCounterDisp = 0;			// 表示管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CPhoneManager::~CPhoneManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPhoneManager::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apMessage[0], 0, sizeof(m_apMessage));	// メッセージの情報
	m_pStaging	= NULL;			// 演出の情報
	m_pFace		= NULL;			// 表情の情報
	m_pStress	= NULL;			// 強調の情報
	m_pPhone	= NULL;			// スマホの情報
	m_pFade		= NULL;			// フェードの情報
	m_state		= STATE_NONE;	// 状態
	m_fMove		= 0.0f;			// スマホの移動量
	m_fScale	= 1.0f;			// スマホの拡大率
	m_bDisp		= false;		// 表示状況
	m_nCounterState	= 0;		// 状態管理カウンター
	m_nCounterDisp = 0;			// 表示管理カウンター

	//--------------------------------------------------------
	//	フェードの生成・設定
	//--------------------------------------------------------
	// フェードの生成
	m_pFade = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		SCREEN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		fade::COL		// 色
	);
	if (m_pFade == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFade->SetPriority(PRIORITY);

	// 自動描画をOFFに設定
	m_pFade->SetEnableDraw(false);

	//--------------------------------------------------------
	//	スマホの生成・設定
	//--------------------------------------------------------
	// スマホの生成
	m_pPhone = CObject2D::Create
	( // 引数
		SCREEN_CENT,	// 位置
		phone::SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		phone::COL		// 色
	);
	if (m_pPhone == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pPhone->BindTexture(mc_apTextureFile[TEXTURE_PHONE]);

	// 優先順位を設定
	m_pPhone->SetPriority(PRIORITY);

	// 自動描画をOFFに設定
	m_pPhone->SetEnableDraw(false);

	//--------------------------------------------------------
	//	メッセージの生成・設定
	//--------------------------------------------------------
	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		// メッセージの生成
		m_apMessage[nCntPhone] = CAnim2D::Create
		( // 引数
			message::TEX_PART.x,								// 横分割数
			message::TEX_PART.y,								// 縦分割数
			message::POS + (message::SPACE * (float)nCntPhone),	// 位置
			message::SIZE										// 大きさ
		);
		if (m_apMessage[nCntPhone] == NULL)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apMessage[nCntPhone]->SetPriority(PRIORITY);

		// 自動描画をOFFに設定
		m_apMessage[nCntPhone]->SetEnableDraw(false);

		// パターンを設定
		m_apMessage[nCntPhone]->SetPattern(nCntPhone);
	}

	//--------------------------------------------------------
	//	強調の生成・設定
	//--------------------------------------------------------
	// 強調の生成
	m_pStress = CObject2D::Create
	( // 引数
		stress::POS,					// 位置
		stress::SIZE * face::SET_SCALE,	// 大きさ
		stress::ROT,					// 向き
		stress::COL						// 色
	);
	if (m_pStress == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pStress->BindTexture(mc_apTextureFile[TEXTURE_STRESS]);

	// 優先順位を設定
	m_pStress->SetPriority(PRIORITY);

	// 自動描画をOFFに設定
	m_pStress->SetEnableDraw(false);

	//--------------------------------------------------------
	//	表情の生成・設定
	//--------------------------------------------------------
	// 表情の生成
	m_pFace = CObject2D::Create
	( // 引数
		face::POS,	// 位置
		face::SIZE * face::SET_SCALE	// 大きさ
	);
	if (m_pFace == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pFace->SetPriority(PRIORITY);

	// 自動描画をOFFに設定
	m_pFace->SetEnableDraw(false);

	//--------------------------------------------------------
	//	演出の生成・設定
	//--------------------------------------------------------
	// 演出の生成
	m_pStaging = CObject2D::Create
	( // 引数
		staging::POS,	// 位置
		staging::SIZE	// 大きさ
	);
	if (m_pStaging == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pStaging->SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPhoneManager::Uninit(void)
{
	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		// メッセージの終了
		m_apMessage[nCntPhone]->Uninit();
	}

	// 演出の終了
	m_pStaging->Uninit();

	// 表情の終了
	m_pFace->Uninit();

	// 強調の終了
	m_pStress->Uninit();

	// スマホの終了
	m_pPhone->Uninit();

	// フェードの終了
	m_pFade->Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPhoneManager::Update(void)
{
	// スマホ演出スキップ
	SkipPhoneStaging();

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 無し

		break;

	case STATE_FADEIN:	// フェードイン状態

		// フェードインの更新
		UpdateFade();

		break;

	case STATE_PHONE_TAKE:	// スマホの取り出し状態

		// スマホ取出の更新
		UpdatePhoneTake();

		// 処理を抜ける
		break;

	case STATE_PHONE_SCALE_WAIT:	// スマホの拡大待機状態

		if (UpdateDispWait(phone::CNT_WAIT_SCALE))
		{ // 待機完了した場合

			// 拡大率を初期化
			m_fScale = 1.0f;

			// スマホの拡大状態を設定
			m_state = STATE_PHONE_SCALE;
		}

		break;

	case STATE_PHONE_SCALE:	// スマホの拡大状態

		// スマホ拡大の更新
		UpdatePhoneScale();

		break;

	case STATE_MESSAGE:	// メッセージ受信状態

		// メッセージ受信の更新
		UpdateMessage();

		break;

	case STATE_FACE_WAIT:	// 表情の拡大待機状態

		if (UpdateDispWait(face::CNT_WAIT_SCALE))
		{ // 待機完了した場合

			// 拡大率を設定
			m_fScale = face::SET_SCALE;

			// 自動描画をONに設定
			m_pStress->SetEnableDraw(true);
			m_pFace->SetEnableDraw(true);

			// 表情の拡大状態を設定
			m_state = STATE_FACE;
		}

		break;

	case STATE_FACE:	// 表情の拡大状態

		// 表情拡大の更新
		UpdateFace();

		break;

	case STATE_PHONE_RETURN_WAIT:	// スマホのしまい待機状態

		if (UpdateDispWait(phone::CNT_WAIT_RETURN))
		{ // 待機完了した場合

			// スマホのしまい状態を設定
			m_state = STATE_PHONE_RETURN;
		}

		break;

	case STATE_PHONE_RETURN:	// スマホのしまい状態

		// スマホ収納の更新
		UpdatePhoneReturn();

		break;

	case STATE_STAGING:	// 演出の表示状態

		// 演出表示の更新
		UpdateStaging();

		break;

	case STATE_END_WAIT:	// 終了待機状態

		if (UpdateDispWait(staging::CNT_WAIT_END))
		{ // 待機完了した場合

			// 終了状態を設定
			m_state = STATE_END;

			// 演出の自動描画をOFFにする
			m_pStaging->SetEnableDraw(false);

			// 表示をOFFにする
			SetEnableDisp(false);
		}

		break;

	case STATE_END:	// 終了状態

		// 無し

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		// メッセージの更新
		m_apMessage[nCntPhone]->Update();
	}

	// 演出の更新
	m_pStaging->Update();

	// 表情の更新
	m_pFace->Update();

	// 強調の更新
	m_pStress->Update();

	// スマホの更新
	m_pPhone->Update();

	// フェードの更新
	m_pFade->Update();

	// メッセージ相対位置の設定
	SetPositionRelative();
}

//============================================================
//	スマホ表示の開始処理
//============================================================
void CPhoneManager::SetLook(const EType type)
{
	//--------------------------------------------------------
	//	テクスチャの設定
	//--------------------------------------------------------
	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		// メッセージのテクスチャを登録・割当
		m_apMessage[nCntPhone]->BindTexture(mc_apMessageTextureFile[type]);
	}

	// 表情のテクスチャを登録・割当
	m_pFace->BindTexture(mc_apFaceTextureFile[type]);

	// 演出のテクスチャを登録・割当
	m_pStaging->BindTexture(mc_apStagingTextureFile[type]);

	//--------------------------------------------------------
	//	情報の初期化
	//--------------------------------------------------------
	// メッセージの自動描画をOFFに設定
	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		m_apMessage[nCntPhone]->SetEnableDraw(false);
	}

	// 演出の自動描画をONにする
	m_pStaging->SetEnableDraw(true);

	// 演出の位置を初期化
	m_pStaging->SetVec3Position(staging::POS);

	// スマホの位置・大きさを初期化
	m_pPhone->SetVec3Position(phone::POS);
	m_pPhone->SetVec3Sizing(phone::SIZE);

	// ポリゴンの大きさを初期化
	m_pStress->SetVec3Sizing(stress::SIZE * face::SET_SCALE);	// 強調
	m_pFace->SetVec3Sizing(face::SIZE * face::SET_SCALE);		// 表情

	// ポリゴンの色を初期化
	m_pPhone->SetColor(phone::COL);		// スマホ
	m_pFade->SetColor(fade::COL);		// フェード
	m_pStress->SetColor(stress::COL);	// 強調
	m_pFace->SetColor(XCOL_WHITE);		// 表情

	// 情報を初期化
	m_fScale = 1.0f;		// 拡大率
	m_fMove = 0.0f;			// スマホの移動量
	m_nCounterState = 0;	// 状態管理カウンター
	m_nCounterDisp = 0;		// 表示管理カウンター

	// 表示をONにする
	SetEnableDisp(true);

	// 状態を設定
	m_state = STATE_FADEIN;	// フェードイン状態
}

//============================================================
//	状態の設定処理
//============================================================
void CPhoneManager::SetState(const EState state)
{
	// 引数の状態を設定
	m_state = state;
}

//============================================================
//	状態取得処理
//============================================================
CPhoneManager::EState CPhoneManager::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	表示の設定処理
//============================================================
void CPhoneManager::SetEnableDisp(const bool bDisp)
{
	// 引数の表示状況を設定
	m_bDisp = bDisp;

	// 描画状況を表示状況と一致させる
	m_pFade->SetEnableDraw(bDisp);
	m_pPhone->SetEnableDraw(bDisp);
	m_pStress->SetEnableDraw(bDisp);
	m_pFace->SetEnableDraw(bDisp);

	// メッセージの自動描画をOFFに設定
	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		m_apMessage[nCntPhone]->SetEnableDraw(false);
	}
}

//============================================================
//	表示取得処理
//============================================================
bool CPhoneManager::IsDisp(void) const
{
	// 表示状況を返す
	return m_bDisp;
}

//============================================================
//	生成処理
//============================================================
CPhoneManager *CPhoneManager::Create(void)
{
	// ポインタを宣言
	CPhoneManager *pPhoneManager = NULL;	// スマホマネージャー生成用

	if (pPhoneManager == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pPhoneManager = new CPhoneManager;	// スマホマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (pPhoneManager != NULL)
	{ // 使用されている場合
		
		// スマホマネージャーの初期化
		if (FAILED(pPhoneManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pPhoneManager;
			pPhoneManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pPhoneManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CPhoneManager::Release(CPhoneManager *&prPhoneManager)
{
	if (prPhoneManager != NULL)
	{ // 使用中の場合

		// スマホマネージャーの終了
		prPhoneManager->Uninit();

		// メモリ開放
		delete prPhoneManager;
		prPhoneManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	フェードインの更新処理
//============================================================
void CPhoneManager::UpdateFade(void)
{
	// 変数を宣言
	D3DXCOLOR colFade = m_pFade->GetColor();	// フェードの色

	// フェードのα値を加算
	colFade.a += fade::CHANGE_ALPHA;
	
	if (colFade.a >= fade::STOP_ALPHA)
	{ // フェードのα値が一定値以上の場合

		// フェードのα値を補正
		colFade.a = fade::STOP_ALPHA;

		// スマホの取り出し状態にする
		m_state = STATE_PHONE_TAKE;

		// サウンドの再生
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_VIBRATION);	// スマホバイブ音
	}

	// フェードの色を反映
	m_pFade->SetColor(colFade);
}

//============================================================
//	スマホ取出の更新処理
//============================================================
void CPhoneManager::UpdatePhoneTake(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPhone = m_pPhone->GetVec3Position();	// スマホの位置

	// スマホの位置減算量を加算
	m_fMove += phone::ADD_MOVE_TAKE;

	// スマホの位置を減算
	posPhone.y -= m_fMove;

	if (posPhone.y <= phone::STOP_POS_TAKE)
	{ // スマホの位置が一定値以下の場合

		// スマホの位置を補正
		posPhone.y = phone::STOP_POS_TAKE;

		// スマホの移動量を初期化
		m_fMove = 0.0f;

		// スマホの拡大待機状態を設定
		m_state = STATE_PHONE_SCALE_WAIT;
	}

	// スマホの位置を反映
	m_pPhone->SetVec3Position(posPhone);
}

//============================================================
//	スマホ拡大の更新処理
//============================================================
void CPhoneManager::UpdatePhoneScale(void)
{
	// 拡大率を加算
	m_fScale += phone::ADD_SCALE;

	// スマホの大きさを設定
	m_pPhone->SetVec3Sizing(phone::SIZE * m_fScale);

	if (m_fScale >= phone::SET_SCALE)
	{ // 拡大率が最大値以上の場合

		// 拡大率を補正
		m_fScale = 1.0f;

		// スマホの大きさを補正
		m_pPhone->SetVec3Sizing(phone::SIZE * phone::SET_SCALE);

		// カウンターを設定
		m_nCounterState = message::INIT_CNT;

		// メッセージ受信状態を設定
		m_state = STATE_MESSAGE;
	}
}

//============================================================
//	メッセージ受信の更新処理
//============================================================
void CPhoneManager::UpdateMessage(void)
{
	// 状態管理カウンターを加算
	m_nCounterState++;

	if (m_nCounterState >= message::CNT_DISP)
	{ // カウンターが一定値以上の場合

		// 状態管理カウンターを初期化
		m_nCounterState = 0;

		// メッセージを表示
		m_apMessage[m_nCounterDisp]->SetEnableDraw(true);

		// サウンドの再生
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_MESSAGE);	// メッセージ音

		// 表示管理カウンターを加算
		m_nCounterDisp++;

		if (m_nCounterDisp >= NUM_MESSAGE)
		{ // すべてのメッセージを表示した場合

			// 表示管理カウンターを初期化
			m_nCounterDisp = 0;

			// 表情の拡大待機状態を設定
			m_state = STATE_FACE_WAIT;
		}
	}
}

//============================================================
//	表情拡大の更新処理
//============================================================
void CPhoneManager::UpdateFace(void)
{
	// 拡大率を加算
	m_fScale += face::ADD_SCALE;

	// 大きさを設定
	m_pFace->SetVec3Sizing(face::SIZE * m_fScale);		// 表情
	m_pStress->SetVec3Sizing(face::SIZE * m_fScale);	// 強調

	if (m_fScale >= 1.0f)
	{ // 拡大率が最大値以上の場合

		// 拡大率を補正
		m_fScale = 1.0f;

		// 大きさを補正
		m_pFace->SetVec3Sizing(face::SIZE);		// 表情
		m_pStress->SetVec3Sizing(face::SIZE);	// 強調

		// スマホのしまい待機状態を設定
		m_state = STATE_PHONE_RETURN_WAIT;

		// サウンドの再生
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_STICK);	// くっつき音
	}
}

//============================================================
//	スマホ収納の更新処理
//============================================================
void CPhoneManager::UpdatePhoneReturn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPhone = m_pPhone->GetVec3Position();	// 便箋の位置
	D3DXCOLOR colFade = m_pFade->GetColor();			// フェードの色
	D3DXCOLOR colFace = m_pFace->GetColor();			// 表情の色
	D3DXCOLOR colStress = m_pStress->GetColor();		// 強調の色

	//--------------------------------------------------------
	//	フェードの透明化
	//--------------------------------------------------------
	if (colFade.a > 0.0f)
	{ // フェードのα値が一定値より大きい場合

		// フェードのα値を減算
		colFade.a -= fade::CHANGE_ALPHA;

		if (colFade.a <= 0.0f)
		{ // フェードのα値が一定値以下の場合

			// フェードのα値を補正
			colFade.a = 0.0f;
		}

		// フェードの色を反映
		m_pFade->SetColor(colFade);
	}

	//--------------------------------------------------------
	//	表情の透明化
	//--------------------------------------------------------
	if (colFace.a > 0.0f)
	{ // 表情のα値が一定値より大きい場合

		// 表情のα値を減算
		colFace.a -= face::CHANGE_ALPHA;

		if (colFace.a <= 0.0f)
		{ // 表情のα値が一定値以下の場合

			// 表情のα値を補正
			colFace.a = 0.0f;
		}

		// 表情の色を反映
		m_pFace->SetColor(colFace);
	}

	//--------------------------------------------------------
	//	強調の透明化
	//--------------------------------------------------------
	if (colStress.a > 0.0f)
	{ // 強調のα値が一定値より大きい場合

		// 強調のα値を減算
		colStress.a -= stress::CHANGE_ALPHA;

		if (colStress.a <= 0.0f)
		{ // 強調のα値が一定値以下の場合

			// 強調のα値を補正
			colStress.a = 0.0f;
		}

		// 強調の色を反映
		m_pStress->SetColor(colStress);
	}

	//--------------------------------------------------------
	//	スマホの移動
	//--------------------------------------------------------
	if (posPhone.y < phone::STOP_POS_RETURN)
	{ // スマホの位置が一定値より小さい場合

		// スマホの移動量を加算
		m_fMove += phone::ADD_MOVE_RETURN;

		// スマホの位置を加算
		posPhone.y += m_fMove;
	}
	else if (posPhone.y >= phone::STOP_POS_RETURN)
	{ // スマホの位置が一定値以上の場合

		// スマホの位置を補正
		posPhone.y = phone::STOP_POS_RETURN;

		// スマホの移動量を初期化
		m_fMove = 0.0f;

		if (colFade.a <= 0.0f
		&&  colFace.a <= 0.0f
		&&  colStress.a <= 0.0f)
		{ // α値が下がり切っている場合

			// 演出の表示状態を設定
			m_state = STATE_STAGING;

			// 表示をOFFにする
			SetEnableDisp(false);
		}
	}

	// スマホの位置を反映
	m_pPhone->SetVec3Position(posPhone);
}

//============================================================
//	演出表示の更新処理
//============================================================
void CPhoneManager::UpdateStaging(void)
{
	// 変数を宣言
	D3DXVECTOR3 posStaging = m_pStaging->GetVec3Position();	// 演出位置

	// 横位置を右に移動
	posStaging.x += staging::MOVE_POS;

	if (posStaging.x > staging::STOP_POS)
	{ // 位置が停止位置を超えた場合

		// 停止位置に補正
		posStaging.x = staging::STOP_POS;

		// 終了待機状態を設定
		m_state = STATE_END_WAIT;

		// サウンドの再生
		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
	}

	// 演出位置を反映
	m_pStaging->SetVec3Position(posStaging);
}

//============================================================
//	スマホ演出スキップ処理
//============================================================
void CPhoneManager::SkipPhoneStaging(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetInstance()->GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetInstance()->GetPad();		// パッド

	if (pKeyboard->IsTrigger(DIK_RETURN)  || pKeyboard->IsTrigger(DIK_SPACE)
	||  pPad->IsTrigger(CInputPad::KEY_A) || pPad->IsTrigger(CInputPad::KEY_B)
	||  pPad->IsTrigger(CInputPad::KEY_X) || pPad->IsTrigger(CInputPad::KEY_Y))
	{
		if (m_state != STATE_END && m_state != STATE_END_WAIT && m_state != STATE_STAGING)
		{ // スキップ以降の状態ではない場合

			// 演出の表示状態を設定
			m_state = STATE_STAGING;

			// 表示をOFFにする
			SetEnableDisp(false);
		}
	}
}

//============================================================
//	メッセージ相対位置の設定処理
//============================================================
void CPhoneManager::SetPositionRelative(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPhone = m_pPhone->GetVec3Position();	// スマホ位置

	for (int nCntPhone = 0; nCntPhone < NUM_MESSAGE; nCntPhone++)
	{ // 受信メッセージ数分繰り返す

		// メッセージの位置を設定
		m_apMessage[nCntPhone]->SetVec3Position(posPhone + message::POS + (message::SPACE * (float)nCntPhone));
	}
}

//============================================================
//	表示待機処理
//============================================================
bool CPhoneManager::UpdateDispWait(const int nWait)
{
	if (m_nCounterState < nWait)
	{ // カウンターが待機カウントまで達していない場合

		// カウンターを加算
		m_nCounterState++;

		// 待機未完了を返す
		return false;
	}
	else
	{ // カウンターが待機完了した場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 待機完了を返す
		return true;
	}
}
