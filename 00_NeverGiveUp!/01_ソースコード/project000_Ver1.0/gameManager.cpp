//============================================================
//
//	ゲームマネージャー処理 [gameManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "gameManager.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "camera.h"
#include "player.h"
#include "timerManager.h"
#include "phoneManager.h"
#include "editStageManager.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CPhoneManager *CGameManager::m_pPhone = NULL;			// スマホマネージャーの情報
CEditStageManager *CGameManager::m_pEditStage = NULL;	// エディットステージの情報

//************************************************************
//	親クラス [CGameManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CGameManager::CGameManager()
{
	// メンバ変数をクリア
	m_state = STATE_NONE;	// 状態
}

//============================================================
//	デストラクタ
//============================================================
CGameManager::~CGameManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CGameManager::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_INIT_PHONE;	// 状態

	if (m_pPhone == NULL)
	{ // スマホマネージャーが使用されていない場合

		// スマホマネージャーの生成
		m_pPhone = CPhoneManager::Create();
		if (m_pPhone == NULL)
		{ // 生成に失敗した場合

			// 失敗を返す
			return E_FAIL;
		}
	}
	else { assert(false); }	// 使用済み

#if _DEBUG

	if (m_pEditStage == NULL)
	{ // エディットステージが使用されていない場合

		// エディットステージの生成
		m_pEditStage = CEditStageManager::Create();
		if (m_pEditStage == NULL)
		{ // 生成に失敗した場合

			// 失敗を返す
			return E_FAIL;
		}
	}
	else { assert(false); }	// 使用済み

#endif	// _DEBUG

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CGameManager::Uninit(void)
{
	if (m_pPhone != NULL)
	{ // スマホマネージャーが使用されている場合

		// スマホマネージャーの破棄
		CPhoneManager::Release(m_pPhone);
	}
	else { assert(false); }	// 非使用中

	if (m_pEditStage != NULL)
	{ // エディットステージが使用されている場合

		// エディットステージの破棄
		CEditStageManager::Release(m_pEditStage);
	}
}

//============================================================
//	更新処理
//============================================================
void CGameManager::Update(void)
{
	// ポインタを宣言
	CTimerManager *pTimer = CSceneGame::GetTimerManager();	// タイマーマネージャー
	assert(pTimer != NULL);
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー
	assert(pPlayer != NULL);

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:

		// 無し

		break;

	case STATE_INIT_PHONE:

		// スマホを見始めるように設定
		m_pPhone->SetLook(CPhoneManager::TYPE_START);

		// スマホ表示状態を設定
		m_state = STATE_PHONE;

		// 処理を抜けずにこのまま遅刻表示状態へ移行

	case STATE_PHONE:

		if (m_pPhone != NULL)
		{ // スマホマネージャーが使用されている場合

			// スマホマネージャーの更新
			m_pPhone->Update();
		}
		else { assert(false); }	// 非使用中

		if (m_pPhone->GetState() == CPhoneManager::STATE_END)
		{ // 遅刻の表示が終了した場合

			// 通常初期化状態を設定
			m_state = STATE_INIT_NORMAL;
		}

		break;

	case STATE_INIT_NORMAL:

		// 描画の描画をONにする
		CScene::GetPlayer()->SetEnableDraw(true);

		// タイムを計測開始
		CSceneGame::GetTimerManager()->Start();	// 計測を開始

		// 通常状態を設定
		m_state = STATE_NORMAL;

		// 処理を抜けずにこのまま通常状態へ移行

	case STATE_NORMAL:

		if (pTimer->GetState() == CTimerManager::STATE_END && pTimer->Get() <= 0)
		{ // 計測終了している且つ、残り時間が 0以下の場合

			// クリア失敗表示の初期化状態を設定
			m_state = STATE_INIT_END;
		}

		break;

	case STATE_INIT_END:

		// スマホを見始めるように設定
		m_pPhone->SetLook(CPhoneManager::TYPE_END);

		// クリア失敗の表示状態を設定
		m_state = STATE_END;

		// 処理を抜けずにこのままクリア失敗の表示状態へ移行

	case STATE_END:

		if (m_pPhone != NULL)
		{ // スマホマネージャーが使用されている場合

			// スマホマネージャーの更新
			m_pPhone->Update();
		}
		else { assert(false); }	// 非使用中

		if (m_pPhone->GetState() == CPhoneManager::STATE_END)
		{ // クリア失敗の表示が終了した場合

			// プレイヤーをクリア失敗状態にする
			pPlayer->SetState(CPlayer::STATE_OVER);
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (m_pEditStage != NULL)
	{ // エディットステージが使用されている場合

		// エディットステージの更新
		m_pEditStage->Update();
	}
}

//============================================================
//	状態の設定処理
//============================================================
void CGameManager::SetState(const EState state)
{
	if (state == STATE_INIT_PHONE
	||  state == STATE_INIT_NORMAL
	||  state == STATE_INIT_END)
	{ // 初期化状態の場合

		// 状態を設定
		m_state = state;
	}
	else { assert(false); }	// 初期化状態以外
}

//============================================================
//	状態取得処理
//============================================================
CGameManager::EState CGameManager::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	生成処理
//============================================================
CGameManager *CGameManager::Create(void)
{
	// ポインタを宣言
	CGameManager *pGameManager = NULL;	// ゲームマネージャー生成用

	if (pGameManager == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pGameManager = new CGameManager;	// ゲームマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (pGameManager != NULL)
	{ // 使用されている場合
		
		// ゲームマネージャーの初期化
		if (FAILED(pGameManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pGameManager;
			pGameManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pGameManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CGameManager::Release(CGameManager *&prGameManager)
{
	if (prGameManager != NULL)
	{ // 使用中の場合

		// ゲームマネージャーの終了
		prGameManager->Uninit();

		// メモリ開放
		delete prGameManager;
		prGameManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	スマホマネージャー取得処理
//============================================================
CPhoneManager *CGameManager::GetPhone(void)
{
	// スマホマネージャーの情報を返す
	return m_pPhone;
}

//============================================================
//	エディットステージ取得処理
//============================================================
CEditStageManager *CGameManager::GetEditStage(void)
{
	// エディットステージの情報を返す
	return m_pEditStage;
}
