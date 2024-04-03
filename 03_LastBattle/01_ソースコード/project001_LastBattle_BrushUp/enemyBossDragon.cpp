//============================================================
//
//	ボスドラゴン処理 [enemyBossDragon.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyBossDragon.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "stage.h"
#include "camera.h"
#include "multiModel.h"
#include "sceneGame.h"
#include "gameManager.h"
#include "rankingManager.h"
#include "gauge2D.h"
#include "shadow.h"
#include "magicCircle.h"
#include "retentionManager.h"
#include "random.h"
#include "hitStop.h"
#include "flash.h"
#include "timerUI.h"
#include "cinemaScope.h"
#include "player.h"
#include "collision.h"
#include "particle3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *MODEL_FILE[] =	// モデルファイル
	{
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\00_waist.x",		// 腰
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\01_body.x",		// 体
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\02_jaw.x",		// 顎
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\03_head.x",		// 頭
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\04_armUL.x",		// 左上腕
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\05_armUR.x",		// 右上腕
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\06_armDL.x",		// 左下腕
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\07_armDR.x",		// 右下腕
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\08_handL.x",		// 左手
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\09_handR.x",		// 右手
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\10_legUL.x",		// 左太もも
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\11_legUR.x",		// 右太もも
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\12_legDL.x",		// 左脛
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\13_legDR.x",		// 右脛
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\14_footL.x",		// 左足
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\15_footR.x",		// 右足
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\16_wingL_00.x",	// 左翼00
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\17_wingR_00.x",	// 右翼00
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\18_wingL_01.x",	// 左翼01
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\19_wingR_01.x",	// 右翼01
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\20_wingL_02.x",	// 左翼02
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\21_wingR_02.x",	// 右翼02
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\22_wingL_03.x",	// 左翼03
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\23_wingR_03.x",	// 右翼03
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\24_tail_00.x",	// 尻尾00
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\25_tail_01.x",	// 尻尾01
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\26_tail_02.x",	// 尻尾02
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\27_tail_03.x",	// 尻尾03
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\28_tail_04.x",	// 尻尾04
		"data\\MODEL\\ENEMY\\BOSS_DRAGON\\29_tail_05.x",	// 尻尾05
	};

	const D3DXVECTOR3 DEATH_CAMERA_ROT[] =	// 死亡時カメラ向き
	{
		VEC3_ZERO,	// 初回は向き設定なし
		D3DXVECTOR3(2.11f, -0.76f, 0.0f),
		D3DXVECTOR3(1.61f, 0.49f, 0.0f),
	};

	const CCamera::SSwing LAND_SWING	= CCamera::SSwing(10.0f, 1.5f, 0.12f);	// 着地のカメラ揺れ
	const CCamera::SSwing HOWL_SWING	= CCamera::SSwing(14.0f, 2.0f, 0.09f);	// 咆哮のカメラ揺れ
	const CCamera::SSwing RIDE_SWING	= CCamera::SSwing(9.5f, 2.0f, 0.09f);	// 飛び上がり前の咆哮のカメラ揺れ
	const CCamera::SSwing DEATH_SWING	= CCamera::SSwing(16.5f, 2.0f, 0.2f);	// 死亡時のカメラ揺れ
	const int	KNOCK_LIFE		= 600;			// ノックバックする体力
	const int	BLEND_FRAME		= 16;			// モーションのブレンドフレーム
	const int	LAND_MOTION_KEY	= 9;			// モーションの着地の瞬間キー
	const int	HOWL_MOTION_KEY	= 13;			// モーションの咆哮の開始キー
	const int	HOWL_WAIT_FRAME	= 40;			// 咆哮の余韻フレーム
	const int	ATK_WAIT_FRAME	= 60;			// 攻撃の余韻フレーム
	const int	STAN_ATK_WAIT	= -30;			// スタン後の攻撃の余韻計測開始フレーム
	const float	REV_ROTA		= 0.15f;		// 向き変更の補正係数
	const float	SCALE_MAGIC		= 35.0f;		// 魔法陣の半径変動量
	const float	MOVE_MAGIC		= 30.0f;		// 魔法陣の上下移動量
	const float	MAGIC_CIRCLE_RADIUS	= 400.0f;	// 魔法陣の半径
	const float	MAGIC_ALPHA_RADIUS	= 400.0f;	// 魔法陣の透明半径
	const float	MAGIC_DELPOS_PLUSY	= 250.0f;	// 魔法陣の消失位置の加算量Y
	const float	RIDE_FIND_RADIUS	= 550.0f;	// ライド時のボス検知半径

	const int	NUM_MULTI_FLASH		= 2;	// 死亡後連続するフラッシュの回数
	const int	HITSTOP_DEATH_START	= 90;	// 死亡初めのヒットストップの長さ
	const int	HITSTOP_DEATH_MULTI	= 55;	// 死亡後連続するヒットストップの長さ
	const float	FLASH_INITALPHA_DEATH_START	= 0.55f;	// 死亡初めのフラッシュ開始透明度
	const float	FLASH_SUBALPHA_DEATH_START	= 0.02f;	// 死亡後連続するフラッシュ透明度減算量
	const float	FLASH_INITALPHA_DEATH_MULTI	= 0.35f;	// 死亡初めのフラッシュ開始透明度
	const float	FLASH_SUBALPHA_DEATH_MULTI	= 0.02f;	// 死亡後連続するフラッシュ透明度減算量

	const float	DEATH_CANERA_DIS = 900.0f;	// 死亡カメラの距離
	const D3DXVECTOR3 DEATH_CAMERA_OFFSET	= D3DXVECTOR3(35.0f, 136.0f, 0.0f);		// 死亡カメラの位置オフセット
	const CCamera::SSwing DEATH_START_SWING	= CCamera::SSwing(10.0f, 1.5f, 0.12f);	// 死亡初めのカメラ揺れ
	const CCamera::SSwing DEATH_MULTI_SWING	= CCamera::SSwing(8.6f, 1.5f, 0.24f);	// 死亡後連続するカメラ揺れ
	const D3DXVECTOR3 EXP_OFFSET = D3DXVECTOR3(0.0f, 140.0f, 0.0f);	// 爆発位置オフセット

	const float	RIDE_ROTATE_DIS  = 2400.0f;	// 旋回時のステージ中央から離れる距離
	const float	RIDE_ROTATE_POSY = 1200.0f;	// 旋回時のY座標
	const float	RIDE_ROTATE_MOVE = 0.01f;	// 旋回時の回転速度

	const int	FLYUP_MOTION_KEY = 2;		// 飛び上がりモーションの咆哮の開始キー
	const int	ROTATE_FRAME     = 480;		// 合計旋回フレーム
	const float	STAN_CANERA_DIS  = 600.0f;	// スタンカメラの距離
	const D3DXVECTOR3 STAN_CAMERA_OFFSET	= D3DXVECTOR3(0.0f, 145.0f, 0.0f);	// スタンカメラの位置オフセット
	const D3DXVECTOR3 STAN_CAMERA_ROT		= D3DXVECTOR3(1.46f, 0.0f, 0.0f);	// スタンカメラの向き

	const int	HITSTOP_KNOCK			= 11;		// ノックバック時のヒットストップの長さ
	const float	FLASH_INITALPHA_KNOCK	= 0.65f;	// ノックバック時のフラッシュ開始透明度
	const float	FLASH_SUBALPHA_KNOCK	= 0.02f;	// ノックバック時のフラッシュ透明度減算量
	const float	KNOCK_CAM_DIS			= 600.0f;	// ノックバックカメラの距離
	const CCamera::SSwing KNOCK_SWING	= CCamera::SSwing(10.0f, 1.5f, 0.12f);		// ノックバック時のカメラ揺れ
	const D3DXVECTOR3 KNOCK_CAM_OFFPOS	= D3DXVECTOR3(0.0f, 100.0f, 0.0f);			// ノックバックカメラの位置オフセット
	const D3DXVECTOR3 KNOCK_CAM_OFFROT	= D3DXVECTOR3(1.75f, 0.45f, 0.0f);			// ノックバックカメラの向きオフセット
	const D3DXVECTOR3 KNOCK_PLAYER_POS_OFFSET = D3DXVECTOR3(0.0f, 0.0f, -600.0f);	// ノックバック時のプレイヤー位置オフセット

	// 体力の情報
	namespace lifeInfo
	{
		const char *TEX_FRAME = "data\\TEXTURE\\lifeframe001.png";	// 体力フレーム表示のテクスチャファイル

		const D3DXVECTOR3	POS			 = D3DXVECTOR3(832.5f, 685.0f, 0.0f);		// 位置
		const D3DXVECTOR3	SIZE_GAUGE	 = D3DXVECTOR3(448.0f, 15.7f, 0.0f);		// ゲージ大きさ
		const D3DXVECTOR3	SIZE_FRAME	 = D3DXVECTOR3(448.5f, 33.5f, 0.0f);		// フレーム大きさ
		const D3DXVECTOR3	OFFSET_FRAME = D3DXVECTOR3(-0.5f, -13.0f, 0.0f);		// フレームオフセット
		const D3DXCOLOR		COL_FRONT	 = D3DXCOLOR(0.77f, 0.19f, 0.94f, 1.0f);	// 表ゲージ色
		const D3DXCOLOR		COL_BACK	 = D3DXCOLOR(0.02f, 0.008f, 0.03f, 1.0f);	// 裏ゲージ色
		const int			CHANGE_FRAME = 10;	// 表示値変動フレーム
	}

	// 影の情報
	namespace shadowInfo
	{
		const D3DXVECTOR3 SIZE = D3DXVECTOR3(300.0f, 0.0f, 300.0f);	// 影の大きさ

		const float MIN_FADE_LEVEL = 0.01f;	// 最小透明度の加減量
		const float MAX_FADE_LEVEL = 0.05f;	// 最大透明度の加減量
	}
}

//************************************************************
//	マクロ定義
//************************************************************
// ランダム攻撃のON/OFF
#if 0
#define RANDOM_ATTACK_ON	// ランダム攻撃
#define ATTACK (CEnemyAttack::ATTACK_02)
#endif

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(MODEL_FILE) == CEnemyBossDragon::MODEL_MAX,	"ERROR : Model Count Mismatch");
static_assert(NUM_ARRAY(DEATH_CAMERA_ROT) == NUM_MULTI_FLASH + 1,	"ERROR : Array Count Mismatch");

//************************************************************
//	子クラス [CEnemyBossDragon] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyBossDragon::CEnemyBossDragon(const EType type) : CEnemy(type),
	m_pLife				(nullptr),			// 体力の情報
	m_pAttack			(nullptr),			// 攻撃の情報
	m_pMagicCircle		(nullptr),			// 魔法陣の情報
	m_action			(ACT_NONE),			// 行動
	m_nCounterSameAct	(0),				// 同じ行動の連続数
	m_nCounterAttack	(0),				// 攻撃管理カウンター
	m_nCounterRotate	(0),				// ライド旋回カウンター
	m_nCounterFlash		(0),				// フラッシュ管理カウンター
	m_oldAtk	(CEnemyAttack::ATTACK_00),	// 前回の攻撃
	m_curAtk	(CEnemyAttack::ATTACK_00)	// 今回の攻撃
{
	// メンバ変数をクリア
	memset(&m_teleport, 0, sizeof(m_teleport));	// テレポートの情報
	memset(&m_ride, 0, sizeof(m_ride));			// ライドの情報
}

//============================================================
//	デストラクタ
//============================================================
CEnemyBossDragon::~CEnemyBossDragon()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyBossDragon::Init(void)
{
	// メンバ変数を初期化
	memset(&m_teleport, 0, sizeof(m_teleport));		// テレポートの情報
	memset(&m_ride, 0, sizeof(m_ride));				// ライドの情報
	m_oldAtk			= CEnemyAttack::ATTACK_00;	// 前回の攻撃
	m_curAtk			= CEnemyAttack::ATTACK_00;	// 今回の攻撃
	m_pLife				= nullptr;		// 体力の情報
	m_pAttack			= nullptr;		// 攻撃の情報
	m_pMagicCircle		= nullptr;		// 魔法陣の情報
	m_action			= ACT_NONE;		// 行動
	m_nCounterSameAct	= 0;			// 同じ行動の連続数
	m_nCounterAttack	= 0;			// 攻撃管理カウンター
	m_nCounterRotate	= 0;			// ライド旋回カウンター
	m_nCounterFlash		= 0;			// フラッシュ管理カウンター

	// 敵の初期化
	if (FAILED(CEnemy::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 体力の生成
	m_pLife = CGauge2D::Create
	( // 引数
		GetStatusInfo().nMaxLife,	// 最大表示値
		lifeInfo::CHANGE_FRAME,		// 表示値変動フレーム
		lifeInfo::POS,				// 位置
		lifeInfo::SIZE_GAUGE,		// ゲージ大きさ
		lifeInfo::COL_FRONT,		// 表ゲージ色
		lifeInfo::COL_BACK,			// 裏ゲージ色
		true,						// 枠描画状況
		lifeInfo::TEX_FRAME,		// フレームテクスチャパス
		lifeInfo::SIZE_FRAME,		// 枠大きさ
		lifeInfo::OFFSET_FRAME		// 枠オフセット
	);

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, shadowInfo::SIZE, this);
	if (m_pShadow == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 魔法陣の生成
	m_pMagicCircle = CMagicCircle::Create(VEC3_ZERO, VEC3_ZERO, 0.0f, MAGIC_ALPHA_RADIUS);
	if (m_pMagicCircle == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 自動描画をOFFにする
	m_pMagicCircle->SetEnableDraw(false);

	// なにもしない状態にする
	SetState(STATE_NONE);

	// 待機モーションを設定
	SetMotion(MOTION_IDOL);

	// 自動描画をOFFにする
	SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemyBossDragon::Uninit(void)
{
	// 影の終了
	m_pShadow->DeleteObjectParent();	// 親オブジェクトを削除
	SAFE_UNINIT(m_pShadow);

	// 攻撃の破棄
	SAFE_REF_RELEASE(m_pAttack);

	// 敵の終了
	CEnemy::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemyBossDragon::Update(void)
{
	// 敵の更新
	CEnemy::Update();

	// 影の更新
	m_pShadow->Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemyBossDragon::Draw(CShader *pShader)
{
	CDrawBossShader	*pDrawBossShader = CDrawBossShader::GetInstance();	// ボス描画シェーダー情報
	if (pDrawBossShader->IsEffectOK())
	{ // エフェクトが使用可能な場合

		// 敵の描画
		CEnemy::Draw(pDrawBossShader);
	}
	else
	{ // エフェクトが使用不可能な場合

		// エフェクトエラー
		assert(false);

		// 敵の描画
		CEnemy::Draw(pShader);
	}
}

//============================================================
//	ヒット処理
//============================================================
bool CEnemyBossDragon::Hit(const int nDamage)
{
	if (IsDeath())						{ return false; }	// 死亡済み
	if (m_action == ACT_MAGIC_FADEIN)	{ return false; }	// 魔法陣フェードイン中
	if (m_action == ACT_MAGIC_FADEOUT)	{ return false; }	// 魔法陣フェードアウト中
	if (m_pLife->GetNum() <= 0)			{ return false; }	// 体力なし

	int nState = GetState();
	if (nState != STATE_NORMAL
	&&  nState != STATE_RIDE_ROTATE)
	{ // 通常・ライド旋回状態以外の場合抜ける

		return false;
	}

	int nOldLife = m_pLife->GetNum();	// ダメージ前の体力
	int nCurLife = 0;					// ダメージ後の体力

	// 体力にダメージを与える
	m_pLife->AddNum(-nDamage);

	// ダメージ後の体力を取得
	nCurLife = m_pLife->GetNum();

	if (nCurLife > 0)
	{ // 体力が残っている場合

		// ダメージを受ける前の状態を保存
		SetPrevState((EState)GetState());

		if (nOldLife >= KNOCK_LIFE
		&&  nCurLife <  KNOCK_LIFE)
		{ // 今回で体力が一定値を下回った場合

			// ノックバック状態にする
			SetState(STATE_KNOCK);
		}
		else
		{ // 通常体力減少の場合

			// ダメージ状態にする
			SetState(STATE_DAMAGE);
		}
	}
	else
	{ // 体力が残っていない場合

		// 死亡状態にする
		SetState(STATE_DEATH);
	}

	return true;
}

//============================================================
//	更新状況の設定処理
//============================================================
void CEnemyBossDragon::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);		// 自身
	m_pShadow->SetEnableUpdate(bUpdate);	// 影
}

//============================================================
//	描画状況の設定処理
//============================================================
void CEnemyBossDragon::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pShadow->SetEnableDraw(bDraw);	// 影
}

//============================================================
//	腰モデルのインデックス取得処理
//============================================================
int CEnemyBossDragon::GetWaistModelID(void) const
{
	// 腰モデルのインデックスを返す
	return MODEL_WAIST;
}

//============================================================
//	頭モデルのインデックス取得処理
//============================================================
int CEnemyBossDragon::GetHeadModelID(void) const
{
	// 頭モデルのインデックスを返す
	return MODEL_HEAD;
}

//============================================================
//	ライド可能かの取得処理
//============================================================
bool CEnemyBossDragon::IsRideOK(const D3DXVECTOR3& rPos) const
{
	// スタン状態ではない場合ライド不可
	if (GetState() != STATE_STAN) { return false; }

	// XZ平面の円の当たり判定
	bool bRideOK = collision::Circle2D
	( // 引数
		GetVec3Position(),	// 判定位置
		rPos,				// 判定目標位置
		RIDE_FIND_RADIUS,	// 判定半径
		0.0f				// 判定目標半径
	);

	// 円判定内ならライド可能
	return bRideOK;
}

//============================================================
//	UI描画の設定処理
//============================================================
void CEnemyBossDragon::SetEnableDrawUI(const bool bDraw)
{
	// UI描画の設定
	CEnemy::SetEnableDrawUI(bDraw);

	// UIオブジェクトに描画状況を反映
	m_pLife->SetEnableDraw(bDraw);	// 体力
}

//============================================================
//	体力ゲージの優先順位の設定処理
//============================================================
void CEnemyBossDragon::SetLifePriority(const int nPrio)
{
	// 体力ゲージの優先順位を設定
	assert(m_pLife != nullptr);
	m_pLife->SetPriority(nPrio);
}

//============================================================
//	通常状態の初期化処理
//============================================================
void CEnemyBossDragon::InitNormal(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置

	// 通常状態の初期化
	CEnemy::InitNormal();

	// 通常状態にする
	SetState(STATE_NORMAL);

	// 自動描画をONにする
	SetEnableDraw(true);

	// 透明度を不透明に再設定
	SetAlpha(1.0f);

	// マテリアルを再設定
	ResetMaterial();

	// 待機モーションを設定
	SetMotion(MOTION_IDOL);

	// 着地判定
	UpdateLanding(&posEnemy);

	// 位置範囲外の補正
	LimitPosition(&posEnemy);

	// 位置を反映
	SetVec3Position(posEnemy);
}

//============================================================
//	切り抜き用の描画処理
//============================================================
void CEnemyBossDragon::DrawCrop(void)
{
	// 自動描画がOFFの場合抜ける
	if (!IsDraw()) { return; }

	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイス情報
	CMonoShader	*pMonoShader  = CMonoShader::GetInstance();	// 単色描画シェーダー情報
	if (pMonoShader->IsEffectOK())
	{ // エフェクトが使用可能な場合

		// ピクセル描画色を画面クリア色に設定
		pMonoShader->SetColor(GET_RENDERER->GetClearColor());

		// ステンシルテストを有効にする
		pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

		// 比較参照値を設定する
		pDevice->SetRenderState(D3DRS_STENCILREF, 1);

		// ステンシルマスクを指定する 
		pDevice->SetRenderState(D3DRS_STENCILMASK, 255);

		// ステンシル比較関数を指定する
		pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);

		// ステンシル結果に対しての反映設定
		pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);	// Zテスト・ステンシルテスト成功
		pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);		// Zテスト・ステンシルテスト失敗
		pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);		// Zテスト失敗・ステンシルテスト成功

		// 敵の描画
		CEnemy::Draw(pMonoShader);

		// ステンシルテストを無効にする
		pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	else { assert(false); }
}

//============================================================
//	テレポートの設定処理
//============================================================
void CEnemyBossDragon::SetTeleport
(
	const D3DXVECTOR3 &rPos,	// テレポート目標位置
	const D3DXVECTOR3 &rRot,	// テレポート目標向き
	const EMotion motion,		// テレポート後モーション
	const bool bLook			// テレポート先にカメラを向かせるか
)
{
	// テレポート後の位置・向きを保存
	m_teleport.pos = rPos;
	m_teleport.rot = rRot;

	// 魔法陣の位置・向きを現在位置の足元に設定
	m_pMagicCircle->SetVec3Position(GetVec3Position());
	m_pMagicCircle->SetVec3Rotation(GetVec3Rotation());

	if (motion == MOTION_NONE
	||  motion == MOTION_IDOL
	||  motion == MOTION_FLY_IDOL)
	{ // 設定できるモーションの場合

		// テレポート後のモーションを設定
		m_teleport.motion = motion;
	}
	else
	{ // 設定できないモーションの場合

		// モーション指定なしを設定
		assert(false);
		m_teleport.motion = MOTION_NONE;
	}

	// テレポート後のカメラ補正を設定
	m_teleport.bLook = bLook;

	// テレポートの初期化状態にする
	m_teleport.state = TELEPORT_INIT;

	// 魔法陣によるフェードインの行動をとらせる
	m_action = ACT_MAGIC_FADEIN;
}

//============================================================
//	地面殴りの行動設定処理
//============================================================
void CEnemyBossDragon::SetActPunchGround(void)
{
	// 地面殴りモーションを設定
	SetMotion(MOTION_PUNCH_GROUND, BLEND_FRAME);

	// 地面を殴る行動をとらせる
	m_action = ACT_PUNCH_GROUND;
}

//============================================================
//	炎攻撃の行動設定処理
//============================================================
void CEnemyBossDragon::SetActFireAttack(void)
{
	// 炎攻撃モーションを設定
	SetMotion(MOTION_FIRE_ATTACK, BLEND_FRAME);

	// 炎を吐く行動をとらせる
	m_action = ACT_FIRE_ATTACK;
}

//============================================================
//	ひっかき攻撃の行動設定処理
//============================================================
void CEnemyBossDragon::SetActClawAttack(void)
{
	// ひっかき攻撃モーションを設定
	SetMotion(MOTION_CLAW_ATTACK, BLEND_FRAME);

	// ひっかき攻撃の行動をとらせる
	m_action = ACT_CLAW_ATTACK;
}

//============================================================
//	しっぽ攻撃の行動設定処理
//============================================================
void CEnemyBossDragon::SetActTailAttack(void)
{
	// しっぽ攻撃モーションを設定
	SetMotion(MOTION_TAIL_ATTACK, BLEND_FRAME);

	// しっぽ攻撃の行動をとらせる
	m_action = ACT_TAIL_ATTACK;
}

//============================================================
//	空中攻撃の行動設定処理
//============================================================
void CEnemyBossDragon::SetActFlyAttack(void)
{
	// 空中攻撃モーションを設定
	SetMotion(MOTION_FLY_ATTACK, BLEND_FRAME);

	// 空中で攻撃する行動をとらせる
	m_action = ACT_FLY_ATTACK;
}

//============================================================
//	行動取得処理
//============================================================
CEnemyBossDragon::EAction CEnemyBossDragon::GetAction(void)
{
	// 現在の行動を返す
	return m_action;
}

//============================================================
//	モデルファイル取得処理
//============================================================
const char *CEnemyBossDragon::GetModelFileName(const int nModel) const
{
	if (nModel > NONE_IDX && nModel < MODEL_MAX)
	{ // 使用できるインデックスの場合

		// 引数のインデックスのモデルを返す
		return MODEL_FILE[nModel];
	}
	else { assert(false); return NONE_STRING; }	// 範囲外
}

//============================================================
//	モーションの更新処理
//============================================================
void CEnemyBossDragon::UpdateMotion(void)
{
	if (IsDeath()) { return; }	// 死亡している

	switch (GetMotionType())
	{ // モーションごとの処理
	case MOTION_IDOL:			// 待機モーション
	case MOTION_KNOCK:			// ノックバックモーション
	case MOTION_STAN:			// スタンモーション
	case MOTION_SHAKE_OFF:		// 空中振り下ろしモーション
	case MOTION_DEATH:			// 死亡モーション
		break;

	case MOTION_HOWL:			// 咆哮モーション

		if (GetMotionKey() == 2 && GetMotionKeyCounter() == 0
		||  GetMotionKey() == 5 && GetMotionKeyCounter() == 0)
		{
			// 羽ばたき音の再生
			PLAY_SOUND(CSound::LABEL_SE_WING);
		}
		else if (GetMotionKey() == 9 && GetMotionKeyCounter() == 0)
		{
			// 着地音の再生
			PLAY_SOUND(CSound::LABEL_SE_LAND_B);
		}

		break;

	case MOTION_FLY_IDOL:		// 空中待機モーション

		if (GetMotionKey() == 2 && GetMotionKeyCounter() == 0)
		{
			// 羽ばたき音の再生
			PLAY_SOUND(CSound::LABEL_SE_WING);
		}

		break;

	case MOTION_HOWL_FLYUP:		// 咆哮飛び上がりモーション

		if (GetMotionKey() == 7 && GetMotionKeyCounter() == 0)
		{
			// 羽ばたき音の再生
			PLAY_SOUND(CSound::LABEL_SE_WING);
		}

		break;

	case MOTION_PUNCH_GROUND:	// 地面殴りモーション

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 待機モーションに移行
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}

		break;

	case MOTION_CLAW_ATTACK:	// ひっかき攻撃モーション

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 待機モーションに移行
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}

		break;

	case MOTION_FIRE_ATTACK:	// 炎攻撃モーション

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 待機モーションに移行
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}

		break;

	case MOTION_TAIL_ATTACK:	// しっぽ攻撃モーション

		if (GetMotionKey() == 5 && GetMotionKeyCounter() == 0)
		{
			// 羽ばたき音の再生
			PLAY_SOUND(CSound::LABEL_SE_WING);
		}

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 待機モーションに移行
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}

		break;

	case MOTION_FLY_ATTACK:		// 空中攻撃モーション

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 空中待機モーションに移行
			SetMotion(MOTION_FLY_IDOL, BLEND_FRAME);
		}

		break;

	case MOTION_FLY_RUSH:		// 空中突進攻撃モーション

		// TODO：使うようになったらなんに遷移するか指定
		assert(false);

		break;

	case MOTION_COME_DOWN:		// 上空復帰モーション

		if (GetMotionKey() == 2 && GetMotionKeyCounter() == 0
		||  GetMotionKey() == 5 && GetMotionKeyCounter() == 0)
		{
			// 羽ばたき音の再生
			PLAY_SOUND(CSound::LABEL_SE_WING);
		}
		else if (GetMotionKey() == 9 && GetMotionKeyCounter() == 0)
		{
			// 着地音の再生
			PLAY_SOUND(CSound::LABEL_SE_LAND_B);
		}

		if (IsMotionFinish())
		{ // モーションが終了していた場合

			// 待機モーションに移行
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	スポーン状態の設定処理
//============================================================
void CEnemyBossDragon::SetSpawn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();	// 敵向き

	// スポーン状態の設定
	CEnemy::SetSpawn();

	// 透明度を不透明に再設定
	SetAlpha(1.0f);

	// 自動描画をONにする
	SetEnableDraw(true);

	// 咆哮モーションを設定
	SetMotion(MOTION_HOWL);

	// 着地判定
	UpdateLanding(&posEnemy);

	// 位置範囲外の補正
	LimitPosition(&posEnemy);

	// 位置を反映
	SetVec3Position(posEnemy);

	// 向きを反映
	SetVec3Rotation(rotEnemy);
	SetDestRotation(rotEnemy);

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}

	// カメラをボス注目状態に設定
	GET_MANAGER->GetCamera()->SetState(CCamera::STATE_LOOK_BOSS);
	GET_MANAGER->GetCamera()->SetDestLookBoss();	// カメラ目標位置の初期化
}

//============================================================
//	 ノックバック状態の設定処理
//============================================================
void CEnemyBossDragon::SetKnock(void)
{
	// ノックバック状態の設定
	CEnemy::SetKnock();

	// ゲーム画面ではない場合抜ける
	if (GET_MANAGER->GetMode() != CScene::MODE_GAME) { return; }

	CCinemaScope *pScope = CSceneGame::GetCinemaScope();	// シネマスコープの情報
	CHitStop *pHitStop	= CSceneGame::GetHitStop();	// ヒットストップ情報
	CFlash   *pFlash	= CSceneGame::GetFlash();	// フラッシュ情報
	CCamera  *pCamera	= GET_MANAGER->GetCamera();	// カメラ情報

	// カウンターを初期化
	m_nCounterAttack = 0;	// 攻撃管理カウンター
	m_nCounterRotate = 0;	// ライド旋回カウンター
	m_nCounterFlash  = 0;	// フラッシュ管理カウンター

	// 攻撃クラスを破棄
	SAFE_REF_RELEASE(m_pAttack);

	// 次の攻撃を設定
	m_curAtk = CEnemyAttack::ATTACK_00;

	// ノックバックモーションを設定
	SetMotion(MOTION_KNOCK);

	// スコープインさせる
	pScope->SetScopeIn();

	// ヒットストップさせる
	pHitStop->SetStop(HITSTOP_KNOCK);

	// フラッシュを設定
	pFlash->Set
	( // 引数
		FLASH_INITALPHA_KNOCK,	// 開始透明度
		FLASH_SUBALPHA_KNOCK	// 透明度減算量
	);

	// カメラ揺れを設定
	pCamera->SetSwing(CCamera::TYPE_MAIN, KNOCK_SWING);

	// カメラを何もしない状態に設定 (固定カメラにする)
	pCamera->SetState(CCamera::STATE_NONE);

	// ノックバック演出の設定
	SetKnockStaging();

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}
}

//============================================================
//	スタン状態の設定処理
//============================================================
void CEnemyBossDragon::SetStan(void)
{
	// スタン状態の設定
	CEnemy::SetStan();

	// カウンターを初期化
	m_nCounterAttack = 0;	// 攻撃管理カウンター
	m_nCounterRotate = 0;	// ライド旋回カウンター
	m_nCounterFlash  = 0;	// フラッシュ管理カウンター

	// スタンモーションを設定
	SetMotion(MOTION_STAN, BLEND_FRAME);

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}
}

//============================================================
//	ライド飛び上がり状態の設定処理
//============================================================
void CEnemyBossDragon::SetRideFlyUp(void)
{
	CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報

	// ライド飛び上がり状態の設定
	CEnemy::SetRideFlyUp();

	// カウンターを初期化
	m_nCounterAttack = 0;	// 攻撃管理カウンター
	m_nCounterRotate = 0;	// ライド旋回カウンター
	m_nCounterFlash  = 0;	// フラッシュ管理カウンター

	// 咆哮飛び上がりモーションを設定
	SetMotion(MOTION_HOWL_FLYUP, BLEND_FRAME);

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}

	// カメラを何もしない状態に設定 (固定カメラにする)
	pCamera->SetState(CCamera::STATE_NONE);

	// カメラの注視点をボスの中心に設定
	pCamera->SetPositionR(GetVec3Position() + STAN_CAMERA_OFFSET);

	// カメラの向きを設定
	pCamera->SetRotation(GetVec3Rotation() + STAN_CAMERA_ROT);

	// カメラの距離を設定
	pCamera->SetDistance(STAN_CANERA_DIS);
}

//============================================================
//	ライド旋回状態の設定処理
//============================================================
void CEnemyBossDragon::SetRideRotate(void)
{
	// ライド旋回状態の設定
	CEnemy::SetRideRotate();

	// カウンターを初期化
	m_nCounterAttack = 0;	// 攻撃管理カウンター
	m_nCounterFlash  = 0;	// フラッシュ管理カウンター

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}
}

//============================================================
//	ライド終了状態の設定処理
//============================================================
void CEnemyBossDragon::SetRideEnd(void)
{
	CStage *pStage = CScene::GetStage();				// ステージ情報
	CPlayer *pPlayer = CScene::GetPlayer();				// プレイヤー情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 posEnemy = GetVec3Position();			// ボス位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();			// ボス向き

	// ライド終了状態の設定
	CEnemy::SetRideEnd();

	// カウンターを初期化
	m_nCounterAttack = 0;	// 攻撃管理カウンター
	m_nCounterRotate = 0;	// ライド旋回カウンター
	m_nCounterFlash  = 0;	// フラッシュ管理カウンター

	// 上空復帰モーションを設定
	SetMotion(MOTION_COME_DOWN);

	// ボスの位置を中央に設定
	posEnemy = pStage->GetStageLimit().center;	// ステージ中央を設定

	// 着地判定・ステージ範囲外の補正
	UpdateLanding(&posEnemy);
	pStage->LimitPosition(posEnemy, GetStatusInfo().fRadius);

	// 位置を反映
	SetVec3Position(posEnemy);

	// 向きをプレイヤー方向にする
	rotEnemy.y = atan2f(posEnemy.x - posPlayer.x, posEnemy.z - posPlayer.z);
	SetVec3Rotation(rotEnemy);
	SetDestRotation(rotEnemy);

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}
}

//============================================================
//	死亡状態の設定処理
//============================================================
void CEnemyBossDragon::SetDeath(void)
{
	// 死亡状態の設定
	CEnemy::SetDeath();

	// ゲーム画面ではない場合抜ける
	if (GET_MANAGER->GetMode() != CScene::MODE_GAME) { return; }

	CGameManager *pGameManager = CSceneGame::GetGameManager();	// ゲームマネージャーの情報
	CCinemaScope *pScope	= CSceneGame::GetCinemaScope();		// シネマスコープの情報
	CTimerUI *pTimer	= CSceneGame::GetTimerUI();	// タイマーUIの情報
	CHitStop *pHitStop	= CSceneGame::GetHitStop();	// ヒットストップ情報
	CFlash   *pFlash	= CSceneGame::GetFlash();	// フラッシュ情報
	CCamera  *pCamera	= GET_MANAGER->GetCamera();	// カメラ情報

	// フラッシュ管理カウンターを初期化
	m_nCounterFlash = 0;

	// ゲームUIの自動描画をOFFにする
	pGameManager->SetDrawGameUI(false);

	// スコープインさせる
	pScope->SetScopeIn();

	// 死亡モーションを設定
	SetMotion(MOTION_DEATH);

	// ヒットストップさせる
	pHitStop->SetStop(HITSTOP_DEATH_START);

	// フラッシュを設定
	pFlash->Set
	( // 引数
		FLASH_INITALPHA_DEATH_START,	// 開始透明度
		FLASH_SUBALPHA_DEATH_START		// 透明度減算量
	);

	for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
	{ // パーツ数分繰り返す

		// パーツ拡大率を修正
		GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
	}

	// カメラ揺れを設定
	pCamera->SetSwing(CCamera::TYPE_MAIN, DEATH_START_SWING);

	// カメラを何もしない状態に設定 (固定カメラにする)
	pCamera->SetState(CCamera::STATE_NONE);

	// タイマーの計測終了
	pTimer->End();

	// ランキングに設定
	CRankingManager::Set(pTimer->Get());

	// 最終攻撃音の再生
	PLAY_SOUND(CSound::LABEL_SE_LAST_ATTACK);
}

//============================================================
//	スポーン状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateSpawn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置
	assert(GetMotionType() == MOTION_HOWL);		// モーションが咆哮中じゃない

	if (GetMotionKey() == LAND_MOTION_KEY && GetMotionKeyCounter() == 0)
	{ // モーションが着地したタイミングの場合

		// 着地のカメラ揺れ設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, LAND_SWING);
	}
	else if (GetMotionKey() == HOWL_MOTION_KEY && GetMotionKeyCounter() == 0)
	{ // モーションが口を開け始めたタイミングの場合

		// 咆哮のカメラ揺れ設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, HOWL_SWING);

		// ドラゴン咆哮の再生
		PLAY_SOUND(CSound::LABEL_SE_DRAGON_ROAR_L);
	}
	else if (IsMotionFinish())
	{ // モーションが終了していた場合

		// カウンターを加算
		m_nCounterAttack++;
		if (m_nCounterAttack > HOWL_WAIT_FRAME)
		{ // 余韻が終了した場合

			// カウンターを初期化
			m_nCounterAttack = 0;

			// なにもしない状態にする
			SetState(STATE_NONE);

			// 待機モーションにする
			SetMotion(MOTION_IDOL, BLEND_FRAME);
		}
	}

	// 着地判定
	UpdateLanding(&posEnemy);

	// 位置範囲外の補正
	LimitPosition(&posEnemy);

	// 位置を反映
	SetVec3Position(posEnemy);
}

//============================================================
//	通常状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateNormal(void)
{
	// 攻撃の更新
	UpdateAttack();

	// 行動の更新
	UpdateAction();
}

//============================================================
//	ノックバック状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateKnock(void)
{
	// ノックバック状態の更新
	CEnemy::UpdateKnock();

	// 別モーション指定エラー
	assert(GetMotionType() == MOTION_KNOCK);

	if (IsMotionFinish())
	{ // モーションが終了した場合

		CGameManager *pGameManager = CSceneGame::GetGameManager();	// ゲームマネージャーの情報
		CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報

		// ゲームを通常状態にする
		pGameManager->SetState(CGameManager::STATE_NORMAL);

		// カメラを追従状態に設定
		pCamera->SetState(CCamera::STATE_FOLLOW);
		pCamera->SetDestFollow();

		// スタン状態にする
		SetState(STATE_STAN);
	}
}

//============================================================
//	スタン状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateStan(void)
{
	// スタン状態の更新
	CEnemy::UpdateStan();

	if (GetState() == STATE_NORMAL)
	{ // 親クラス更新でスタンが終了した場合

		CCinemaScope *pScope = CSceneGame::GetCinemaScope();	// シネマスコープの情報

		// 攻撃までの待機時間を設定
		m_nCounterAttack = STAN_ATK_WAIT;

		// 待機モーションに移行
		SetMotion(MOTION_IDOL, BLEND_FRAME);

		// スコープアウトさせる
		pScope->SetScopeOut();
	}
}

//============================================================
//	ライド飛び上がり状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateRideFlyUp(void)
{
	// ライド飛び上がり状態の更新
	CEnemy::UpdateRideFlyUp();

	// 別モーション指定エラー
	assert(GetMotionType() == MOTION_HOWL_FLYUP);

	if (GetMotionKey() == FLYUP_MOTION_KEY && GetMotionKeyCounter() == 0)
	{ // モーションが口を開け始めたタイミングの場合

		// 飛び上がり前の咆哮のカメラ揺れ設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, RIDE_SWING);

		// ドラゴン咆哮の再生
		PLAY_SOUND(CSound::LABEL_SE_DRAGON_ROAR_S);
	}
	else if (IsMotionFinish())
	{ // モーションが終了している場合

		CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報

		// ライド情報を初期化
		m_ride.fDir = 0.0f;	// ボス方向を初期化

		// 旋回飛行モーションを設定
		SetMotion(MOTION_FLY_IDOL);

		// スコープアウトさせる
		CSceneGame::GetCinemaScope()->SetScopeOut();

		// カメラを騎乗状態に設定
		pCamera->SetState(CCamera::STATE_RIDE);
		pCamera->SetDestRide();

		// ライド旋回状態にする
		SetState(STATE_RIDE_ROTATE);
	}
}

//============================================================
//	ライド旋回状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateRideRotate(void)
{
	// ライド旋回状態時の更新
	CEnemy::UpdateRideRotate();

	CStage *pStage = CScene::GetStage();		// ステージ情報
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();	// 敵向き
	D3DXVECTOR3 posCenter = pStage->GetStageLimit().center;	// ステージ中央位置

	// 敵の位置を設定
	posEnemy.x = posCenter.x + sinf(m_ride.fDir) * RIDE_ROTATE_DIS;
	posEnemy.y = RIDE_ROTATE_POSY;
	posEnemy.z = posCenter.z + cosf(m_ride.fDir) * RIDE_ROTATE_DIS;

	// 敵の向きを進行方向に設定
	rotEnemy.y = atan2f(posCenter.x - posEnemy.x, posCenter.z - posEnemy.z);
	rotEnemy.y += D3DX_PI * 0.5f;

	// 敵位置の設定方向を回転
	m_ride.fDir += RIDE_ROTATE_MOVE;
	useful::NormalizeRot(m_ride.fDir);

	// 位置・向き反映
	SetVec3Position(posEnemy);
	SetVec3Rotation(rotEnemy);

	if (CScene::GetPlayer()->GetState() != CPlayer::STATE_RIDE_END)
	{ // ライドが終了していない場合

		// カウンターを加算
		m_nCounterRotate++;
		if (m_nCounterRotate > ROTATE_FRAME)
		{ // ライドの最大経過時間の場合

			// ライド終了状態にする
			CScene::GetPlayer()->SetRideEnd();

			// 空中振り下ろしモーションを設定
			SetMotion(MOTION_SHAKE_OFF);

			// ドラゴン咆哮の再生
			PLAY_SOUND(CSound::LABEL_SE_DRAGON_ROAR_S);
		}
	}
}

//============================================================
//	ライド終了状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateRideEnd(void)
{
	// ライド終了状態時の更新
	CEnemy::UpdateRideEnd();

	if (GetMotionType() != MOTION_COME_DOWN)
	{ // モーションが上空復帰ではなくなった場合

		// 通常状態を設定
		SetState(STATE_NORMAL);
	}
}

//============================================================
//	死亡状態時の更新処理
//============================================================
void CEnemyBossDragon::UpdateDeath(void)
{
	// ゲーム画面ではない場合抜ける
	if (GET_MANAGER->GetMode() != CScene::MODE_GAME) { return; }

	CHitStop *pHitStop	= CSceneGame::GetHitStop();	// ヒットストップ情報
	CFlash	*pFlash		= CSceneGame::GetFlash();	// フラッシュ情報
	CCamera	*pCamera	= GET_MANAGER->GetCamera();	// カメラ情報

	if (pFlash->GetState() == CFlash::FLASH_NONE)
	{ // フラッシュが終了した場合

		if (m_nCounterFlash < NUM_MULTI_FLASH)
		{ // 設定したフラッシュ回数に達していない場合

			if (m_nCounterFlash == 0)
			{ // 初回のフラッシュが終了した場合

				// 死亡演出の設定
				SetDeathStaging();
			}

			// フラッシュ数を加算
			m_nCounterFlash++;

			// ヒットストップさせる
			pHitStop->SetStop(HITSTOP_DEATH_MULTI);

			// フラッシュを設定
			pFlash->Set
			( // 引数
				FLASH_INITALPHA_DEATH_MULTI,	// 開始透明度
				FLASH_SUBALPHA_DEATH_MULTI		// 透明度減算量
			);

			// カメラの向きを設定
			pCamera->SetRotation(DEATH_CAMERA_ROT[m_nCounterFlash]);

			// カメラ揺れを設定
			pCamera->SetSwing(CCamera::TYPE_MAIN, DEATH_MULTI_SWING);

			// ノイズ音の再生
			PLAY_SOUND(CSound::LABEL_SE_NOISE);
		}
		else
		{ // 設定したフラッシュ回数に達した場合

			if (GetMotionKey() == 4 && GetMotionKeyCounter() == 0)
			{
				// 着地音の再生
				PLAY_SOUND(CSound::LABEL_SE_LAND_B);
			}
			else if (IsMotionFinish())
			{ // モーションが終了した場合

				if (IsDraw())
				{ // 自動描画がONの場合

					// 爆発の生成
					CParticle3D::Create(CParticle3D::TYPE_BIG_EXPLOSION, GetVec3Position() + EXP_OFFSET);

					// 自動描画をOFFにする
					SetEnableDraw(false);

					// カメラ揺れを設定
					GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, DEATH_SWING);

					// 爆発音の再生
					PLAY_SOUND(CSound::LABEL_SE_EXPLOSION);
				}

				// リザルト画面に遷移させる
				CSceneGame::GetGameManager()->TransitionResult(CRetentionManager::WIN_CLEAR);
			}
		}
	}
}

//============================================================
//	位置範囲外の補正処理
//============================================================
void CEnemyBossDragon::LimitPosition(D3DXVECTOR3 *pPos)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	if (!IsFly())
	{ // 飛行していない場合

		// ステージ範囲外の補正
		pStage->LimitPosition(*pPos, GetStatusInfo().fRadius);
	}
}

//============================================================
//	攻撃の選択処理
//============================================================
void CEnemyBossDragon::SelectAttack(void)
{
	// 前回の攻撃を保存
	m_oldAtk = m_curAtk;

	// ランダム生成情報を生成
	CRandom<CEnemyAttack::EAttack> *pRandom = CRandom<CEnemyAttack::EAttack>::Create();
	if (pRandom == nullptr) { return; }		// 生成ミス

	// 生成可能な攻撃を設定
	if (m_pAttack == nullptr) { return; }	// 攻撃クラス無し
	m_pAttack->SetRandomArray(pRandom, m_oldAtk, m_nCounterSameAct);

	// 攻撃を取得
	m_curAtk = pRandom->GetRandomNum();

	// 連続攻撃回数を設定
	if (m_oldAtk == m_curAtk)
	{ // 前回の攻撃と同一の場合

		m_nCounterSameAct++;
	}
	else
	{ // 前回の攻撃と別の場合

		m_nCounterSameAct = 0;
	}

	// ランダム生成情報を破棄
	CRandom<CEnemyAttack::EAttack>::Release(pRandom);
}

//============================================================
//	攻撃の更新処理
//============================================================
void CEnemyBossDragon::UpdateAttack(void)
{
	if (m_pAttack == nullptr)
	{ // 攻撃が存在しない場合

		// カウンターを加算
		m_nCounterAttack++;
		if (m_nCounterAttack > ATK_WAIT_FRAME)
		{ // 攻撃の余韻が終了した場合

			// カウンターを初期化
			m_nCounterAttack = 0;

			// 攻撃の生成
			m_pAttack = CEnemyAttack::Create
			( // 引数
#ifndef RANDOM_ATTACK_ON
				m_curAtk, this
#else
				ATTACK, this
#endif
			);
			if (m_pAttack == nullptr)
			{ // 生成に失敗した場合

				// 関数を抜ける
				assert(false);
				return;
			}
		}

		for (int nCntParts = 0; nCntParts < GetNumModel(); nCntParts++)
		{ // パーツ数分繰り返す

			// パーツ拡大率を修正
			GetMultiModel(nCntParts)->SetVec3Scaling(VEC3_ONE);
		}
	}
	else
	{ // 攻撃が存在する場合

		// 攻撃の更新
		if (m_pAttack->Update())
		{ // 攻撃が終了している場合

			// 次攻撃の選択
			SelectAttack();

			// 攻撃の破棄
			SAFE_REF_RELEASE(m_pAttack);

			// 何もしない行動をとらせる
			m_action = ACT_NONE;
		}
	}
}

//============================================================
//	行動の更新処理
//============================================================
void CEnemyBossDragon::UpdateAction(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();		// 敵位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();		// 敵向き
	D3DXVECTOR3 rotDestEnemy = GetDestRotation();	// 敵目標向き

	// 過去位置の更新
	UpdateOldPosition();

	if (!IsFly())
	{ // 飛行していない場合

		// 重力の更新
		UpdateGravity();
	}

	switch (m_action)
	{ // 行動ごとの処理
	case ACT_NONE:	// 何もしない
		break;

	case ACT_MAGIC_FADEIN:	// 魔法陣フェードイン

		// 魔法陣フェードイン行動時の更新
		UpdateMagicFadeIn(posEnemy);

		break;

	case ACT_MAGIC_FADEOUT:	// 魔法陣フェードアウト

		// 魔法陣フェードアウト行動時の更新
		UpdateMagicFadeOut(&posEnemy, &rotEnemy, &rotDestEnemy);

		break;

	case ACT_PUNCH_GROUND:	// 地面殴り

		// 地面殴りの行動時の更新
		UpdatePunchGround();

		break;

	case ACT_FIRE_ATTACK:	// 炎攻撃

		// 炎攻撃の行動時の更新
		UpdateFireAttack();

		break;

	case ACT_CLAW_ATTACK:	// ひっかき攻撃

		// ひっかき攻撃の行動時の更新
		UpdateClawAttack();

		break;

	case ACT_TAIL_ATTACK:	// しっぽ攻撃

		// しっぽ攻撃の行動時の更新
		UpdateTailAttack();

		break;

	case ACT_FLY_ATTACK:	// 空中攻撃

		// 空中攻撃の行動時の更新
		UpdatePunchGround();

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 位置更新
	UpdatePosition(&posEnemy);

	// 着地判定
	UpdateLanding(&posEnemy);

	// 向き更新
	UpdateRotation(&rotEnemy, &rotDestEnemy);

	// 位置範囲外の補正
	LimitPosition(&posEnemy);

	// 位置を反映
	SetVec3Position(posEnemy);

	// 向きを反映
	SetVec3Rotation(rotEnemy);

	// 目標向きを反映
	SetDestRotation(rotDestEnemy);
}

//============================================================
//	ノックバック演出の設定処理
//============================================================
void CEnemyBossDragon::SetKnockStaging(void)
{
	// ステージに残る演出の邪魔になるオブジェクトを破棄
	{
		// 破棄するラベルを要素に追加
		std::vector<ELabel> releaseLabel;	// 破棄ラベル配列
		releaseLabel.push_back(CObject::LABEL_EFFECT);		// エフェクト
		releaseLabel.push_back(CObject::LABEL_PARTICLE);	// パーティクル
		releaseLabel.push_back(CObject::LABEL_FIRE);		// 炎
		releaseLabel.push_back(CObject::LABEL_THUNDER);		// 雷
		releaseLabel.push_back(CObject::LABEL_WAVE);		// 波動

		// 指定したラベルのオブジェクト全破棄
		CObject::ReleaseAll(releaseLabel);

		// 破棄ラベル配列をクリア
		releaseLabel.clear();
	}

	// プレイヤー・敵の演出設定
	{
		CStage  *pStage  = CScene::GetStage();	// ステージ情報
		CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報

		// ゲーム画面を演出状態にする
		CSceneGame::GetGameManager()->SetState(CGameManager::STATE_STAGING);

		// 自身の位置をステージ中央にする
		D3DXVECTOR3 posEnemy = pStage->GetStageLimit().center;	// ボス設定位置
		UpdateLanding(&posEnemy);	// 着地判定
		LimitPosition(&posEnemy);	// 位置範囲外の補正
		SetVec3Position(posEnemy);	// 位置を反映

		// 自身の向きを初期化
		SetVec3Rotation(VEC3_ZERO);	// 向き
		SetDestRotation(VEC3_ZERO);	// 目標向き

		// プレイヤーの位置を設定
		D3DXVECTOR3 posPlayer = posEnemy + KNOCK_PLAYER_POS_OFFSET;
		pPlayer->SetVec3Position(posPlayer);	// 位置を反映

		// プレイヤーの向きを設定
		D3DXVECTOR3 rotPlayer = VEC3_ZERO;
		rotPlayer.y = atan2f(posPlayer.x - posEnemy.x, posPlayer.z - posEnemy.z);
		pPlayer->SetVec3Rotation(rotPlayer);	// 向きを反映
		pPlayer->SetDestRotation(rotPlayer);	// 目標向きを反映

		// 通常状態を初期化
		pPlayer->InitNormal();
	}

	// カメラ情報の設定
	{
		CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報

		// カメラの注視点をボスの中心に設定
		pCamera->SetPositionR(GetVec3Position() + KNOCK_CAM_OFFPOS);

		// カメラの向きを設定
		pCamera->SetRotation(GetVec3Rotation() + KNOCK_CAM_OFFROT);

		// カメラの距離を設定
		pCamera->SetDistance(KNOCK_CAM_DIS);
	}
}

//============================================================
//	死亡演出の設定処理
//============================================================
void CEnemyBossDragon::SetDeathStaging(void)
{
	// ステージに残る演出の邪魔になるオブジェクトを破棄
	{
		// 破棄するラベルを要素に追加
		std::vector<ELabel> releaseLabel;	// 破棄ラベル配列
		releaseLabel.push_back(CObject::LABEL_EFFECT);		// エフェクト
		releaseLabel.push_back(CObject::LABEL_PARTICLE);	// パーティクル
		releaseLabel.push_back(CObject::LABEL_FIRE);		// 炎
		releaseLabel.push_back(CObject::LABEL_THUNDER);		// 雷
		releaseLabel.push_back(CObject::LABEL_WAVE);		// 波動

		// 指定したラベルのオブジェクト全破棄
		CObject::ReleaseAll(releaseLabel);

		// 破棄ラベル配列をクリア
		releaseLabel.clear();
	}

	// プレイヤー・敵の演出設定
	{
		// ゲーム画面を演出状態にする
		CSceneGame::GetGameManager()->SetState(CGameManager::STATE_STAGING);

		// 自身の位置をステージ中央にする
		CStage *pStage = CScene::GetStage();	// ステージ情報
		CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報
		D3DXVECTOR3 posEnemy = pStage->GetStageLimit().center;	// ボス設定位置
		UpdateLanding(&posEnemy);	// 着地判定
		LimitPosition(&posEnemy);	// 位置範囲外の補正
		SetVec3Position(posEnemy);	// 位置を反映

		// 自身の向きを初期化
		SetVec3Rotation(VEC3_ZERO);	// 向き
		SetDestRotation(VEC3_ZERO);	// 目標向き

		// 剣の自動描画をOFFにする
		pPlayer->SetNoneTwinSword();
	}

	// カメラ情報の設定
	{
		CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報

		// カメラの注視点をボスの中心に設定
		pCamera->SetPositionR(GetVec3Position() + DEATH_CAMERA_OFFSET);

		// カメラの距離を設定
		pCamera->SetDistance(DEATH_CANERA_DIS);
	}
}

//============================================================
//	飛行フラグ取得処理
//============================================================
bool CEnemyBossDragon::IsFly(void) const
{
	// 変数を宣言
	EMotion curMotion = (EMotion)GetMotionType();	// 現在のモーション

	// TODO：空中モーション追加したら記述

	// 飛行状況を設定
	bool bFly = (curMotion == MOTION_FLY_IDOL
			  || curMotion == MOTION_FLY_ATTACK
			  || curMotion == MOTION_FLY_RUSH
			  || m_teleport.motion == MOTION_FLY_IDOL
			  || m_teleport.motion == MOTION_FLY_ATTACK
			  || m_teleport.motion == MOTION_FLY_RUSH);

	// 飛行状況を返す
	return bFly;
}

//============================================================
//	魔法陣フェードイン行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateMagicFadeIn(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	float fMagicPosY = m_pMagicCircle->GetVec3Position().y;	// 魔法陣の位置

	switch (m_teleport.state)
	{ // テレポート状態ごとの処理
	case TELEPORT_INIT:		// テレポートの初期化
	{
		// 影の完全に透明にできるようにする
		m_pShadow->SetMinAlpha(0.0f);

		// 魔法陣の自動描画をONにする
		m_pMagicCircle->SetEnableDraw(true);

		// 魔法陣の位置を足の下にする
		fMagicPosY = rPos.y;

		// テレポート音の再生
		PLAY_SOUND(CSound::LABEL_SE_TELEPORT);

		// 魔法陣の出現状態にする
		m_teleport.state = TELEPORT_APPEAR;

		// 処理を抜けずにこのまま魔法陣の出現に移行
	}
	case TELEPORT_APPEAR:	// 魔法陣の出現
	{
		// 変数を宣言
		float fMagicRadius = m_pMagicCircle->GetRadius();	// 魔法陣の半径

		// 魔法陣を拡大
		fMagicRadius += SCALE_MAGIC;
		if (fMagicRadius >= MAGIC_CIRCLE_RADIUS)
		{ // 拡大しきった場合

			// 半径を補正
			fMagicRadius = MAGIC_CIRCLE_RADIUS;

			// 魔法陣の移動状態にする
			m_teleport.state = TELEPORT_MOVE;
		}

		// 魔法陣の半径を反映
		m_pMagicCircle->SetRadius(fMagicRadius);

		break;
	}
	case TELEPORT_MOVE:	// 魔法陣の移動
	{
		// 変数を宣言
		float fDelPosY = GetMultiModel(MODEL_HEAD)->GetMtxWorld()._42 + MAGIC_DELPOS_PLUSY;	// 魔法陣の消失位置

		// 魔法陣を上に動かす
		fMagicPosY += MOVE_MAGIC;
		if (fMagicPosY >= fDelPosY)
		{ // 消失位置に到達した場合

			// 位置を補正
			fMagicPosY = fDelPosY;

			// 魔法陣の消失状態にする
			m_teleport.state = TELEPORT_DISAPPEAR;
		}

		break;
	}
	case TELEPORT_DISAPPEAR:	// 魔法陣の消失
	{
		// 変数を宣言
		float fMagicRadius = m_pMagicCircle->GetRadius();	// 魔法陣の半径

		// 魔法陣を縮小
		fMagicRadius -= SCALE_MAGIC;
		if (fMagicRadius <= 0.0f)
		{ // 縮小しきった場合

			// 半径を補正
			fMagicRadius = 0.0f;

			// テレポートの初期化状態にする
			m_teleport.state = TELEPORT_INIT;

			// 魔法陣によるフェードアウトの行動をとらせる
			m_action = ACT_MAGIC_FADEOUT;
		}

		// 魔法陣の半径を反映
		m_pMagicCircle->SetRadius(fMagicRadius);

		break;
	}
	default:	// 例外処理
		assert(false);
		break;
	}

	// 影の最小透明度を少しずつ透明にする
	float fMinAlpha = m_pShadow->GetMinAlpha() - shadowInfo::MIN_FADE_LEVEL;
	useful::LimitMinNum(fMinAlpha, 0.0f);
	m_pShadow->SetMinAlpha(fMinAlpha);

	// 影の最大透明度を少しずつ透明にする
	float fMaxAlpha = m_pShadow->GetMaxAlpha() - shadowInfo::MAX_FADE_LEVEL;
	useful::LimitMinNum(fMaxAlpha, 0.0f);
	m_pShadow->SetMaxAlpha(fMaxAlpha);

	// 魔法陣の位置を反映
	m_pMagicCircle->SetVec3Position(D3DXVECTOR3(rPos.x, fMagicPosY, rPos.z));
}

//============================================================
//	魔法陣フェードアウト行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateMagicFadeOut(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pRot, D3DXVECTOR3 *pDestRot)
{
	// 変数を宣言
	float fMagicPosY = m_pMagicCircle->GetVec3Position().y;	// 魔法陣の位置

	switch (m_teleport.state)
	{ // テレポート状態ごとの処理
	case TELEPORT_INIT:		// テレポートの初期化
	{
		if (m_teleport.motion > MOTION_NONE
		&&  m_teleport.motion < MOTION_MAX)
		{ // モーション指定がある場合

			// モーションを設定
			SetMotion(m_teleport.motion, BLEND_FRAME);

			// テレポート後モーションを初期化
			m_teleport.motion = MOTION_NONE;
		}

		// ボスの位置・向き・目標向きをテレポート先に変更
		*pPos = m_teleport.pos;
		*pRot = m_teleport.rot;
		*pDestRot = m_teleport.rot;

		// 位置範囲外の補正
		LimitPosition(pPos);

		if (m_teleport.bLook)
		{ // テレポート先を視認する場合

			// テレポート後のボスを視認させる
			GET_MANAGER->GetCamera()->SetFollowLook(*pPos);
		}

		// 魔法陣の位置を頭の上にする
		fMagicPosY  = GetMultiModel(MODEL_HEAD)->GetMtxWorld()._42 + MAGIC_DELPOS_PLUSY;
		fMagicPosY += pPos->y - GetOldPosition().y;	// 過去位置から今フレームでずれたY座標分を加算

		// テレポート音の再生
		PLAY_SOUND(CSound::LABEL_SE_TELEPORT);

		// 魔法陣の出現状態にする
		m_teleport.state = TELEPORT_APPEAR;

		// 処理を抜けずにこのまま魔法陣の出現に移行
	}
	case TELEPORT_APPEAR:	// 魔法陣の出現
	{
		// 変数を宣言
		float fMagicRadius = m_pMagicCircle->GetRadius();	// 魔法陣の半径

		// 魔法陣を拡大
		fMagicRadius += SCALE_MAGIC;
		if (fMagicRadius >= MAGIC_CIRCLE_RADIUS)
		{ // 拡大しきった場合

			// 半径を補正
			fMagicRadius = MAGIC_CIRCLE_RADIUS;

			// 魔法陣の移動状態にする
			m_teleport.state = TELEPORT_MOVE;
		}

		// 魔法陣の半径を反映
		m_pMagicCircle->SetRadius(fMagicRadius);

		break;
	}
	case TELEPORT_MOVE:	// 魔法陣の移動
	{
		// 変数を宣言
		float fDelPosY = GetVec3Position().y;	// 魔法陣の消失位置

		// 魔法陣を下に動かす
		fMagicPosY -= MOVE_MAGIC;
		if (fMagicPosY <= fDelPosY)
		{ // 消失位置に到達した場合

			// 位置を補正
			fMagicPosY = fDelPosY;

			// 魔法陣の消失状態にする
			m_teleport.state = TELEPORT_DISAPPEAR;
		}

		break;
	}
	case TELEPORT_DISAPPEAR:	// 魔法陣の消失
	{
		// 変数を宣言
		float fMagicRadius = m_pMagicCircle->GetRadius();	// 魔法陣の半径

		// 魔法陣を縮小
		fMagicRadius -= SCALE_MAGIC;
		if (fMagicRadius <= 0.0f)
		{ // 縮小しきった場合

			// 半径を補正
			fMagicRadius = 0.0f;

			// 魔法陣の自動描画をOFFにする
			m_pMagicCircle->SetEnableDraw(false);

			// 何もしない行動をとらせる
			m_action = ACT_NONE;
		}

		// 魔法陣の半径を反映
		m_pMagicCircle->SetRadius(fMagicRadius);

		break;
	}
	default:	// 例外処理
		assert(false);
		break;
	}

	// 影の最小透明度を少しずつ元に戻す
	float fMinAlpha = m_pShadow->GetMinAlpha() + shadowInfo::MIN_FADE_LEVEL;
	useful::LimitMaxNum(fMinAlpha, shadow::MIN_ALPHA);
	m_pShadow->SetMinAlpha(fMinAlpha);

	// 影の最大透明度を少しずつ元に戻す
	float fMaxAlpha = m_pShadow->GetMaxAlpha() + shadowInfo::MAX_FADE_LEVEL;
	useful::LimitMaxNum(fMaxAlpha, shadow::MAX_ALPHA);
	m_pShadow->SetMaxAlpha(fMaxAlpha);

	// 魔法陣の位置を反映
	m_pMagicCircle->SetVec3Position(D3DXVECTOR3(pPos->x, fMagicPosY, pPos->z));
}

//============================================================
//	地面殴り行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdatePunchGround(void)
{

}

//============================================================
//	炎攻撃の行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateFireAttack(void)
{

}

//============================================================
//	ひっかき攻撃の行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateClawAttack(void)
{

}

//============================================================
//	しっぽ攻撃の行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateTailAttack(void)
{

}

//============================================================
//	空中攻撃の行動時の更新処理
//============================================================
void CEnemyBossDragon::UpdateFlyAttack(void)
{

}
