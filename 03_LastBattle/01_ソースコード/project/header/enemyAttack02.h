//============================================================
//
//	敵攻撃02ヘッダー [enemyAttack02.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_ATTACK02_H_
#define _ENEMY_ATTACK02_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack.h"

//************************************************************
//	クラス定義
//************************************************************
// 敵攻撃02クラス
/*
	@brief ボスの攻撃管理クラス 雷を中心からステージ外周に向かって放つ
*/
class CEnemyAttack02 : public CEnemyAttack
{
public:
	// 状態列挙
	enum EState
	{
		STATE_INIT_TELEPORT = 0,	// テレポートの初期化
		STATE_TELEPORT,				// テレポート
		STATE_INIT_WAIT,			// 攻撃待機の初期化
		STATE_WAIT,					// 攻撃待機
		STATE_THUNDER,				// 雷発射
		STATE_END,					// 終了
		STATE_MAX					// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemyAttack02(CEnemyBossDragon *pBoss);

	// デストラクタ
	~CEnemyAttack02() override;

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
	// メンバ関数
	void InitTeleport(void);	// テレポートの初期化
	void UpdateThunder(void);	// 雷発射の更新

	// メンバ変数
	EState m_state;			// 状態
	int m_nCounterWait;		// 余韻管理カウンター
	int m_nCounterNumAtk;	// 攻撃回数カウンター
	int m_nWaitFrame;		// 余韻フレーム数
};

#endif	// _ENEMY_ATTACK02_H_
