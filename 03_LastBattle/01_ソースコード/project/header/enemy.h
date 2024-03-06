//============================================================
//
//	敵ヘッダー [enemy.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_H_
#define _ENEMY_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectChara.h"
#include "motion.h"

//************************************************************
//	前方宣言
//************************************************************
class CCollSphere;	// 円判定クラス

//************************************************************
//	クラス定義
//************************************************************
// 敵クラス
class CEnemy : public CObjectChara
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_BOSS_DRAGON = 0,	// ボスドラゴン
		TYPE_MINI_DRAGON,		// ミニドラゴン
		TYPE_MAX				// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// なにもしない状態
		STATE_SPAWN,		// スポーン状態
		STATE_NORMAL,		// 通常状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_INVULN,		// 無敵状態
		STATE_KNOCK,		// ノックバック状態
		STATE_STAN,			// スタン状態
		STATE_RIDE_FLYUP,	// ライド飛び上がり状態
		STATE_RIDE_ROTATE,	// ライド旋回状態
		STATE_RIDE_END,		// ライド終了状態
		STATE_DEATH,		// 死亡状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemy(const EType type);

	// デストラクタ
	virtual ~CEnemy();

	// ステータス構造体
	struct SStatusInfo
	{
		int   nMaxLife;		// 最大体力
		float fRadius;		// 半径
		float fHeight;		// 縦幅
		float fLookRev;		// 視認の補正係数
		float fCollRadius;	// 判定半径
	};

	// パーツ情報構造体
	struct SParts
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
		int nParentID;		// 親インデックス
	};

	// パーツ構造体
	struct SPartsInfo
	{
		SParts aInfo[motion::MAX_PARTS];	// パーツ情報
		int nNumParts;	// パーツ数
	};

	// 当たり判定構造体
	struct SCollInfo
	{
		CCollSphere *apInfo[motion::MAX_PARTS];	// 判定情報
		int nNumColl;	// 判定数
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画
	void SetState(const int nState) override;		// 状態設定
	int GetState(void) const override;				// 状態取得
	int GetType(void) const override;				// 種類取得
	float GetRadius(void) const override;			// 半径取得
	float GetHeight(void) const override;			// 縦幅取得

	// 純粋仮想関数
	virtual int GetWaistModelID(void) const = 0;	// 腰モデルのインデックス取得
	virtual int GetHeadModelID(void) const = 0;		// 頭モデルのインデックス取得

	// 仮想関数
	virtual void InitNormal(void);	// 通常状態の初期化
	virtual void DrawCrop(void);	// 切り抜き用の描画
	virtual bool Hit(const int nDamage);					// ヒット
	virtual void SetEnableDrawUI(const bool bDraw);			// UI描画設定
	virtual void SetLifePriority(const int nPrio);			// 体力ゲージの優先順位設定
	virtual bool IsRideOK(const D3DXVECTOR3& rPos) const;	// ライド可能か取得

	// 静的メンバ関数
	static CEnemy *Create	// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot		// 向き
	);
	static CListManager<CEnemy> *GetList(void);			// リスト取得
	static SStatusInfo GetStatusInfo(const int nType);	// ステータス情報取得
	static void ReleaseSetup(void);						// セットアップ破棄
	static void LoadAllSetup(void);						// 全セットアップ
	static void LoadSetup(const EType typeID);			// セットアップ

	// メンバ関数
	void UpdateOldPosition(void);					// 過去位置更新
	D3DXVECTOR3 GetOldPosition(void) const;			// 過去位置取得
	void SetDestRotation(const D3DXVECTOR3& rRot);	// 目標向き設定
	D3DXVECTOR3 GetDestRotation(void) const;		// 目標向き取得
	void SetMovePosition(const D3DXVECTOR3& rMove);	// 位置移動量設定
	D3DXVECTOR3 GetMovePosition(void) const;		// 位置移動量取得
	SStatusInfo GetStatusInfo(void) const;			// ステータス情報取得
	SPartsInfo GetPartsInfo(void) const;			// パーツ情報取得
	CMotion::SInfo GetMotionInfo(void) const;		// モーション情報取得

protected:
	// 純粋仮想関数
	virtual const char *GetModelFileName(const int nModel) const = 0;	// モデルファイル取得
	virtual void UpdateMotion(void) = 0;	// モーションの更新

	// 仮想関数
	virtual void SetSpawn(void);			// スポーン状態の設定
	virtual void SetInvuln(void);			// 無敵状態の設定
	virtual void SetKnock(void);			// ノックバック状態の設定
	virtual void SetStan(void);				// スタン状態の設定
	virtual void SetRideFlyUp(void);		// ライド飛び上がり状態の設定
	virtual void SetRideRotate(void);		// ライド旋回状態の設定
	virtual void SetRideEnd(void);			// ライド終了状態の設定
	virtual void SetDeath(void);			// 死亡状態の設定
	virtual void UpdateNone(void);			// なにもしない状態時の更新
	virtual void UpdateSpawn(void);			// スポーン状態時の更新
	virtual void UpdateNormal(void);		// 通常状態時の更新
	virtual void UpdateDamage(void);		// ダメージ状態時の更新
	virtual void UpdateInvuln(void);		// 無敵状態時の更新
	virtual void UpdateKnock(void);			// ノックバック状態時の更新
	virtual void UpdateStan(void);			// スタン状態時の更新
	virtual void UpdateRideFlyUp(void);		// ライド飛び上がり状態時の更新
	virtual void UpdateRideRotate(void);	// ライド旋回状態時の更新
	virtual void UpdateRideEnd(void);		// ライド終了状態時の更新
	virtual void UpdateDeath(void);			// 死亡状態時の更新

	// メンバ関数
	void UpdateLook	// 対象視認
	( // 引数
		const D3DXVECTOR3 &rPosLook,	// 視認対象位置
		const D3DXVECTOR3 &rPosEnemy,	// 敵位置
		D3DXVECTOR3 *pRotEnemy			// 敵向き
	);
	void UpdateGravity(void);				// 重力の更新
	bool UpdateLanding(D3DXVECTOR3 *pPos);	// 着地状況の更新
	void UpdatePosition(D3DXVECTOR3 *pPos);	// 位置の更新
	void UpdateRotation	// 向きの更新
	( // 引数
		D3DXVECTOR3 *pRot,		// 向き
		D3DXVECTOR3 *pDestRot	// 目標向き
	);
	bool UpdateFadeOut(const float fAdd);	// フェードアウト状態時の更新
	bool UpdateFadeIn(const float fSub);	// フェードイン状態時の更新
	void SetPrevState(const EState state) { m_prevState = state; }	// ダメージを受ける前の状態設定

private:
	// 状態更新の関数ポインタ型エイリアス定義
	typedef void (CEnemy::*AFuncUpdateState)(void);

	// 静的メンバ変数
	static CListManager<CEnemy> *m_pList;			// オブジェクトリスト
	static CMotion::SInfo m_aMotionInfo[TYPE_MAX];	// モーション情報
	static SPartsInfo m_aPartsInfo[TYPE_MAX];		// パーツ情報
	static SStatusInfo m_aStatusInfo[TYPE_MAX];		// ステータス情報
	static SCollInfo m_aCollInfo[TYPE_MAX];			// 当たり判定情報
	static AFuncUpdateState m_aFuncUpdateState[];	// 状態更新関数

	// メンバ変数
	CListManager<CEnemy>::AIterator m_iterator;	// イテレーター
	D3DXVECTOR3	m_oldPos;			// 過去位置
	D3DXVECTOR3	m_destRot;			// 目標向き
	D3DXVECTOR3	m_move;				// 移動量
	EState	m_state;				// 状態
	EState	m_prevState;			// ダメージを受ける前の状態
	int		m_nCounterState;		// 状態管理カウンター
	float	m_fSinAlpha;			// 透明向き
	bool	m_bJump;				// ジャンプ状況
	const EType m_type;				// 種類定数
	const SStatusInfo m_status;		// ステータス定数
	const SPartsInfo m_parts;		// パーツ定数
	const CMotion::SInfo m_motion;	// モーション定数
};

#endif	// _ENEMY_H_
