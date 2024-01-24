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
#include "main.h"

//************************************************************
//	クラス定義
//************************************************************
// カメラクラス
class CCamera
{
public:
	// カメラ列挙
	enum TYPE
	{
		TYPE_MAIN,		// メインカメラ
		TYPE_MODELUI,	// モデルUI表示カメラ
		TYPE_MAX		// この列挙型の総数
	};

	// 状態列挙
	enum STATE
	{
		STATE_NONE = 0,		// なにもしない状態
		STATE_CONTROL,		// 操作状態
		STATE_ROTATE,		// 回転状態
		STATE_BARGAINING,	// 寄り引き状態
		STATE_UP,			// 上向き状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CCamera();

	// デストラクタ
	~CCamera();

	// カメラ構造体
	typedef struct
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
		D3DXMATRIX		mtxProjection;	// プロジェクションマトリックス
		D3DXMATRIX		mtxView;		// ビューマトリックス
		D3DVIEWPORT9	viewport;		// ビューポート
	}Camera;

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetCamera(const TYPE type);	// カメラ設定
	Camera GetCamera(const TYPE type);	// カメラ取得
	void SetState(const STATE state);	// カメラ状態設定
	STATE GetState(void) const;			// カメラ状態取得
	void SetDestRotate(void);			// カメラ目標位置設定 (回転)
	void SetDestBargaining(void);		// カメラ目標位置設定 (寄り引き)

	void SetEnableUpdate(const bool bUpdate);		// 更新状況設定
	void SetRotation(const D3DXVECTOR3& rRot);		// 現在向き設定
	void SetDestRotation(const D3DXVECTOR3& rRot);	// 目標向き設定
	D3DXVECTOR3 GetRotation(void) const;			// 現在向き取得
	D3DXVECTOR3 GetDestRotation(void) const;		// 目標向き取得

	// 静的メンバ関数
	static CCamera *Create(void);				// 生成
	static HRESULT Release(CCamera *&prCamera);	// 破棄

private:
	// メンバ関数
	void Rotate(void);		// カメラの更新 (回転)
	void Bargaining(void);	// カメラの更新 (寄り引き)
	void Control(void);		// カメラの更新 (操作)
	void Move(void);		// 位置の更新 (操作)
	void Distance(void);	// 距離の更新 (操作)
	void Rotation(void);	// 向きの更新 (操作)

	// メンバ変数
	Camera m_aCamera[TYPE_MAX];	// カメラの情報
	STATE m_state;	// 状態
	bool m_bUpdate;	// 更新状況
};

#endif	// _CAMERA_H_
