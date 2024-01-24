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
#include "main.h"

//************************************************************
//	クラス定義
//************************************************************
// ゲームマネージャークラス
class CGameManager
{
public:
	// リザルト列挙
	enum RESULT
	{
		RESULT_NONE,	// 無し
		RESULT_FAILED,	// クリア失敗
		RESULT_CLEAR,	// クリア成功
		RESULT_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CGameManager();

	// デストラクタ
	~CGameManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新

	// 静的メンバ関数
	static CGameManager *Create(void);	// 生成
	static HRESULT Release(CGameManager *&prGameManager);	// 破棄

	// メンバ関数
	void SetResult(const RESULT result);	// クリア状況設定
	void SetScore(const int nScore);		// 獲得スコア設定
	void SetTime(const long nTime);			// 経過タイム設定
	RESULT GetResult(void) const;			// クリア状況取得
	int GetScore(void) const;				// 獲得スコア取得
	long GetTime(void) const;				// 経過タイム取得

private:
	// メンバ変数
	RESULT m_result;	// クリア状況
	int m_nScore;		// 獲得スコア
	long m_nTime;		// 経過タイム
};

#endif	// _GAMEMANAGER_H_
