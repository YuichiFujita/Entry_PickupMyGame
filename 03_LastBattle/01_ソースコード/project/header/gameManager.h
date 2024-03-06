//============================================================
//
//	ゲームマネージャーヘッダー [gameManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "retentionManager.h"

//************************************************************
//	前方宣言
//************************************************************
class CModelFont;	// モデル文字クラス
class CSkip;		// スキップ表示クラス

//************************************************************
//	クラス定義
//************************************************************
// ゲームマネージャークラス
class CGameManager
{
public:
	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_START,	// 開始状態
		STATE_NORMAL,	// 通常状態
		STATE_STAGING,	// 演出状態
		STATE_MAX		// この列挙型の総数
	};

	// 開始状態列挙
	enum EStartState
	{
		START_PLAYER = 0,	// プレイヤースポーン状態
		START_BOSS,			// ボススポーン状態
		START_END,			// 終了状態
		START_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CGameManager();

	// デストラクタ
	~CGameManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetState(const EState state);		// 状態設定
	EState GetState(void) const;			// 状態取得
	void SetDrawGameUI(const bool bDraw);	// ゲーム画面のUI描画設定
	void TransitionResult(const CRetentionManager::EWin win);	// リザルト画面遷移

	// 静的メンバ関数
	static CGameManager *Create(void);	// 生成
	static void Release(CGameManager *&prGameManager);	// 破棄

private:
	// メンバ関数
	void UpdateStart(void);	// 開始状態の更新
	void EndStart(void);	// 開始演出の終了
	void SkipStart(void);	// 開始演出のスキップ

	// メンバ変数
	CModelFont	*m_pBossName;	// ボスの名前モデル情報
	CSkip		*m_pSkip;		// スキップ情報
	EState		m_state;		// 状態
	EStartState	m_startState;	// 開始状態
};

#endif	// _GAMEMANAGER_H_
