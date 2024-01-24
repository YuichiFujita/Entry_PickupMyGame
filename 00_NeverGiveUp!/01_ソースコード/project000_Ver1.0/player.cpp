//============================================================
//
//	プレイヤー処理 [player.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "player.h"
#include "manager.h"
#include "sceneGame.h"
#include "gameManager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "collision.h"
#include "fade.h"

#include "multiModel.h"
#include "objectOrbit.h"
#include "shadow.h"
#include "object2D.h"
#include "timerManager.h"
#include "rankingManager.h"
#include "stage.h"
#include "field.h"
#include "building.h"
#include "obstacle.h"
#include "savePoint.h"
#include "goalPoint.h"
#include "friend.h"

#include "effect3D.h"
#include "particle3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* SETUP_TXT = "data\\TXT\\player.txt";	// プレイヤーセットアップテキスト

	const int PRIORITY = 3;	// プレイヤーの優先順位
	const int CNT_WAIT[CRetentionManager::RESULT_MAX] = { 0, 40, 220 };	// リザルト遷移時の余韻フレーム

	// プレイヤー基本情報
	namespace basic
	{
		const float	MOVE		= 2.8f;		// 移動量
		const float	JUMP		= 21.0f;	// ジャンプ上昇量
		const float	GRAVITY		= 1.0f;		// 重力
		const float	RADIUS		= 20.0f;	// 半径
		const float	HEIGHT		= 100.0f;	// 縦幅
		const float	REV_ROTA	= 0.15f;	// 向き変更の補正係数
		const float	BLOW_SIDE	= 10.0f;	// 吹っ飛び時の横移動量
		const float	BLOW_UP		= 30.0f;	// 吹っ飛び時の縦移動量
		const float	ADD_MOVE	= 0.08f;	// 非アクション時の速度加算量
		const float	MIN_VARY	= 0.001f;	// 向きと目標向きの違う量の許容値

		const int	LAND_SE_CNT = 10;			// 着地時にSEを鳴らす耐空フレーム
		const int	JUMP_SAVECONTROL_CNT = 10;	// ジャンプ操作を保持するフレーム

		const float	JUMPPAD_MOVE	= 50.0f;	// ジャンプパッドの上移動量
		const float	NOR_JUMP_REV	= 0.16f;	// 通常状態時の空中の移動量の減衰係数
		const float	NOR_LAND_REV	= 0.16f;	// 通常状態時の地上の移動量の減衰係数
		const float	DMG_JUMP_REV	= 0.015f;	// ダメージ状態時の空中の移動量の減衰係数
		const float	DMG_LAND_REV	= 0.25f;	// ダメージ状態時の地上の移動量の減衰係数
		const float DMG_SUB_ALPHA	= 0.025f;	// ダメージ状態時の透明度の減算量
		const float	SPAWN_ADD_ALPHA	= 0.03f;	// スポーン状態時の透明度の加算量

		const D3DXVECTOR3 DMG_ADDROT = D3DXVECTOR3(0.04f, 0.0f, -0.02f);	// ダメージ状態時のプレイヤー回転量
	}

	// スライディング情報
	namespace slide
	{
		const float	CONTROL_MIN		= 2.4f;		// スライディングが可能になる移動量
		const float	PLUS_SUB_MOVE	= 0.0035f;	// 移動量の最大追加減算量

		const int	MIN_END_CNT	= 30;		// スライディングの解除操作ができるようになるカウント
		const int	MAX_END_CNT	= 80;		// スライディングが強制終了するカウント
		const float	MIN_MOVE	= 1.25f;	// 移動量の最低速度
		const float	SUB_MOVE	= 0.01f;	// スライディング時の速度減算量
	}

	// 壁走り情報
	namespace walldash
	{
		const float	CONTROL_MIN		= 2.0f;		// 壁走りが可能になる移動量
		const float	PLUS_SUB_MOVE	= 0.0f;		// 移動量の最大追加減算量
		const float	PLUS_COLL_SIZE	= 90.0f;	// 判定の拡張量
		const float	MAX_GRAVITY		= -8.2f;	// 重力の最大値
		const float	GRAVITY_LOW		= 0.28f;	// 上昇量がない場合の重力
		const float	GRAVITY_HIGH	= 1.3f;		// 上昇量がある場合の重力
		const float	PLUSMOVE_UP		= 22.0f;	// 壁ダッシュ成功時の上昇量
		const float	PLUSMOVE_SIDE	= 3.0f;		// 壁ダッシュ成功時の加速量
		const float	REV_PLUSMOVE	= 0.1f;		// プラス移動量の減衰係数

		const int	MIN_END_CNT	= 16;		// 壁走りの解除操作ができるようになるカウント
		const float	MIN_MOVE	= 1.5f;		// 移動量の最低速度
		const float	SUB_MOVE	= 0.004f;	// 壁走り時の速度減算量
		const float	COLL_SIZE	= 100.0f;	// 壁走りの判定大きさ
		const float	BOOST_RATE	= 0.0f;		// 加速できる位置割合
	}

	// クリア表示情報
	namespace clear
	{
		const D3DXVECTOR3 POS	= D3DXVECTOR3(-420.0f, 620.0f, 0.0f);	// クリア表示の位置
		const D3DXVECTOR3 SIZE	= D3DXVECTOR3(823.0f, 200.0f, 0.0f);	// クリア表示の大きさ

		const float MOVE_POS = 82.0f;	// 移動量
		const float STOP_POS = 450.0f;	// 停止位置
	}

	// エフェクト情報
	namespace effect
	{
		const float	SMOKE_RADIUS	= 30.0f;	// 煙の半径
		const int	SMOKE_LIFE		= 20;		// 煙の寿命
	}

	// パーティクル情報
	namespace particle
	{
		const D3DXCOLOR	COL_HEAL = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);	// 回復パーティクルの色
	}

	// プレイヤー他クラス情報
	namespace other
	{
		const int ORBIT_PARTS[]		= { CPlayer::MODEL_ARMDL, CPlayer::MODEL_ARMDR, CPlayer::MODEL_LEGDL, CPlayer::MODEL_LEGDR };	// 軌跡を出すパーツのインデックス
		const int ORBIT_PART		= 20;	// 軌跡の分割数
		const D3DXCOLOR ORBIT_COL	= D3DXCOLOR(0.0f, 0.9f, 1.0f, 0.8f);	// 軌跡の色

		const D3DXVECTOR3 SHADOW_SIZE = D3DXVECTOR3(80.0f, 0.0f, 80.0f);	// 影の大きさ
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CPlayer::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\clear000.png",	// クリア表示テクスチャ
};
const char *CPlayer::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\PLAYER\\00_waist.x",	// 腰
	"data\\MODEL\\PLAYER\\01_body.x",	// 体
	"data\\MODEL\\PLAYER\\02_head.x",	// 頭
	"data\\MODEL\\PLAYER\\03_armUL.x",	// 左上腕
	"data\\MODEL\\PLAYER\\04_armUR.x",	// 右上腕
	"data\\MODEL\\PLAYER\\05_armDL.x",	// 左下腕
	"data\\MODEL\\PLAYER\\06_armDR.x",	// 右下腕
	"data\\MODEL\\PLAYER\\07_handL.x",	// 左手
	"data\\MODEL\\PLAYER\\08_handR.x",	// 右手
	"data\\MODEL\\PLAYER\\09_legUL.x",	// 左太もも
	"data\\MODEL\\PLAYER\\10_legUR.x",	// 右太もも
	"data\\MODEL\\PLAYER\\11_legDL.x",	// 左脛
	"data\\MODEL\\PLAYER\\12_legDR.x",	// 右脛
	"data\\MODEL\\PLAYER\\13_footL.x",	// 左足
	"data\\MODEL\\PLAYER\\14_footR.x",	// 右足
};

//************************************************************
//	子クラス [CPlayer] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayer::CPlayer() : CObjectChara(CObject::LABEL_PLAYER, PRIORITY)
{
	// メンバ変数をクリア
	memset(&m_apOrbit[0], 0, sizeof(m_apOrbit));	// 軌跡の情報
	m_pShadow			= NULL;			// 影の情報
	m_pGoal				= NULL;			// ゴールの情報
	m_pClear			= NULL;			// クリア表示の情報
	m_oldPos			= VEC3_ZERO;	// 過去位置
	m_move				= VEC3_ZERO;	// 移動量
	m_destRot			= VEC3_ZERO;	// 目標向き
	m_land				= LAND_NONE;	// 着地物
	m_state				= STATE_NONE;	// 状態
	m_nCounterState		= 0;			// 状態管理カウンター
	m_nCounterSlide		= 0;			// スライディング管理カウンター
	m_nCounterWallDash	= 0;			// 壁走り管理カウンター
	m_nCounterLand		= 0;			// 着地管理カウンター
	m_nCounterJump		= 0;			// ジャンプ管理カウンター
	m_fMove				= 0.0f;			// 移動量
	m_fPlusMove			= 0.0f;			// プラス移動量
	m_bJump				= false;		// ジャンプ状況
	m_bJumpControl		= false;		// ジャンプ操作状況
	m_bSlide			= false;		// スライディング状況
	m_bSlideControl		= false;		// スライディング操作状況
	m_bWallDash			= false;		// 壁走り状況
	m_bWallDashControl	= false;		// 壁走り操作状況
}

//============================================================
//	デストラクタ
//============================================================
CPlayer::~CPlayer()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayer::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apOrbit[0], 0, sizeof(m_apOrbit));	// 軌跡の情報
	m_pShadow			= NULL;			// 影の情報
	m_pGoal				= NULL;			// ゴールの情報
	m_pClear			= NULL;			// クリア表示の情報
	m_oldPos			= VEC3_ZERO;	// 過去位置
	m_move				= VEC3_ZERO;	// 移動量
	m_destRot			= VEC3_ZERO;	// 目標向き
	m_land				= LAND_NONE;	// 着地物
	m_state				= STATE_NONE;	// 状態
	m_nCounterState		= 0;			// 状態管理カウンター
	m_nCounterSlide		= 0;			// スライディング管理カウンター
	m_nCounterWallDash	= 0;			// 壁走り管理カウンター
	m_nCounterLand		= 0;			// 着地管理カウンター
	m_nCounterJump		= 0;			// ジャンプ管理カウンター
	m_fMove				= basic::MOVE;	// 移動量
	m_fPlusMove			= 0.0f;			// プラス移動量
	m_bJump				= true;			// ジャンプ状況
	m_bJumpControl		= false;		// ジャンプ操作状況
	m_bSlide			= false;		// スライディング状況
	m_bSlideControl		= false;		// スライディング操作状況
	m_bWallDash			= false;		// 壁走り状況
	m_bWallDashControl	= false;		// 壁走り操作状況

	// オブジェクトキャラクターの初期化
	if (FAILED(CObjectChara::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// セットアップの読み込み
	LoadSetup();

	// モデル情報の設定
	SetModelInfo();

	for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の生成
		m_apOrbit[nCntPlayer] = CObjectOrbit::Create
		( // 引数
			GetMultiModel(other::ORBIT_PARTS[nCntPlayer])->GetPtrMtxWorld(),	// 親マトリックス	
			other::ORBIT_COL,	// 色
			(CObjectOrbit::EOffset)nCntPlayer,	// オフセット
			other::ORBIT_PART	// 分割数
		);
		if (m_apOrbit[nCntPlayer] == NULL)
		{ // 非使用中の場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 自動描画をOFFに設定
		m_apOrbit[nCntPlayer]->SetEnableDraw(false);
	}

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, other::SHADOW_SIZE, this);
	if (m_pShadow == NULL)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// クリア表示の生成
	m_pClear = CObject2D::Create(clear::POS, clear::SIZE);
	if (m_pClear == NULL)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを読込・割当
	m_pClear->BindTexture(mc_apTextureFile[TEXTURE_CLEAR]);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayer::Uninit(void)
{
	for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の終了
		m_apOrbit[nCntPlayer]->Uninit();
	}

	// 影の終了
	m_pShadow->Uninit();

	// クリア表示の終了
	m_pClear->Uninit();

	// オブジェクトキャラクターの終了
	CObjectChara::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayer::Update(void)
{
	// 変数を宣言
	EMotion currentMotion = MOTION_WAVEHAND;	// 現在のモーション

	// 過去位置の更新
	UpdateOldPosition();

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:
		break;

	case STATE_SPAWN:

		// スポーン状態時の更新
		currentMotion = UpdateSpawn();

		break;

	case STATE_NORMAL:

		// 通常状態の更新
		currentMotion = UpdateNormal();

		break;

	case STATE_DAMAGE:

		// ダメージ状態時の更新
		currentMotion = UpdateDamage();

		break;

	case STATE_UNION:

		// 合流状態時の更新
		currentMotion = UpdateUnion();

		break;

	case STATE_CLEAR:

		// ゲームクリア状態時の更新
		UpdateClear();

		break;

	case STATE_OVER:
		break;

	default:
		assert(false);
		break;
	}

	for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の更新
		m_apOrbit[nCntPlayer]->Update();
	}

	// 影の更新
	m_pShadow->Update();

	// クリア表示の更新
	m_pClear->Update();

	// モーション・オブジェクトキャラクターの更新
	UpdateMotion(currentMotion);
}

//============================================================
//	描画処理
//============================================================
void CPlayer::Draw(void)
{
	// オブジェクトキャラクターの描画
	CObjectChara::Draw();

	for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡の描画
		m_apOrbit[nCntPlayer]->Draw();
	}
}

//============================================================
//	ヒット処理
//============================================================
void CPlayer::Hit(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();	// プレイヤー向き

	if (IsDeath() != true)
	{ // 死亡フラグが立っていない場合

		if (m_state == STATE_NORMAL)
		{ // 通常状態の場合

			// カウンターを初期化
			m_nCounterState = 0;

			// 吹っ飛びベクトルを設定
			m_move.x = sinf(rotPlayer.y) * basic::BLOW_SIDE;
			m_move.y = basic::BLOW_UP;
			m_move.z = cosf(rotPlayer.y) * basic::BLOW_SIDE;

			// 状態を変更
			SetState(STATE_DAMAGE);	// ダメージ状態

			// 待機モーションを設定
			SetMotion(MOTION_IDOL);

			// 爆発パーティクルを生成
			CParticle3D::Create(CParticle3D::TYPE_SMALL_EXPLOSION, D3DXVECTOR3(posPlayer.x, posPlayer.y + basic::HEIGHT * 0.5f, posPlayer.z));

			// サウンドの再生
			CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_HIT);	// ヒット音
		}
	}
}

//============================================================
//	状態の設定処理
//============================================================
void CPlayer::SetState(const int nState)
{
	if (m_state != CPlayer::STATE_CLEAR
	&&  m_state != CPlayer::STATE_OVER)
	{ // ゲーム終了に関する状態ではない場合

		// 引数の状態を設定
		m_state = (EState)nState;

		if (m_state == CPlayer::STATE_CLEAR)
		{ // ゲームクリア状態になった場合

			// ポインタを宣言
			CTimerManager *pTimer = CSceneGame::GetTimerManager();	// タイマーマネージャー
			assert(pTimer != NULL);

			// タイムの計測を終了
			pTimer->End();

			// ランキングに設定
			CRankingManager::Set(pTimer->GetLimit() - pTimer->Get());

			// 成功でリザルト遷移
			ResultTransition(CRetentionManager::RESULT_CLEAR);
		}
		else if (m_state == CPlayer::STATE_OVER)
		{ // ゲームオーバー状態になった場合

			// 失敗でリザルト遷移
			ResultTransition(CRetentionManager::RESULT_FAILED);
		}
		else if (m_state == CPlayer::STATE_UNION)
		{ // 合流状態になった場合

			for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
			{ // 軌跡の生成数分繰り返す

				// 軌跡を消失状態に設定
				m_apOrbit[nCntPlayer]->SetState(CObjectOrbit::STATE_VANISH);
			}
		}
	}
}

//============================================================
//	状態取得処理
//============================================================
int CPlayer::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	半径取得処理
//============================================================
float CPlayer::GetRadius(void) const
{
	// 半径を返す
	return basic::RADIUS;
}

//============================================================
//	縦幅取得処理
//============================================================
float CPlayer::GetHeight(void) const
{
	// 縦幅を返す
	return basic::HEIGHT;
}

//============================================================
//	更新状況の設定処理
//============================================================
void CPlayer::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);		// 自身
	m_pShadow->SetEnableUpdate(bUpdate);	// 影
}

//============================================================
//	描画状況の設定処理
//============================================================
void CPlayer::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pShadow->SetEnableDraw(bDraw);	// 影
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CPlayer::GetMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX  mtxRot, mtxTrans, mtxWorld;		// 計算用マトリックス
	D3DXVECTOR3 posPlayer = GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();	// プレイヤー向き

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotPlayer.y, rotPlayer.x, rotPlayer.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, posPlayer.x, posPlayer.y, posPlayer.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスを返す
	return mtxWorld;
}

//============================================================
//	生成処理
//============================================================
CPlayer *CPlayer::Create(CScene::EMode mode)
{
	// ポインタを宣言
	CPlayer *pPlayer = NULL;	// プレイヤー生成用

	if (pPlayer == NULL)
	{ // 使用されていない場合

		switch (mode)
		{ // モードごとの処理
		case CScene::MODE_TITLE:

			// 無し

			break;

		case CScene::MODE_TUTORIAL:

			// メモリ確保
			pPlayer = new CPlayer;	// プレイヤー

			break;

		case CScene::MODE_GAME:

			// メモリ確保
			pPlayer = new CPlayer;	// プレイヤー

			break;

		case CScene::MODE_RESULT:

			// 無し

			break;

		case CScene::MODE_RANKING:

			// 無し

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}
	else { assert(false); return NULL; }	// 使用中

	if (pPlayer != NULL)
	{ // 使用されている場合

		// プレイヤーの初期化
		if (FAILED(pPlayer->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pPlayer;
			pPlayer = NULL;

			// 失敗を返す
			return NULL;
		}
	}

	// 確保したアドレスを返す
	return pPlayer;
}

//============================================================
//	出現の設定処理
//============================================================
void CPlayer::SetSpawn(void)
{
	if (CSavePoint::GetNumAll() <= 0)
	{ // セーブポイントが一つもない場合

		// 処理を抜ける
		return;
	}

	// 変数を宣言
	CSavePoint::SInfo SavePointInfo = CSavePoint::GetSavePointInfo();	// セーブポイントの情報

	// 情報を初期化
	SetState(STATE_SPAWN);		// スポーン状態の設定
	SetMotion(MOTION_IDOL);		// 待機モーションを設定

	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター
	m_nCounterSlide = 0;	// スライディング管理カウンター
	m_nCounterWallDash = 0;	// 壁走り管理カウンター
	m_nCounterLand = 0;		// 着地管理カウンター
	m_nCounterJump = 0;		// ジャンプ管理カウンター

	// 真偽を初期化
	m_bJumpControl = false;		// ジャンプ操作を初期化
	m_bSlideControl = false;	// スライディング操作を初期化
	m_bWallDashControl = false;	// 壁走り操作を初期化

	// 位置を設定
	SetVec3Position(SavePointInfo.pos);

	// 向きを設定
	SetVec3Rotation(SavePointInfo.rot);
	m_destRot = SavePointInfo.rot;

	// 移動量を初期化
	m_move = VEC3_ZERO;

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を透明に再設定
	SetAlpha(0.0f);

	for (int nCntPlayer = 0; nCntPlayer < NUM_ORBIT; nCntPlayer++)
	{ // 軌跡の生成数分繰り返す

		// 軌跡を通常状態に再設定
		m_apOrbit[nCntPlayer]->SetState(CObjectOrbit::STATE_NORMAL);
	}

	// プレイヤー自身の描画を再開
	CObject::SetEnableDraw(true);

	// 回復パーティクルを生成
	CParticle3D::Create(CParticle3D::TYPE_HEAL, SavePointInfo.pos, particle::COL_HEAL);

	// 追従カメラの目標位置の設定
	CManager::GetInstance()->GetCamera()->SetDestFollow();

	// サウンドの再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_SPAWN);	// 生成音
}

//============================================================
//	ゴールの設定処理
//============================================================
void CPlayer::SetGoal(CGoalPoint *pGoal)
{
	// 引数のゴールを設定
	m_pGoal = pGoal;
}

//============================================================
//	スポーン状態時の更新処理
//============================================================
CPlayer::EMotion CPlayer::UpdateSpawn(void)
{
	// 変数を宣言
	EMotion currentMotion = MOTION_IDOL;	// 現在のモーション

	// フェードアウト状態時の更新
	if (UpdateFadeOut(basic::SPAWN_ADD_ALPHA))
	{ // 不透明になり切った場合

		// 状態を設定
		SetState(STATE_NORMAL);
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	通常状態時の更新処理
//============================================================
CPlayer::EMotion CPlayer::UpdateNormal(void)
{
	// 変数を宣言
	EMotion currentMotion = MOTION_IDOL;		// 現在のモーション
	D3DXVECTOR3 posPlayer = GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();	// プレイヤー向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報
	if (pStage == NULL)
	{ // ステージが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_IDOL;
	}

	// 移動操作
	currentMotion = UpdateMove(posPlayer);

	// 重力の更新
	UpdateGravity();

	// 着地判定
	UpdateLanding(posPlayer);

	// ジャンプ操作
	UpdateJump();

	// スライディング操作
	UpdateSliding();

	// 向き更新
	UpdateRotation(rotPlayer);

	// 向き変更の当たり判定
	CollisionRotationBuilding(posPlayer);

	// ステージ範囲外の補正
	pStage->LimitPosition(posPlayer, basic::RADIUS);

	// 障害物の当たり判定
	CollisionObstacle(posPlayer);

	// 位置を反映
	SetVec3Position(posPlayer);

	// 向きを反映
	SetVec3Rotation(rotPlayer);

	// エフェクトの更新
	UpdateEffect(posPlayer);

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[プレイヤー移動速度]：%f\n", m_fMove);
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[プレイヤー移動量]：%f %f %f\n", m_move.x, m_move.y, m_move.z);
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bJump) ? "[ジャンプ]：ON\n" : "[ジャンプ]：OFF\n");
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bSlide) ? "[スライディング]：ON\n" : "[スライディング]：OFF\n");
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bWallDash) ? "[壁走り]：ON\n" : "[壁走り]：OFF\n");

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	ダメージ状態時の更新処理
//============================================================
CPlayer::EMotion CPlayer::UpdateDamage(void)
{
	// 変数を宣言
	EMotion currentMotion = MOTION_IDOL;		// 現在のモーション
	D3DXVECTOR3 posPlayer = GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();	// プレイヤー向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報
	if (pStage == NULL)
	{ // ステージが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_IDOL;
	}

	// 向きを加算
	rotPlayer += basic::DMG_ADDROT;

	// 重力の更新
	UpdateGravity();

	// 着地判定
	UpdateLanding(posPlayer);

	// ステージ範囲外の補正
	pStage->LimitPosition(posPlayer, basic::RADIUS);

	// 位置を反映
	SetVec3Position(posPlayer);

	// 向きを反映
	SetVec3Rotation(rotPlayer);

	// フェードイン状態時の更新
	if (UpdateFadeIn(basic::DMG_SUB_ALPHA))
	{ // 透明になり切った場合

		// 出現の設定
		SetSpawn();
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	合流状態時の更新処理
//============================================================
CPlayer::EMotion CPlayer::UpdateUnion(void)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報
	if (pStage == NULL)
	{ // ステージが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_IDOL;
	}

	if (m_pGoal == NULL)
	{ // ゴールが設定されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_IDOL;
	}

	// 変数を宣言
	D3DXVECTOR3 sizePlayer = VEC3_ZERO;							// プレイヤー大きさ
	D3DXVECTOR3 posPlayer = GetVec3Position();					// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();					// プレイヤー向き
	D3DXVECTOR3 sizeGoal = m_pGoal->GetVec3Sizing();			// ゴール大きさ
	D3DXVECTOR3 posGoal = m_pGoal->GetVec3Position();			// ゴール位置
	D3DXVECTOR3 posFriend = m_pGoal->GetVec3FriendPosition();	// 友達位置
	EMotion currentMotion = MOTION_WAVEHAND;					// 現在のモーション

	if (m_bJump)
	{ // ジャンプ中の場合

		// 移動操作
		currentMotion = UpdateMove(posPlayer);
	}
	else
	{ // ジャンプ中ではない場合

		// 目標向きを設定
		m_destRot.y = atan2f(posPlayer.x - posFriend.x, posPlayer.z - posFriend.z);

		// カメラをズーム状態に設定
		CManager::GetInstance()->GetCamera()->SetState(CCamera::STATE_ZOOM);

		if (fabsf(rotPlayer.y - m_destRot.y) <= basic::MIN_VARY)
		{ // 向きがある程度一致した場合

			// ゲームクリア状態にする
			SetState(STATE_CLEAR);
		}
	}

	// 重力の更新
	UpdateGravity();

	// 着地判定
	UpdateLanding(posPlayer);

	// 向き更新
	UpdateRotation(rotPlayer);

	// ゴール範囲内の補正
	collision::InBoxPillar(posPlayer, posGoal, sizePlayer, sizePlayer, sizeGoal, sizeGoal);

	// ステージ範囲外の補正
	pStage->LimitPosition(posPlayer, basic::RADIUS);

	// 位置を反映
	SetVec3Position(posPlayer);

	// 向きを反映
	SetVec3Rotation(rotPlayer);

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[プレイヤー移動速度]：%f\n", m_fMove);
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "[プレイヤー移動量]：%f %f %f\n", m_move.x, m_move.y, m_move.z);
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bJump) ? "[ジャンプ]：ON\n" : "[ジャンプ]：OFF\n");
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bSlide) ? "[スライディング]：ON\n" : "[スライディング]：OFF\n");
	CManager::GetInstance()->GetDebugProc()->Print(CDebugProc::POINT_LEFT, (m_bWallDash) ? "[壁走り]：ON\n" : "[壁走り]：OFF\n");

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	ゲームクリア状態時の更新処理
//============================================================
void CPlayer::UpdateClear(void)
{
	// 変数を宣言
	D3DXVECTOR3 posClear = m_pClear->GetVec3Position();	// クリア表示位置

	if (posClear.x < clear::STOP_POS)
	{ // 位置が停止位置を未満の場合

		// 横位置を右に移動
		posClear.x += clear::MOVE_POS;

		if (posClear.x > clear::STOP_POS)
		{ // 位置が停止位置を超えた場合

			// 停止位置に補正
			posClear.x = clear::STOP_POS;

			// サウンドの再生
			CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_DECISION_001);	// 決定音01
		}
	}

	// クリア表示位置を反映
	m_pClear->SetVec3Position(posClear);
}

//============================================================
//	過去位置の更新処理
//============================================================
void CPlayer::UpdateOldPosition(void)
{
	// 過去位置を更新
	m_oldPos = GetVec3Position();
}

//============================================================
//	移動量・壁走りの更新処理
//============================================================
CPlayer::EMotion CPlayer::UpdateMove(D3DXVECTOR3& rPos)
{
	// 壁走りの更新
	UpdateWallDash(rPos);

	// 移動量を更新
	m_move.x += sinf(m_destRot.y + D3DX_PI) * (m_fMove + m_fPlusMove);
	m_move.z += cosf(m_destRot.y + D3DX_PI) * (m_fMove + m_fPlusMove);

	if (!m_bSlide && !m_bWallDash)
	{ // スライディング中ではない場合

		if (m_fMove < basic::MOVE)
		{ // 移動量が最高速度ではない場合

			// 移動量を加算
			m_fMove += basic::ADD_MOVE;

			if (m_fMove > basic::MOVE)
			{ // 移動量が最高速度を超えた場合

				// 移動量を補正
				m_fMove = basic::MOVE;
			}
		}
	}

	// プラス移動量を減算
	m_fPlusMove += (0.0f - m_fPlusMove) * walldash::REV_PLUSMOVE;

	// 移動モーションを返す
	return MOTION_MOVE;
}

//============================================================
//	壁走りの更新処理
//============================================================
void CPlayer::UpdateWallDash(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	float fRate = 0.0f;	// 割合
	bool bHit = false;	// 壁走り可能状況

	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetInstance()->GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetInstance()->GetPad();		// パッド
	CInputMouse		*pMouse		= CManager::GetInstance()->GetMouse();		// マウス

	// 看板との当たり判定
	bHit = CollisionSignboard(rPos, &fRate);	// 壁走り可能状況を設定

	if (!m_bWallDash)
	{ // 壁走りしていない場合

		if (bHit)
		{ // 壁走り可能な場合

			if (pKeyboard->IsTrigger(DIK_S)
			||  pPad->IsTrigger(CInputPad::KEY_A)
			||  pMouse->IsTrigger(CInputMouse::KEY_RIGHT))
			{ // 壁走りの操作が行われた場合

				// 壁走り操作が行われた情報を保存
				m_bWallDashControl = true;
			}
		}

		if (m_fMove >= walldash::CONTROL_MIN)
		{ // 壁走りが可能な移動速度の場合

			if (bHit && m_bWallDashControl)
			{ // 壁走り可能且つ、壁走りの操作が行われた場合

				// 壁走り操作が行われた情報を初期化
				m_bWallDashControl = false;

				// 壁走りしている状態にする
				m_bWallDash = true;

				// 壁走りモーションを設定
				SetMotion(MOTION_WALLDASH);

				// サウンドの再生
				CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_SLIDE);	// スライディング音
			}
		}
	}
	else
	{ // 壁走りしている場合

		// 変数を宣言
		float fPlusSubMove = walldash::PLUS_SUB_MOVE * m_nCounterWallDash;	// 移動量の追加減算量

		// カウンターを加算
		m_nCounterWallDash++;

		if (m_fMove > walldash::MIN_MOVE)
		{ // 移動量が最低速度ではない場合

			// 移動量を減算
			m_fMove -= walldash::SUB_MOVE + fPlusSubMove;

			if (m_fMove < walldash::MIN_MOVE)
			{ // 移動量が最低速度を超えた場合

				// 移動量を補正
				m_fMove = walldash::MIN_MOVE;
			}
		}

		if (bHit)
		{ // 壁走り可能な場合

			if (m_nCounterWallDash > walldash::MIN_END_CNT)
			{ // 壁走りの解除操作ができるカウントに到達した場合

				if (!(pKeyboard->IsPress(DIK_S) || pPad->IsPress(CInputPad::KEY_A) || pMouse->IsPress(CInputMouse::KEY_RIGHT)))
				{ // 壁走り解除の操作が行われた場合

					// カウンターを初期化
					m_nCounterWallDash = 0;

					// 壁走りしていない状態にする
					m_bWallDash = false;

					// 壁走り操作が行われた情報を初期化
					m_bWallDashControl = false;

					if (fRate >= walldash::BOOST_RATE)
					{ // 加速できるプレイヤーの位置割合以上だった場合

						// 縦移動量を加算
						m_move.y += walldash::PLUSMOVE_UP;

						// プラス移動量を設定
						m_fPlusMove = walldash::PLUSMOVE_SIDE;

						// ジャンプモーションを設定
						SetMotion(MOTION_JUMP);

						// サウンドの再生
						CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_JUMP);	// ジャンプ音
					}
				}
			}
		}
		else
		{ // 壁走り不可能の場合

			// カウンターを初期化
			m_nCounterWallDash = 0;

			// 壁走りしていない状態にする
			m_bWallDash = false;

			// 壁走り操作が行われた情報を初期化
			m_bWallDashControl = false;
		}

		// バイブの設定
		pPad->SetVibration(CInputPad::TYPE_WALLDASH);
	}
}

//============================================================
//	ジャンプの更新処理
//============================================================
void CPlayer::UpdateJump(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetInstance()->GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetInstance()->GetPad();		// パッド
	CInputMouse		*pMouse		= CManager::GetInstance()->GetMouse();		// マウス

	if (m_bJumpControl)
	{ // ジャンプ操作が行われている場合

		// ジャンプ管理カウンターを加算
		m_nCounterJump++;

		if (m_nCounterJump >= basic::JUMP_SAVECONTROL_CNT)
		{ // 操作情報を削除するフレーム数が経過した場合

			// ジャンプ管理カウンターを初期化
			m_nCounterJump = 0;

			// ジャンプ操作が行われた情報を初期化
			m_bJumpControl = false;
		}
	}

	if (pKeyboard->IsTrigger(DIK_W)
	||  pPad->IsTrigger(CInputPad::KEY_B)
	||  pMouse->IsTrigger(CInputMouse::KEY_LEFT))
	{ // ジャンプの操作が行われた場合

		// ジャンプ操作が行われた情報を保存
		m_bJumpControl = true;
	}

	if (!m_bJump && !m_bWallDash)
	{ // ジャンプと壁走りをしていない場合

		if (m_bJumpControl)
		{ // ジャンプの操作が行われた場合

			// ジャンプ管理カウンターを初期化
			m_nCounterJump = 0;

			// ジャンプ操作が行われた情報を初期化
			m_bJumpControl = false;

			// カウンターを初期化
			m_nCounterSlide = 0;

			// スライディングしていない状態にする
			m_bSlide = false;

			// 上移動量
			m_move.y += basic::JUMP;

			// ジャンプしている状態にする
			m_bJump = true;

			// ジャンプモーションを設定
			SetMotion(MOTION_JUMP);

			// サウンドの再生
			CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_JUMP);	// ジャンプ音
		}
	}
}

//============================================================
//	スライディングの更新処理
//============================================================
void CPlayer::UpdateSliding(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetInstance()->GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetInstance()->GetPad();		// パッド
	CInputMouse		*pMouse		= CManager::GetInstance()->GetMouse();		// マウス

	if (!m_bSlide)
	{ // スライディングしていない場合

		if (pKeyboard->IsTrigger(DIK_S)
		||  pPad->IsTrigger(CInputPad::KEY_A)
		||  pMouse->IsTrigger(CInputMouse::KEY_RIGHT))
		{ // スライディングの操作が行われた場合

			if (!m_bWallDash)
			{ // 壁走りをしていない場合

				// スライディング操作が行われた情報を保存
				m_bSlideControl = true;
			}
		}

		if (!m_bJump && !m_bWallDash)
		{ // ジャンプ・壁走りをしていない場合

			if (m_fMove >= slide::CONTROL_MIN)
			{ // スライディングが可能な移動速度の場合

				if (m_bSlideControl)
				{ // スライディングの操作が行われた場合

					// スライディング操作が行われた情報を初期化
					m_bSlideControl = false;

					// スライディングしている状態にする
					m_bSlide = true;

					// スライディングモーションを設定
					SetMotion(MOTION_SLIDE);

					// サウンドの再生
					CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_SLIDE);	// スライディング音
				}
			}
		}
	}
	else
	{ // スライディングしている場合

		// 変数を宣言
		float fPlusSubMove = (slide::PLUS_SUB_MOVE / (float)slide::MAX_END_CNT) * (float)(slide::MAX_END_CNT - m_nCounterSlide);	// 移動量の追加減算量

		// カウンターを加算
		m_nCounterSlide++;

		if (m_fMove > slide::MIN_MOVE)
		{ // 移動量が最低速度ではない場合

			// 移動量を減算
			m_fMove -= slide::SUB_MOVE + fPlusSubMove;

			if (m_fMove < slide::MIN_MOVE)
			{ // 移動量が最低速度を超えた場合

				// 移動量を補正
				m_fMove = slide::MIN_MOVE;
			}
		}

		if (m_nCounterSlide > slide::MIN_END_CNT)
		{ // スライディングの解除操作ができるカウントに到達した場合

			if (!(pKeyboard->IsPress(DIK_S) || pPad->IsPress(CInputPad::KEY_A) || pMouse->IsPress(CInputMouse::KEY_RIGHT)))
			{ // スライディング解除の操作が行われた場合

				// カウンターを初期化
				m_nCounterSlide = 0;

				// スライディングしていない状態にする
				m_bSlide = false;
			}
		}

		if (m_nCounterSlide > slide::MAX_END_CNT)
		{ // スライディングが強制終了するカウントに到達した場合

			// カウンターを初期化
			m_nCounterSlide = 0;

			// スライディングしていない状態にする
			m_bSlide = false;
		}
	}
}

//============================================================
//	重力の更新処理
//============================================================
void CPlayer::UpdateGravity(void)
{
	if (m_bWallDash)
	{ // 壁走り中の場合

		if (m_move.y > 0.0f)
		{ // 上昇量がある場合

			// 重力を加算
			m_move.y -= walldash::GRAVITY_HIGH;

			if (m_move.y < 0.0f)
			{ // 移動量が下がり切った場合

				// 移動量を初期化
				m_move.y = 0.0f;
			}
		}
		else
		{ // 上昇量がない場合

			// 重力を加算
			m_move.y -= walldash::GRAVITY_LOW;

			if (m_move.y < walldash::MAX_GRAVITY)
			{ // 移動量が下がりすぎた場合

				// 移動量を初期化
				m_move.y = walldash::MAX_GRAVITY;
			}
		}
	}
	else
	{ // 壁走り中ではない場合

		// 重力を加算
		m_move.y -= basic::GRAVITY;
	}
}

//============================================================
//	着地状況の更新処理
//============================================================
bool CPlayer::UpdateLanding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// ジャンプしている状態にする
	m_bJump = true;

	// 位置の更新
	// ビルの当たり判定・着地判定
	if (CollisionBuilding(rPos))
	{ // ビルに着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;

		// 着地物をビルに設定
		m_land = LAND_BUILDING;
	}

	// 障害物の当たり判定・着地判定
	if (LandObstacle(rPos))
	{ // 障害物に着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;

		// 着地物を障害物に設定
		m_land = LAND_OBSTACLE;
	}

	// 地面・制限位置の着地判定
	if (CScene::GetStage()->LandFieldPosition(rPos, m_move)
	||  CScene::GetStage()->LandLimitPosition(rPos, m_move, 0.0f))
	{ // プレイヤーが着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;

		// 着地物をその他に設定
		m_land = LAND_OTHER;
	}

	if (!bLand)
	{ // 着地していなかった場合

		if (m_state == STATE_NORMAL
		&&  GetMotionType() == MOTION_MOVE)
		{ // 通常状態且つ、移動モーションだった場合

			// 落下モーションを設定
			SetMotion(MOTION_FALL);
		}

		// 着地カウンターを加算
		m_nCounterLand++;

		// 着地物をしていないに設定
		m_land = LAND_NONE;
	}
	else
	{ // 着地した場合

		if (m_nCounterLand >= basic::LAND_SE_CNT)
		{ // 着地カウンターが一定値以上の場合

			switch (m_land)
			{ // 着地物ごとの処理
			case LAND_OBSTACLE:

				// サウンドの再生
				CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_WALK_OBS);	// 歩行音（障害物）

				break;

			default:

				// サウンドの再生
				CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_WALK_BUILD);	// 歩行音（ビル）

				break;
			}
		}

		// 着地カウンターを初期化
		m_nCounterLand = 0;
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	向きの更新処理
//============================================================
void CPlayer::UpdateRotation(D3DXVECTOR3& rRot)
{
	// 変数を宣言
	float fDiffRot = 0.0f;	// 差分向き

	// 目標向きの正規化
	useful::NormalizeRot(m_destRot.y);

	// 目標向きまでの差分を計算
	fDiffRot = m_destRot.y - rRot.y;

	// 差分向きの正規化
	useful::NormalizeRot(fDiffRot);

	// 向きの更新
	rRot.y += fDiffRot * basic::REV_ROTA;

	// 向きの正規化
	useful::NormalizeRot(rRot.y);
}

//============================================================
//	エフェクトの更新処理
//============================================================
void CPlayer::UpdateEffect(D3DXVECTOR3& rPos)
{
	if (m_bSlide || m_bWallDash)
	{ // スライディングまたは壁走りの場合

		// 煙を生成
		CEffect3D::Create(rPos, effect::SMOKE_RADIUS, CEffect3D::TYPE_SMOKE, effect::SMOKE_LIFE, D3DXVECTOR3((float)(rand() % 2), (float)(rand() % 4), (float)(rand() % 2)), VEC3_ZERO, XCOL_WHITE, -(float)(rand() % 12), false);
	}
}

//============================================================
//	モーション・オブジェクトキャラクターの更新処理
//============================================================
void CPlayer::UpdateMotion(int nMotion)
{
	// 変数を宣言
	int nAnimMotion = GetMotionType();	// 現在再生中のモーション

	if (nMotion != NONE_IDX)
	{ // モーションが設定されている場合

		if (IsMotionLoop(nAnimMotion))
		{ // ループするモーションだった場合

			if (nAnimMotion != nMotion)
			{ // 現在のモーションが再生中のモーションと一致しない場合

				// 現在のモーションの設定
				SetMotion(nMotion);
			}
		}
	}

	// オブジェクトキャラクターの更新
	CObjectChara::Update();

	switch (GetMotionType())
	{ // モーションの種類ごとの処理
	case MOTION_MOVE:		// 移動モーション

		if (GetMotionPose() % 4 == 0 && GetMotionCounter() == 0)
		{ // 足がついたタイミングの場合

			switch (m_land)
			{ // 着地物ごとの処理
			case LAND_OBSTACLE:

				// サウンドの再生
				CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_WALK_OBS);	// 歩行音（障害物）

				break;

			default:

				// サウンドの再生
				CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_WALK_BUILD);	// 歩行音（ビル）

				break;
			}
		}

		break;

	case MOTION_JUMP:		// ジャンプモーション

		if (!m_bJump)
		{ // ジャンプ中ではない場合

			// 現在のモーションの設定
			SetMotion(nMotion);
		}

		// 処理を抜ける
		break;

	case MOTION_SLIDE:		// スライディングモーション

		if (!m_bSlide)
		{ // スライディング中ではない場合

			// 現在のモーションの設定
			SetMotion(nMotion);
		}

		// 処理を抜ける
		break;

	case MOTION_WALLDASH:	// 壁走りモーション

		if (!m_bWallDash)
		{ // 壁走り中ではない場合

			if (m_bJump)
			{ // ジャンプ中の場合

				// 現在のモーションの設定
				SetMotion(MOTION_FALL);
			}
			else
			{ // ジャンプ中ではない場合

				// 現在のモーションの設定
				SetMotion(nMotion);
			}
		}

		// 処理を抜ける
		break;

	case MOTION_FALL:		// 落下モーション

		if (!m_bJump)
		{ // ジャンプ中ではない場合

			// 現在のモーションの設定
			SetMotion(nMotion);
		}

		// 処理を抜ける
		break;
	}
}

//============================================================
//	フェードアウト状態時の更新処理
//============================================================
bool CPlayer::UpdateFadeOut(const float fAdd)
{
	// 変数を宣言
	bool bAlpha = false;		// 透明状況
	float fAlpha = GetAlpha();	// 透明度

	// プレイヤー自身の描画を再開
	CObject::SetEnableDraw(true);

	// 透明度を上げる
	fAlpha += fAdd;

	if (fAlpha >= GetMaxAlpha())
	{ // 透明度が上がり切った場合

		// 透明度を補正
		fAlpha = GetMaxAlpha();

		// 不透明になり切った状態にする
		bAlpha = true;
	}

	// 透明度を設定
	SetAlpha(fAlpha);

	// 透明状況を返す
	return bAlpha;
}

//============================================================
//	フェードイン状態時の更新処理
//============================================================
bool CPlayer::UpdateFadeIn(const float fSub)
{
	// 変数を宣言
	bool bAlpha = false;		// 透明状況
	float fAlpha = GetAlpha();	// 透明度

	// 透明度を下げる
	fAlpha -= fSub;

	if (fAlpha <= 0.0f)
	{ // 透明度が下がり切った場合

		// 透明度を補正
		fAlpha = 0.0f;

		// 透明になり切った状態にする
		bAlpha = true;

		// プレイヤー自身の描画を停止
		CObject::SetEnableDraw(false);
	}

	// 透明度を設定
	SetAlpha(fAlpha);

	// 透明状況を返す
	return bAlpha;
}

//============================================================
//	リザルト遷移処理
//============================================================
void CPlayer::ResultTransition(const CRetentionManager::EResult result)
{
	if (CManager::GetInstance()->GetFade()->GetState() == CFade::FADE_NONE)
	{ // フェードしていない場合

		// ポインタを宣言
		CRetentionManager *m_pRetention = CManager::GetInstance()->GetRetentionManager();	// データ保存マネージャー
		assert(m_pRetention != NULL);

		CTimerManager *pTimer = CSceneGame::GetTimerManager();	// タイマーマネージャー
		assert(pTimer != NULL);

		// クリア状況を設定
		m_pRetention->SetResult(result);

		// 経過時間を設定
		m_pRetention->SetTime(pTimer->GetLimit() - pTimer->Get());

		// シーンの設定
		CManager::GetInstance()->SetScene(CScene::MODE_RESULT, CNT_WAIT[result]);	// リザルト画面
	}
}

//============================================================
//	四方向変換処理
//============================================================
CPlayer::EDirection CPlayer::FourDirection
(
	const float fRot,		// 向き
	const float fTolerance	// 許容値
)
{
	if (fabsf(fRot) <= fTolerance)
	{ // 許容値以下の誤差の場合

		// 方向：0°を返す
		return DIRECTION_DOWN;
	}
	else if (fabsf(fRot - HALF_PI_TWO_DP) <= fTolerance)
	{ // 許容値以下の誤差の場合

		// 方向：90°を返す
		return DIRECTION_RIGHT;
	}
	else if (fabsf(fRot - PI_TWO_DP) <= fTolerance)
	{ // 許容値以下の誤差の場合

		// 方向：180°を返す
		return DIRECTION_UP;
	}
	else if (fabsf(fRot - -HALF_PI_TWO_DP) <= fTolerance)
	{ // 許容値以下の誤差の場合

		// 方向：270°を返す
		return DIRECTION_LEFT;
	}

	// 方向なしを返す
	return DIRECTION_NONE;
}

//============================================================
//	向き変更の当たり判定
//============================================================
void CPlayer::CollisionRotationBuilding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinPlayer = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// プレイヤー最小大きさ
	D3DXVECTOR3 sizeMaxPlayer = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// プレイヤー最大大きさ
	float fDisPosY = 0.0f;	// プレイヤーとビルのY距離
	bool bInitDis = false;	// Y距離の初期化状況

	// ポインタを宣言
	CObject *pCurrentRotObj = NULL;	// 現在の向き設定オブジェクト

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 posBuild = VEC3_ZERO;		// ビル位置
				D3DXVECTOR3 rotBuild = VEC3_ZERO;		// ビル向き
				D3DXVECTOR3 sizeMinBuild = VEC3_ZERO;	// ビル最小大きさ
				D3DXVECTOR3 sizeMaxBuild = VEC3_ZERO;	// ビル最大大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
				{ // オブジェクトラベルがビルではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() != CBuilding::COLLISION_GROUND)
				{ // 状態が地面ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ビルの位置を設定
				posBuild = pObjCheck->GetVec3Position();

				// ビルの向きを設定
				rotBuild = pObjCheck->GetVec3Rotation();

				// ビルの最小の大きさを設定
				sizeMinBuild = pObjCheck->GetVec3Sizing();
				sizeMinBuild.y = 0.0f;	// 縦の大きさを初期化

				// ビルの最大の大きさを設定
				sizeMaxBuild = pObjCheck->GetVec3Sizing();
				sizeMaxBuild.y *= 2.0f;	// 縦の大きさを倍にする

				// 向き変更の当たり判定
				if (collision::Box2D
				( // 引数
					rPos,			// 判定位置
					posBuild,		// 判定目標位置
					sizeMaxPlayer,	// 判定サイズ(右・上・後)
					sizeMinPlayer,	// 判定サイズ(左・下・前)
					sizeMaxBuild,	// 判定目標サイズ(右・上・後)
					sizeMinBuild	// 判定目標サイズ(左・下・前)
				))
				{ // 判定内だった場合

					// 変数を宣言
					float fDis = rPos.y - (posBuild.y + sizeMaxBuild.y);	// プレイヤーとビルのY距離

					if (fDis >= 0.0f)
					{ // プレイヤーとビルのY距離がプラスの場合

						if (!bInitDis)
						{ // 初期化していない場合

							// 現在の距離を代入
							fDisPosY = fDis;

							// 現在の向き設定オブジェクトを代入
							pCurrentRotObj = pObjCheck;

							// 初期化済みにする
							bInitDis = true;
						}
						else
						{ // 初期化している場合

							if (fDis < fDisPosY)
							{ // より近いビルの場合

								// 現在の距離を代入
								fDisPosY = fDis;

								// 現在の向き設定オブジェクトを代入
								pCurrentRotObj = pObjCheck;
							}
						}
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	if (pCurrentRotObj != NULL)
	{ // ビルがあった場合

		// 変数を宣言
		D3DXVECTOR3 posBuild = VEC3_ZERO;	// ビル位置
		D3DXVECTOR3 rotBuild = VEC3_ZERO;	// ビル向き
		D3DXVECTOR3 sizeBuild = VEC3_ZERO;	// ビル大きさ
		D3DXVECTOR3 posLeft = VEC3_ZERO;	// プレイヤーから見たビルの左位置
		D3DXVECTOR3 posRight = VEC3_ZERO;	// プレイヤーから見たビルの右位置
		
		// ビルの位置を設定
		posBuild = pCurrentRotObj->GetVec3Position();

		// ビルの向きを設定
		rotBuild = pCurrentRotObj->GetVec3Rotation();

		// ビルの大きさを設定
		sizeBuild = pCurrentRotObj->GetVec3Sizing();
		sizeBuild.y *= 2.0f;	// 縦の大きさを倍にする

		float fRotLeft = m_destRot.y - HALF_PI;		// プレイヤーの90度左向き
		useful::NormalizeRot(fRotLeft);				// 左向き正規化
		float fRotRight = m_destRot.y + HALF_PI;	// プレイヤーの90度右向き
		useful::NormalizeRot(fRotRight);			// 右向き正規化

		if (FourDirection(m_destRot.y, QRTR_PI * 0.5f) == FourDirection(rotBuild.y, QRTR_PI * 0.5f))
		{ // 向きが違う場合

			// 関数を抜ける
			return;
		}

		// ビルの左位置を計算
		posLeft.x = posBuild.x - sinf(fRotLeft) * sizeBuild.x;
		posLeft.y = posBuild.y + sizeBuild.y;
		posLeft.z = posBuild.z - cosf(fRotLeft) * sizeBuild.z;

		// ビルの右位置を計算
		posRight.x = posBuild.x - sinf(fRotRight) * sizeBuild.x;
		posRight.y = posBuild.y + sizeBuild.y;
		posRight.z = posBuild.z - cosf(fRotRight) * sizeBuild.z;

		if (collision::LineOuterProduct(posLeft, posRight, rPos) <= 0.0f)
		{ // プレイヤーの進行方向から見てビルの中心より奥側の場合

			// プレイヤーの位置にビルの中心位置を設定
			rPos.x = posBuild.x;
			rPos.z = posBuild.z;

			// プレイヤーの移動量を初期化
			m_move.x = 0.0f;
			m_move.z = 0.0f;

			// プレイヤーの目標向きにビルの向きを設定
			m_destRot.y = rotBuild.y;
		}
	}
}

//============================================================
//	ビルとの一軸ごとの当たり判定
//============================================================
bool CPlayer::ResponseSingleBuilding(const EAxis axis, D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinPlayer = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// プレイヤー最小大きさ
	D3DXVECTOR3 sizeMaxPlayer = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// プレイヤー最大大きさ
	bool bMin = false;	// 不の方向の判定状況
	bool bMax = false;	// 正の方向の判定状況
	bool bHit = false;	// 各軸の判定情報

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 posBuild = VEC3_ZERO;		// ビル位置
				D3DXVECTOR3 rotBuild = VEC3_ZERO;		// ビル向き
				D3DXVECTOR3 sizeMinBuild = VEC3_ZERO;	// ビル最小大きさ
				D3DXVECTOR3 sizeMaxBuild = VEC3_ZERO;	// ビル最大大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
				{ // オブジェクトラベルがビルではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ビルの位置を設定
				posBuild = pObjCheck->GetVec3Position();

				// ビルの向きを設定
				rotBuild = pObjCheck->GetVec3Rotation();

				// ビルの最小の大きさを設定
				sizeMinBuild = pObjCheck->GetVec3Sizing();
				sizeMinBuild.y = 0.0f;	// 縦の大きさを初期化

				// ビルの最大の大きさを設定
				sizeMaxBuild = pObjCheck->GetVec3Sizing();
				sizeMaxBuild.y *= 2.0f;	// 縦の大きさを倍にする

				switch (axis)
				{ // 判定軸ごとの処理
				case AXIS_X:	// X軸

					// X軸の衝突判定
					collision::ResponseSingleX
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxPlayer,	// 判定サイズ(右・上・後)
						sizeMinPlayer,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 左からの判定
						&bMax			// 右からの判定
					);

					if (bMin || bMax)
					{ // 横から当たっていた場合

						// ぶつかり状況にする
						bHit = true;
					}

					break;

				case AXIS_Y:	// Y軸

					// Y軸の衝突判定
					collision::ResponseSingleY
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxPlayer,	// 判定サイズ(右・上・後)
						sizeMinPlayer,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 下からの判定
						&bMax			// 上からの判定
					);

					if (bMax)
					{ // 上から当たっていた場合

						// 着地している状況にする
						bHit = true;
					}

					break;

				case AXIS_Z:	// Z軸

					// Z軸の衝突判定
					collision::ResponseSingleZ
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxPlayer,	// 判定サイズ(右・上・後)
						sizeMinPlayer,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 前からの判定
						&bMax			// 後からの判定
					);

					if (bMin || bMax)
					{ // 横から当たっていた場合

						// ぶつかり状況にする
						bHit = true;
					}

					break;

				default:	// 例外処理
					assert(false);
					break;
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 各軸の判定情報を返す
	return bHit;
}

//============================================================
//	看板との当たり判定
//============================================================
bool CPlayer::CollisionSignboard(D3DXVECTOR3& rPos, float *pRate)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = D3DXVECTOR3(rPos.x, rPos.y + basic::HEIGHT * 0.5f, rPos.z);	// プレイヤー判定位置

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_SIGNBOARD)
				{ // オブジェクトラベルが看板ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 変数を宣言
				D3DXMATRIX mtxSign;		// 看板マトリックス
				D3DXVECTOR3 posSign;	// 看板位置
				D3DXVECTOR3 sizeSign;	// 看板大きさ
				bool bHit = false;		// 判定情報

				// 変数配列を宣言
				D3DXVECTOR3 aGap[4];	// ずれ量
				D3DXVECTOR3 aPos[4];	// 頂点位置

				// 看板の情報を取得
				mtxSign = *pObjCheck->GetPtrMtxWorld();	// マトリックス
				posSign = pObjCheck->GetVec3Position();	// 位置
				sizeSign = pObjCheck->GetVec3Sizing();	// 大きさ

				// ずれ量を設定
				aGap[0] = D3DXVECTOR3(-sizeSign.x - walldash::PLUS_COLL_SIZE, 0.0f, 0.0f);
				aGap[1] = D3DXVECTOR3(-sizeSign.x - walldash::PLUS_COLL_SIZE, 0.0f, -walldash::COLL_SIZE);
				aGap[2] = D3DXVECTOR3( sizeSign.x + walldash::PLUS_COLL_SIZE, 0.0f, -walldash::COLL_SIZE);
				aGap[3] = D3DXVECTOR3( sizeSign.x + walldash::PLUS_COLL_SIZE, 0.0f, 0.0f);

				for (int nCntSign = 0; nCntSign < 4; nCntSign++)
				{ // 判定用の頂点数分繰り返す

					// 変数を宣言
					D3DXMATRIX mtxGap;		// 頂点のずれ量マトリックス
					D3DXMATRIX mtxTrans;	// 計算用マトリックス

					// ワールドマトリックスの初期化
					D3DXMatrixIdentity(&mtxGap);

					// 位置を反映
					D3DXMatrixTranslation(&mtxTrans, aGap[nCntSign].x, aGap[nCntSign].y, aGap[nCntSign].z);
					D3DXMatrixMultiply(&mtxGap, &mtxGap, &mtxTrans);

					// マトリックスを掛け合わせる
					D3DXMatrixMultiply(&mtxGap, &mtxGap, &mtxSign);

					// 位置を設定
					aPos[nCntSign] = D3DXVECTOR3(mtxGap._41, mtxGap._42, mtxGap._43);
				}

				if (posPlayer.y < posSign.y + sizeSign.y
				&&  posPlayer.y > posSign.y - sizeSign.y)
				{ // プレイヤーの縦座標が看板内の場合

					// 壁走り範囲の判定
					bHit = collision::BoxOuterPillar
					( // 引数
						aPos[0],	// 四角の各頂点
						aPos[1],	// 四角の各頂点
						aPos[2],	// 四角の各頂点
						aPos[3],	// 四角の各頂点
						posPlayer	// 判定位置
					);
					if (bHit)
					{ // 範囲内の場合

						// 変数を宣言
						float fWidth	= sqrtf((aPos[1].x - aPos[2].x) * (aPos[1].x - aPos[2].x) + (aPos[1].z - aPos[2].z) * (aPos[1].z - aPos[2].z)) * 0.5f;			// 看板の横幅
						float fPlayer	= sqrtf((aPos[1].x - posPlayer.x) * (aPos[1].x - posPlayer.x) + (aPos[1].z - posPlayer.z) * (aPos[1].z - posPlayer.z)) * 0.5f;	// 左端から見たプレイヤー位置

						if (pRate != NULL)
						{ // 割合の格納ポイントが存在する場合

							// プレイヤー位置の割合を代入
							*pRate = (1.0f / fWidth) * fPlayer;
						}

						// 壁走り範囲内を返す
						return true;
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 壁走り範囲外を返す
	return false;
}

//============================================================
//	障害物との当たり判定
//============================================================
void CPlayer::CollisionObstacle(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinPlayer = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// プレイヤー最小大きさ
	D3DXVECTOR3 sizeMaxPlayer = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// プレイヤー最大大きさ

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 pos0, pos1, pos2, pos3;	// 頂点座標代入用
				D3DXVECTOR3 posObs  = VEC3_ZERO;	// 障害物位置
				D3DXVECTOR3 rotObs  = VEC3_ZERO;	// 障害物向き
				D3DXVECTOR3 sizeObs = VEC3_ZERO;	// 障害物大きさ
				float fAngle  = 0.0f;	// 対角線の角度
				float fLength = 0.0f;	// 対角線の長さ
				bool  bHit = false;		// 判定情報

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
				{ // オブジェクトラベルが障害物ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() == CObstacle::STATE_LAND)
				{ // 着地できる特殊状態の場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 障害物の情報を取得
				posObs  = pObjCheck->GetVec3Position();	// 位置
				rotObs  = pObjCheck->GetVec3Rotation();	// 向き
				sizeObs = pObjCheck->GetVec3Sizing();	// 大きさ
				fAngle  = pObjCheck->GetAngle();		// 対角線角度
				fLength = pObjCheck->GetLength();		// 対角線長さ

				// 頂点座標を計算
				pos0.x = posObs.x + sinf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos0.y = 0.0f;
				pos0.z = posObs.z + cosf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos1.x = posObs.x + sinf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos1.y = 0.0f;
				pos1.z = posObs.z + cosf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos3.x = posObs.x + sinf(rotObs.y - fAngle) * fLength;
				pos3.y = 0.0f;
				pos3.z = posObs.z + cosf(rotObs.y - fAngle) * fLength;
				pos2.x = posObs.x + sinf(rotObs.y + fAngle) * fLength;
				pos2.y = 0.0f;
				pos2.z = posObs.z + cosf(rotObs.y + fAngle) * fLength;

				// 障害物との判定
				if (rPos.y + basic::HEIGHT > posObs.y
				&&  rPos.y < posObs.y + sizeObs.y)
				{ // Y座標が範囲内の場合

					// 二軸の矩形の衝突判定
					bHit = collision::BoxOuterPillar
					( // 引数
						pos0,	// 四角の各頂点
						pos1,	// 四角の各頂点
						pos2,	// 四角の各頂点
						pos3,	// 四角の各頂点
						rPos	// 判定位置
					);
				}

				if (bHit)
				{ // 当たっていた場合

					switch (pObjCheck->GetState())
					{ // 特殊状態ごとの処理
					case CObstacle::STATE_NONE:	// 特殊状態無し

						switch (pObjCheck->GetDodge())
						{ // 回避法ごとの処理
						case CObstacle::DODGE_NONE:		// 回避方法なし

							// 無し

							break;

						case CObstacle::DODGE_JUMP:		// ジャンプ回避

							if (!m_bJump)
							{ // ジャンプ中ではない場合

								// ヒット
								Hit();
							}

							break;

						case CObstacle::DODGE_SLIDE:	// スライディング回避

							if (!m_bSlide)
							{ // スライディング中ではない場合

								// ヒット
								Hit();
							}

							break;

						default:	// 例外処理
							assert(false);
							break;
						}

						break;

					case CObstacle::STATE_JUMP:	// 触れるとジャンプする特殊状態

						if (!m_bJump)
						{ // ジャンプ中ではない場合

							// 移動量を追加
							m_move.y += basic::JUMPPAD_MOVE;

							// ジャンプモーションを設定
							SetMotion(MOTION_JUMP);

							// ジャンプしている状態にする
							m_bJump = true;

							// サウンドの再生
							CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_JUMPPAD);	// ジャンプパッド音
						}

						break;

					
					default:	// 例外処理
						assert(false);
						break;
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	障害物との着地判定
//============================================================
bool CPlayer::LandObstacle(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinPlayer = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// プレイヤー最小大きさ
	D3DXVECTOR3 sizeMaxPlayer = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// プレイヤー最大大きさ
	bool bLand = false;	// 着地状況

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 pos0, pos1, pos2, pos3;	// 頂点座標代入用
				D3DXVECTOR3 posObs  = VEC3_ZERO;	// 障害物位置
				D3DXVECTOR3 rotObs  = VEC3_ZERO;	// 障害物向き
				D3DXVECTOR3 sizeObs = VEC3_ZERO;	// 障害物大きさ
				float fAngle  = 0.0f;	// 対角線の角度
				float fLength = 0.0f;	// 対角線の長さ
				bool  bHit = false;		// 判定情報

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
				{ // オブジェクトラベルが障害物ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() != CObstacle::STATE_LAND)
				{ // 着地できる特殊状態ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 障害物の情報を取得
				posObs  = pObjCheck->GetVec3Position();	// 位置
				rotObs  = pObjCheck->GetVec3Rotation();	// 向き
				sizeObs = pObjCheck->GetVec3Sizing();	// 大きさ
				fAngle  = pObjCheck->GetAngle();		// 対角線角度
				fLength = pObjCheck->GetLength();		// 対角線長さ

				// 頂点座標を計算
				pos0.x = posObs.x + sinf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos0.y = 0.0f;
				pos0.z = posObs.z + cosf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos1.x = posObs.x + sinf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos1.y = 0.0f;
				pos1.z = posObs.z + cosf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos3.x = posObs.x + sinf(rotObs.y - fAngle) * fLength;
				pos3.y = 0.0f;
				pos3.z = posObs.z + cosf(rotObs.y - fAngle) * fLength;
				pos2.x = posObs.x + sinf(rotObs.y + fAngle) * fLength;
				pos2.y = 0.0f;
				pos2.z = posObs.z + cosf(rotObs.y + fAngle) * fLength;

				// 障害物との判定
				if (rPos.y + basic::HEIGHT > posObs.y
				&&  rPos.y < posObs.y + sizeObs.y)
				{ // Y座標が範囲内の場合

					// 二軸の矩形の衝突判定
					bHit = collision::BoxOuterPillar
					( // 引数
						pos0,	// 四角の各頂点
						pos1,	// 四角の各頂点
						pos2,	// 四角の各頂点
						pos3,	// 四角の各頂点
						rPos	// 判定位置
					);
				}

				if (bHit)
				{ // 当たっていた場合

					if (m_oldPos.y >= posObs.y + sizeObs.y)
					{ // 前回の位置が着地点以上の場合

						// プレイヤー位置を障害物の上にする
						rPos.y = posObs.y + sizeObs.y;

						// 着地している状況にする
						bLand = true;

						// 縦移動量を初期化
						m_move.y = 0.0f;
					}
					else if (m_oldPos.y + basic::HEIGHT <= posObs.y)
					{ // 前回の位置が原点以下の場合

						// プレイヤー位置を障害物の上にする
						rPos.y = posObs.y - basic::HEIGHT;

						// 縦移動量を初期化
						m_move.y = 0.0f;
					}
					else
					{ // 前回の位置が着地点より小さい場合

						switch (pObjCheck->GetDodge())
						{ // 回避法ごとの処理
						case CObstacle::DODGE_NONE:		// 回避方法なし

							// 無し
			
							break;

						case CObstacle::DODGE_JUMP:		// ジャンプ回避

							// ヒット
							Hit();

							break;

						case CObstacle::DODGE_SLIDE:	// スライディング回避

							if (!m_bSlide)
							{ // スライディング中ではない場合

								// ヒット
								Hit();
							}

							break;

						default:	// 例外処理
							assert(false);
							break;
						}
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	ビルとの当たり判定
//============================================================
bool CPlayer::CollisionBuilding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// 移動量を加算
	rPos.x += m_move.x;

	// X軸の当たり判定
	if (ResponseSingleBuilding(AXIS_X, rPos))
	{ // 横に当たっていた場合

		// ヒット
		Hit();
	}

	// 移動量を加算
	rPos.y += m_move.y;

	// Y軸の当たり判定
	if (ResponseSingleBuilding(AXIS_Y, rPos))
	{ // 上に当たっていた場合

		// 着地している状況にする
		bLand = true;
	}

	// 移動量を加算
	rPos.z += m_move.z;

	// Z軸の当たり判定
	if (ResponseSingleBuilding(AXIS_Z, rPos))
	{ // 横に当たっていた場合

		// ヒット
		Hit();
	}

	if (m_state == STATE_DAMAGE)
	{ // ダメージ状態の場合

		// 移動量を減衰
		if (m_bJump)
		{ // 空中の場合
//			m_move.x *= (1.0f - basic::DMG_JUMP_REV);
			m_move.x += (0.0f - m_move.x) * basic::DMG_JUMP_REV;
//			m_move.z -= m_move.z * basic::DMG_JUMP_REV;
			m_move.z += (0.0f - m_move.z) * basic::DMG_JUMP_REV;
		}
		else
		{ // 地上の場合

			m_move.x += (0.0f - m_move.x) * basic::DMG_LAND_REV;
			m_move.z += (0.0f - m_move.z) * basic::DMG_LAND_REV;
		}
	}
	else
	{ // ダメージ状態ではない場合

		// 移動量を減衰
		if (m_bJump)
		{ // 空中の場合

			m_move.x += (0.0f - m_move.x) * basic::NOR_JUMP_REV;
			m_move.z += (0.0f - m_move.z) * basic::NOR_JUMP_REV;
		}
		else
		{ // 地上の場合

			m_move.x += (0.0f - m_move.x) * basic::NOR_LAND_REV;
			m_move.z += (0.0f - m_move.z) * basic::NOR_LAND_REV;
		}
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	セットアップ処理
//============================================================
void CPlayer::LoadSetup(void)
{
	// 変数を宣言
	CMotion::SMotionInfo info;		// ポーズの代入用
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nID			= 0;	// インデックスの代入用
	int nParentID	= 0;	// 親インデックスの代入用
	int nNowPose	= 0;	// 現在のポーズ番号
	int nNowKey		= 0;	// 現在のキー番号
	int nLoop		= 0;	// ループのON/OFFの変換用
	int nEnd		= 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// ポーズ代入用の変数を初期化
	memset(&info, 0, sizeof(info));

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// キャラクターの設定
			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{ // 読み込んだ文字列が CHARACTERSET の場合

				do
				{ // 読み込んだ文字列が END_CHARACTERSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "PARTSSET") == 0)
					{ // 読み込んだ文字列が PARTSSET の場合

						do
						{ // 読み込んだ文字列が END_PARTSSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "INDEX") == 0)
							{ // 読み込んだ文字列が INDEX の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nID);			// モデルのインデックスを読み込む
							}
							else if (strcmp(&aString[0], "PARENT") == 0)
							{ // 読み込んだ文字列が PARENT の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nParentID);	// モデルの親のインデックスを読み込む
							}
							else if (strcmp(&aString[0], "POS") == 0)
							{ // 読み込んだ文字列が POS の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%f", &pos.x);		// X座標を読み込む
								fscanf(pFile, "%f", &pos.y);		// Y座標を読み込む
								fscanf(pFile, "%f", &pos.z);		// Z座標を読み込む
							}
							else if (strcmp(&aString[0], "ROT") == 0)
							{ // 読み込んだ文字列が ROT の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%f", &rot.x);		// X向きを読み込む
								fscanf(pFile, "%f", &rot.y);		// Y向きを読み込む
								fscanf(pFile, "%f", &rot.z);		// Z向きを読み込む
							}
						} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// 読み込んだ文字列が END_PARTSSET ではない場合ループ

						// パーツ情報の設定
						CObjectChara::SetPartsInfo(nID, nParentID, pos, rot, mc_apModelFile[nID]);
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// 読み込んだ文字列が END_CHARACTERSET ではない場合ループ
			}

			// モーションの設定
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // 読み込んだ文字列が MOTIONSET の場合

				// 現在のポーズ番号を初期化
				nNowPose = 0;

				do
				{ // 読み込んだ文字列が END_MOTIONSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "LOOP") == 0)
					{ // 読み込んだ文字列が LOOP の場合

						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &nLoop);		// ループのON/OFFを読み込む

						// 読み込んだ値をbool型に変換
						info.bLoop = (nLoop == 0) ? false : true;
					}
					else if (strcmp(&aString[0], "NUM_KEY") == 0)
					{ // 読み込んだ文字列が NUM_KEY の場合

						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &info.nNumKey);	// キーの総数を読み込む
					}
					else if (strcmp(&aString[0], "KEYSET") == 0)
					{ // 読み込んだ文字列が KEYSET の場合

						// 現在のキー番号を初期化
						nNowKey = 0;

						do
						{ // 読み込んだ文字列が END_KEYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "FRAME") == 0)
							{ // 読み込んだ文字列が FRAME の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &info.aKeyInfo[nNowPose].nFrame);	// キーが切り替わるまでのフレーム数を読み込む
							}
							else if (strcmp(&aString[0], "KEY") == 0)
							{ // 読み込んだ文字列が KEY の場合

								do
								{ // 読み込んだ文字列が END_KEY ではない場合ループ

									// ファイルから文字列を読み込む
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "POS") == 0)
									{ // 読み込んだ文字列が POS の場合

										fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.x);	// X位置を読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.y);	// Y位置を読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.z);	// Z位置を読み込む

										// 読み込んだ位置にパーツの初期位置を加算
										info.aKeyInfo[nNowPose].aKey[nNowKey].pos += GetPartsPosition(nNowKey);
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // 読み込んだ文字列が ROT の場合

										fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);	// X向きを読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);	// Y向きを読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);	// Z向きを読み込む

										// 読み込んだ向きにパーツの初期向きを加算
										info.aKeyInfo[nNowPose].aKey[nNowKey].rot += GetPartsRotation(nNowKey);

										// 初期向きを正規化
										useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);
										useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);
										useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);
									}

								} while (strcmp(&aString[0], "END_KEY") != 0);	// 読み込んだ文字列が END_KEY ではない場合ループ

								// 現在のキー番号を加算
								nNowKey++;
							}
						} while (strcmp(&aString[0], "END_KEYSET") != 0);	// 読み込んだ文字列が END_KEYSET ではない場合ループ

						// 現在のポーズ番号を加算
						nNowPose++;
					}
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);	// 読み込んだ文字列が END_MOTIONSET ではない場合ループ

				// モーション情報の設定
				CObjectChara::SetMotionInfo(info);
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "プレイヤーセットアップファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
