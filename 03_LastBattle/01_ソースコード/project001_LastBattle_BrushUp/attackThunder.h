//============================================================
//
//	雷攻撃ヘッダー [attackThunder.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ATTACK_THUNDER_H_
#define _ATTACK_THUNDER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	定数宣言
//************************************************************
namespace attackThunder
{
	const int NUM_THUNDER	= 3;	// 雷の生成数
	const int WARN_FRAME	= 80;	// 警告表示持続フレーム
}

//************************************************************
//	前方宣言
//************************************************************
class CThunder;		// 雷クラス
class CObject3D;	// オブジェクト3Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 雷攻撃クラス
class CAttackThunder : public CObject
{
public:
	// 状態列挙
	enum EState
	{
		STATE_WARN = 0,	// 警告表示
		STATE_ATTACK,	// 攻撃開始
		STATE_MAX		// この列挙の総数
	};

	// コンストラクタ
	CAttackThunder();

	// デストラクタ
	~CAttackThunder() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CAttackThunder *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,							// 位置
		const bool bSound,									// 効果音再生フラグ
		const int nWarnFrame = attackThunder::WARN_FRAME	// 警告表示フレーム数
	);
	static CListManager<CAttackThunder> *GetList(void);		// リスト取得

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ関数
	void SetOriginPosition(const D3DXVECTOR3 &rPos);	// 原点位置設定
	void SetThunderPosition(const D3DXVECTOR3 &rPos);	// 雷位置設定
	HRESULT SetAttack(void);		// 攻撃状態の設定
	void CollisionPlayer(void);		// プレイヤーとの当たり判定

	// 静的メンバ変数
	static CListManager<CAttackThunder> *m_pList;	// オブジェクトリスト

	// メンバ変数
	CListManager<CAttackThunder>::AIterator m_iterator;	// イテレーター
	CThunder *m_apThunder[attackThunder::NUM_THUNDER];	// 雷の情報
	CObject3D *m_pWarning;		// 警告表示の情報
	D3DXVECTOR3 m_posOrigin;	// 雷の原点位置
	EState m_state;				// 状態
	int m_nCounterState;		// 状態管理カウンター
	int m_nWarnFrame;			// 警告表示フレーム数
	bool m_bSound;				// 効果音再生フラグ
};

#endif	// _ATTACK_THUNDER_H_
