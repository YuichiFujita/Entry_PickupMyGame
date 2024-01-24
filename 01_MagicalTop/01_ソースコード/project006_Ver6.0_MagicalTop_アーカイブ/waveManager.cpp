//============================================================
//
//	ウェーブマネージャー処理 [waveManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "waveManager.h"
#include "manager.h"
#include "sound.h"
#include "texture.h"
#include "model.h"
#include "modelUI.h"
#include "object2D.h"
#include "value.h"
#include "scene.h"
#include "sceneGame.h"
#include "target.h"
#include "player.h"
#include "camera.h"
#include "field.h"
#include "flower.h"
#include "weed.h"
#include "score.h"
#include "timerManager.h"
#include "warningSpawn.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CWaveManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\wave000.png",	// ウェーブ表示テクスチャ
};

const char *CWaveManager::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\FONT\\season.x",	// SEASONフォントモデル
	"data\\MODEL\\FONT\\end.x",		// ENDフォントモデル
};

const char *CWaveManager::mc_apSeasonModelFile[] =	// シーズン表記モデル定数
{
	"data\\MODEL\\FONT\\spring.x",	// SPRINGフォントモデル
	"data\\MODEL\\FONT\\summer.x",	// SUMMERフォントモデル
	"data\\MODEL\\FONT\\autumn.x",	// AUTUMNフォントモデル
	"data\\MODEL\\FONT\\winter.x",	// WINTERフォントモデル
};

CWaveManager::Season CWaveManager::m_aWaveInfo[CWaveManager::SEASON_MAX] = {};	// ウェーブ情報

//************************************************************
//	マクロ定義
//************************************************************
#define WAVE_SETUP_TXT	"data\\TXT\\wave.txt"	// セットアップテキスト相対パス

#define POS_SEASON_LEFT		(D3DXVECTOR3(-130.0f, 75.0f, -0.0f))	// シーズン左表示の位置
#define POS_SEASON_RIGHT	(D3DXVECTOR3(130.0f, -75.0f, -0.0f))	// シーズン右表示の位置

#define SIZE_SEASON	(D3DXVECTOR3(0.25f, 0.25f, 0.25f))	// シーズンの拡大率
#define SIZE_WAVE	(D3DXVECTOR3(740.0f, 280.0f, 0.0f))	// ウェーブの大きさ
#define SIZE_NUM	(D3DXVECTOR3(210.0f, 250.0f, 0.0f))	// 数字の大きさ

#define WAVE_PRIO	(6)			// ウェーブの優先順位
#define SPACE_EDGE	(155.0f)	// 端から見たウェーブ表示のずれ量
#define MUL_SIZEUP		(12.0f)	// ウェーブ・数字の大きさ上昇乗算量
#define MUL_SIZEDOWN	(0.85f)	// ウェーブ・数字の大きさ下降乗算量

#define SEASON_STARTCNT	(90)	// 季節の開始処理の移行までの余韻カウンター
#define WAVE_STARTCNT	(90)	// ウェーブ開始処理の移行までの余韻カウンター
#define NEXT_WAITCNT	(30)	// 敵全滅時の移行までの余韻カウンター
#define SEASON_MOVECNT	(30)	// シーズン表示の動き停止までの余韻カウンター

#define SEASON_ADD_ROT	(0.2f)		// シーズン表示のX向き加算量
#define SEASON_SUB_ROT	(0.005f)	// シーズン表示のY向き加算量
#define SEASON_MUL_SIZE	(1.05f)		// シーズン表示の拡大率乗算量

#define PLUS_FLOWER	(10)	// 季節進行時のマナフラワーの生成数
#define PLUS_WEED	(10)	// 季節進行時の草の生成数
#define WAIT_FRAME	(240)	// リザルト遷移時の余韻フレーム

//************************************************************
//	親クラス [CWaveManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWaveManager::CWaveManager()
{
	// メンバ変数をクリア
	memset(&m_apModelUI[0], 0, sizeof(m_apModelUI));	// シーズン表示の情報
	m_pObject2D = NULL;			// ウェーブ表示の情報
	m_pValue = NULL;			// ウェーブ数表示の情報
	m_state = STATE_NONE;		// 状態
	m_nSeason = SEASON_SPRING;	// 季節管理カウンター
	m_nWave = 0;				// ウェーブ管理カウンター
	m_nPoint = 0;				// 出現管理カウンター
	m_nCounterState = 0;		// 状態管理カウンター
	m_nCounterFrame = 0;		// ウェーブ余韻管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CWaveManager::~CWaveManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWaveManager::Init(void)
{
	// 変数配列を宣言
	D3DXVECTOR3 aPos[NUM_MODEL_UI] =	// 位置配列
	{ // 初期値
		POS_SEASON_LEFT,	// 左位置
		POS_SEASON_RIGHT	// 右位置
	};

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ
	CModel *pModel = CManager::GetModel();			// モデルへのポインタ

	// メンバ変数を初期化
	memset(&m_apModelUI[0], 0, sizeof(m_apModelUI));	// シーズン表示の情報
	m_pObject2D = NULL;					// ウェーブ表示の情報
	m_pValue = NULL;					// ウェーブ数表示の情報
	m_state = STATE_SEASON_START_INIT;	// 状態
	m_nSeason = SEASON_SPRING;			// 季節管理カウンター
	m_nWave = 0;						// ウェーブ管理カウンター
	m_nPoint = 0;						// 出現管理カウンター
	m_nCounterState = 0;				// 状態管理カウンター
	m_nCounterFrame = 0;				// ウェーブ余韻管理カウンター

	// セットアップの読み込み
	LoadSetup();

	//--------------------------------------------------------
	//	モデルUIの生成・設定
	//--------------------------------------------------------
	for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
	{ // モデルUIの使用数分繰り返す

		// シーズン表示用のモデルUIの生成
		m_apModelUI[nCntWave] = CModelUI::Create
		( // 引数
			aPos[nCntWave],	// 位置
			VEC3_ZERO,		// 向き
			SIZE_SEASON		// 拡大率
		);
		if (UNUSED(m_apModelUI[nCntWave]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// モデルを登録・割当
		m_apModelUI[nCntWave]->BindModel(pModel->GetModel(pModel->Regist(mc_apModelFile[0])));

		// 優先順位を設定
		m_apModelUI[nCntWave]->SetPriority(WAVE_PRIO);

		// 描画をしない設定にする
		m_apModelUI[nCntWave]->SetEnableDraw(false);
	}

	//--------------------------------------------------------
	//	オブジェクト2Dの生成・設定
	//--------------------------------------------------------
	// ウェーブ表示用のオブジェクト2Dの生成
	m_pObject2D = CObject2D::Create
	( // 引数
		D3DXVECTOR3	// 位置
		( // 引数
			0.0f + SPACE_EDGE + (SIZE_WAVE.x * 0.5f),	// x
			SCREEN_HEIGHT * 0.5f,						// y
			0.0											// z
		),
		SIZE_WAVE	// 大きさ
	);
	if (UNUSED(m_pObject2D))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pObject2D->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_NORMAL]));

	// 優先順位を設定
	m_pObject2D->SetPriority(WAVE_PRIO);

	// 描画をしない設定にする
	m_pObject2D->SetEnableDraw(false);

	//--------------------------------------------------------
	//	数字オブジェクトの生成・設定
	//--------------------------------------------------------
	// ウェーブ数表示用の数字オブジェクトの生成
	m_pValue = CValue::Create
	( // 引数
		CValue::TEXTURE_NORMAL,	// テクスチャ
		D3DXVECTOR3				// 位置
		( // 引数
			(float)SCREEN_WIDTH - SPACE_EDGE - (SIZE_NUM.x * 0.5f),	// x
			SCREEN_HEIGHT * 0.5f,									// y
			0.0f													// z
		),
		SIZE_NUM	// 大きさ
	);
	if (UNUSED(m_pValue))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 数字を設定
	m_pValue->SetNumber(0);

	// 優先順位を設定
	m_pValue->SetPriority(WAVE_PRIO);

	// 描画をしない設定にする
	m_pValue->SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CWaveManager::Uninit(void)
{
	for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
	{ // モデルUIの使用数分繰り返す

		// シーズン表示用のモデルUIを破棄
		m_apModelUI[nCntWave]->Uninit();
	}

	// ウェーブ表示用のオブジェクト2Dを破棄
	m_pObject2D->Uninit();

	// ウェーブ数表示用の数字オブジェクトを破棄
	m_pValue->Uninit();

	for (int nCntSeason = 0; nCntSeason < SEASON_MAX; nCntSeason++)
	{ // 季節数分繰り返す

		if (USED(m_aWaveInfo[nCntSeason].pWave))
		{ // ウェーブが使用されている場合

			for (int nCntWave = 0; nCntWave < m_aWaveInfo[nCntSeason].nNumWave; nCntWave++)
			{ // ウェーブ数分繰り返す

				// 出現情報をメモリ開放
				delete[] m_aWaveInfo[nCntSeason].pWave[nCntWave].pPoint;
				m_aWaveInfo[nCntSeason].pWave[nCntWave].pPoint = NULL;
			}

			// ウェーブ情報をメモリ開放
			delete[] m_aWaveInfo[nCntSeason].pWave;
			m_aWaveInfo[nCntSeason].pWave = NULL;
		}
	}
}

//============================================================
//	更新処理
//============================================================
void CWaveManager::Update(void)
{
	// 状態の更新
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 無し

		break;

	case STATE_SEASON_START_INIT:	// 季節の開始初期化状態

		// 季節の開始初期化
		InitSeasonStart();

		// 季節の開始処理に続く

	case STATE_SEASON_START:	// 季節の開始状態

		// 季節の開始
		UpdateSeasonStart();

		break;

	case STATE_WAVE_START_INIT:	// ウェーブ開始初期化状態

		// ウェーブ開始初期化
		InitWaveStart();

		// ウェーブ開始処理に続く

	case STATE_WAVE_START:		// ウェーブ開始状態

		// ウェーブ開始
		UpdateWaveStart();

		break;

	case STATE_PROGRESSION:		// ウェーブ進行状態

		// ウェーブ進行
		UpdateProgression();

		break;

	case STATE_SEASON_END_INIT:	// 季節の終了初期化状態

		// 季節の終了初期化
		InitWaveEnd();

		// 季節の終了処理に続く

	case STATE_SEASON_END:		// 季節の終了状態

		// 季節の終了
		UpdateWaveEnd();

		break;

	case STATE_WAIT:	// 次季節の開始待機状態

		// levelupManager側で次の季節に移行する

		break;

	case STATE_END:		// 終了状態

		// リザルト遷移
		ResultTransition(CGameManager::RESULT_CLEAR);

		// 処理を抜ける
		return;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (CScene::GetPlayer()->GetState() == CPlayer::STATE_DEATH
	||  CScene::GetTarget()->GetState() == CTarget::STATE_DESTROY)
	{ // プレイヤーが死亡状態、またはターゲットが破壊状態の場合

		// リザルト遷移
		ResultTransition(CGameManager::RESULT_FAILED, WAIT_FRAME);

		// 処理を抜ける
		return;
	}

	for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
	{ // モデルUIの使用数分繰り返す

		// シーズン表示用のモデルUIの更新
		m_apModelUI[nCntWave]->Update();
	}

	// ウェーブ表示用のオブジェクト2Dの更新
	m_pObject2D->Update();

	// ウェーブ数表示用の数字オブジェクトの更新
	m_pValue->Update();
}

//============================================================
//	季節の移行処理
//============================================================
void CWaveManager::NextSeason(void)
{
	if (m_state == STATE_WAIT)
	{ // 次の季節の開始待機状態の場合

		// 状態を設定
		m_state = STATE_SEASON_START_INIT;	// 季節の開始状態

		// カメラ更新をONにする
		CManager::GetCamera()->SetEnableUpdate(true);

		// カメラ状態を設定
		CManager::GetCamera()->SetState(CCamera::STATE_UP);	// 上向き状態
	}
	else { assert(false); }	// 使用不可タイミング
}

//============================================================
//	状態取得処理
//============================================================
CWaveManager::STATE CWaveManager::GetState(void)
{
	// 状態を返す
	return m_state;
}

//============================================================
//	生成処理
//============================================================
CWaveManager *CWaveManager::Create(void)
{
	// ポインタを宣言
	CWaveManager *pWaveManager = NULL;	// ウェーブマネージャー生成用

	if (UNUSED(pWaveManager))
	{ // 使用されていない場合

		// メモリ確保
		pWaveManager = new CWaveManager;	// ウェーブマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pWaveManager))
	{ // 使用されている場合
		
		// ウェーブマネージャーの初期化
		if (FAILED(pWaveManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pWaveManager;
			pWaveManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pWaveManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CWaveManager::Release(CWaveManager *&prWaveManager)
{
	if (USED(prWaveManager))
	{ // 使用中の場合

		// ウェーブマネージャーの終了
		prWaveManager->Uninit();

		// メモリ開放
		delete prWaveManager;
		prWaveManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	リザルト遷移処理
//============================================================
void CWaveManager::ResultTransition(const CGameManager::RESULT result, const int nWait)
{
	// ポインタを宣言
	CGameManager *m_pGameManager = CManager::GetGameManager();	// ゲームマネージャー

	// クリア状況を設定
	m_pGameManager->SetResult(result);

	// 獲得スコアを設定
	m_pGameManager->SetScore(CSceneGame::GetScore()->Get());

	// 経過時間を設定
	m_pGameManager->SetTime(CSceneGame::GetTimerManager()->Get());

	// タイムの計測を終了
	CSceneGame::GetTimerManager()->End();

	// シーンの設定
	CManager::SetScene(CScene::MODE_RESULT, nWait);	// リザルト画面
}

//============================================================
//	季節の設定処理
//============================================================
void CWaveManager::SetSeason(const SEASON season)
{
	// 地面を変更
	CScene::GetField()->SetSeason(season);

	// 花を変更
	CFlower::SetSeason(season);

	// 草を変更
	CWeed::SetSeason(season);
}

//============================================================
//	季節の開始初期化処理
//============================================================
void CWaveManager::InitSeasonStart(void)
{
	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ

	for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
	{ // モデルUIの使用数分繰り返す

		// 向きを初期化
		m_apModelUI[nCntWave]->SetRotation(VEC3_ZERO);

		// 拡大率を初期化
		m_apModelUI[nCntWave]->SetScaling(SIZE_SEASON);

		// 描画をする設定にする
		m_apModelUI[nCntWave]->SetEnableDraw(true);
	}

	// モデルを登録・割当
	m_apModelUI[0]->BindModel(pModel->GetModel(pModel->Regist(mc_apModelFile[MODEL_SEASON])));

	// モデルを登録・割当
	m_apModelUI[1]->BindModel(pModel->GetModel(pModel->Regist(mc_apSeasonModelFile[m_nSeason])));

	// 状態を変更
	m_state = STATE_SEASON_START;	// 季節の開始状態
}

//============================================================
//	季節の開始処理
//============================================================
void CWaveManager::UpdateSeasonStart(void)
{
	if (m_nCounterState <= SEASON_STARTCNT)
	{ // カウンターが一定値以下の場合

		if (m_nCounterState <= SEASON_MOVECNT)
		{ // カウンターが一定値以下の場合

			for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
			{ // モデルUIの使用数分繰り返す

				// 変数を宣言
				D3DXVECTOR3 rotSeason = m_apModelUI[nCntWave]->GetRotation();	// シーズン向き
				D3DXVECTOR3 scaleSeason = m_apModelUI[nCntWave]->GetScaling();	// シーズン拡大率
				float fMul = (nCntWave == 0) ? 1.0f : -1.0f;					// 向き加算量の計算用値

				// 向きを加算
				rotSeason.x += SEASON_ADD_ROT * fMul;
				rotSeason.y -= SEASON_SUB_ROT * fMul;

				// 向きを正規化
				useful::NormalizeRot(rotSeason.x);
				useful::NormalizeRot(rotSeason.y);

				// 拡大率を大きくする
				scaleSeason *= SEASON_MUL_SIZE;

				if (scaleSeason.x > 1.0f)
				{ // 拡大率が 1.0fを超えた場合

					// 拡大率を補正
					scaleSeason = VEC3_ONE;
				}

				// 向きと拡大率の更新
				m_apModelUI[nCntWave]->SetRotation(rotSeason);
				m_apModelUI[nCntWave]->SetScaling(scaleSeason);
			}
		}

		// 状態管理カウンターを加算
		m_nCounterState++;

		if (m_nCounterState == (int)(SEASON_MOVECNT * 0.5f))
		{ // カウンターが一定値の場合

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
	}
	else
	{ // カウンターが一定値より大きい場合

		for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
		{ // モデルUIの使用数分繰り返す

			// 描画をしない設定にする
			m_apModelUI[nCntWave]->SetEnableDraw(false);
		}

		// 状態管理カウンターを初期化
		m_nCounterState = 0;

		// 状態を変更
		m_state = STATE_WAVE_START_INIT;	// ウェーブ開始初期化状態
	}
}

//============================================================
//	ウェーブ開始初期化処理
//============================================================
void CWaveManager::InitWaveStart(void)
{
	// 変数を宣言
	D3DXVECTOR3 sizeWave = m_pObject2D->GetScaling();	// ウェーブ文字大きさ
	D3DXVECTOR3 sizeNum = m_pValue->GetScaling();		// ウェーブ数字大きさ

	// 数字を設定
	m_pValue->SetNumber(m_nWave + 1);

	// オブジェクトのサイズを大きくする
	sizeWave *= MUL_SIZEUP;
	sizeNum *= MUL_SIZEUP;

	// 描画をする設定にする
	m_pObject2D->SetEnableDraw(true);
	m_pValue->SetEnableDraw(true);

	// 状態を変更
	m_state = STATE_WAVE_START;	// ウェーブ開始状態

	// 大きさを設定
	m_pObject2D->SetScaling(sizeWave);
	m_pValue->SetScaling(sizeNum);

	// サウンドの再生
	CManager::GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
}

//============================================================
//	ウェーブ開始処理
//============================================================
void CWaveManager::UpdateWaveStart(void)
{	
	// 変数を宣言
	D3DXVECTOR3 sizeWave = m_pObject2D->GetScaling();	// ウェーブ文字大きさ
	D3DXVECTOR3 sizeNum = m_pValue->GetScaling();		// ウェーブ数字大きさ

	if (m_nCounterState <= WAVE_STARTCNT)
	{ // カウンターが一定値以下の場合

		// 状態管理カウンターを加算
		m_nCounterState++;

		// オブジェクトのサイズを小さくする
		sizeWave *= MUL_SIZEDOWN;
		sizeNum *= MUL_SIZEDOWN;

		// ウェーブ文字のサイズを補正
		if (sizeWave.x < SIZE_WAVE.x
		||  sizeWave.y < SIZE_WAVE.y)
		{ // 大きさが下回った場合

			// 大きさを補正
			sizeWave.x = SIZE_WAVE.x;
			sizeWave.y = SIZE_WAVE.y;
		}

		// ウェーブ数字のサイズを補正
		if (sizeNum.x < SIZE_NUM.x
		||  sizeNum.y < SIZE_NUM.y)
		{ // 大きさが下回った場合

			// 大きさを補正
			sizeNum.x = SIZE_NUM.x;
			sizeNum.y = SIZE_NUM.y;
		}
	}
	else
	{ // カウンターが一定値より大きい場合

		// 状態管理カウンターを初期化
		m_nCounterState = 0;

		// 描画をしない設定にする
		m_pObject2D->SetEnableDraw(false);
		m_pValue->SetEnableDraw(false);

		// 状態を変更
		m_state = STATE_PROGRESSION;	// ウェーブ進行状態

		// プレイヤーを再出現させる
		CScene::GetPlayer()->SetRespawn(PLAY_SPAWN_POS);

		if (m_nWave == 0)
		{ // 初回ウェーブの場合

			// 季節の設定
			SetSeason((SEASON)m_nSeason);

			// マナフラワーランダム生成
			CFlower::RandomSpawn(PLUS_FLOWER, (CFlower::TYPE)m_nSeason);

			// 草ランダム生成
			CWeed::RandomSpawn(PLUS_WEED, (CWeed::TYPE)m_nSeason);

			// カメラ状態を設定
			CManager::GetCamera()->SetState(CCamera::STATE_BARGAINING);	// 寄り引き状態
		}

		// タイムを計測再開
		CSceneGame::GetTimerManager()->EnableStop(false);
	}

	// 大きさを設定
	m_pObject2D->SetScaling(sizeWave);
	m_pValue->SetScaling(sizeNum);
}

//============================================================
//	ウェーブ進行処理
//============================================================
void CWaveManager::UpdateProgression(void)
{
	if (m_nCounterFrame > 0)
	{ // カウンターが 0より大きい場合

		if (CEnemy::GetNumAll() <= 0)
		{ // 敵が全滅している場合

			// 余韻管理カウンターを初期化
			m_nCounterFrame = 0;
		}
		else
		{ // 敵が全滅していない場合

			// 余韻管理カウンターを減算
			m_nCounterFrame--;
		}
	}
	else if (m_nPoint < m_aWaveInfo[m_nSeason].pWave[m_nWave].nNumPoint)
	{ // カウンターが 0以下且つ、出現が残っている場合

		for (int nCntType = 0; nCntType < CEnemy::TYPE_MAX; nCntType++)
		{ // 敵の種類の総数分繰り返す

			// 敵の種類ごとに敵をランダムスポーンさせる
			CEnemy::RandomSpawn(m_aWaveInfo[m_nSeason].pWave[m_nWave].pPoint[m_nPoint].aNumSpawn[nCntType], (CEnemy::TYPE)nCntType);
		}

		// 出現警告を表示
		CSceneGame::GetWarningSpawn()->SetDisp();

		// 余韻管理カウンターを設定
		m_nCounterFrame = m_aWaveInfo[m_nSeason].pWave[m_nWave].pPoint[m_nPoint].nFrame;

		// 出現管理カウンターを加算
		m_nPoint++;

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_WARNING);	// 警告音
	}
	else
	{ // カウンターが 0以下且つ、全出現が終了した場合

		if (m_nCounterState <= NEXT_WAITCNT)
		{ // カウンターが一定値以下の場合

			// 状態管理カウンターを加算
			m_nCounterState++;
		}
		else
		{ // カウンターが一定値より大きい場合

			// 敵の全消失
			CEnemy::SetAllVanish();

			// プレイヤーを表示しない状態にする
			CScene::GetPlayer()->SetDisp(false);

			// タイムを計測停止
			CSceneGame::GetTimerManager()->EnableStop(true);

			// 状態管理カウンターを初期化
			m_nCounterState = 0;

			// ウェーブ管理カウンターを加算
			m_nWave++;

			if (m_nWave < m_aWaveInfo[m_nSeason].nNumWave)
			{ // ウェーブがまだある場合

				// カウンターを初期化
				m_nPoint = 0;	// 出現管理カウンター

				// 状態を変更
				m_state = STATE_WAVE_START_INIT;	// ウェーブ開始初期化状態
			}
			else
			{ // 全ウェーブが終了した場合

				// カウンターを初期化
				m_nWave = 0;	// ウェーブ管理カウンター
				m_nPoint = 0;	// 出現管理カウンター

				// 状態を変更
				m_state = STATE_SEASON_END_INIT;	// 季節の終了初期化状態
			}
		}
	}
}

//============================================================
//	季節の終了初期化処理
//============================================================
void CWaveManager::InitWaveEnd(void)
{
	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ

	for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
	{ // モデルUIの使用数分繰り返す

		// 向きを初期化
		m_apModelUI[nCntWave]->SetRotation(VEC3_ZERO);

		// 拡大率を初期化
		m_apModelUI[nCntWave]->SetScaling(SIZE_SEASON);

		// 描画をする設定にする
		m_apModelUI[nCntWave]->SetEnableDraw(true);

		// モデルを登録・割当
		m_apModelUI[nCntWave]->BindModel(pModel->GetModel(pModel->Regist(mc_apModelFile[nCntWave])));
	}

	// 状態を変更
	m_state = STATE_SEASON_END;	// 季節の終了状態
}

//============================================================
//	季節の終了処理
//============================================================
void CWaveManager::UpdateWaveEnd(void)
{
	if (m_nCounterState <= SEASON_STARTCNT)
	{ // カウンターが一定値以下の場合

		if (m_nCounterState <= SEASON_MOVECNT)
		{ // カウンターが一定値以下の場合

			for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
			{ // モデルUIの使用数分繰り返す

				// 変数を宣言
				D3DXVECTOR3 rotSeason = m_apModelUI[nCntWave]->GetRotation();	// シーズン向き
				D3DXVECTOR3 scaleSeason = m_apModelUI[nCntWave]->GetScaling();	// シーズン拡大率
				float fMul = (nCntWave == 0) ? 1.0f : -1.0f;					// 向き加算量の計算用値

				// 向きを加算
				rotSeason.x += SEASON_ADD_ROT * fMul;
				rotSeason.y -= SEASON_SUB_ROT * fMul;

				// 向きを正規化
				useful::NormalizeRot(rotSeason.x);
				useful::NormalizeRot(rotSeason.y);

				// 拡大率を大きくする
				scaleSeason *= SEASON_MUL_SIZE;

				if (scaleSeason.x > 1.0f)
				{ // 拡大率が 1.0fを超えた場合

					// 拡大率を補正
					scaleSeason = VEC3_ONE;
				}

				// 向きと拡大率の更新
				m_apModelUI[nCntWave]->SetRotation(rotSeason);
				m_apModelUI[nCntWave]->SetScaling(scaleSeason);
			}
		}

		// 状態管理カウンターを加算
		m_nCounterState++;

		if (m_nCounterState == (int)(SEASON_MOVECNT * 0.5f))
		{ // カウンターが一定値の場合

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
	}
	else
	{ // カウンターが一定値より大きい場合

		for (int nCntWave = 0; nCntWave < NUM_MODEL_UI; nCntWave++)
		{ // モデルUIの使用数分繰り返す

			// 描画をしない設定にする
			m_apModelUI[nCntWave]->SetEnableDraw(false);
		}

		// 状態管理カウンターを初期化
		m_nCounterState = 0;

		// 季節管理カウンターを加算
		m_nSeason++;

		if (m_nSeason < SEASON_MAX)
		{ // 季節がまだある場合

			// 状態を変更
			m_state = STATE_WAIT;	// 次季節の開始待機状態
		}
		else
		{ // 季節がもうない場合

			// 状態を変更
			m_state = STATE_END;	// 終了状態
		}
	}
}

//============================================================
//	セットアップ処理
//============================================================
void CWaveManager::LoadSetup(void)
{
	// 変数を宣言
	int nSeason	= 0;	// 季節の設定用
	int nWave	= 0;	// ウェーブの設定用
	int nPoint	= 0;	// 出現の設定用
	int nType	= 0;	// 出現種類の代入用
	int nEnd	= 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 静的メンバ変数の情報をクリア
	memset(&m_aWaveInfo[0], 0, sizeof(m_aWaveInfo));	// ウェーブ情報

	// ファイルを読み込み形式で開く
	pFile = fopen(WAVE_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// ウェーブの設定
			if (strcmp(&aString[0], "SEASONSET") == 0)
			{ // 読み込んだ文字列が SEASONSET の場合

				// ウェーブ番号を初期化
				nWave = 0;

				do
				{ // 読み込んだ文字列が END_SEASONSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "NUM_WAVE") == 0)
					{ // 読み込んだ文字列が NUM_WAVE の場合

						fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
						fscanf(pFile, "%d", &m_aWaveInfo[nSeason].nNumWave);	// ウェーブ数を読み込む

						if (UNUSED(m_aWaveInfo[nSeason].pWave))
						{ // 非使用中の場合

							// ウェーブ数分メモリ確保
							m_aWaveInfo[nSeason].pWave = new Wave[m_aWaveInfo[nSeason].nNumWave];

							if (USED(m_aWaveInfo[nSeason].pWave))
							{ // 確保に成功した場合

								// メモリクリア
								memset(m_aWaveInfo[nSeason].pWave, 0, sizeof(Wave) * m_aWaveInfo[nSeason].nNumWave);
							}
							else { assert(false); }	// 確保失敗
						}
						else { assert(false); }	// 使用中
					}
					else if (strcmp(&aString[0], "WAVE") == 0)
					{ // 読み込んだ文字列が WAVE の場合

						// 出現番号を初期化
						nPoint = 0;

						do
						{ // 読み込んだ文字列が END_WAVE ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "NUM_POINT") == 0)
							{ // 読み込んだ文字列が NUM_POINT の場合

								fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aWaveInfo[nSeason].pWave[nWave].nNumPoint);	// 出現数を読み込む

								if (UNUSED(m_aWaveInfo[nSeason].pWave[nWave].pPoint))
								{ // 非使用中の場合

									// 出現数分メモリ確保
									m_aWaveInfo[nSeason].pWave[nWave].pPoint = new Point[m_aWaveInfo[nSeason].pWave[nWave].nNumPoint];

									if (USED(m_aWaveInfo[nSeason].pWave[nWave].pPoint))
									{ // 確保に成功した場合

										// メモリクリア
										memset(m_aWaveInfo[nSeason].pWave[nWave].pPoint, 0, sizeof(Point) * m_aWaveInfo[nSeason].pWave[nWave].nNumPoint);
									}
									else { assert(false); }	// 確保失敗
								}
								else { assert(false); }	// 使用中
							}
							else if (strcmp(&aString[0], "POINT") == 0)
							{ // 読み込んだ文字列が POINT の場合

								do
								{ // 読み込んだ文字列が END_POINT ではない場合ループ

									// ファイルから文字列を読み込む
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "FRAME") == 0)
									{ // 読み込んだ文字列が FRAME の場合

										fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
										fscanf(pFile, "%d", &m_aWaveInfo[nSeason].pWave[nWave].pPoint[nPoint].nFrame);	// 猶予フレームを読み込む
									}
									else if (strcmp(&aString[0], "SPAWN") == 0)
									{ // 読み込んだ文字列が SPAWN の場合

										fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
										fscanf(pFile, "%d", &nType);		// 出現種類を読み込む
										fscanf(pFile, "%d", &m_aWaveInfo[nSeason].pWave[nWave].pPoint[nPoint].aNumSpawn[nType]);	// 出現数を読み込む
									}
								} while (strcmp(&aString[0], "END_POINT") != 0);	// 読み込んだ文字列が END_POINT ではない場合ループ

								// 例外処理
								assert(nPoint < m_aWaveInfo[nSeason].pWave[nWave].nNumPoint);	// 出現オーバー

								// 出現番号を加算
								nPoint++;
							}
						} while (strcmp(&aString[0], "END_WAVE") != 0);	// 読み込んだ文字列が END_WAVE ではない場合ループ

						// 例外処理
						assert(nPoint == m_aWaveInfo[nSeason].pWave[nWave].nNumPoint);	// 出現未設定
						assert(nWave < m_aWaveInfo[nSeason].nNumWave);	// ウェーブオーバー

						// ウェーブ番号を加算
						nWave++;
					}
				} while (strcmp(&aString[0], "END_SEASONSET") != 0);	// 読み込んだ文字列が END_SEASONSET ではない場合ループ

				// 例外処理
				assert(nWave == m_aWaveInfo[nSeason].nNumWave);	// ウェーブ未設定
				assert(nSeason < SEASON_MAX);	// 季節オーバー

				// 季節番号を加算
				nSeason++;
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ

		// 例外処理
		assert(nSeason == SEASON_MAX);	// 季節未設定

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ウェーブセットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
