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
#include "main.h"
#include "objectChara.h"

//************************************************************
//	前方宣言
//************************************************************
class CShadow;			// 影クラス
class CObjectBillboard;	// オブジェクトビルボードクラス
class CEnemyWay;		// 敵方向表示クラス
class CBubble;			// バブルクラス

//************************************************************
//	クラス定義
//************************************************************
// 敵クラス
class CEnemy : public CObjectChara
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_WARNING = 0,	// 警告表示
		TEXTURE_MAX				// この列挙型の総数
	}TEXTURE;

	// 種類列挙
	typedef enum
	{
		TYPE_HUMAN = 0,	// 歩兵
		TYPE_CAR,		// 戦車
		TYPE_MAX		// この列挙型の総数
	}TYPE;

	// 状態列挙
	typedef enum
	{
		STATE_SPAWN = 0,	// スポーン状態
		STATE_NORMAL,		// 通常状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_DEATH,		// 死亡状態
		STATE_VANISH,		// 消失状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CEnemy(const TYPE type);

	// デストラクタ
	~CEnemy();

	// パーツ情報構造体
	typedef struct
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
		int nParentID;		// 親インデックス
	}Parts;

	// パーツ構造体
	typedef struct
	{
		Parts aInfo[MAX_PARTS];	// パーツ情報
		int nNumParts;			// パーツ数
	}PartsInfo;

	// ステータス構造体
	typedef struct
	{
		D3DXVECTOR3 bullPos;		// 弾の発射位置
		D3DXVECTOR3 bubbleSize;		// バブルの大きさ
		int		nLife;				// 体力
		int		nScore;				// スコア加算量
		int		nExp;				// 経験値生成量
		int		nCounterAttack;		// 攻撃頻度
		int		nBullParts;			// 弾の発射パーツ
		int		nBullLife;			// 弾の寿命
		int		nBullDamage;		// 弾の攻撃力
		float	fBullRadius;		// 弾の半径
		float	fBullMove;			// 弾の移動量
		float	fShadowRadius;		// 影の半径
		float	fSpawnHeight;		// 生成するY座標
		float	fCollRadius;		// 当たり判定の半径
		float	fRadius;			// 半径
		float	fHeight;			// 縦幅
		float	fKnockBackUp;		// ノックバック上移動量
		float	fKnockBackSide;		// ノックバック横移動量
		float	fKnockBackRevision;	// ノックバック減衰係数
		float	fForwardMove;		// 前進の移動量
		float	fLookRevision;		// プレイヤー方向を向く補正係数
		float	fAttackRadius;		// 攻撃範囲
	}StatusInfo;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Hit(const int nDmg);	// ヒット
	void HitKnockBack(const int nDmg, const D3DXVECTOR3& vec);	// ノックバックヒット

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CEnemy *Create	// 生成
	( // 引数
		const TYPE type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot		// 向き
	);
	static void RandomSpawn	// ランダム生成
	( // 引数
		const int nNum,	// 生成数
		const TYPE type	// 種類
	);
	static StatusInfo GetStatusInfo(const int nType);	// ステータス情報取得
	static void SetAllVanish(void);	// 全消失
	static int GetNumAll(void);		// 総数取得

	// メンバ関数
	void UpdateOldPosition(void);					// 過去位置更新
	void UpdateKnockBack(D3DXVECTOR3& rPos);		// ノックバック更新
	void SetMovePosition(const D3DXVECTOR3& rMove);	// 位置移動量設定
	void SetMoveRotation(const D3DXVECTOR3& rMove);	// 向き変更量設定
	void SetState(const int nState);				// 状態設定
	D3DXMATRIX GetMtxWorld(void) const;				// マトリックス取得
	D3DXVECTOR3 GetOldPosition(void) const;			// 過去位置取得
	D3DXVECTOR3 GetMovePosition(void) const;		// 位置移動量取得
	D3DXVECTOR3 GetMoveRotation(void) const;		// 向き変更量取得
	int GetType(void) const;						// 種類取得
	int GetState(void) const;						// 状態取得
	float GetRadius(void) const;					// 半径取得
	float GetHeight(void) const;					// 縦幅取得
	StatusInfo GetStatusInfo(void) const;			// ステータス情報取得
	PartsInfo GetPartsInfo(void) const;				// パーツ情報取得
	CMotion::Info GetMotionInfo(void) const;		// モーション情報取得

protected:
	// 純粋仮想関数
	virtual const char* GetModelFileName(const int nModel) const = 0;	// モデルファイル取得
	virtual void CollisionFind(void) = 0;	// 検知範囲の当たり判定

	// 仮想関数
	virtual void Spawn(void);	// スポーン動作
	virtual void Damage(void);	// ダメージ動作
	virtual bool Death(void);	// 死亡動作
	virtual bool Vanish(void);	// 消失動作

	// メンバ関数
	void Look	// 対象視認
	( // 引数
		const D3DXVECTOR3& rPosLook,	// 視認対象位置
		const D3DXVECTOR3& rPosEnemy,	// 敵位置
		D3DXVECTOR3& rRotEnemy			// 敵向き
	);

	void Attack(const D3DXVECTOR3& rLookPos, const D3DXVECTOR3& rThisPos, const float fLookRadius);	// 攻撃
	void CollisionTarget(D3DXVECTOR3& rPos);			// ターゲットとの当たり判定
	void CollisionSpawnEnemy(D3DXVECTOR3& rPos);		// スポーン時の敵との当たり判定
	void CollisionNormalEnemy(D3DXVECTOR3& rPos);		// 通常時の敵との当たり判定
	void SetDrawWarning(const bool bDraw);				// 警告の描画状況設定
	void SetPositionWarning(const D3DXVECTOR3& rPos);	// 警告の位置設定
	void RandomRotation(void);	// ランダム回転
	void SubBubble(void);		// バブル削除

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];			// テクスチャ定数
	static StatusInfo m_aStatusInfo[TYPE_MAX];		// ステータス情報
	static PartsInfo m_aPartsInfo[TYPE_MAX];		// パーツ情報
	static CMotion::Info m_aMotionInfo[TYPE_MAX];	// モーション情報
	static int m_nNumAll;							// 敵の総数

	// メンバ変数
	CShadow *m_pShadow;				// 影の情報
	CObjectBillboard *m_pWarning;	// 警告の情報
	CEnemyWay *m_pWay;				// 方向の情報
	CBubble *m_pBubble;				// バブルの情報
	D3DXVECTOR3	m_oldPos;			// 過去位置
	D3DXVECTOR3	m_movePos;			// 位置移動量
	D3DXVECTOR3 m_moveKnock;		// ノックバック移動量
	D3DXVECTOR3	m_moveRot;			// 向き変更量
	D3DXVECTOR3 m_deathMoveRot;		// 死亡時の向き変更量
	STATE m_state;					// 状態
	int m_nCounterEffect;			// エフェクト管理カウンター
	int m_nCounterBubble;			// バブル管理カウンター
	int m_nCounterState;			// 状態管理カウンター
	int m_nCounterAtk;				// 攻撃管理カウンター
	const TYPE m_type;				// 種類定数
	const StatusInfo m_status;		// ステータス定数
	const PartsInfo m_parts;		// パーツ定数
	const CMotion::Info m_motion;	// モーション定数
};

// 歩兵敵クラス
class CEnemyHuman : public CEnemy
{
public:
	// 種類列挙
	typedef enum
	{
		MODEL_WAIST,				// 腰
		MODEL_BODY,					// 体
		MODEL_HEAD,					// 頭
		MODEL_ARMUL,				// 左上腕
		MODEL_ARMUR,				// 右上腕
		MODEL_ARMDL,				// 左下腕
		MODEL_ARMDR,				// 右下腕
		MODEL_HANDL,				// 左手
		MODEL_HANDR,				// 右手
		MODEL_LEGUL,				// 左太もも
		MODEL_LEGUR,				// 右太もも
		MODEL_LEGDL,				// 左脛
		MODEL_LEGDR,				// 右脛
		MODEL_FOOTL,				// 左足
		MODEL_FOOTR,				// 右足
		MODEL_WAISTPOUCHBACK,		// ウエストポーチ(後)
		MODEL_WAISTPOUCHRIGHT,		// ウエストポーチ(左)
		MODEL_WAISTPOUCHBACKRIGHT,	// ウエストポーチ(左後)
		MODEL_LEGURPOUCH,			// 右太ももポーチ
		MODEL_ARMOR,				// アーマー
		MODEL_KNIFE,				// ナイフ
		MODEL_ASSAULT,				// 銃
		MODEL_MAX					// この列挙型の総数
	}MODEL;

	// モーション列挙
	typedef enum
	{
		MOTION_MOVE = 0,	// 歩行モーション
		MOTION_ACTION,		// 攻撃モーション
		MOTION_MAX			// この列挙型の総数
	}MOTION;

	// コンストラクタ
	CEnemyHuman(const TYPE type);

	// デストラクタ
	~CEnemyHuman();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	const char* GetModelFileName(const int nModel) const;	// モデルファイル取得

private:
	// オーバーライド関数
	void Spawn(void);			// スポーン動作
	void CollisionFind(void);	// 検知範囲の当たり判定

	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数
};

// 戦車敵クラス
class CEnemyCar : public CEnemy
{
public:
	// 種類列挙
	typedef enum
	{
		MODEL_CATERPILLAR = 0,	// キャタピラ
		MODEL_CANNON,			// キャノン
		MODEL_MAX				// この列挙型の総数
	}MODEL;

	// スポーン状態列挙
	typedef enum
	{
		SPAWN_WAIT = 0,	// 待機状態
		SPAWN_FALL,		// 落下状態
		SPAWN_MAX		// この列挙型の総数
	}SPAWN;

	// コンストラクタ
	CEnemyCar(const TYPE type);

	// デストラクタ
	~CEnemyCar();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	const char* GetModelFileName(const int nModel) const;	// モデルファイル取得

private:
	// オーバーライド関数
	void Spawn(void);			// スポーン動作
	void CollisionFind(void);	// 検知範囲の当たり判定

	// メンバ関数
	bool SetRotationCannon		// キャノン向き設定
	( // 引数
		const D3DXVECTOR3& rLookPos,	// 視認対象位置
		const D3DXVECTOR3& rEnemyPos,	// 敵位置
		const D3DXVECTOR3& rEnemyRot	// 敵向き
	);

	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	SPAWN m_spawn;			// スポーン状態
	int m_nCounterSpawn;	// スポーン管理カウンター
};

#endif	// _ENEMY_H_
