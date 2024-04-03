//============================================================
//
//	カメラヘッダー [camera.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _CAMERA_H_
#define _CAMERA_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	クラス定義
//************************************************************
// カメラクラス
class CCamera
{
public:
	// カメラ列挙
	enum EType
	{
		TYPE_MAIN = 0,	// メインカメラ
		TYPE_MODELUI,	// モデルUI表示カメラ
		TYPE_MAX		// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// なにもしない状態
		STATE_CONTROL,		// 操作状態
		STATE_TITLE_WAIT,	// タイトル待機状態
		STATE_TITLE_ATK,	// タイトル攻撃状態
		STATE_ROTATE,		// 回転状態
		STATE_FOLLOW,		// 追従状態
		STATE_RIDE,			// 騎乗状態
		STATE_LOOK_PLAYER,	// プレイヤー注目状態
		STATE_LOOK_BOSS,	// ボス注目状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CCamera();

	// デストラクタ
	~CCamera();

	// カメラ揺れ構造体
	struct SSwing
	{
	public:
		// コンストラクタ
		SSwing() {}
		SSwing(const float ShiftLength, const float SubAngle, const float SubLength)
		{
			shiftPos		= VEC3_ZERO;	// 位置ずれ量
			fShiftAngle		= 0.0f;			// 位置をずらす角度
			fShiftLength	= ShiftLength;	// 位置をずらす距離
			fSubAngle		= SubAngle;		// ずらす角度の減算量
			fSubLength		= SubLength;	// ずらす距離の減算量
		}

		// デストラクタ
		~SSwing() {}

		// メンバ変数
		D3DXVECTOR3	shiftPos;		// 位置ずれ量
		float		fShiftAngle;	// 位置をずらす角度
		float		fShiftLength;	// 位置をずらす距離
		float		fSubAngle;		// ずらす角度の減算量
		float		fSubLength;		// ずらす距離の減算量
	};

	// カメラ構造体
	struct SCamera
	{
		D3DXVECTOR3		posV;			// 現在の視点
		D3DXVECTOR3		posR;			// 現在の注視点
		D3DXVECTOR3		destPosV;		// 目標の視点
		D3DXVECTOR3		destPosR;		// 目標の注視点
		D3DXVECTOR3		vecU;			// 上方向ベクトル
		D3DXVECTOR3		rot;			// 現在の向き
		D3DXVECTOR3		destRot;		// 目標の向き
		float			fDis;			// 現在の視点と注視点の距離
		float			fDestDis;		// 目標の視点と注視点の距離
		SSwing			swingInfo;		// カメラ揺れ情報
		D3DVIEWPORT9	viewportInfo;	// ビューポート情報
		D3DXMATRIX		mtxProjection;	// プロジェクションマトリックス
		D3DXMATRIX		mtxView;		// ビューマトリックス
	};

	// 視認情報の構造体
	struct SLook
	{
		int nCounterForce;	// カメラ強制操作カウンター
		float fDiffRotY;	// 差分Y軸向き
		float fOldRotY;		// 過去Y軸向き
		float fDiffDis;		// 差分距離
		float fOldDis;		// 過去距離
	};

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Reset(void);	// 再設定
	void SetCamera(const EType type);		// カメラ設定
	SCamera GetCamera(const EType type);	// カメラ取得
	void SetState(const EState state);		// カメラ状態設定
	EState GetState(void) const;			// カメラ状態取得
	SLook GetLook(void) const { return m_look; }			// 視認情報取得
	void SetFollowLook(const D3DXVECTOR3 &rLookPos);		// 追従カメラの位置視認
	void SetSwing(const EType type, const SSwing swing);	// カメラ揺れ設定
	void SetEnableUpdate(const bool bUpdate);				// 更新状況設定

	void SetDestTitleWait(void);	// カメラ目標位置設定 (タイトル待機)
	void SetDestTitleAtk(void);		// カメラ目標位置設定 (タイトル攻撃)
	void SetDestRotate(void);		// カメラ目標位置設定 (回転)
	void SetDestFollow(void);		// カメラ目標位置設定 (追従)
	void SetDestRide(void);			// カメラ目標位置設定 (騎乗)
	void SetDestLookPlayer(void);	// カメラ目標位置設定 (プレイヤー注目)
	void SetDestLookBoss(void);		// カメラ目標位置設定 (ボス注目)

	void SetPositionV(const D3DXVECTOR3& rPos);	// 視点設定
	void SetPositionR(const D3DXVECTOR3& rPos);	// 注視点設定
	void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	void SetDistance(const float fDis);			// 距離設定
	D3DXVECTOR3 GetPositionV(void) const		{ return m_aCamera[TYPE_MAIN].posV; }		// 現在の視点取得
	D3DXVECTOR3 GetDestPositionV(void) const	{ return m_aCamera[TYPE_MAIN].destPosV; }	// 目標の視点取得
	D3DXVECTOR3 GetPositionR(void) const		{ return m_aCamera[TYPE_MAIN].posR; }		// 現在の注視点取得
	D3DXVECTOR3 GetDestPositionR(void) const	{ return m_aCamera[TYPE_MAIN].destPosR; }	// 目標の注視点取得
	D3DXVECTOR3 GetRotation(void) const			{ return m_aCamera[TYPE_MAIN].rot; }		// 現在の向き取得
	D3DXVECTOR3 GetDestRotation(void) const		{ return m_aCamera[TYPE_MAIN].destRot; }	// 目標の向き取得
	float GetDistance(void) const				{ return m_aCamera[TYPE_MAIN].fDis; }		// 現在の距離取得
	float GetDestDistance(void) const			{ return m_aCamera[TYPE_MAIN].fDestDis; }	// 目標の距離取得

	// 静的メンバ関数
	static CCamera *Create(void);				// 生成
	static void Release(CCamera *&prCamera);	// 破棄

private:
	// メンバ関数
	void TitleWait(void);	// カメラの更新 (タイトル待機)
	void TitleAtk(void);	// カメラの更新 (タイトル攻撃)
	void Rotate(void);		// カメラの更新 (回転)
	void Follow(void);		// カメラの更新 (追従)
	void Ride(void);		// カメラの更新 (騎乗)
	void LookPlayer(void);	// カメラの更新 (プレイヤー注目)
	void LookBoss(void);	// カメラの更新 (ボス注目)
	void Control(void);		// カメラの更新 (操作)
	void Move(void);		// 位置の更新 (操作)
	void Distance(void);	// 距離の更新 (操作)
	void Rotation(void);	// 向きの更新 (操作)
	void Swing(void);		// カメラ揺れの更新

	// メンバ変数
	SCamera	m_aCamera[TYPE_MAX];	// カメラの情報
	SLook	m_look;		// 視認の情報
	EState	m_state;	// 状態
	bool	m_bUpdate;	// 更新状況
};

#endif	// _CAMERA_H_
