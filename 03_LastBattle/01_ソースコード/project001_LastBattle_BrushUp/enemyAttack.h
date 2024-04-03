//============================================================
//
//	敵攻撃ヘッダー [enemyAttack.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_ATTACK_H_
#define _ENEMY_ATTACK_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "random.h"

//************************************************************
//	前方宣言
//************************************************************
class CEnemyBossDragon;	// ボスドラゴンクラス

//************************************************************
//	クラス定義
//************************************************************
// 敵攻撃クラス
class CEnemyAttack
{
public:
	// 攻撃列挙
	enum EAttack
	{
		ATTACK_00 = 0,		// 攻撃00(地面殴り波動)
		ATTACK_01,			// 攻撃01(雷ランダム生成)
		ATTACK_02,			// 攻撃02(雷外周向かい生成)
		ATTACK_03,			// 攻撃03(雷プレイヤー位置生成)
		ATTACK_04,			// 攻撃04(炎外周吐き出し)
		ATTACK_05,			// 攻撃05(ひっかき攻撃)
		ATTACK_06,			// 攻撃06(しっぽ攻撃)
		ATTACK_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemyAttack(CEnemyBossDragon *pBoss);

	// デストラクタ
	virtual ~CEnemyAttack();

	// 純粋仮想関数
	virtual bool Update(void) = 0;	// 更新
	virtual void SetRandomArray		// 攻撃選択肢の要素設定
	( // 引数
		CRandom<EAttack> *pRandom,	// ランダム攻撃クラス
		EAttack oldAtk,				// 前回の攻撃
		int nSameAct				// 同じ行動の連続数
	) = 0;

	// 仮想関数
	virtual HRESULT Init(void);	// 初期化
	virtual void Uninit(void);	// 終了

	// 静的メンバ関数
	static CEnemyAttack *Create	// 生成
	( // 引数
		const EAttack attackID,	// 攻撃インデックス
		CEnemyBossDragon *pBoss	// ボスへのポインタ
	);
	static void Release(CEnemyAttack *&prEnemyAttack);	// 破棄

	// メンバ関数
	CEnemyBossDragon *GetBoss(void);	// ボス取得

private:
	// メンバ変数
	CEnemyBossDragon *m_pBoss;	// ボスへのポインタ
};

#endif	// _ENEMY_ATTACK_H_
