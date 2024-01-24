//============================================================
//
//	ウェーブマネージャーヘッダー [waveManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WAVEMANAGER_H_
#define _WAVEMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "enemy.h"
#include "gameManager.h"

//************************************************************
//	マクロ定義
//************************************************************
#define NUM_MODEL_UI	(2)	// モデルUIの使用数

//************************************************************
//	前方宣言
//************************************************************
class CModelUI;		// モデルUIクラス
class CObject2D;	// オブジェクト2Dクラス
class CValue;		// 数字クラス

//************************************************************
//	クラス定義
//************************************************************
// ウェーブマネージャークラス
class CWaveManager
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_NORMAL = 0,	// ウェーブ
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// 状態列挙
	typedef enum
	{
		STATE_NONE = 0,				// 何もしない状態
		STATE_SEASON_START_INIT,	// 季節の開始初期化状態
		STATE_SEASON_START,			// 季節の開始状態
		STATE_WAVE_START_INIT,		// ウェーブ開始初期化状態
		STATE_WAVE_START,			// ウェーブ開始状態
		STATE_PROGRESSION,			// ウェーブ進行状態
		STATE_SEASON_END_INIT,		// 季節の終了初期化状態
		STATE_SEASON_END,			// 季節の終了状態
		STATE_WAIT,					// 次季節の開始待機状態
		STATE_END,					// 終了状態
		STATE_MAX					// この列挙型の総数
	}STATE;

	// モデル列挙
	typedef enum
	{
		MODEL_SEASON = 0,	// SEASONフォント
		MODEL_END,			// ENDフォント
		MODEL_MAX,			// この列挙型の総数
	}MODEL;

	// 季節列挙
	typedef enum
	{
		SEASON_SPRING = 0,	// 春
		SEASON_SUMMER,		// 夏
		SEASON_AUTUMN,		// 秋
		SEASON_WINTER,		// 冬
		SEASON_MAX			// この列挙型の総数
	}SEASON;

	// コンストラクタ
	CWaveManager();

	// デストラクタ
	~CWaveManager();

	// 出現構造体
	typedef struct
	{
		int aNumSpawn[CEnemy::TYPE_MAX];	// 生成数
		int nFrame;		// 猶予フレーム
	}Point;

	// ウェーブ構造体
	typedef struct
	{
		Point *pPoint;	// 出現情報
		int nNumPoint;	// 出現数
	}Wave;

	// 季節構造体
	typedef struct
	{
		Wave *pWave;	// ウェーブ情報
		int nNumWave;	// ウェーブ数
	}Season;

	// メンバ関数
	HRESULT Init(void);		// 初期化
	void Uninit(void);		// 終了
	void Update(void);		// 更新
	void NextSeason(void);	// 季節移行
	STATE GetState(void);	// 状態取得

	// 静的メンバ関数
	static void LoadSetup(void);		// セットアップ
	static CWaveManager *Create(void);	// 生成
	static HRESULT Release(CWaveManager *&prWaveManager);	// 破棄

private:
	// メンバ関数
	void ResultTransition(const CGameManager::RESULT result, const int nWait = 0);	// リザルト遷移
	void SetSeason(const SEASON season);	// 季節設定
	void InitSeasonStart(void);		// 季節の開始初期化
	void UpdateSeasonStart(void);	// 季節の開始
	void InitWaveStart(void);		// ウェーブ開始初期化
	void UpdateWaveStart(void);		// ウェーブ開始
	void UpdateProgression(void);	// ウェーブ進行
	void InitWaveEnd(void);			// 季節の終了初期化
	void UpdateWaveEnd(void);		// 季節の終了

	// 静的メンバ変数
	static const char *mc_apTextureFile[];		// テクスチャ定数
	static const char *mc_apModelFile[];		// モデル定数
	static const char *mc_apSeasonModelFile[];	// シーズン表記モデル定数
	static Season m_aWaveInfo[SEASON_MAX];		// ウェーブ情報

	// メンバ変数
	CModelUI *m_apModelUI[NUM_MODEL_UI];	// シーズン表示の情報
	CObject2D *m_pObject2D;	// ウェーブ表示の情報
	CValue *m_pValue;		// ウェーブ数表示の情報
	STATE m_state;			// 状態
	int m_nSeason;			// 季節管理カウンター
	int m_nWave;			// ウェーブ管理カウンター
	int m_nPoint;			// 出現管理カウンター
	int m_nCounterState;	// 状態管理カウンター
	int m_nCounterFrame;	// ウェーブ余韻管理カウンター
};

#endif	// _WAVEMANAGER_H_
