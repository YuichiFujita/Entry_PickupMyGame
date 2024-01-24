//============================================================
//
//	魔法マネージャーヘッダー [magicManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MAGICMANAGER_H_
#define _MAGICMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "magic.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectGauge3D;	// オブジェクトゲージ3Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 魔法マネージャークラス
class CMagicManager
{
public:
	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_ATTACK,		// 攻撃状態
		STATE_HEAL,			// 回復状態
		STATE_MAX,			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CMagicManager();

	// デストラクタ
	~CMagicManager();

	// 射撃状況構造体
	typedef struct
	{
		float fRot;		// 発射向き
		bool bControl;	// 射撃操作が行われているか
		bool bShot;		// 実際に射撃できているか
	}Shot;

	// メンバ関数
	HRESULT Init(void);		// 初期化
	void Uninit(void);		// 終了
	void Update(void);		// 更新
	Shot ShotMagic(void);	// 魔法発射
	void SetEnableHealMana(const bool bHeal);	// マナ回復状況設定
	void SetEnableManaDraw(const bool bDraw);	// マナ描画状況設定
	void AddMana(const int nAdd);		// マナ加算
	void SetMana(const int nMana);		// マナ設定
	void SetMaxMana(const int nMana);	// 最大マナ設定
	int GetMana(void) const;			// マナ残量取得
	int GetMaxMana(void) const;			// 最大マナ取得

	// 静的メンバ関数
	static CMagicManager *Create(CObject *pPlayer, const int nMaxMana);	// 生成
	static HRESULT Release(CMagicManager *&prMagicManager);	// 破棄

private:
	// メンバ変数
	CObjectGauge3D *m_pMana;	// マナの情報
	STATE m_state;				// 状態
	int m_nCounterEffect;		// エフェクト管理カウンター
	int m_nCounterMagic;		// 魔法管理カウンター
	int m_nCounterState;		// 状態管理カウンター
	int m_nCounterHeal;			// 回復管理カウンター
	bool m_bHeal;				// 回復状況
};

#endif	// _MAGICMANAGER_H_
