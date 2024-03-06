//============================================================
//
//	ボスドラゴンヘッダー [enemyBossDragon.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_BOSS_DRAGON_H_
#define _ENEMY_BOSS_DRAGON_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "enemy.h"
#include "enemyAttack.h"

//************************************************************
//	前方宣言
//************************************************************
class CGauge2D;		// ゲージ2Dクラス
class CShadow;		// 影クラス
class CEnemyAttack;	// 敵攻撃クラス
class CMagicCircle;	// 魔法陣クラス

//************************************************************
//	クラス定義
//************************************************************
// ボスドラゴンクラス
class CEnemyBossDragon : public CEnemy
{
public:
	// モデル列挙
	enum EModel
	{
		MODEL_WAIST = 0,	// 腰
		MODEL_BODY,			// 体
		MODEL_JAW,			// 顎
		MODEL_HEAD,			// 頭
		MODEL_ARM_UL,		// 左上腕
		MODEL_ARM_UR,		// 右上腕
		MODEL_ARM_DL,		// 左下腕
		MODEL_ARM_DR,		// 右下腕
		MODEL_HAND_L,		// 左手
		MODEL_HAND_R,		// 右手
		MODEL_LEG_UL,		// 左太もも
		MODEL_LEG_UR,		// 右太もも
		MODEL_LEG_DL,		// 左脛
		MODEL_LEG_DR,		// 右脛
		MODEL_FOOT_L,		// 左足
		MODEL_FOOT_R,		// 右足
		MODEL_WING_L_00,	// 左翼00
		MODEL_WING_R_00,	// 右翼00
		MODEL_WING_L_01,	// 左翼01
		MODEL_WING_R_01,	// 右翼01
		MODEL_WING_L_02,	// 左翼02
		MODEL_WING_R_02,	// 右翼02
		MODEL_WING_L_03,	// 左翼03
		MODEL_WING_R_03,	// 右翼03
		MODEL_TAIL_00,		// 尻尾00
		MODEL_TAIL_01,		// 尻尾01
		MODEL_TAIL_02,		// 尻尾02
		MODEL_TAIL_03,		// 尻尾03
		MODEL_TAIL_04,		// 尻尾04
		MODEL_TAIL_05,		// 尻尾05
		MODEL_MAX			// この列挙型の総数
	};

	// モーション列挙
	enum EMotion
	{
		MOTION_NONE = -1,		// モーション指定無し
		MOTION_HOWL,			// 咆哮モーション
		MOTION_IDOL,			// 待機モーション
		MOTION_FLY_IDOL,		// 空中待機モーション
		MOTION_PUNCH_GROUND,	// 地面殴りモーション
		MOTION_FIRE_ATTACK,		// 炎攻撃モーション
		MOTION_CLAW_ATTACK,		// ひっかき攻撃モーション
		MOTION_TAIL_ATTACK,		// しっぽ攻撃モーション
		MOTION_FLY_ATTACK,		// 空中攻撃モーション
		MOTION_FLY_RUSH,		// 空中突進攻撃モーション
		MOTION_KNOCK,			// ノックバックモーション
		MOTION_STAN,			// スタンモーション
		MOTION_HOWL_FLYUP,		// 咆哮飛び上がりモーション
		MOTION_SHAKE_OFF,		// 空中振り下ろしモーション
		MOTION_COME_DOWN,		// 上空復帰モーション
		MOTION_DEATH,			// 死亡モーション
		MOTION_MAX				// この列挙型の総数
	};

	// 行動列挙
	enum EAction
	{
		ACT_NONE = 0,		// 何もしない
		ACT_MAGIC_FADEIN,	// 魔法陣フェードイン
		ACT_MAGIC_FADEOUT,	// 魔法陣フェードアウト
		ACT_PUNCH_GROUND,	// 地面殴り
		ACT_FIRE_ATTACK,	// 炎攻撃
		ACT_CLAW_ATTACK,	// ひっかき攻撃
		ACT_TAIL_ATTACK,	// しっぽ攻撃
		ACT_FLY_ATTACK,		// 空中攻撃
		ACT_MAX				// この列挙型の総数
	};

	// テレポート列挙
	enum ETeleport
	{
		TELEPORT_INIT = 0,	// テレポートの初期化
		TELEPORT_APPEAR,	// 魔法陣の出現
		TELEPORT_MOVE,		// 魔法陣の移動
		TELEPORT_DISAPPEAR,	// 魔法陣の消失
		TELEPORT_MAX		// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemyBossDragon(const EType type);

	// デストラクタ
	~CEnemyBossDragon() override;

	// テレポート構造体
	struct STeleport
	{
		D3DXVECTOR3 pos;	// テレポート目標位置
		D3DXVECTOR3 rot;	// テレポート目標向き
		ETeleport state;	// テレポート状態
		EMotion motion;		// テレポート後モーション
		bool bLook;			// テレポート先にカメラを向かせるか
	};

	// ライド構造体
	struct SRide
	{
		float fDir;	// ステージ中央から見たボスの方向
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	bool Hit(const int nDamage) override;					// ヒット
	void SetEnableUpdate(const bool bUpdate) override;		// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;			// 描画状況設定
	int  GetWaistModelID(void) const override;				// 腰モデルのインデックス取得
	int  GetHeadModelID(void) const override;				// 頭モデルのインデックス取得
	bool IsRideOK(const D3DXVECTOR3& rPos) const override;	// ライド可能か取得
	void SetEnableDrawUI(const bool bDraw) override;		// UI描画設定
	void SetLifePriority(const int nPrio) override;			// 体力ゲージの優先順位設定

	// メンバ関数
	void InitNormal(void);	// 通常状態の初期化
	void DrawCrop(void);	// 切り抜き用の描画

	void SetTeleport	// テレポート設定
	( // 引数
		const D3DXVECTOR3& rPos,			// テレポート目標位置
		const D3DXVECTOR3& rRot,			// テレポート目標向き
		const EMotion motion = MOTION_IDOL,	// テレポート後モーション
		const bool bLook = true				// テレポート先にカメラを向かせるか
	);
	void SetActPunchGround(void);	// 地面殴りの行動設定
	void SetActFireAttack(void);	// 炎攻撃の行動設定
	void SetActClawAttack(void);	// ひっかき攻撃の行動設定
	void SetActTailAttack(void);	// しっぽ攻撃の行動設定
	void SetActFlyAttack(void);		// 空中攻撃の行動設定
	EAction GetAction(void);		// 行動取得

private:
	// オーバーライド関数
	const char *GetModelFileName(const int nModel) const override;	// モデルファイル取得
	void UpdateMotion(void) override;		// モーションの更新
	void SetSpawn(void) override;			// スポーン状態の設定
	void SetKnock(void) override;			// ノックバック状態の設定
	void SetStan(void) override;			// スタン状態の設定
	void SetRideFlyUp(void) override;		// ライド飛び上がり状態の設定
	void SetRideRotate(void) override;		// ライド旋回状態の設定
	void SetRideEnd(void) override;			// ライド終了状態の設定
	void SetDeath(void) override;			// 死亡状態の設定
	void UpdateSpawn(void) override;		// スポーン状態時の更新
	void UpdateNormal(void) override;		// 通常状態時の更新
	void UpdateKnock(void) override;		// ノックバック状態時の更新
	void UpdateStan(void) override;			// スタン状態時の更新
	void UpdateRideFlyUp(void) override;	// ライド飛び上がり状態時の更新
	void UpdateRideRotate(void) override;	// ライド旋回状態時の更新
	void UpdateRideEnd(void) override;		// ライド終了状態時の更新
	void UpdateDeath(void) override;		// 死亡状態時の更新

	// メンバ関数
	void LimitPosition(D3DXVECTOR3 *pPos);	// 位置範囲外の補正
	void SelectAttack(void);	// 攻撃選択
	void UpdateAttack(void);	// 攻撃更新
	void UpdateAction(void);	// 行動更新
	void SetKnockStaging(void);	// ノックバック演出の設定
	void SetDeathStaging(void);	// 死亡演出の設定
	bool IsFly(void) const;		// 飛行フラグの取得

	void UpdateMagicFadeIn(const D3DXVECTOR3& rPos);	// 魔法陣フェードイン行動時の更新
	void UpdateMagicFadeOut	// 魔法陣フェードアウト行動時の更新
	( // 引数
		D3DXVECTOR3 *pPos,		// 位置
		D3DXVECTOR3 *pRot,		// 向き
		D3DXVECTOR3 *pDestRot	// 目標向き
	);
	void UpdatePunchGround(void);	// 地面殴りの行動時の更新
	void UpdateFireAttack(void);	// 炎攻撃の行動時の更新
	void UpdateClawAttack(void);	// ひっかき攻撃の行動時の更新
	void UpdateTailAttack(void);	// しっぽ攻撃の行動時の更新
	void UpdateFlyAttack(void);		// 空中攻撃の行動時の更新

	// メンバ変数
	CGauge2D *m_pLife;				// 体力の情報
	CShadow *m_pShadow;				// 影の情報
	CEnemyAttack *m_pAttack;		// 攻撃の情報
	CMagicCircle *m_pMagicCircle;	// 魔法陣の情報
	CEnemyAttack::EAttack m_oldAtk;	// 前回の攻撃
	CEnemyAttack::EAttack m_curAtk;	// 今回の攻撃
	STeleport m_teleport;	// テレポートの情報
	SRide m_ride;			// ライドの情報
	EAction m_action;		// 行動
	int m_nCounterSameAct;	// 同じ行動の連続数
	int m_nCounterAttack;	// 攻撃管理カウンター
	int m_nCounterRotate;	// ライド旋回カウンター
	int m_nCounterFlash;	// フラッシュ管理カウンター
};

#endif	// _ENEMY_BOSS_DRAGON_H_
