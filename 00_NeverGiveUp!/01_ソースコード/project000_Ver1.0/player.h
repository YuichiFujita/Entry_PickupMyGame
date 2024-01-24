//============================================================
//
//	プレイヤーヘッダー [player.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectChara.h"
#include "scene.h"
#include "retentionManager.h"

//************************************************************
//	マクロ定義
//************************************************************
#define NUM_ORBIT	(4)	// 軌跡の生成数

//************************************************************
//	前方宣言
//************************************************************
class CObjectOrbit;	// 軌跡クラス
class CShadow;		// 影クラス
class CGoalPoint;	// ゴールポイントクラス
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// プレイヤークラス
class CPlayer : public CObjectChara
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_CLEAR = 0,	// クリア表示
		TEXTURE_MAX			// この列挙型の総数
	};

	// モデル列挙
	enum EModel
	{
		MODEL_WAIST = 0,	// 腰
		MODEL_BODY,			// 体
		MODEL_HEAD,			// 頭
		MODEL_ARMUL,		// 左上腕
		MODEL_ARMUR,		// 右上腕
		MODEL_ARMDL,		// 左下腕
		MODEL_ARMDR,		// 右下腕
		MODEL_HANDL,		// 左手
		MODEL_HANDR,		// 右手
		MODEL_LEGUL,		// 左太もも
		MODEL_LEGUR,		// 右太もも
		MODEL_LEGDL,		// 左脛
		MODEL_LEGDR,		// 右脛
		MODEL_FOOTL,		// 左足
		MODEL_FOOTR,		// 右足
		MODEL_MAX			// この列挙型の総数
	};

	// モーション列挙
	enum EMotion
	{
		MOTION_IDOL = 0,	// 待機モーション
		MOTION_MOVE,		// 移動モーション
		MOTION_JUMP,		// ジャンプモーション
		MOTION_SLIDE,		// スライディングモーション
		MOTION_WALLDASH,	// 壁走りモーション
		MOTION_FALL,		// 落下モーション
		MOTION_WAVEHAND,	// 手振りモーション
		MOTION_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_SPAWN,	// スポーン状態
		STATE_NORMAL,	// 通常状態
		STATE_DAMAGE,	// ダメージ状態
		STATE_UNION,	// 合流状態
		STATE_CLEAR,	// ゲームクリア状態
		STATE_OVER,		// ゲームオーバー状態
		STATE_MAX		// この列挙型の総数
	};

	// 着地列挙
	enum ELand
	{
		LAND_NONE = 0,	// 着地していない
		LAND_BUILDING,	// ビル着地
		LAND_OBSTACLE,	// 障害物着地
		LAND_OTHER,		// その他着地
		LAND_MAX		// この列挙型の総数
	};

	// 軸列挙
	enum EAxis
	{
		AXIS_X = 0,	// X軸
		AXIS_Y,		// Y軸
		AXIS_Z,		// Z軸
		AXIS_MAX	// この列挙型の総数
	};

	// 方向列挙
	enum EDirection
	{
		DIRECTION_NONE = 0,	// 方向無し
		DIRECTION_DOWN,		// 方向：0°
		DIRECTION_RIGHT,	// 方向：90°
		DIRECTION_UP,		// 方向：180°
		DIRECTION_LEFT,		// 方向：270°
		DIRECTION_MAX		// この列挙型の総数
	};
	
	// コンストラクタ
	CPlayer();

	// デストラクタ
	~CPlayer();

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画
	void Hit(void) override;		// ヒット

	void SetState(const int nState) override;	// 状態設定
	int GetState(void) const override;			// 状態取得
	float GetRadius(void) const override;		// 半径取得
	float GetHeight(void) const override;		// 縦幅取得

	void SetEnableUpdate(const bool bUpdate) override;	// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;		// 描画状況設定
	D3DXMATRIX GetMtxWorld(void) const override;		// マトリックス取得

	// 静的メンバ関数
	static CPlayer *Create(CScene::EMode mode);	// 生成

	// メンバ関数
	void SetSpawn(void);	// 出現設定
	void SetGoal(CGoalPoint *pGoal);	// ゴール設定

private:
	// メンバ関数
	void LoadSetup(void);		// セットアップ
	EMotion UpdateSpawn(void);	// スポーン状態時の更新
	EMotion UpdateNormal(void);	// 通常状態時の更新
	EMotion UpdateDamage(void);	// ダメージ状態時の更新
	EMotion UpdateUnion(void);	// 合流状態時の更新
	void UpdateClear(void);		// ゲームクリア状態時の更新

	void UpdateOldPosition(void);			// 過去位置の更新
	EMotion UpdateMove(D3DXVECTOR3& rPos);	// 移動量・壁走りの更新
	void UpdateWallDash(D3DXVECTOR3& rPos);	// 壁走りの更新
	void UpdateJump(void);					// ジャンプの更新
	void UpdateSliding(void);				// スライディングの更新
	void UpdateGravity(void);				// 重力の更新
	bool UpdateLanding(D3DXVECTOR3& rPos);	// 着地状況の更新
	void UpdateRotation(D3DXVECTOR3& rRot);	// 向きの更新
	void UpdateEffect(D3DXVECTOR3& rPos);	// エフェクトの更新
	void UpdateMotion(int nMotion);			// モーション・オブジェクトキャラクターの更新
	bool UpdateFadeOut(const float fAdd);	// フェードアウト状態時の更新
	bool UpdateFadeIn(const float fSub);	// フェードイン状態時の更新

	void ResultTransition(const CRetentionManager::EResult result);		// リザルト遷移
	EDirection FourDirection(const float fRot, const float fTolerance);	// 四方向変換
	void CollisionRotationBuilding(D3DXVECTOR3& rPos);					// 向き変更の当たり判定
	bool ResponseSingleBuilding(const EAxis axis, D3DXVECTOR3& rPos);	// ビルとの一軸ごとの当たり判定
	bool CollisionSignboard(D3DXVECTOR3& rPos, float *pRate = NULL);	// 看板との当たり判定
	void CollisionObstacle(D3DXVECTOR3& rPos);	// 障害物との当たり判定
	bool LandObstacle(D3DXVECTOR3& rPos);		// 障害物との着地判定
	bool CollisionBuilding(D3DXVECTOR3& rPos);	// ビルとの当たり判定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	CObjectOrbit *m_apOrbit[NUM_ORBIT];	// 軌跡の情報
	CShadow *m_pShadow;			// 影の情報
	CGoalPoint *m_pGoal;		// ゴールの情報
	CObject2D *m_pClear;		// クリア表示の情報
	D3DXVECTOR3	m_oldPos;		// 過去位置
	D3DXVECTOR3	m_move;			// 移動量
	D3DXVECTOR3	m_destRot;		// 目標向き
	ELand	m_land;				// 着地物
	EState	m_state;			// 状態
	int		m_nCounterState;	// 状態管理カウンター
	int		m_nCounterSlide;	// スライディング管理カウンター
	int		m_nCounterWallDash;	// 壁走り管理カウンター
	int		m_nCounterLand;		// 着地管理カウンター
	int		m_nCounterJump;		// ジャンプ管理カウンター
	float	m_fMove;			// 移動量
	float	m_fPlusMove;		// プラス移動量
	bool	m_bJump;			// ジャンプ状況
	bool	m_bJumpControl;		// ジャンプ操作状況
	bool	m_bSlide;			// スライディング状況
	bool	m_bSlideControl;	// スライディング操作状況
	bool	m_bWallDash;		// 壁走り状況
	bool	m_bWallDashControl;	// 壁走り操作状況
};

#endif	// _PLAYER_H_
