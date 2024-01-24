//============================================================
//
//	友達ヘッダー [friend.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FRIEND_H_
#define _FRIEND_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectChara.h"
#include "scene.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectBillboard;	// オブジェクトビルボードクラス
class CShadow;	// 影クラス

//************************************************************
//	クラス定義
//************************************************************
// 友達クラス
class CFriend : public CObjectChara
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_HAPPY = 0,	// 嬉しい感情
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
		MOTION_WAVEHAND,	// 手振りモーション
		MOTION_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_NORMAL,	// 通常状態
		STATE_UNION,	// 合流状態
		STATE_MAX		// この列挙型の総数
	};

	// 軸列挙
	enum EAxis
	{
		AXIS_X = 0,	// X軸
		AXIS_Y,		// Y軸
		AXIS_Z,		// Z軸
		AXIS_MAX	// この列挙型の総数
	};

	// コンストラクタ
	CFriend();

	// デストラクタ
	~CFriend();

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画

	void SetState(const int nState) override;	// 状態設定
	int GetState(void) const override;			// 状態取得
	float GetRadius(void) const override;		// 半径取得
	float GetHeight(void) const override;		// 縦幅取得

	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetEnableUpdate(const bool bUpdate) override;		// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;			// 描画状況設定
	D3DXMATRIX GetMtxWorld(void) const override;			// マトリックス取得

	// 静的メンバ関数
	static CFriend *Create(void);	// 生成

private:
	// メンバ関数
	void LoadSetup(void);		// セットアップ
	EMotion UpdateNormal(void);	// 通常状態時の更新
	EMotion UpdateUnion(void);	// 合流状態時の更新

	void UpdateOldPosition(void);			// 過去位置の更新
	void UpdateGravity(void);				// 重力の更新
	bool UpdateLanding(D3DXVECTOR3& rPos);	// 着地状況の更新
	void UpdateRotation(D3DXVECTOR3& rRot);	// 向きの更新
	void UpdateMotion(int nMotion);			// モーション・オブジェクトキャラクターの更新

	bool ResponseSingleBuilding(const EAxis axis, D3DXVECTOR3& rPos);	// ビルとの一軸ごとの当たり判定
	bool LandObstacle(D3DXVECTOR3& rPos);		// 障害物との着地判定
	bool CollisionBuilding(D3DXVECTOR3& rPos);	// ビルとの当たり判定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	CObjectBillboard *m_pEmotion;	// 感情吹き出しの情報
	CShadow *m_pShadow;				// 影の情報
	D3DXVECTOR3	m_oldPos;			// 過去位置
	D3DXVECTOR3	m_move;				// 移動量
	D3DXVECTOR3	m_destRot;			// 目標向き
	EState	m_state;				// 状態
	int		m_nCounterState;		// 状態管理カウンター
	bool	m_bJump;				// ジャンプ状況
};

#endif	// _FRIEND_H_
