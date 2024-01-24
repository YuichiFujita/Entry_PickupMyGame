//============================================================
//
//	強化マネージャー処理 [levelupManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "levelupManager.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "sceneGame.h"
#include "waveManager.h"
#include "shopManager.h"
#include "object2D.h"
#include "stage.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CLevelupManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\shop008.png",	// 操作テクスチャ
};

//************************************************************
//	マクロ定義
//************************************************************
#define CONTROL_PRIO	(6)		// 操作表示の優先順位
#define ADD_ROT			(0.05f)	// 点滅向きの加算量
#define POS_CONTROL		(D3DXVECTOR3(SCREEN_CENT.x, 665.0f, 0.0f))	// 操作方法表示の位置
#define SIZE_CONTROL	(D3DXVECTOR3(SCREEN_WIDTH, 100.0f, 0.0f))	// 操作方法表示の大きさ
#define COL_CONTROL		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f))			// 操作方法表示の色

//************************************************************
//	親クラス [CLevelupManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CLevelupManager::CLevelupManager()
{
	// メンバ変数をクリア
	m_pShopManager	= NULL;			// ショップマネージャーの情報
	m_pControl		= NULL;			// 操作情報
	m_state			= STATE_NORMAL;	// 状態
	m_fSinRot		= 0.0f;			// ポリゴン点滅向き
}

//============================================================
//	デストラクタ
//============================================================
CLevelupManager::~CLevelupManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CLevelupManager::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャ

	// メンバ変数を初期化
	m_pShopManager	= NULL;			// ショップマネージャーの情報
	m_pControl		= NULL;			// 操作情報
	m_state			= STATE_NORMAL;	// 状態
	m_fSinRot		= -HALF_PI;		// ポリゴン点滅向き

	// ショップマネージャーの生成
	m_pShopManager = CShopManager::Create();
	if (UNUSED(m_pShopManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ショップを閉店する
	m_pShopManager->SetEnableOpen(false);

	// 操作情報の生成
	m_pControl = CObject2D::Create
	( // 引数
		POS_CONTROL,	// 位置
		SIZE_CONTROL,	// 大きさ
		VEC3_ZERO,		// 向き
		COL_CONTROL		// 色
	);
	if (UNUSED(m_pControl))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを読込・割当
	m_pControl->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_CONTROL]));

	// 優先順位を設定
	m_pControl->SetPriority(CONTROL_PRIO);

	// 描画をしない状態にする
	m_pControl->SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CLevelupManager::Uninit(void)
{
	// ショップマネージャーを破棄
	if (FAILED(m_pShopManager->Release(m_pShopManager)))
	{ // 破棄に失敗した場合

		// 例外処理
		assert(false);
	}

	// 操作情報を破棄
	m_pControl->Uninit();
}

//============================================================
//	更新処理
//============================================================
void CLevelupManager::Update(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetPad();		// パッド

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:	// 通常状態

		if (CSceneGame::GetWaveManager()->GetState() == CWaveManager::STATE_WAIT)
		{ // 次の季節へ移行待ちの場合

			// ショップを開店する
			m_pShopManager->SetEnableOpen(true);

			// 操作情報を表示
			m_pControl->SetEnableDraw(true);

			// 状態を設定
			m_state = STATE_SELECT;	// 強化選択状態

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}

		break;

	case STATE_SELECT:	// 強化選択状態

		if (pKeyboard->GetTrigger(DIK_SPACE) || pPad->GetTrigger(CInputPad::KEY_B))
		{ // 強化終了の操作が行われた場合

			// ショップを閉店する
			m_pShopManager->SetEnableOpen(false);

			// 操作情報を非表示
			m_pControl->SetEnableDraw(false);

			// ポリゴン点滅向きを初期化
			m_fSinRot = -HALF_PI;

			// 操作情報の色を初期化
			m_pControl->SetColor(COL_CONTROL);

			// 状態を設定
			m_state = STATE_NORMAL;	// 通常状態

			// 次季節へ移行
			CSceneGame::GetWaveManager()->NextSeason();
		}

		// 点滅の更新
		UpdateBlink();

		// ショップマネージャーの更新
		m_pShopManager->Update();

		// 操作情報の更新
		m_pControl->Update();

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	生成処理
//============================================================
CLevelupManager *CLevelupManager::Create(void)
{
	// ポインタを宣言
	CLevelupManager *pLevelupManager = NULL;	// 強化マネージャー生成用

	if (UNUSED(pLevelupManager))
	{ // 使用されていない場合

		// メモリ確保
		pLevelupManager = new CLevelupManager;	// 強化マネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pLevelupManager))
	{ // 使用されている場合
		
		// 強化マネージャーの初期化
		if (FAILED(pLevelupManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pLevelupManager;
			pLevelupManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pLevelupManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CLevelupManager::Release(CLevelupManager *&prLevelupManager)
{
	if (USED(prLevelupManager))
	{ // 使用中の場合

		// 強化マネージャーの終了
		prLevelupManager->Uninit();

		// メモリ開放
		delete prLevelupManager;
		prLevelupManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	点滅の更新処理
//============================================================
void CLevelupManager::UpdateBlink(void)
{
	// 変数を宣言
	D3DXCOLOR colControl = m_pControl->GetColor();	// 操作表示の色

	// 操作表示の点滅向きを加算
	m_fSinRot += ADD_ROT;
	useful::NormalizeRot(m_fSinRot);	// 向きを補正

	// 操作表示のα値を変更
	colControl.a = (1.0f / 2.0f) * (sinf(m_fSinRot) + 1.0f);

	// 操作表示の色を反映
	m_pControl->SetColor(colControl);
}
