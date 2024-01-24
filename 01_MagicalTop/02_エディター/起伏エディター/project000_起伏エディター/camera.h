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
	// コンストラクタ
	CCamera();

	// デストラクタ
	~CCamera();

	// カメラ構造体
	typedef struct
	{
		D3DXVECTOR3  posV;			// 現在の視点
		D3DXVECTOR3  posR;			// 現在の注視点
		D3DXVECTOR3  destPosV;		// 目標の視点
		D3DXVECTOR3  destPosR;		// 目標の注視点
		D3DXVECTOR3  vecU;			// 上方向ベクトル
		D3DXVECTOR3  rot;			// 現在の向き
		D3DXVECTOR3  destRot;		// 目標の向き
		float        fDis;			// 視点と注視点の距離
		D3DXMATRIX   mtxProjection;	// プロジェクションマトリックス
		D3DXMATRIX   mtxView;		// ビューマトリックス
		D3DVIEWPORT9 viewport;		// ビューポート
	}Camera;

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetCamera(void);		// カメラ設定
	void SetDestCamera(void);	// カメラ目標位置設定

	void SetRotation(const D3DXVECTOR3& rRot);		// 現在向き設定
	void SetDestRotation(const D3DXVECTOR3& rRot);	// 目標向き設定
	Camera GetCamera(void);							// カメラ取得
	D3DXVECTOR3 GetRotation(void) const;			// 現在向き取得
	D3DXVECTOR3 GetDestRotation(void) const;		// 目標向き取得

	// 静的メンバ関数
	static CCamera *Create(void);				// 生成
	static HRESULT Release(CCamera *&prCamera);	// 破棄

private:
	// メンバ関数
	void MoveFollowCamera(void);	// 位置の更新 (追従)
	void MoveCamera(void);	// 位置の更新 (操作)
	void DisCamera(void);	// 距離の更新 (操作)
	void RotCamera(void);	// 向きの更新 (操作)

	// メンバ変数
	Camera m_camera;	// カメラの情報
};

#endif	// _CAMERA_H_
