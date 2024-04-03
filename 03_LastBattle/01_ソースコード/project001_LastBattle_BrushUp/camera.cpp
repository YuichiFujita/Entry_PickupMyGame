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
#include "stage.h"
#include "player.h"
#include "enemy.h"
#include "enemyBossDragon.h"
#include "multiModel.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	// カメラ基本情報
	namespace basic
	{
		const D3DXVECTOR3 INIT_VECU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);	// 上方向ベクトルの初期値
		const D3DXVECTOR3 INIT_POSV = D3DXVECTOR3(0.0f, 0.0f, -600.0f);	// 視点の初期値
		const float VIEW_NEAR	= 10.0f;	// モデルが見えるZ軸の最小値
		const float VIEW_FAR	= 50000.0f;	// モデルが見えるZ軸の最大値
		const float MIN_DIS		= 1.0f;		// カメラの視点から注視点への距離の最小
		const float MAX_DIS		= 10000.0f;	// カメラの視点から注視点への距離の最大

		const float VIEW_ASPECT		= (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;	// アスペクト比
		const float VIEW_ANGLE		= D3DXToRadian(45.0f);	// 視野角
		const float LIMIT_ROT_HIGH	= D3DX_PI - 0.04f;		// X上回転の制限値
		const float LIMIT_ROT_LOW	= 0.04f;				// X下回転の制限値
	}

	// カメラ揺れ情報
	namespace swing
	{
		const float REV_DIS = 0.001f;	// カメラ揺れ計算時のカメラ距離の補正係数
	}

	// タイトルカメラ情報
	namespace title
	{
		const D3DXVECTOR3 INIT_POSR = D3DXVECTOR3(0.0f, 20.0f, 0.0f);	// タイトルカメラの注視点の初期値
		const D3DXVECTOR2 INIT_ROT  = D3DXVECTOR2(1.57f, -0.5f);		// タイトルカメラの向き初期値

		const float	INIT_DIS	= 300.0f;	// タイトルカメラの距離初期値
		const float	ADD_ROTY	= 0.0f;		// タイトルカメラの向き加算量Y
		const float	DEST_ROTY	= -0.5f;	// タイトルカメラ最終目標向きY
		const int	MOVE_FRAME	= 0;		// タイトルカメラ最終目標向きに向かうフレーム数
	}

	// 回転カメラ情報
	namespace rotate
	{
		const D3DXVECTOR3 INIT_POSR	= D3DXVECTOR3(0.0f, 50.0f, 0.0f);	// 回転カメラの注視点の初期値
		const D3DXVECTOR2 INIT_ROT	= D3DXVECTOR2(1.4f, 0.0f);			// 回転カメラの向き初期値

		const float INIT_DIS	= 150.0f;	// 回転カメラの距離初期値
		const float ADD_ROTY	= 0.0025f;	// 回転カメラの向き加算量Y
	}

	// 追従カメラ情報
	namespace follow
	{
		const D3DXVECTOR3 REV_POSV = D3DXVECTOR3(0.4f, 0.45f, 0.4f);	// カメラ視点の補正係数
		const D3DXVECTOR3 REV_POSR = D3DXVECTOR3(1.0f, 0.35f, 1.0f);	// カメラ注視点の補正係数

		const float	STICK_REV	= 0.00000225f;	// カメラ操作スティックの傾き量の補正係数

		const float	ROTX_REV	= 0.5f;		// カメラピッチ回転の補正係数
		const float	REV_ROT		= 1.0f;		// カメラ向きの補正係数
		const float	INIT_DIS	= 550.0f;	// 追従カメラの距離
		const float	INIT_ROTX	= 1.8f;		// 追従カメラの向きX初期値

		const int	LOOK_BOSS_FRAME	= 18;				// 追従カメラのボス視認速度
		const float	LIMIT_ROT_HIGH	= D3DX_PI - 0.5f;	// X上回転の制限値
		const float	LIMIT_ROT_LOW	= 1.1f;				// X下回転の制限値
		const float	MAX_SUB_DIS		= 1500.0f;			// 下方向カメラの距離減算量
	}

	// 騎乗カメラ情報
	namespace ride
	{
		const D3DXVECTOR3 OFFSET_POSR = D3DXVECTOR3(0.0f, 340.0f, 0.0f);	// 回転カメラの注視点のオフセット
		const D3DXVECTOR2 INIT_ROT = D3DXVECTOR2(1.8f, 0.0f);				// 回転カメラの向き初期値

		const float INIT_DIS = 560.0f;	// 回転カメラの距離初期値
		const float SUB_ROTY = 0.6f;	// 回転カメラの向き減算量Y
	}

	// プレイヤー注目情報
	namespace lookPlayer
	{
		const D3DXVECTOR2	INIT_ROT = D3DXVECTOR2(1.80f, 1.0f);	// 初期向き
		const float			INIT_DIS = 280.0f;	// 初期距離
		const float			UP_POSR  = 52.5f;	// 注視点の上加算量

		const int	SWORD_SHAKE_KEY		= 3;	// プレイヤーの剣振り下ろしキー
		const int	SWORD_SHAKE_FRAME	= 10;	// プレイヤーの剣振り下ろしキーフレーム
		const int	CHANGE_POSV_FRAME	= 60;	// 目標視点への移動速度
		const float	DESTPOSV_ROTY	= -1.35f;	// 目標視点へのY向き
		const float	DESTPOSV_DIS	= 190.0f;	// 目標視点への距離
	}

	// ボス注目情報
	namespace lookBoss
	{
		const D3DXVECTOR3 INIT_POSV = D3DXVECTOR3(0.0f, 100.0f, 0.0f);	// 初期視点
	}

	// 操作カメラ情報
	namespace control
	{
		const float REV_MOVE_MOUSE	= 1.0f;		// マウス操作でのカメラの移動の補正係数
		const float REV_DIS_MOUSE	= 0.2f;		// マウス操作でのカメラの距離の補正係数
		const float REV_ROT_MOUSE	= 0.0045f;	// マウス操作でのカメラの回転量の補正係数
	}
}

//************************************************************
//	親クラス [CCamera] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CCamera::CCamera() :
	m_state		(STATE_NONE),	// 状態
	m_bUpdate	(false)			// 更新状況
{
	// メンバ変数をクリア
	memset(&m_aCamera[0],	0, sizeof(m_aCamera));	// カメラの情報
	memset(&m_look,			0, sizeof(m_look));		// 視認の情報
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
	memset(&m_look, 0, sizeof(m_look));	// 視認の情報
	m_state		= STATE_NONE;	// 状態
	m_bUpdate	= true;			// 更新状況

	//--------------------------------------------------------
	//	メインカメラの初期化
	//--------------------------------------------------------
	// カメラ情報を初期化
	m_aCamera[TYPE_MAIN].posV		= VEC3_ZERO;		// 現在の視点
	m_aCamera[TYPE_MAIN].posR		= VEC3_ZERO;		// 現在の注視点
	m_aCamera[TYPE_MAIN].destPosV	= VEC3_ZERO;		// 目標の視点
	m_aCamera[TYPE_MAIN].destPosR	= VEC3_ZERO;		// 目標の注視点
	m_aCamera[TYPE_MAIN].vecU		= basic::INIT_VECU;	// 上方向ベクトル
	m_aCamera[TYPE_MAIN].rot		= VEC3_ZERO;		// 現在の向き
	m_aCamera[TYPE_MAIN].destRot	= VEC3_ZERO;		// 目標の向き
	m_aCamera[TYPE_MAIN].fDis		= 0.0f;				// 現在の視点と注視点の距離
	m_aCamera[TYPE_MAIN].fDestDis	= 0.0f;				// 目標の視点と注視点の距離

	// カメラ揺れ情報を初期化
	m_aCamera[TYPE_MAIN].swingInfo.shiftPos		= VEC3_ZERO;	// 位置ずれ量
	m_aCamera[TYPE_MAIN].swingInfo.fShiftAngle	= 0.0f;			// 位置をずらす角度
	m_aCamera[TYPE_MAIN].swingInfo.fShiftLength	= 0.0f;			// 位置をずらす距離
	m_aCamera[TYPE_MAIN].swingInfo.fSubAngle	= 0.0f;			// ずらす角度の減算量
	m_aCamera[TYPE_MAIN].swingInfo.fSubLength	= 0.0f;			// ずらす距離の減算量

	// ビューポート情報を初期化
	m_aCamera[TYPE_MAIN].viewportInfo.X			= 0;				// 左上隅のピクセル座標X
	m_aCamera[TYPE_MAIN].viewportInfo.Y			= 0;				// 左上隅のピクセル座標Y
	m_aCamera[TYPE_MAIN].viewportInfo.Width		= SCREEN_WIDTH;		// 描画する画面の横幅
	m_aCamera[TYPE_MAIN].viewportInfo.Height	= SCREEN_HEIGHT;	// 描画する画面の縦幅
	m_aCamera[TYPE_MAIN].viewportInfo.MinZ		= 0.0f;
	m_aCamera[TYPE_MAIN].viewportInfo.MaxZ		= 1.0f;

	//--------------------------------------------------------
	//	モデルUI表示カメラの初期化
	//--------------------------------------------------------
	// カメラ情報を初期化
	m_aCamera[TYPE_MODELUI].posV		= basic::INIT_POSV;	// 現在の視点
	m_aCamera[TYPE_MODELUI].posR		= VEC3_ZERO;		// 現在の注視点
	m_aCamera[TYPE_MODELUI].destPosV	= basic::INIT_POSV;	// 目標の視点
	m_aCamera[TYPE_MODELUI].destPosR	= VEC3_ZERO;		// 目標の注視点
	m_aCamera[TYPE_MODELUI].vecU		= basic::INIT_VECU;	// 上方向ベクトル
	m_aCamera[TYPE_MODELUI].rot			= VEC3_ZERO;		// 現在の向き
	m_aCamera[TYPE_MODELUI].destRot		= VEC3_ZERO;		// 目標の向き
	m_aCamera[TYPE_MODELUI].fDis		= 0.0f;				// 現在の視点と注視点の距離
	m_aCamera[TYPE_MODELUI].fDestDis	= 0.0f;				// 目標の視点と注視点の距離

	// カメラ揺れ情報を初期化
	m_aCamera[TYPE_MODELUI].swingInfo.shiftPos		= VEC3_ZERO;	// 位置ずれ量
	m_aCamera[TYPE_MODELUI].swingInfo.fShiftAngle	= 0.0f;			// 位置をずらす角度
	m_aCamera[TYPE_MODELUI].swingInfo.fShiftLength	= 0.0f;			// 位置をずらす距離
	m_aCamera[TYPE_MODELUI].swingInfo.fSubAngle		= 0.0f;			// ずらす角度の減算量
	m_aCamera[TYPE_MODELUI].swingInfo.fSubLength	= 0.0f;			// ずらす距離の減算量

	// ビューポート情報を初期化
	m_aCamera[TYPE_MODELUI].viewportInfo.X		= 0;				// 左上隅のピクセル座標X
	m_aCamera[TYPE_MODELUI].viewportInfo.Y		= 0;				// 左上隅のピクセル座標Y
	m_aCamera[TYPE_MODELUI].viewportInfo.Width	= SCREEN_WIDTH;		// 描画する画面の横幅
	m_aCamera[TYPE_MODELUI].viewportInfo.Height	= SCREEN_HEIGHT;	// 描画する画面の縦幅
	m_aCamera[TYPE_MODELUI].viewportInfo.MinZ	= 0.0f;
	m_aCamera[TYPE_MODELUI].viewportInfo.MaxZ	= 0.5f;

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

		case STATE_TITLE_WAIT:	// タイトル待機状態

			// カメラの更新 (タイトル待機)
			TitleWait();

			break;

		case STATE_TITLE_ATK:	// タイトル攻撃状態

			// カメラの更新 (タイトル攻撃)
			TitleAtk();

			break;

		case STATE_ROTATE:	// 回転状態

			// カメラの更新 (回転)
			Rotate();

			break;

		case STATE_FOLLOW:	// 追従状態

			// カメラの更新 (追従)
			Follow();

			break;

		case STATE_RIDE:	// 騎乗状態

			// カメラの更新 (騎乗)
			Ride();

			break;

		case STATE_LOOK_PLAYER:	// プレイヤー注目状態

			// カメラの更新 (プレイヤー注目)
			LookPlayer();

			break;

		case STATE_LOOK_BOSS:	// ボス注目状態

			// カメラの更新 (ボス注目)
			LookBoss();

			break;

		default:	// 例外処理
			assert(false);
			break;
		}

		// カメラ揺れの更新
		Swing();
	}

	// デバッグ表示
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[ 視点 ]：%f %f %f\n", m_aCamera[TYPE_MAIN].posV.x, m_aCamera[TYPE_MAIN].posV.y, m_aCamera[TYPE_MAIN].posV.z);
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[注視点]：%f %f %f\n", m_aCamera[TYPE_MAIN].posR.x, m_aCamera[TYPE_MAIN].posR.y, m_aCamera[TYPE_MAIN].posR.z);
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[ 向き ]：%f %f %f\n", m_aCamera[TYPE_MAIN].rot.x, m_aCamera[TYPE_MAIN].rot.y, m_aCamera[TYPE_MAIN].rot.z);
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[ 距離 ]：%f\n", m_aCamera[TYPE_MAIN].fDis);
}

//============================================================
//	再設定処理
//============================================================
void CCamera::Reset(void)
{
	// 視認の情報を初期化
	memset(&m_look, 0, sizeof(m_look));

	for (int nCntCamera = 0; nCntCamera < TYPE_MAX; nCntCamera++)
	{ // カメラの総数分繰り返す

		// カメラ揺れ情報を初期化
		SSwing *pSwing = &m_aCamera[nCntCamera].swingInfo;
		pSwing->shiftPos	 = VEC3_ZERO;	// 位置ずれ量
		pSwing->fShiftAngle	 = 0.0f;		// 位置をずらす角度
		pSwing->fShiftLength = 0.0f;		// 位置をずらす距離
		pSwing->fSubAngle	 = 0.0f;		// ずらす角度の減算量
		pSwing->fSubLength	 = 0.0f;		// ずらす距離の減算量
	}
}

//============================================================
//	カメラ設定処理
//============================================================
void CCamera::SetCamera(const EType type)
{
	if (type <= NONE_IDX || type >= TYPE_MAX) { assert(false); return; }	// 種類範囲外

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	// ビューポートの設定
	pDevice->SetViewport(&m_aCamera[type].viewportInfo);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_aCamera[type].mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH
	( // 引数
		&m_aCamera[type].mtxProjection,	// プロジェクションマトリックス
		basic::VIEW_ANGLE,	// 視野角
		basic::VIEW_ASPECT,	// 画面のアスペクト比
		basic::VIEW_NEAR,	// Z軸の最小値
		basic::VIEW_FAR		// Z軸の最大値
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
//	カメラ目標位置の設定処理 (タイトル待機)
//============================================================
void CCamera::SetDestTitleWait(void)
{
	if (m_state != STATE_TITLE_WAIT) { return; }	// カメラタイトル待機状態以外

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = title::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.y = title::INIT_ROT.y;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);		// 現在向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot);	// 目標向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = title::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = m_aCamera[TYPE_MAIN].destPosR = title::INIT_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

}

//============================================================
//	カメラ目標位置の設定処理 (タイトル攻撃)
//============================================================
void CCamera::SetDestTitleAtk(void)
{
	// カメラ強制操作カウンターを設定
	m_look.nCounterForce = title::MOVE_FRAME;

	// 向き情報を設定
	m_aCamera[TYPE_MAIN].destRot.y = title::DEST_ROTY;					// 目標向きを設定
	m_look.fDiffRotY = title::DEST_ROTY - m_aCamera[TYPE_MAIN].rot.y;	// 差分向きを保存
	m_look.fOldRotY  = m_aCamera[TYPE_MAIN].rot.y;						// 視認開始時の向きを保存
}

//============================================================
//	カメラ目標位置の設定処理 (回転)
//============================================================
void CCamera::SetDestRotate(void)
{
	if (m_state != STATE_ROTATE) { return; }	// カメラ回転状態以外

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = rotate::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.y = rotate::INIT_ROT.y;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);		// 現在向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot);	// 目標向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = rotate::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = m_aCamera[TYPE_MAIN].destPosR = rotate::INIT_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの目標位置の設定処理 (追従)
//============================================================
void CCamera::SetDestFollow(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();	// プレイヤーリスト
	if (pList == nullptr)		 { return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { return; }	// プレイヤーが1人じゃない
	if (m_state != STATE_FOLLOW) { return; }	// カメラ追従状態以外

	auto player = pList->GetList().front();	// プレイヤー情報
	D3DXVECTOR3 diffPosV = VEC3_ZERO;		// 視点の差分位置
	D3DXVECTOR3 diffPosR = VEC3_ZERO;		// 注視点の差分位置
	D3DXVECTOR3 diffRot  = VEC3_ZERO;		// 差分向き

	//----------------------------------------------------
	//	向きの更新
	//----------------------------------------------------
	// 向きを設定
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = follow::INIT_ROTX;
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.y = player->GetVec3Rotation().y + D3DX_PI;

	// 向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot);

	//----------------------------------------------------
	//	距離の更新
	//----------------------------------------------------
	// 目標距離を設定
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = follow::INIT_DIS;
	if (m_aCamera[TYPE_MAIN].rot.x < HALF_PI)
	{ // 下から向き始めた場合

		// 地面を貫通しないよう補正
		m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis -= (follow::MAX_SUB_DIS / HALF_PI) * (HALF_PI - m_aCamera[TYPE_MAIN].rot.x);
	}

	//----------------------------------------------------
	//	位置の更新
	//----------------------------------------------------
	// 注視点をプレイヤーの頭の位置にする
	m_aCamera[TYPE_MAIN].posR = m_aCamera[TYPE_MAIN].destPosR = player->GetVec3Position() + D3DXVECTOR3(0.0f, player->GetHeight(), 0.0f);

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].destPosR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].destPosR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].destPosR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラ目標位置の設定処理 (騎乗)
//============================================================
void CCamera::SetDestRide(void)
{
	if (m_state != STATE_RIDE) { return; }	// カメラ騎乗状態以外
	CPlayer	*pPlayer = CScene::GetPlayer();	// プレイヤー情報
	CEnemy	*pBoss	 = CScene::GetBoss();	// ボス情報
	CStage	*pStage	 = CScene::GetStage();	// ステージ情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();		// プレイヤー位置
	D3DXVECTOR3 posBoss   = pBoss->GetVec3Position();		// ボス位置
	D3DXVECTOR3 posCenter = pStage->GetStageLimit().center;	// ステージ中央位置

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの設定
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = ride::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.x = atan2f(posCenter.x - posBoss.x, posCenter.z - posBoss.z) - ride::SUB_ROTY;

	// 向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot);

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = ride::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の設定
	m_aCamera[TYPE_MAIN].posR = m_aCamera[TYPE_MAIN].destPosR = posBoss + ride::OFFSET_POSR;

	// 視点の設定
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラ目標位置の設定処理 (プレイヤー注目)
//============================================================
void CCamera::SetDestLookPlayer(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();	// プレイヤーリスト
	if (pList == nullptr)				{ return; }		// リスト未使用
	if (pList->GetNumAll() != 1)		{ return; }		// プレイヤーが1人じゃない
	if (m_state != STATE_LOOK_PLAYER)	{ return; }		// カメラプレイヤー注目状態以外
	auto player = pList->GetList().front();				// プレイヤー情報

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = lookPlayer::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.y = lookPlayer::INIT_ROT.y;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);		// 現在向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot);	// 目標向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = lookPlayer::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = player->GetVec3Position();		// プレイヤー位置を指定
	m_aCamera[TYPE_MAIN].posR.y += lookPlayer::UP_POSR;			// 少し上にする
	m_aCamera[TYPE_MAIN].destPosR = m_aCamera[TYPE_MAIN].posR;	// 目標位置も同一にする

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラ目標位置の設定処理 (ボス注目)
//============================================================
void CCamera::SetDestLookBoss(void)
{
	CEnemy *pBoss = CScene::GetBoss();			// ボス情報
	if (pBoss == nullptr)			{ return; }	// ボスが存在しない
	if (m_state != STATE_LOOK_BOSS)	{ return; }	// ボス注目状態以外

	CMultiModel *pWaist = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_WAIST);	// 腰モデル情報
	D3DXVECTOR3 posBoss = useful::GetMatrixPosition(pWaist->GetMtxWorld());		// ボスの腰位置

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 向きの更新
	m_aCamera[TYPE_MAIN].rot.x = m_aCamera[TYPE_MAIN].destRot.x = atan2f(posBoss.z - lookBoss::INIT_POSV.z, posBoss.y - lookBoss::INIT_POSV.y);
	m_aCamera[TYPE_MAIN].rot.y = m_aCamera[TYPE_MAIN].destRot.y = 0.0f;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.x);		// 現在向きを正規化
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot.x);	// 目標向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = 1.0f;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 視点の更新
	m_aCamera[TYPE_MAIN].posV = m_aCamera[TYPE_MAIN].destPosV = lookBoss::INIT_POSV;

	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR.x = m_aCamera[TYPE_MAIN].destPosR.x = m_aCamera[TYPE_MAIN].posV.x + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posR.y = m_aCamera[TYPE_MAIN].destPosR.y = m_aCamera[TYPE_MAIN].posV.y + ((m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posR.z = m_aCamera[TYPE_MAIN].destPosR.z = m_aCamera[TYPE_MAIN].posV.z + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	視点の設定処理
//============================================================
void CCamera::SetPositionV(const D3DXVECTOR3& rPosV)
{
	SCamera *pCamera = &m_aCamera[TYPE_MAIN];	// メインカメラ情報

	// 視点・目標視点を設定
	pCamera->posV = pCamera->destPosV = rPosV;

	// 注視点・目標注視点の更新
	pCamera->posR.x = pCamera->destPosR.x = pCamera->posV.x + ((pCamera->fDis * sinf(pCamera->rot.x)) * sinf(pCamera->rot.y));
	pCamera->posR.y = pCamera->destPosR.y = pCamera->posV.y + ((pCamera->fDis * cosf(pCamera->rot.x)));
	pCamera->posR.z = pCamera->destPosR.z = pCamera->posV.z + ((pCamera->fDis * sinf(pCamera->rot.x)) * cosf(pCamera->rot.y));
}

//============================================================
//	注視点の設定処理
//============================================================
void CCamera::SetPositionR(const D3DXVECTOR3& rPosR)
{
	SCamera *pCamera = &m_aCamera[TYPE_MAIN];	// メインカメラ情報

	// 注視点・目標注視点を設定
	pCamera->posR = pCamera->destPosR = rPosR;

	// 視点・目標視点の更新
	pCamera->posV.x = pCamera->destPosV.x = pCamera->posR.x + ((-pCamera->fDis * sinf(pCamera->rot.x)) * sinf(pCamera->rot.y));
	pCamera->posV.y = pCamera->destPosV.y = pCamera->posR.y + ((-pCamera->fDis * cosf(pCamera->rot.x)));
	pCamera->posV.z = pCamera->destPosV.z = pCamera->posR.z + ((-pCamera->fDis * sinf(pCamera->rot.x)) * cosf(pCamera->rot.y));
}

//============================================================
//	向きの設定処理
//============================================================
void CCamera::SetRotation(const D3DXVECTOR3& rRot)
{
	SCamera *pCamera = &m_aCamera[TYPE_MAIN];	// メインカメラ情報

	// 向き・目標向きを設定
	pCamera->rot = pCamera->destRot = rRot;

	// 視点・目標視点の更新
	pCamera->posV.x = pCamera->destPosV.x = pCamera->posR.x + ((-pCamera->fDis * sinf(pCamera->rot.x)) * sinf(pCamera->rot.y));
	pCamera->posV.y = pCamera->destPosV.y = pCamera->posR.y + ((-pCamera->fDis * cosf(pCamera->rot.x)));
	pCamera->posV.z = pCamera->destPosV.z = pCamera->posR.z + ((-pCamera->fDis * sinf(pCamera->rot.x)) * cosf(pCamera->rot.y));
}

//============================================================
//	距離の設定処理
//============================================================
void CCamera::SetDistance(const float fDis)
{
	SCamera *pCamera = &m_aCamera[TYPE_MAIN];	// メインカメラ情報

	// 距離・目標距離を設定
	pCamera->fDis = pCamera->fDestDis = fDis;

	// 視点・目標視点の更新
	pCamera->posV.x = pCamera->destPosV.x = pCamera->posR.x + ((-pCamera->fDis * sinf(pCamera->rot.x)) * sinf(pCamera->rot.y));
	pCamera->posV.y = pCamera->destPosV.y = pCamera->posR.y + ((-pCamera->fDis * cosf(pCamera->rot.x)));
	pCamera->posV.z = pCamera->destPosV.z = pCamera->posR.z + ((-pCamera->fDis * sinf(pCamera->rot.x)) * cosf(pCamera->rot.y));
}

//============================================================
//	カメラ取得処理
//============================================================
CCamera::SCamera CCamera::GetCamera(const EType type)
{
	// カメラの情報を返す
	return m_aCamera[type];
}

//============================================================
//	カメラ状態の設定処理
//============================================================
void CCamera::SetState(const EState state)
{
	// 状態を設定
	m_state = state;

	// 再設定
	Reset();
}

//============================================================
//	カメラ状態取得処理
//============================================================
CCamera::EState CCamera::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	追従カメラの位置視認処理
//============================================================
void CCamera::SetFollowLook(const D3DXVECTOR3& rLookPos)
{
	// 変数を宣言
	D3DXVECTOR3 vecLook = VEC3_ZERO;	// 視認オブジェクト方向
	float fRotY = 0.0f;

	// 視認するオブジェクトの方向を求める
	vecLook = rLookPos - m_aCamera[TYPE_MAIN].posR;
	vecLook.y = 0.0f;	// Y方向は無視

	// ボス視認カウンターを設定
	m_look.nCounterForce = follow::LOOK_BOSS_FRAME;

	// 視認ベクトルから向きを求める
	fRotY = atan2f(vecLook.x, vecLook.z);

	// 目標向きを設定
	m_aCamera[TYPE_MAIN].destRot.y = fRotY;

	// 差分向きを保存
	m_look.fDiffRotY = fRotY - m_aCamera[TYPE_MAIN].rot.y;

	// 視認開始時の向きを保存
	m_look.fOldRotY = m_aCamera[TYPE_MAIN].rot.y;
}

//============================================================
//	カメラ揺れの設定処理
//============================================================
void CCamera::SetSwing(const EType type, const SSwing swing)
{
	if (m_state == STATE_CONTROL) { return; }	// カメラ操作状態

	if (type > NONE_IDX && type < TYPE_MAX)
	{ // 種類が範囲内の場合

		// 引数のカメラ揺れ情報を設定
		m_aCamera[type].swingInfo = swing;
	}
	else { assert(false); }	// 範囲外
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
//	生成処理
//============================================================
CCamera *CCamera::Create(void)
{
	// カメラの生成
	CCamera *pCamera = new CCamera;
	if (pCamera == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// カメラの初期化
		if (FAILED(pCamera->Init()))
		{ // 初期化に失敗した場合

			// カメラの破棄
			SAFE_DELETE(pCamera);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pCamera;
	}
}

//============================================================
//	破棄処理
//============================================================
void CCamera::Release(CCamera *&prCamera)
{
	// カメラの終了
	assert(prCamera != nullptr);
	prCamera->Uninit();

	// メモリ開放
	SAFE_DELETE(prCamera);
}

//============================================================
//	カメラの更新処理 (タイトル待機)
//============================================================
void CCamera::TitleWait(void)
{
	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x =  title::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y += title::ADD_ROTY;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = title::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = title::INIT_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (タイトル攻撃)
//============================================================
void CCamera::TitleAtk(void)
{
	//--------------------------------------------------------
	//	向き・距離の更新
	//--------------------------------------------------------
	if (m_look.nCounterForce > 0)
	{ // カメラの強制操作がONの場合

		// カウンターを減算
		m_look.nCounterForce--;

		// カメラの目標向きを求める
		float fRate = easeing::InOutQuad(-(m_look.nCounterForce - title::MOVE_FRAME), 0, title::MOVE_FRAME);	// カウンターからイージング値を計算
		float fCurRotY = m_look.fDiffRotY * fRate;	// 差分向きと割合から今の向きを求める
		useful::NormalizeRot(fCurRotY);				// 目標向きを正規化

		// 左右の現在向きの更新
		m_aCamera[TYPE_MAIN].rot.y = m_look.fOldRotY + fCurRotY;
		useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);	// 現在向きを正規化
	}

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = title::INIT_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (回転)
//============================================================
void CCamera::Rotate(void)
{
	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = rotate::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y += rotate::ADD_ROTY;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = rotate::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = rotate::INIT_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (追従)
//============================================================
void CCamera::Follow(void)
{
	CInputPad *pPad = GET_INPUTPAD;	// パッド情報
	CListManager<CPlayer> *pList = CPlayer::GetList();	// プレイヤーリスト
	if (pList == nullptr) 		 { return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { return; }	// プレイヤーが1人じゃない
	if (m_state != STATE_FOLLOW) { return; }	// カメラ追従状態以外

	auto player = pList->GetList().front();	// プレイヤー情報
	D3DXVECTOR3 diffPosV = VEC3_ZERO;		// 視点の差分位置
	D3DXVECTOR3 diffPosR = VEC3_ZERO;		// 注視点の差分位置
	D3DXVECTOR3 diffRot  = VEC3_ZERO;		// 差分向き

	//----------------------------------------------------
	//	向きの更新
	//----------------------------------------------------
	if (m_look.nCounterForce > 0)
	{ // カメラの強制操作がONの場合

		// カウンターを減算
		m_look.nCounterForce--;

		// カメラの上下操作のみ受け付ける
		float fRTilt = pPad->GetPressRStickTilt();	// スティックの傾き量
		if (pad::DEAD_ZONE < fRTilt)
		{ // デッドゾーン以上の場合

			// 目標向きを設定
			float fMove = fRTilt * follow::STICK_REV;	// カメラ回転量
			m_aCamera[TYPE_MAIN].destRot.x += sinf(pPad->GetPressRStickRot()) * fMove * follow::ROTX_REV;

			// 目標向きを正規化
			useful::LimitNum(m_aCamera[TYPE_MAIN].destRot.x, follow::LIMIT_ROT_LOW, follow::LIMIT_ROT_HIGH);

			// 差分向きを計算
			diffRot.x = m_aCamera[TYPE_MAIN].destRot.x - m_aCamera[TYPE_MAIN].rot.x;
			useful::NormalizeRot(diffRot.x);	// 差分向きを正規化

			// 現在向きの更新
			m_aCamera[TYPE_MAIN].rot.x += diffRot.x * follow::REV_ROT;
			useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.x);	// 現在向きを正規化
		}

		// カメラの目標向きを求める
		float fRate = easeing::InOutQuad(-(m_look.nCounterForce - follow::LOOK_BOSS_FRAME), 0, follow::LOOK_BOSS_FRAME);	// カウンターからイージング値を計算
		float fCurRotY = m_look.fDiffRotY * fRate;	// 差分向きと割合から今の向きを求める
		useful::NormalizeRot(fCurRotY);				// 目標向きを正規化

		// 左右の現在向きの更新
		m_aCamera[TYPE_MAIN].rot.y = m_look.fOldRotY + fCurRotY;
		useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);	// 現在向きを正規化
	}
	else
	{ // カメラの強制操作がOFFの場合

		float fRTilt = pPad->GetPressRStickTilt();	// スティックの傾き量
		if (pad::DEAD_ZONE < fRTilt)
		{ // デッドゾーン以上の場合

			// 目標向きを設定
			float fMove = fRTilt * follow::STICK_REV;	// カメラ回転量
			m_aCamera[TYPE_MAIN].destRot.x += sinf(pPad->GetPressRStickRot()) * fMove * follow::ROTX_REV;
			m_aCamera[TYPE_MAIN].destRot.y += cosf(pPad->GetPressRStickRot()) * fMove;

			// 目標向きを正規化
			useful::LimitNum(m_aCamera[TYPE_MAIN].destRot.x, follow::LIMIT_ROT_LOW, follow::LIMIT_ROT_HIGH);
			useful::NormalizeRot(m_aCamera[TYPE_MAIN].destRot.y);

			// 差分向きを計算
			diffRot = m_aCamera[TYPE_MAIN].destRot - m_aCamera[TYPE_MAIN].rot;
			useful::NormalizeRot(diffRot);	// 差分向きを正規化

			// 現在向きの更新
			m_aCamera[TYPE_MAIN].rot += diffRot * follow::REV_ROT;
			useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化
		}
	}

	//----------------------------------------------------
	//	距離の更新
	//----------------------------------------------------
	// 目標距離を設定
	m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis = follow::INIT_DIS;
	if (m_aCamera[TYPE_MAIN].rot.x < HALF_PI)
	{ // 下から向き始めた場合

		// 地面を貫通しないよう補正
		m_aCamera[TYPE_MAIN].fDis = m_aCamera[TYPE_MAIN].fDestDis -= (follow::MAX_SUB_DIS / HALF_PI) * (HALF_PI - m_aCamera[TYPE_MAIN].rot.x);
	}

	//----------------------------------------------------
	//	位置の更新
	//----------------------------------------------------
	// 注視点をプレイヤーの頭の位置にする
	m_aCamera[TYPE_MAIN].destPosR = player->GetVec3Position() + D3DXVECTOR3(0.0f, player->GetHeight(), 0.0f);

	// 視点の更新
	m_aCamera[TYPE_MAIN].destPosV.x = m_aCamera[TYPE_MAIN].destPosR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].destPosV.y = m_aCamera[TYPE_MAIN].destPosR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].destPosV.z = m_aCamera[TYPE_MAIN].destPosR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

	// 注視点の差分位置を計算
	diffPosR = m_aCamera[TYPE_MAIN].destPosR - m_aCamera[TYPE_MAIN].posR;

	// 視点の差分位置を計算
	diffPosV = m_aCamera[TYPE_MAIN].destPosV - m_aCamera[TYPE_MAIN].posV;

	// 注視点の現在位置を更新
	m_aCamera[TYPE_MAIN].posR.x += diffPosR.x * follow::REV_POSR.x;
	m_aCamera[TYPE_MAIN].posR.y += diffPosR.y * follow::REV_POSR.y;
	m_aCamera[TYPE_MAIN].posR.z += diffPosR.z * follow::REV_POSR.z;

	// 視点の現在位置を更新
	m_aCamera[TYPE_MAIN].posV.x += diffPosV.x * follow::REV_POSV.x;
	m_aCamera[TYPE_MAIN].posV.y += diffPosV.y * follow::REV_POSV.y;
	m_aCamera[TYPE_MAIN].posV.z += diffPosV.z * follow::REV_POSV.z;
}

//============================================================
//	カメラの更新処理 (騎乗)
//============================================================
void CCamera::Ride(void)
{
	CPlayer	*pPlayer = CScene::GetPlayer();	// プレイヤー情報
	CEnemy	*pBoss	 = CScene::GetBoss();	// ボス情報
	CStage	*pStage	 = CScene::GetStage();	// ステージ情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();		// プレイヤー位置
	D3DXVECTOR3 posBoss   = pBoss->GetVec3Position();		// ボス位置
	D3DXVECTOR3 posCenter = pStage->GetStageLimit().center;	// ステージ中央位置

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = ride::INIT_ROT.x;
	m_aCamera[TYPE_MAIN].rot.y = atan2f(posCenter.x - posBoss.x, posCenter.z - posBoss.z) - ride::SUB_ROTY;
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot);	// 現在向きを正規化

	//--------------------------------------------------------
	//	距離の更新
	//--------------------------------------------------------
	m_aCamera[TYPE_MAIN].fDis = ride::INIT_DIS;

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = posBoss + ride::OFFSET_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (プレイヤー注目)
//============================================================
void CCamera::LookPlayer(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();	// プレイヤーリスト
	if (pList == nullptr)				{ return; }		// リスト未使用
	if (pList->GetNumAll() != 1)		{ return; }		// プレイヤーが1人じゃない
	if (m_state != STATE_LOOK_PLAYER)	{ return; }		// プレイヤー注目状態以外
	auto player = pList->GetList().front();				// プレイヤー情報

	//--------------------------------------------------------
	//	向き・距離の更新
	//--------------------------------------------------------
	if (m_look.nCounterForce > 0)
	{ // カメラの強制操作がONの場合

		// カウンターを減算
		m_look.nCounterForce--;

		// カウンターからイージング値を計算
		float fRate = easeing::InOutSine
		( // 引数
			-(m_look.nCounterForce - lookPlayer::CHANGE_POSV_FRAME),	// 現在カウンター
			0,								// 最小値
			lookPlayer::CHANGE_POSV_FRAME	// 最大値
		);

		// 目標向きを求める
		float fCurRotY = m_look.fDiffRotY * fRate;	// 差分向きと割合から今の向きを求める
		useful::NormalizeRot(fCurRotY);				// 目標向きを正規化

		// 現在向きの更新
		m_aCamera[TYPE_MAIN].rot.y = m_look.fOldRotY - fCurRotY;
		useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);	// 現在向きを正規化

		// 目標距離を求める
		float fCurDis = m_look.fDiffDis * fRate;	// 差分距離きと割合から今の距離を求める

		// 現在距離の更新
		m_aCamera[TYPE_MAIN].fDis = m_look.fOldDis + fCurDis;
	}
	else
	{ // カメラの強制操作がOFFの場合

		// 別モーションの指定がされている場合エラー
		assert(player->GetMotionType(CPlayer::BODY_LOWER) == CPlayer::L_MOTION_SPAWN);

		if (player->GetMotionKey(CPlayer::BODY_LOWER)		 == lookPlayer::SWORD_SHAKE_KEY
		&&  player->GetMotionKeyCounter(CPlayer::BODY_LOWER) == lookPlayer::SWORD_SHAKE_FRAME)
		{ // 剣を振り下ろしたタイミングの場合

			// カメラ強制操作カウンターを設定
			m_look.nCounterForce = lookPlayer::CHANGE_POSV_FRAME;

			// 向き情報を設定
			m_aCamera[TYPE_MAIN].destRot.y = lookPlayer::DESTPOSV_ROTY;					// 目標向きを設定
			m_look.fDiffRotY = lookPlayer::DESTPOSV_ROTY - m_aCamera[TYPE_MAIN].rot.y;	// 差分向きを保存
			m_look.fOldRotY  = m_aCamera[TYPE_MAIN].rot.y;								// 視認開始時の向きを保存

			// 距離情報を設定
			m_aCamera[TYPE_MAIN].fDestDis = lookPlayer::DESTPOSV_DIS;					// 目標距離を設定
			m_look.fDiffDis = lookPlayer::DESTPOSV_DIS - m_aCamera[TYPE_MAIN].fDis;		// 差分距離を保存
			m_look.fOldDis  = m_aCamera[TYPE_MAIN].fDis;								// 視認開始時の距離を保存
		}
	}

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR = player->GetVec3Position();	// プレイヤー位置を指定
	m_aCamera[TYPE_MAIN].posR.y += lookPlayer::UP_POSR;

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラの更新処理 (ボス注目)
//============================================================
void CCamera::LookBoss(void)
{
	CEnemy *pBoss = CScene::GetBoss();			// ボス情報
	if (pBoss == nullptr)			{ return; }	// ボスが存在しない
	if (m_state != STATE_LOOK_BOSS)	{ return; }	// ボス注目状態以外

	CMultiModel *pWaist = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_WAIST);	// 腰モデル情報
	D3DXVECTOR3 posBoss = useful::GetMatrixPosition(pWaist->GetMtxWorld());		// ボスの腰位置

	//--------------------------------------------------------
	//	向きの更新
	//--------------------------------------------------------
	// 現在向きの更新
	m_aCamera[TYPE_MAIN].rot.x = atan2f(posBoss.z - lookBoss::INIT_POSV.z, posBoss.y - lookBoss::INIT_POSV.y);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.x);	// 現在向きを正規化

	//--------------------------------------------------------
	//	位置の更新
	//--------------------------------------------------------
	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR.x = m_aCamera[TYPE_MAIN].posV.x + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posR.y = m_aCamera[TYPE_MAIN].posV.y + ((m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posR.z = m_aCamera[TYPE_MAIN].posV.z + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
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
	CInputMouse	*pMouse = GET_INPUTMOUSE;	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (pMouse->IsPress(CInputMouse::KEY_LEFT) && pMouse->IsPress(CInputMouse::KEY_RIGHT))
	{ // 右クリックと左クリックが押されている場合

		// 視点を移動
		m_aCamera[TYPE_MAIN].posV.x -= sinf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.z -= cosf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.x += sinf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posV.z += cosf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * control::REV_MOVE_MOUSE;

		// 注視点を移動
		m_aCamera[TYPE_MAIN].posR.x -= sinf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.z -= cosf(m_aCamera[TYPE_MAIN].rot.y + (D3DX_PI * 0.5f)) * mouseMove.x * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.x += sinf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * control::REV_MOVE_MOUSE;
		m_aCamera[TYPE_MAIN].posR.z += cosf(m_aCamera[TYPE_MAIN].rot.y) * mouseMove.y * control::REV_MOVE_MOUSE;
	}
}

//============================================================
//	距離の更新処理 (操作)
//============================================================
void CCamera::Distance(void)
{
	// ポインタを宣言
	CInputMouse	*pMouse = GET_INPUTMOUSE;	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (mouseMove.z != 0.0f)
	{ // マウスホイールが操作された場合

		// 距離を加算
		m_aCamera[TYPE_MAIN].fDis += mouseMove.z * control::REV_DIS_MOUSE;
	}

	// 距離の補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].fDis, basic::MIN_DIS, basic::MAX_DIS);
}

//============================================================
//	向きの更新処理 (操作)
//============================================================
void CCamera::Rotation(void)
{
	// ポインタを宣言
	CInputMouse	*pMouse = GET_INPUTMOUSE;	// マウスの取得

	// 変数を宣言
	D3DXVECTOR3 mouseMove = pMouse->GetMove();	// マウスの移動量

	// マウス操作の更新
	if (pMouse->IsPress(CInputMouse::KEY_LEFT) && !pMouse->IsPress(CInputMouse::KEY_RIGHT))
	{ // 左クリックだけが押されている場合

		// カメラのY軸を回転
		m_aCamera[TYPE_MAIN].rot.y += mouseMove.x * control::REV_ROT_MOUSE;

		// カメラのX軸を回転
		m_aCamera[TYPE_MAIN].rot.x += mouseMove.y * control::REV_ROT_MOUSE;
	}

	// 向きの補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].rot.x, basic::LIMIT_ROT_LOW, basic::LIMIT_ROT_HIGH);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);

	// 視点の更新
	m_aCamera[TYPE_MAIN].posV.x = m_aCamera[TYPE_MAIN].posR.x + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posV.y = m_aCamera[TYPE_MAIN].posR.y + ((-m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posV.z = m_aCamera[TYPE_MAIN].posR.z + ((-m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));

	// マウス操作の更新
	if (pMouse->IsPress(CInputMouse::KEY_RIGHT) && !pMouse->IsPress(CInputMouse::KEY_LEFT))
	{ // 右クリックだけが押されている場合

		// カメラのY軸を回転
		m_aCamera[TYPE_MAIN].rot.y += mouseMove.x * control::REV_ROT_MOUSE;

		// カメラのX軸を回転
		m_aCamera[TYPE_MAIN].rot.x += mouseMove.y * control::REV_ROT_MOUSE;
	}

	// 向きの補正
	useful::LimitNum(m_aCamera[TYPE_MAIN].rot.x, basic::LIMIT_ROT_LOW, basic::LIMIT_ROT_HIGH);
	useful::NormalizeRot(m_aCamera[TYPE_MAIN].rot.y);

	// 注視点の更新
	m_aCamera[TYPE_MAIN].posR.x = m_aCamera[TYPE_MAIN].posV.x + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * sinf(m_aCamera[TYPE_MAIN].rot.y));
	m_aCamera[TYPE_MAIN].posR.y = m_aCamera[TYPE_MAIN].posV.y + ((m_aCamera[TYPE_MAIN].fDis * cosf(m_aCamera[TYPE_MAIN].rot.x)));
	m_aCamera[TYPE_MAIN].posR.z = m_aCamera[TYPE_MAIN].posV.z + ((m_aCamera[TYPE_MAIN].fDis * sinf(m_aCamera[TYPE_MAIN].rot.x)) * cosf(m_aCamera[TYPE_MAIN].rot.y));
}

//============================================================
//	カメラ揺れの更新処理
//============================================================
void CCamera::Swing(void)
{
	if (m_state == STATE_CONTROL) { return; }	// カメラ操作状態

	for (int nCntCamera = 0; nCntCamera < TYPE_MAX; nCntCamera++)
	{ // カメラの総数分繰り返す

		SCamera *pCamera = &m_aCamera[nCntCamera];				// カメラ情報
		SSwing  *pSwing  = &m_aCamera[nCntCamera].swingInfo;	// カメラ揺れ情報

		if (pSwing->fShiftLength > 0.0f)
		{ // 注視点のずらし量が設定されている場合

			// 変数を宣言
			D3DXQUATERNION quat;	// クォータニオン
			D3DXMATRIX mtxRot;		// 回転マトリックス
			D3DXVECTOR3 offset;		// 位置ずれオフセット
			float fRotY;			// 位置ずれ向き
			D3DXVECTOR3 vecAxis = pCamera->posR - pCamera->posV;	// 回転軸ベクトル

			// クォータニオンを作成
			D3DXVec3Normalize(&vecAxis, &vecAxis);	// 回転軸を正規化
			D3DXQuaternionRotationAxis(&quat, &vecAxis, pSwing->fShiftAngle);

			// 回転マトリックスを作成
			D3DXMatrixRotationQuaternion(&mtxRot, &quat);

			// 位置をずらす向きを求める
			fRotY = atan2f(-vecAxis.z, vecAxis.x);

			// 位置ずれオフセットを設定
			float fCalcTemp = pSwing->fShiftLength * (fabsf(pCamera->fDis) * swing::REV_DIS);
			offset = D3DXVECTOR3(sinf(fRotY) * fCalcTemp, 0.0f, cosf(fRotY) * fCalcTemp);

			// オフセットを反映した回転マトリックスを座標変換
			D3DXVec3TransformCoord(&pSwing->shiftPos, &offset, &mtxRot);

			// 注視点に位置のずれを加算
			pCamera->posR += pSwing->shiftPos;

			// 位置ずれ量を減算
			pSwing->fShiftAngle  -= pSwing->fSubAngle;
			pSwing->fShiftLength -= pSwing->fSubLength;

			// 角度を補正
			useful::NormalizeRot(pSwing->fShiftAngle);

			// 距離を補正
			useful::LimitMinNum(pSwing->fShiftLength, 0.0f);
		}
	}
}
