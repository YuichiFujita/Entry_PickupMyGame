//============================================================
//
//	カメラ処理 [camera.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "camera.h"
#include "manager.h"
#include "sceneGame.h"
#include "renderer.h"
#include "input.h"
#include "player.h"
#include "target.h"
#include "stage.h"

//************************************************************
//	マクロ定義
//************************************************************
// カメラ基本情報マクロ
#define VIEW_ANGLE	(D3DXToRadian(45.0f))	// 視野角
#define VIEW_ASPECT	((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// アスペクト比

#define VIEW_NEAR	(10.0f)		// モデルが見えるZ軸の最小値
#define VIEW_FAR	(50000.0f)	// モデルが見えるZ軸の最大値

#define INIT_VECU	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))		// 上方向ベクトルの初期値
#define INIT_POSV	(D3DXVECTOR3(0.0f, 0.0f, -600.0f))	// 視点の初期値

// カメラ回転マクロ
#define ROTATE_INITPOSR	(D3DXVECTOR3(0.0f, 450.0f, 0.0f))	// 回転カメラの注視点の初期値
#define ROTATE_INITROT	(D3DXVECTOR2(1.35f, 0.0f))			// 回転カメラの向き初期値

#define ROTATE_INITDIS	(-1750.0f)	// 回転カメラの距離初期値
#define ROTATE_ADDROTY	(0.005f)	// 回転カメラの向き加算量Y

// カメラ寄り引きマクロ
#define INIT_DESTROT_X	(0.65f)	// 初期のカメラ向き
#define REV_DESTROT_X	(1.2f)	// カメラ向き加算時のターゲット距離補正値
#define MIN_DESTROT_X	(0.7f)	// カメラ向きの最低値
#define MAX_DESTROT_X	(1.0f)	// カメラ向きの最高値
#define REV_BARG_ROT	(0.1f)	// カメラ向きの補正係数

#define INIT_DESTDIS	(-500.0f)	// 初期のカメラ距離
#define REV_DESTDIS		(4000.0f)	// カメラ距離加算時のターゲット距離補正値
#define MIN_DESTDIS		(-1800.0f)	// カメラ距離の最低値
#define MAX_DESTDIS		(-950.0f)	// カメラ距離の最高値
#define REV_BARG_DIS	(0.25f)		// カメラ距離の補正係数

#define REV_LENGTH	(0.8f)	// カメラ注視点設定用の割合のターゲット距離補正値
#define MIN_LENGTH	(0.0f)	// カメラ注視点設定用の割合の最低値
#define MAX_LENGTH	(0.15f)	// カメラ注視点設定用の割合の最高値

#define INIT_DESTPOS_Y	(400.0f)	// 初期のカメラ位置
#define REV_DESTPOS_Y	(200.0f)	// カメラ位置加算時のターゲット距離補正値
#define REV_BARG_POS	(0.25f)		// カメラ位置の補正係数

// カメラ上向きマクロ
#define POSRUP_MAX	(1600.0f)	// 上向き状態時の注視点の最大高度
#define POSRUP_ADD	(15.0f)		// 上向き状態時の注視点の高度加算量

// カメラ操作マクロ
#define REV_MOVE_MOUSE	(1.6f)		// マウス操作でのカメラの移動の補正係数
#define REV_DIS_MOUSE	(-0.3f)		// マウス操作でのカメラの距離の補正係数
#define REV_ROT_MOUSE	(0.008f)	// マウス操作でのカメラの回転量の補正係数

#define MIN_DIS	(-10000.0f)	// カメラの視点から注視点への距離の最小
#define MAX_DIS	(-1.0f)		// カメラの視点から注視点への距離の最大
#define LIMIT_ROT_HIGH	(D3DX_PI - 0.1f)	// x回転の制限値 (上)
#define LIMIT_ROT_LOW	(0.1f)				// x回転の制限値 (下)

//************************************************************
//	親クラス [CCamera] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CCamera::CCamera()
{
	// メンバ変数をクリア
	memset(&m_aCamera[0], 0, sizeof(m_aCamera));	// カメラの情報
	m_state = STATE_NONE;	// 状態
	m_bUpdate = false;		// 更新状況
}

//============================================================
//	デストラクタ
//============================================================
CCamera::~CCamera()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CCamera::Init(void)
{
	//--------------------------------------------------------
	//	メンバ変数を初期化
	//--------------------------------------------------------
	m_state = STATE_NONE;	// 状態
	m_bUpdate = true;	 	// 更新状況

	//--------------------------------------------------------
	//	メインカメラの初期化
	//--------------------------------------------------------
	// カメラ情報を初期化
	m_aCamera[TYPE_MAIN].posV		= VEC3_ZERO;	// 現在の視点
	m_aCamera[TYPE_MAIN].posR		= VEC3_ZERO;	// 現在の注視点
	m_aCamera[TYPE_MAIN].destPosV	= VEC3_ZERO;	// 目標の視点
	m_aCamera[TYPE_MAIN].destPosR	= VEC3_ZERO;	// 目標の注視点
	m_aCamera[TYPE_MAIN].vecU		= INIT_VECU;	// 上方向ベクトル
	m_aCamera[TYPE_MAIN].rot		= VEC3_ZERO;	// 現在の向き
	m_aCamera[TYPE_MAIN].destRot	= VEC3_ZERO;	// 目標の向き
	m_aCamera[TYPE_MAIN].fDis		= 0.0f;			// 現在の視点と注視点の距離
	m_aCamera[TYPE_MAIN].fDestDis	= 0.0f;			// 目標の視点と注視点の距離

	// ビューポート情報を初期化
	m_aCamera[TYPE_MAIN].viewport.X			= 0;				// 左上隅のピクセル座標 (x)
	m_aCamera[TYPE_MAIN].viewport.Y			= 0;				// 左上隅のピクセル座標 (y)
	m_aCamera[TYPE_MAIN].viewport.Width		= SCREEN_WIDTH;		// 描画する画面の横幅
	m_aCamera[TYPE_MAIN].viewport.Height	= SCREEN_HEIGHT;	// 描画する画面の縦幅
	m_aCamera[TYPE_MAIN].viewport.MinZ		= 0.0f;
	m_aCamera[TYPE_MAIN].viewport.MaxZ		= 1.0f;

	//--------------------------------------------------------
	//	モデルUI表示カメラの初期化
	//--------------------------------------------------------
	// カメラ情報を初期化
	m_aCamera[TYPE_MODELUI].posV		= INIT_POSV;	// 現在の視点
	m_aCamera[TYPE_MODELUI].posR		= VEC3_ZERO;	// 現在の注視点
	m_aCamera[TYPE_MODELUI].destPosV	= INIT_POSV;	// 目標の視点
	m_aCamera[TYPE_MODELUI].destPosR	= VEC3_ZERO;	// 目標の注視点
	m_aCamera[TYPE_MODELUI].vecU		= INIT_VECU;	// 上方向ベクトル
	m_aCamera[TYPE_MODELUI].rot			= VEC3_ZERO;	// 現在の向き
	m_aCamera[TYPE_MODELUI].destRot		= VEC3_ZERO;	// 目標の向き
	m_aCamera[TYPE_MODELUI].fDis		= 0.0f;			// 現在の視点と注視点の距離
	m_aCamera[TYPE_MODELUI].fDestDis	= 0.0f;			// 目標の視点と注視点の距離

	// ビューポート情報を初期化
	m_aCamera[TYPE_MODELUI].viewport.X		= 0;				// 左上隅のピクセル座標 (x)
	m_aCamera[TYPE_MODELUI].viewport.Y		= 0;				// 左上隅のピクセル座標 (y)
	m_aCamera[TYPE_MODELUI].viewport.Width	= SCREEN_WIDTH;		// 描画する画面の横幅
	m_aCamera[TYPE_MODELUI].viewport.Height	= SCREEN_HEIGHT;	// 描画する画面の縦幅
	m_aCamera[TYPE_MODELUI].viewport.MinZ	= 0.0f;
	m_aCamera[TYPE_MODELUI].viewport.MaxZ	= 0.5f;

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CCamera::Uninit(void)
{

}

//============================================================
//	更新処理
//============================================================
void CCamera::Update(void)
{
	if (m_bUpdate)
	{ // 更新する状況の場合

		switch (m_state)
		{ // 状態ごとの処理
		case STATE_NONE:	// なにもしない状態

			// 無し

			break;

		case STATE_CONTROL:	// 操作状態

			// カメラの更新 (操作)
			Control();

			break;

		case STATE_ROTATE:	// 回転状態

			// カメラの更新 (回転)
			Rotate();

			break;

		case STATE_BARGAINING:	// 寄り引き状態

			// カメラの更新 (寄り引き)
			Bargaining();

			break;

		case STATE_UP:	// 上向き状態

			if (m_aCamera[TYPE_MAIN].posR.y < POSRUP_MAX)
			{ // カメラの注視点Y座標が一定値より低い場合

				// 注視点の現在位置を更新
				m_aCamera[TYPE_MAIN].posR.y += POSRUP_ADD;

				if (m_aCamera[TYPE_MAIN].posR.y > POSRUP_MAX)
				{ // カメラの注視点Y座標が一定値を超えた場合

					// 注視点の現在位置を補正
					m_aCamera[TYPE_MAIN].posR.y += POSRUP_ADD;
				}
			}

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}

	// デバッグ表示
	CManager::GetDebugProc()->Print(" 視点 ：%f %f %f\n", m_aCamera[TYPE_MAIN].posV.x, m_aCamera[TYPE_MAIN].posV.y, m_aCamera[TYPE_MAIN].posV.z);
	CManager::GetDebugProc()->Print("注視点：%f %f %f\n", m_aCamera[TYPE_MAIN].posR.x, m_aCamera[TYPE_MAIN].posR.y, m_aCamera[TYPE_MAIN].posR.z);
	CManager::GetDebugProc()->Print(" 向き ：%f %f %f\n", m_aCamera[TYPE_MAIN].rot.x, m_aCamera[TYPE_MAIN].rot.y, m_aCamera[TYPE_MAIN].rot.z);
	CManager::GetDebugProc()->Print(" 距離 ：%f\n", m_aCamera[TYPE_MAIN].fDis);
}

//============================================================
//	カメラ設定処理
//============================================================
void CCamera::SetCamera(const TYPE type)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// ビューポートの設定
	pDevice->SetViewport(&m_aCamera[type].viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_aCamera[type].mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH
	( // 引数
		&m_aCamera[type].mtxProjection,	// プロジェクションマトリックス
		VIEW_ANGLE,		// 視野角
		VIEW_ASPECT,	// 画面のアスペクト比
		VIEW_NEAR,		// Z軸の最小値
		VIEW_FAR		// Z軸の最大値
	);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_aCamera[type].mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_aCamera[type].mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH
	( // 引数
		&m_aCamera[type].mtxView,	// ビューマトリックス
		&m_aCamera[type].posV,		// 視点
		&m_aCamera[type].posR,		// 注視点
		&m_aCamera[type].vecU		// 上方向ベクトル
	);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_aCamera[type].mtxView);
}

//============================================================
//	カメラ目標位置の設定処理 (回転)
//============================================================
void CCamera::SetDestRotate(void)
{
	if (m_state != STATE_ROTATE)
	{ // カメラの回転状態ではない場合

		// 処理を抜ける
		return;
	}

	//----------------------------------------------------
	//	向きの更新
	//----------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = ROTATE_INITROT.x;
	m_aCamera[TYPE_MAIN].rot.y = ROTATE_INITROT.y;
	useful::Vec3NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

	//----------------------------------------------------
	//	距離の更新
	//----------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = ROTATE_INITDIS;

	//----------------------------------------------------
	//	位置の更新
	//----------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = ROTATE_INITPOSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの目標位置の設定処理 (寄り引き)
//============================================================
void CCamera::SetDestBargaining(void)
{
	if (m_state != STATE_BARGAINING)
	{ // カメラの寄り引き状態ではない場合

		// 処理を抜ける
		return;
	}

	if (USED(CScene::GetPlayer()) && USED(CScene::GetTarget()))
	{ // プレイヤー・ターゲットが使用されている場合

		// 変数を宣言
		D3DXVECTOR3 posPlayer = CScene::GetPlayer()->GetPosition();	// プレイヤー位置
		D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
		float fLength = 0.0f;	// ターゲット距離のカメラ注視点設定用の割合

		float fDisTarget = CScene::GetPlayer()->GetDistanceTarget();	// ターゲットとプレイヤー間の距離
		float fRateTarget = (1.0f / CScene::GetStage()->GetStageLimit().fRadius) * fDisTarget;	// ターゲットとプレイヤー間の距離の割合

		//----------------------------------------------------
		//	向きの更新
		//----------------------------------------------------
		// 距離が離れるにつれてカメラを上に向けていく
		m_aCamera[TYPE_MAIN].destRot.x = INIT_DESTROT_X + (fRateTarget * REV_DESTROT_X);
		useful::LimitNum(m_aCamera[TYPE_MAIN].destRot.x, MIN_DESTROT_X, MAX_DESTROT_X);		// 向き制限

		// カメラをプレイヤーに向かせる
		m_aCamera[TYPE_MAIN].destRot.y = atan2f(posTarget.x - posPlayer.x, posTarget.z - posPlayer.z);

		// 目標向きを正規化
		useful::Vec3NormalizeRot(m_aCamera[TYPE_MAIN].destRot);

		// 現在向きに目標向きを設定
		m_aCamera[TYPE_MAIN].rot = m_aCamera[TYPE_MAIN].destRot;

		//----------------------------------------------------
		//	距離の更新
		//----------------------------------------------------
		// 目標距離を設定
		m_aCamera[TYPE_MAIN].fDestDis = INIT_DESTDIS - (fRateTarget * REV_DESTDIS);
		useful::LimitNum(m_aCamera[TYPE_MAIN].fDestDis, MIN_DESTDIS, MAX_DESTDIS);	// 距離制限

		// 現在距離に目標距離を設定
		m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis;

		//----------------------------------------------------
		//	位置の更新
		//----------------------------------------------------
		// ターゲット距離のカメラ注視点設定用の割合を計算
		fLength = (fRateTarget * REV_LENGTH);
		useful::LimitNum(fLength, MIN_LENGTH, MAX_LENGTH);	// 距離制限

		// 注視点の更新
		m_aCamera[TYPE_MAIN].destPosR = posPlayer - ((posPlayer - posTarget) * fLength);
		m_aCamera[TYPE_MAIN].destPosR.y = INIT_DESTPOS_Y - (fRateTarget * REV_DESTPOS_Y);

		// 視点の更新
		m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].destPosR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
		m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].destPosR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
		m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].destPosR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

		// 注視点の現在位置を更新
		m_aCamera[TYPE_MAIN].posR = m_aCamera[TYPE_MAIN].destPosR;

		// 視点の現在位置を更新
		m_aCamera[TYPE_MAIN].posV = m_aCamera[TYPE_MAIN].destPosV;
	}
}

//============================================================
//	更新状況の設定処理
//============================================================
void CCamera::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	m_bUpdate = bUpdate;
}

//============================================================
//	向きの設定処理
//============================================================
void CCamera::SetRotation(const D3DXVECTOR3& rRot)
{
	// 引数のカメラの向きを設定
	m_aCamera[TYPE_MAIN].rot = rRot;

	// 向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.x);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.z);
}

//============================================================
//	目標向きの設定処理
//============================================================
void CCamera::SetDestRotation(const D3DXVECTOR3& rRot)
{
	// 引数のカメラの目標向きを設定
	m_aCamera[TYPE_MAIN].destRot = rRot;

	// 向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot.x);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot.y);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot.z);
}

//============================================================
//	カメラ取得処理
//============================================================
CCamera::Camera CCamera::GetCamera(const TYPE type)
{
	// カメラの情報を返す
	return m_aCamera[type];
}

//============================================================
//	カメラ状態の設定処理
//============================================================
void CCamera::SetState(const STATE state)
{
	// 状態を設定
	m_state = state;
}

//============================================================
//	カメラ状態取得処理
//============================================================
CCamera::STATE CCamera::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CCamera::GetRotation(void) const
{
	// カメラの向きを返す
	return m_aCamera[TYPE_MAIN].rot;
}

//============================================================
//	目標向き取得処理
//============================================================
D3DXVECTOR3 CCamera::GetDestRotation(void) const
{
	// カメラの目標向きを返す
	return m_aCamera[TYPE_MAIN].destRot;
}

//============================================================
//	生成処理
//============================================================
CCamera *CCamera::Create(void)
{
	// ポインタを宣言
	CCamera *pCamera = NULL;	// カメラ生成用

	if (UNUSED(pCamera))
	{ // 使用されていない場合

		// メモリを確保
		pCamera = new CCamera;	// カメラ
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pCamera))
	{ // 確保に成功している場合

		// カメラの初期化
		if (FAILED(pCamera->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pCamera;
			pCamera = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pCamera;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CCamera::Release(CCamera *&prCamera)
{
	if (USED(prCamera))
	{ // 使用中の場合

		// カメラの終了
		prCamera->Uninit();

		// メモリ開放
		delete prCamera;
		prCamera = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	カメラの更新処理 (回転)
//============================================================
void CCamera::Rotate(void)
{
	//----------------------------------------------------
	//	向きの更新
	//----------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = ROTATE_INITROT.x;
	m_aCamera[TYPE_MAIN].rot.y += ROTATE_ADDROTY;
	useful::Vec3NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

	//----------------------------------------------------
	//	距離の更新
	//----------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = ROTATE_INITDIS;

	//----------------------------------------------------
	//	位置の更新
	//----------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = ROTATE_INITPOSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (寄り引き)
//============================================================
void CCamera::Bargaining(void)
{
	if (USED(CScene::GetPlayer()) && USED(CScene::GetTarget()))
	{ // プレイヤー・ターゲットが使用されている場合

		// 変数を宣言
		D3DXVECTOR3 posPlayer = CScene::GetPlayer()->GetPosition();	// プレイヤー位置
		D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
		D3DXVECTOR3 diffPosV = VEC3_ZERO;	// 視点の差分位置
		D3DXVECTOR3 diffPosR = VEC3_ZERO;	// 注視点の差分位置
		D3DXVECTOR3 diffRot = VEC3_ZERO;	// 差分向き
		float fDiffDis = 0.0f;	// 目標距離
		float fLength = 0.0f;	// ターゲット距離のカメラ注視点設定用の割合

		float fDisTarget = CScene::GetPlayer()->GetDistanceTarget();	// ターゲットとプレイヤー間の距離
		float fRateTarget = (1.0f / CScene::GetStage()->GetStageLimit().fRadius) * fDisTarget;	// ターゲットとプレイヤー間の距離の割合

		//----------------------------------------------------
		//	向きの更新
		//----------------------------------------------------
		// 距離が離れるにつれてカメラを上に向けていく
		m_aCamera[TYPE_MAIN].destRot.x = INIT_DESTROT_X + (fRateTarget * REV_DESTROT_X);
		useful::LimitNum(m_aCamera[TYPE_MAIN].destRot.x, MIN_DESTROT_X, MAX_DESTROT_X);		// 向き制限

		// カメラをプレイヤーに向かせる
		m_aCamera[TYPE_MAIN].destRot.y = atan2f(posTarget.x - posPlayer.x, posTarget.z - posPlayer.z);

		// 目標向きを正規化
		useful::Vec3NormalizeRot(m_aCamera[TYPE_MAIN].destRot);

		// 差分向きを計算
		diffRot = m_aCamera[TYPE_MAIN].destRot - m_aCamera[TYPE_MAIN].rot;
		useful::Vec3NormalizeRot(diffRot);	// 差分向きを正規化

		// 現在向きの更新
		m_aCamera[TYPE_MAIN].rot += diffRot * REV_BARG_ROT;
		useful::Vec3NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

		//----------------------------------------------------
		//	距離の更新
		//----------------------------------------------------
		// 目標距離を設定
		m_aCamera[TYPE_MAIN].fDestDis = INIT_DESTDIS - (fRateTarget * REV_DESTDIS);
		useful::LimitNum(m_aCamera[TYPE_MAIN].fDestDis, MIN_DESTDIS, MAX_DESTDIS);	// 距離制限

		// 差分距離を計算
		fDiffDis = m_aCamera[TYPE_MAIN].fDestDis - m_aCamera[TYPE_MAIN].fDis;

		// 現在距離の更新
		m_aCamera[TYPE_MAIN].fDis += fDiffDis * REV_BARG_DIS;

		//----------------------------------------------------
		//	位置の更新
		//----------------------------------------------------
		// ターゲット距離のカメラ注視点設定用の割合を計算
		fLength = (fRateTarget * REV_LENGTH);
		useful::LimitNum(fLength, MIN_LENGTH, MAX_LENGTH);	// 距離制限

		// 注視点の更新
		m_aCamera[TYPE_MAIN].destPosR = posPlayer - ((posPlayer - posTarget) * fLength);
		m_aCamera[TYPE_MAIN].destPosR.y = INIT_DESTPOS_Y - (fRateTarget * REV_DESTPOS_Y);

		// 視点の更新
		m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].destPosR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
		m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].destPosR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
		m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].destPosR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

		// 注視点の差分位置を計算
		diffPosR = m_aCamera[TYPE_MAIN].destPosR - m_aCamera[TYPE_MAIN].posR;

		// 視点の差分位置を計算
		diffPosV = m_aCamera[TYPE_MAIN].destPosV - m_aCamera[TYPE_MAIN].posV;

		// 注視点の現在位置を更新
		m_aCamera[TYPE_MAIN].posR.x += diffPosR.x * REV_BARG_POS;
		m_aCamera[TYPE_MAIN].posR.y += diffPosR.y * REV_BARG_POS;
		m_aCamera[TYPE_MAIN].posR.z += diffPosR.z * REV_BARG_POS;

		// 視点の現在位置を更新
		m_aCamera[TYPE_MAIN].posV.x += diffPosV.x * REV_BARG_POS;
		m_aCamera[TYPE_MAIN].posV.y += diffPosV.y * REV_BARG_POS;
		m_aCamera[TYPE_MAIN].posV.z += diffPosV.z * REV_BARG_POS;
	}
}

//============================================================
//	カメラの更新処理 (操作)
//============================================================
void CCamera::Control(void)
{
	// 位置の更新
	Move();

	// 距離の更新
	Distance();

	// 向きの更新
	Rotation();
}

//============================================================
//	位置の更新処理 (操作)
//============================================================
void CCamera::Move(void)
{
	// ポインタを宣言
	CInputMouse	*pMouse = CManager::GetMouse();	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (pMouse->GetPress(CInputMouse::KEY_LEFT) && pMouse->GetPress(CInputMouse::KEY_RIGHT))
	{ // 右クリックと左クリックが押されている場合

		// 視点を移動
		m_aCamera[TYPE_MAIN].posV.x -= sinf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.z -= cosf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.x += sinf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.z += cosf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * REV_MOVE_MOUSE;

		// 注視点を移動
		m_aCamera[TYPE_MAIN].posR.x -= sinf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.z -= cosf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.x += sinf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.z += cosf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * REV_MOVE_MOUSE;
	}
}

//============================================================
//	距離の更新処理 (操作)
//============================================================
void CCamera::Distance(void)
{
	// ポインタを宣言
	CInputMouse	*pMouse = CManager::GetMouse();	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (mouseMove.z != 0.0f)
	{ // マウスホイールが操作された場合

		// 距離を加算
		m_aCamera[TYPE_MAIN].fDis += mouseMove.z * REV_DIS_MOUSE;
	}

	// 距離の補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].fDis, MIN_DIS, MAX_DIS);
}

//============================================================
//	向きの更新処理 (操作)
//============================================================
void CCamera::Rotation(void)
{
	// ポインタを宣言
	CInputMouse	*pMouse = CManager::GetMouse();	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (pMouse->GetPress(CInputMouse::KEY_LEFT) && !pMouse->GetPress(CInputMouse::KEY_RIGHT))
	{ // 左クリックだけが押されている場合

		// カメラの y軸を回転
		m_aCamera[TYPE_MAIN].rot.y += mouseMove.x * REV_ROT_MOUSE;

		// カメラの x軸を回転
		m_aCamera[TYPE_MAIN].rot.x += mouseMove.y * REV_ROT_MOUSE;
	}

	// 向きの補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].rot.x, LIMIT_ROT_LOW, LIMIT_ROT_HIGH);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

	// マウス操作の更新
	if (pMouse->GetPress(CInputMouse::KEY_RIGHT) && !pMouse->GetPress(CInputMouse::KEY_LEFT))
	{ // 右クリックだけが押されている場合

		// カメラの y軸を回転
		m_aCamera[TYPE_MAIN].rot.y += mouseMove.x * REV_ROT_MOUSE;

		// カメラの x軸を回転
		m_aCamera[TYPE_MAIN].rot.x += mouseMove.y * REV_ROT_MOUSE;
	}

	// 向きの補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].rot.x, LIMIT_ROT_LOW, LIMIT_ROT_HIGH);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);

	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR.x = m_aCamera[TYPE_MAIN].posV.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posR.y = m_aCamera[TYPE_MAIN].posV.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posR.z = m_aCamera[TYPE_MAIN].posV.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}
