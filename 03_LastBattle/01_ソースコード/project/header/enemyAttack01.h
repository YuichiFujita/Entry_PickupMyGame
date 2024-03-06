//============================================================
//
//	敵攻撃01ヘッダー [enemyAttack01.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_ATTACK01_H_
#define _ENEMY_ATTACK01_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack.h"

//************************************************************
//	クラス定義
//************************************************************
// 敵攻撃01クラス
/*
	@brief ボスの攻撃管理クラス 雷をプレイヤー付近にランダムで放つ
*/
class CEnemyAttack01 : public CEnemyAttack
{
public:
	// 状態列挙
	enum EState
	{
		STATE_INIT_TELEPORT = 0,	// テレポートの初期化
		STATE_TELEPORT,				// テレポート
		STATE_INIT_THUNDER,			// 雷発射の初期化
		STATE_THUNDER,				// 雷発射
		STATE_WAIT,					// 待機
		STATE_CENTER_TELEPORT_INIT,	// 中央テレポートの初期化
		STATE_CENTER_TELEPORT,		// 中央テレポート
		STATE_END,					// 終了
		STATE_MAX					// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemyAttack01(CEnemyBossDragon *pBoss);

	// デストラクタ
	~CEnemyAttack01() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	bool Update(void) override;		// 更新
	void SetRandomArray				// 攻撃選択肢の要素設定
	( // 引数
		CRandom<EAttack> *pRandom,	// ランダム攻撃クラス
		EAttack oldAtk,				// 前回の攻撃
		int nSameAct				// 同じ行動の連続数
	) override;

private:
	// メンバ変数
	EState m_state;			// 状態
	int m_nCounterState;	// 状態管理カウンター
	int m_nCounterNumAtk;	// 攻撃回数カウンター
};

#endif	// _ENEMY_ATTACK01_H_
