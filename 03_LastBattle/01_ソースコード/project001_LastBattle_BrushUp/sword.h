//============================================================
//
//	剣ヘッダー [sword.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SWORD_H_
#define _SWORD_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "multiModel.h"

//************************************************************
//	前方宣言
//************************************************************
class CSwordWaveManager;	// 剣波動生成クラス
class COrbit;	// 軌跡クラス

//************************************************************
//	クラス定義
//************************************************************
// 剣クラス
class CSword : public CMultiModel
{
public:
	// モデル列挙
	enum EModel
	{
		MODEL_SWORD = 0,	// 剣
		MODEL_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_NORMAL,	// 通常状態
		STATE_VANISH,	// 消失状態
		STATE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CSword();

	// デストラクタ
	~CSword() override;

	// 判定構造体
	struct SColl
	{
		D3DXVECTOR3 offset;	// 判定オフセット
		float fRadius;		// 判定半径
	};

	// 判定情報構造体
	struct SCollInfo
	{
		int nNumColl;	// 判定数
		SColl *pColl;	// 判定情報
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;		// 描画
	int GetState(void) const override;					// 状態取得

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CSword *Create	// 生成
	( // 引数
		CObject *pParent,						// 親オブジェクト
		const D3DXVECTOR3 &rPos = VEC3_ZERO,	// 位置
		const D3DXVECTOR3 &rRot = VEC3_ZERO		// 向き
	);

	// メンバ関数
	void SetState(const EState state);	// 状態設定
	void SetEnableAttack(const bool bAttack) { m_bAttack = bAttack; }		// 攻撃状況設定
	bool IsAttack(void) const				 { return m_bAttack; }			// 攻撃状況取得
	CSwordWaveManager *GetWaveManager(void)	 { return m_pWaveManager; }		// 剣波動生成マネージャー取得

private:
	// メンバ関数
	void CollisionEnemy(void);	// 敵との当たり判定

	// 静的メンバ変数
	static SCollInfo m_collInfo;	// 判定情報

	// メンバ変数
	CSwordWaveManager *m_pWaveManager;	// 剣波動生成マネージャー
	COrbit	*m_pOrbit;			// 軌跡の情報
	EState	m_state;			// 状態
	int		m_nCounterState;	// 状態管理カウンター
	bool	m_bAttack;			// 攻撃状況
};

#endif	// _SWORD_H_
