//============================================================
//
//	ゲーム画面処理 [sceneGame.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sceneGame.h"
#include "manager.h"
#include "sound.h"
#include "input.h"

#include "waveManager.h"
#include "timerManager.h"
#include "stage.h"
#include "pause.h"
#include "score.h"
#include "warningSpawn.h"

#include "enemy.h"
#include "magic.h"
#include "sea.h"
#include "wall.h"
#include "scenery.h"
#include "sky.h"
#include "flower.h"
#include "weed.h"
#include "player.h"

//************************************************************
//	マクロ定義
//************************************************************
#define TIME_POS		(D3DXVECTOR3(40.0f, 50.0f, 0.0f))			// タイマー位置
#define TIME_VAL_SIZE	(D3DXVECTOR3(60.0f, 80.0f, 0.0f))			// タイマー数字大きさ
#define TIME_PART_SIZE	(D3DXVECTOR3(35.0f, 80.0f, 0.0f))			// タイマー区切り大きさ
#define TIME_VAL_SPACE	(D3DXVECTOR3(TIME_VAL_SIZE.x, 0.0f, 0.0f))	// タイマー数字空白
#define TIME_PART_SPACE	(D3DXVECTOR3(TIME_PART_SIZE.x, 0.0f, 0.0f))	// タイマー区切り空白

#define SCO_POS		(D3DXVECTOR3(825.0f, 50.0f, 0.0f))		// スコア位置
#define SCO_SIZE	(D3DXVECTOR3(60.0f, 80.0f, 0.0f))		// スコア大きさ
#define SCO_SPACE	(D3DXVECTOR3(SCO_SIZE.x, 0.0f, 0.0f))	// スコア空白

#define WARNING_SIZE	(D3DXVECTOR3(1000.0f, 500.0f, 0.0f))	// 警告大きさ

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CWaveManager	*CSceneGame::m_pWaveManager  = NULL;	// ウェーブマネージャー
CTimerManager	*CSceneGame::m_pTimerManager = NULL;	// タイマーマネージャー
CWarningSpawn	*CSceneGame::m_pWarningSpawn = NULL;	// 出現警告表示オブジェクト
CPause	*CSceneGame::m_pPause	= NULL;					// ポーズ
CScore	*CSceneGame::m_pScore	= NULL;					// スコアオブジェクト

bool CSceneGame::m_bControlCamera = false;	// カメラの操作状況
bool CSceneGame::m_bDrawUI = true;			// UIの描画状況
bool CSceneGame::m_bDrawPause = true;		// ポーズの描画状況

//************************************************************
//	子クラス [CSceneGame] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSceneGame::CSceneGame(const MODE mode) : CScene(mode)
{

}

//============================================================
//	デストラクタ
//============================================================
CSceneGame::~CSceneGame()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSceneGame::Init(void)
{
	// タイマーマネージャーの生成
	m_pTimerManager = CTimerManager::Create
	( // 引数
		TIME_POS,		// 位置
		TIME_VAL_SIZE,	// 数字の大きさ
		TIME_PART_SIZE,	// 区切りの大きさ
		TIME_VAL_SPACE,	// 数字の空白
		TIME_PART_SPACE	// 区切りの空白
	);
	if (UNUSED(m_pTimerManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// スコアオブジェクトの生成
	m_pScore = CScore::Create
	( // 引数
		SCO_POS,	// 位置
		SCO_SIZE,	// 大きさ
		SCO_SPACE	// 空白
	);
	if (UNUSED(m_pScore))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 出現警告表示オブジェクトの生成
	m_pWarningSpawn = CWarningSpawn::Create
	( // 引数
		SCREEN_CENT,	// 位置
		WARNING_SIZE	// 大きさ
	);
	if (UNUSED(m_pWarningSpawn))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シーンの初期化
	CScene::Init();		// ステージ・地面・ターゲット・プレイヤーの生成

	// ウェーブマネージャーの生成
	m_pWaveManager = CWaveManager::Create();
	if (UNUSED(m_pWaveManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ポーズの生成
	m_pPause = CPause::Create();
	if (UNUSED(m_pPause))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 海オブジェクトの生成
	CSea::Create();

	// 壁オブジェクトの生成
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 0.0f,    0.0f, -3000.0f), D3DXToRadian(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),   D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3(-3000.0f, 0.0f,  0.0f),    D3DXToRadian(D3DXVECTOR3(0.0f, 90.0f, 0.0f)),  D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 0.0f,    0.0f,  3000.0f), D3DXToRadian(D3DXVECTOR3(0.0f, 180.0f, 0.0f)), D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 3000.0f, 0.0f,  0.0f),    D3DXToRadian(D3DXVECTOR3(0.0f, 270.0f, 0.0f)), D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));

	// 景色オブジェクトの生成
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, VEC3_ZERO,                                    XCOL_WHITE,                        POSGRID2(32, 1), 12000.0f, 1000.0f, D3DCULL_CW, false);
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, D3DXToRadian(D3DXVECTOR3(0.0f, 85.0f, 0.0f)), D3DXCOLOR(0.7f, 1.0f, 1.0f, 1.0f), POSGRID2(32, 1), 14000.0f, 1600.0f, D3DCULL_CW, false);
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, D3DXToRadian(D3DXVECTOR3(0.0f, 35.0f, 0.0f)), D3DXCOLOR(0.4f, 1.0f, 0.7f, 1.0f), POSGRID2(32, 1), 16000.0f, 2200.0f, D3DCULL_CW, false);

	// 空オブジェクトの生成
	CSky::Create(CSky::TEXTURE_NORMAL, VEC3_ZERO, VEC3_ZERO, XCOL_WHITE, POSGRID2(32, 6), 18000.0f, D3DCULL_CW, false);

	//--------------------------------------------------------
	//	初期設定
	//--------------------------------------------------------
	// 敵セットアップの読込
	CEnemy::LoadSetup();

	// 魔法セットアップの読込
	CMagic::LoadSetup();

	// マナフラワーセットアップの読込
	CFlower::LoadSetup();

	// 草セットアップの読込
	CWeed::LoadSetup();

	// マナフラワーランダム生成
	CFlower::RandomSpawn(30, CFlower::TYPE_SPRING);	// TODO：定数変更

	// 草ランダム生成
	CWeed::RandomSpawn(200, CWeed::TYPE_SPRING);	// TODO：定数変更

	// カメラを設定
	CManager::GetCamera()->SetState(CCamera::STATE_BARGAINING);	// カメラを寄り引き状態に設定
	CManager::GetCamera()->SetDestBargaining();	// 目標位置を設定

	// タイムを計測開始
	m_pTimerManager->Start();			// 計測を開始
	m_pTimerManager->EnableStop(true);	// 計測を一時停止

	// UIの描画状況を設定
	SetEnableDrawUI(m_bDrawUI);

	// ポーズの描画状況を設定
	SetEnableDrawPause(m_bDrawPause);

	// BGMの再生
	CManager::GetSound()->Play(CSound::LABEL_BGM_GAME);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
HRESULT CSceneGame::Uninit(void)
{
	// ウェーブマネージャーの破棄
	if (FAILED(CWaveManager::Release(m_pWaveManager)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// タイマーマネージャーの破棄
	if (FAILED(CTimerManager::Release(m_pTimerManager)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ポーズの破棄
	if (FAILED(CPause::Release(m_pPause)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 終了済みのオブジェクトポインタをNULLにする
	m_pScore = NULL;	// スコアオブジェクト

	// シーンの終了
	CScene::Uninit();

	// 成功を返す
	return S_OK;
}

//============================================================
//	更新処理
//============================================================
void CSceneGame::Update(void)
{
	if (CManager::GetKeyboard()->GetTrigger(DIK_F2))
	{
		SetEnableDrawUI((!m_bDrawUI) ? true : false);
	}
	else if (CManager::GetKeyboard()->GetTrigger(DIK_F3))
	{
		SetEnableDrawPause((!m_bDrawPause) ? true : false);
	}
	else if (CManager::GetKeyboard()->GetTrigger(DIK_F4))
	{
		SetEnableControlCamera((!m_bControlCamera) ? true : false);
	}

	if (USED(m_pTimerManager))
	{ // 使用中の場合

		// タイマーマネージャーの更新
		m_pTimerManager->Update();
	}
	else { assert(false); }	// 非使用中

	if (USED(m_pPause))
	{ // 使用中の場合

		// ポーズの更新
		m_pPause->Update();
	}
	else { assert(false); }	// 非使用中

	if (!m_pPause->IsPause())
	{ // ポーズ中ではない場合

		if (USED(m_pWaveManager))
		{ // 使用中の場合

			// ウェーブマネージャーの更新
			m_pWaveManager->Update();
		}
		else { assert(false); }	// 非使用中

		// シーンの更新
		CScene::Update();
	}
	else
	{ // ポーズ中の場合

		if (CManager::GetCamera()->GetState() == CCamera::STATE_CONTROL)
		{ // カメラが操作状態の場合

			// カメラの更新
			CManager::GetCamera()->Update();
		}
	}

	// TODO：遷移のタイミングで壊れる問題シーンにも死亡フラグ追加で回避
	//		 シーンマネージャーを用意する
}

//============================================================
//	描画処理
//============================================================
void CSceneGame::Draw(void)
{

}

//============================================================
//	ウェーブマネージャー取得処理
//============================================================
CWaveManager *CSceneGame::GetWaveManager(void)
{
	// ウェーブマネージャーのポインタを返す
	return m_pWaveManager;
}

//============================================================
//	タイマーマネージャー取得処理
//============================================================
CTimerManager *CSceneGame::GetTimerManager(void)
{
	// タイマーマネージャーのポインタを返す
	return m_pTimerManager;
}

//============================================================
//	ポーズ取得処理
//============================================================
CPause *CSceneGame::GetPause(void)
{
	// ポーズのポインタを返す
	return m_pPause;
}

//============================================================
//	スコア取得処理
//============================================================
CScore *CSceneGame::GetScore(void)
{
	// スコアのポインタを返す
	return m_pScore;
}

//============================================================
//	出現警告表示取得処理
//============================================================
CWarningSpawn *CSceneGame::GetWarningSpawn(void)
{
	// 出現警告表示のポインタを返す
	return m_pWarningSpawn;
}

//============================================================
//	カメラの操作状況の設定処理
//============================================================
void CSceneGame::SetEnableControlCamera(const bool bControl)
{
	// 引数のカメラ操作状況を設定
	m_bControlCamera = bControl;

	// カメラの操作状況を設定
	if (bControl)
	{ // 操作する状況の場合

		// 操作状態に変更
		CManager::GetCamera()->SetState(CCamera::STATE_CONTROL);
	}
	else
	{ // 操作しない状況の場合

		// 寄り引き状態に変更
		CManager::GetCamera()->SetState(CCamera::STATE_BARGAINING);
	}
}

//============================================================
//	UIの描画状況の設定処理
//============================================================
void CSceneGame::SetEnableDrawUI(const bool bDraw)
{
	// 引数のUIの描画状況を設定
	m_bDrawUI = bDraw;

	// タイマーの描画状況を設定
	m_pTimerManager->SetEnableDraw(bDraw);

	// スコアの描画状況を設定
	m_pScore->SetEnableDraw(bDraw);

	// プレイヤーの経験値を設定
	GetPlayer()->SetEnableDrawExp(bDraw);
}

//============================================================
//	ポーズの描画状況の設定処理
//============================================================
void CSceneGame::SetEnableDrawPause(const bool bDraw)
{
	// 引数のポーズの描画状況を設定
	m_bDrawPause = bDraw;

	// ポーズの描画状況を設定
	m_pPause->SetEnableDraw(m_pPause->IsPause());
}

//============================================================
//	UIの描画状況取得処理
//============================================================
bool CSceneGame::IsDrawUI(void)
{
	// UIの描画状況を返す
	return m_bDrawUI;
}

//============================================================
//	ポーズの描画状況取得処理
//============================================================
bool CSceneGame::IsDrawPause(void)
{
	// ポーズの描画状況を返す
	return m_bDrawPause;
}
