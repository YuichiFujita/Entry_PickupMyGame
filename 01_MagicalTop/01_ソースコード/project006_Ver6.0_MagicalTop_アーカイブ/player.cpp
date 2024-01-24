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
#include "playerTutorial.h"
#include "playerGame.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "collision.h"

#include "multiModel.h"
#include "magicManager.h"
#include "expManager.h"
#include "objectGauge3D.h"
#include "gaugeStar.h"
#include "shadow.h"
#include "objectOrbit.h"
#include "enemy.h"
#include "target.h"
#include "stage.h"
#include "field.h"
#include "particle3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define PLAYER_SETUP_TXT	"data\\TXT\\player.txt"				// セットアップテキスト相対パス
#define PLAY_SHADOW_SIZE	(D3DXVECTOR3(80.0f, 0.0f, 80.0f))	// 影の大きさ
#define PLAY_ORBIT_COL		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f))	// 軌跡の色
#define PLAY_ORBIT_PART		(12)	// 軌跡の分割数
#define PLAY_ORBIT_PRIO		(4)		// 軌跡の優先順位

#define RESPAWN_ROT	(D3DXVECTOR3(0.0f, -HALF_PI, 0.0f))	// 再出現時の向き

#define GAUGE_PLUS_Y	(140.0f)	// ゲージY位置加算量
#define GAUGE_GAUGESIZE	(D3DXVECTOR3(110.0f, 15.0f, 0.0f))	// ゲージ大きさ
#define GAUGE_FRONTCOL	(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))	// 表ゲージ色
#define GAUGE_BACKCOL	(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f))	// 裏ゲージ色

#define STAR_ADDPOS		(D3DXVECTOR3(0.0f, 50.0f, 0.0f))	// 表示位置の加算量
#define STAR_RADIUS		(40.0f)	// 半径
#define STAR_HEAL		(4)		// ゲージ回復量
#define STAR_WAIT_HEAL	(120)	// 回復待機カウント
#define STAR_DIS_CENTER	(65.0f)	// 中心からの距離
#define STAR_FLICKER	(8.0f)	// 揺らめき量

#define VORTEX_ADDROT	(0.085f)	// 渦巻きこまれ時のプレイヤー回転速度
#define VORTEX_ADDDIS	(2.5f)		// 渦巻きこまれ時のプレイヤーと渦の距離離れ量
#define VORTEX_ADDPOSY	(6.5f)		// 渦巻きこまれ時の高さ座標加算量
#define VORTEX_HITDMG	(50)		// 渦巻きこまれ時のダメージ量

#define PLAY_AWAY_REV	(0.08f)		// 空中のプレイヤー移動量の減衰係数
#define PLAY_LAND_REV	(0.2f)		// 地上のプレイヤー移動量の減衰係数
#define PLAY_REV_ROTA	(0.15f)		// プレイヤー向き変更の減衰係数
#define PLAY_GRAVITY	(1.0f)		// プレイヤー重力
#define PLAY_RADIUS		(20.0f)		// プレイヤー半径
#define ENE_HIT_DMG		(30)		// 敵ヒット時のダメージ量

#define FADE_LEVEL		(0.01f)		// フェードのα値の加減量
#define AWAY_SIDE_MOVE	(25.0f)		// 吹っ飛び時の横移動量
#define AWAY_UP_MOVE	(18.0f)		// 吹っ飛び時の上移動量
#define INVULN_CNT		(16)		// 無敵状態に移行するまでのカウンター
#define NORMAL_CNT		(180)		// 通常状態に移行するまでのカウンター

#define CHANGE_ALPHA_CNT	(15)	// 透明度変更カウント
#define INVULN_MIN_ALPHA	(0.55f)	// 最小透明度
#define INVULN_MAX_ALPHA	(1.0f)	// 最大透明度

#define ALWAYS_ADDROT		(0.3f)	// 常時内側に向かせる量
#define PLUS_INSIDE_MOVE	(3.0f)	// ターゲットからの距離に応じた外側への移動加算量
#define PLUS_OUTSIDE_MOVE	(1.5f)	// ターゲットからの距離に応じた内側への移動加算量
#define INSIDE_ADDROT		(0.8f)	// 外側移動時の向き変動量
#define OUTSIDE_SUBROT		(0.5f)	// 内側移動時の向き変動量

#define MOVE_INSIDE			(0.55f)	// 内側への移動量
#define MOVE_LEFT			(0.55f)	// 左側への移動量
#define MOVE_LEFT_DECELE	(0.4f)	// 左移動の減速量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CPlayer::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\orbit000.jpg",	// 軌跡のテクスチャ
};

const char *CPlayer::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\PLAYER\\00_waist.x",	// 腰
	"data\\MODEL\\PLAYER\\01_body.x",	// 体
	"data\\MODEL\\PLAYER\\02_cloak.x",	// マント
	"data\\MODEL\\PLAYER\\03_head.x",	// 頭
	"data\\MODEL\\PLAYER\\04_hat.x",	// 帽子
	"data\\MODEL\\PLAYER\\05_armUL.x",	// 左上腕
	"data\\MODEL\\PLAYER\\06_armUR.x",	// 右上腕
	"data\\MODEL\\PLAYER\\07_armDL.x",	// 左下腕
	"data\\MODEL\\PLAYER\\08_armDR.x",	// 右下腕
	"data\\MODEL\\PLAYER\\09_handL.x",	// 左手
	"data\\MODEL\\PLAYER\\10_handR.x",	// 右手
	"data\\MODEL\\PLAYER\\11_legUL.x",	// 左太もも
	"data\\MODEL\\PLAYER\\12_legUR.x",	// 右太もも
	"data\\MODEL\\PLAYER\\13_legDL.x",	// 左脛
	"data\\MODEL\\PLAYER\\14_legDR.x",	// 右脛
	"data\\MODEL\\PLAYER\\15_footL.x",	// 左足
	"data\\MODEL\\PLAYER\\16_footR.x",	// 右足
	"data\\MODEL\\PLAYER\\17_rod.x",	// 杖
};

CPlayer::StatusInfo CPlayer::m_aStatusInfo[LEVEL_MAX] = {};	// ステータス情報

//************************************************************
//	子クラス [CPlayer] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayer::CPlayer() : CObjectChara(CObject::LABEL_PLAYER)
{
	// メンバ変数をクリア
	memset(&m_level, 0, sizeof(m_level));	// レベル
	m_pMagic		= NULL;			// 魔法マネージャーの情報
	m_pExp			= NULL;			// 経験値マネージャーの情報
	m_pLife			= NULL;			// 体力の情報
	m_pDash			= NULL;			// ダッシュの情報
	m_pShadow		= NULL;			// 影の情報
	m_pOrbit		= NULL;			// 軌跡の情報
	m_oldPos		= VEC3_ZERO;	// 過去位置
	m_move			= VEC3_ZERO;	// 移動量
	m_destRot		= VEC3_ZERO;	// 目標向き
	m_state			= STATE_NONE;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nNumModel		= 0;			// パーツの総数
	m_fDisTarget	= 0.0f;			// ターゲットとの距離
	m_fVortexRot	= 0.0f;			// 渦巻き方向
	m_fVortexDis	= 0.0f;			// 渦巻との距離
	m_bJump			= false;		// ジャンプ状況
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
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_pMagic		= NULL;			// 魔法マネージャーの情報
	m_pExp			= NULL;			// 経験値マネージャーの情報
	m_pLife			= NULL;			// 体力の情報
	m_pDash			= NULL;			// ダッシュの情報
	m_pShadow		= NULL;			// 影の情報
	m_pOrbit		= NULL;			// 軌跡の情報
	m_oldPos		= VEC3_ZERO;	// 過去位置
	m_move			= VEC3_ZERO;	// 移動量
	m_destRot		= VEC3_ZERO;	// 目標向き
	m_state			= STATE_NONE;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nNumModel		= 0;			// パーツの総数
	m_fDisTarget	= 0.0f;			// ターゲットとの距離
	m_fVortexRot	= 0.0f;			// 渦巻き方向
	m_fVortexDis	= 0.0f;			// 渦巻との距離
	m_bJump			= true;			// ジャンプ状況

	// レベル情報を初期化
	m_level.nLife		= LEVEL_00;	// 体力
	m_level.nDefense	= LEVEL_00;	// 防御力
	m_level.nDash		= LEVEL_00;	// ダッシュ
	m_level.nDefense	= LEVEL_00;	// 防御力
	m_level.nSpeed		= LEVEL_00;	// 素早さ
	m_level.nMulExp		= LEVEL_00;	// 経験値ボーナス

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

	// 魔法マネージャーの生成
	m_pMagic = CMagicManager::Create(this, m_aStatusInfo[m_level.nMana].nMana);
	if (UNUSED(m_pMagic))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 経験値マネージャーの生成
	m_pExp = CExpManager::Create();
	if (UNUSED(m_pExp))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 体力の生成
	m_pLife = CObjectGauge3D::Create
	( // 引数
		CObject::LABEL_GAUGE,				// オブジェクトラベル
		this,								// ゲージ表示オブジェクト
		m_aStatusInfo[m_level.nLife].nLife,	// 最大表示値
		(int)(NORMAL_CNT * 0.25f),			// 表示値変動フレーム
		GAUGE_PLUS_Y,						// 表示Y位置の加算量
		GAUGE_GAUGESIZE,					// ゲージ大きさ
		GAUGE_FRONTCOL,						// 表ゲージ色
		GAUGE_BACKCOL,						// 裏ゲージ色
		true,								// 枠描画状況
		CObjectGauge3D::TYPE_PLAYER,		// 枠種類
		GAUGE_GAUGESIZE						// 枠大きさ
	);
	if (UNUSED(m_pLife))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ダッシュの生成
	m_pDash = CGaugeStar::Create
	( // 引数
		m_aStatusInfo[m_level.nDash].nDash,	// 最大ゲージ量
		STAR_RADIUS,						// 半径
		STAR_HEAL,							// ゲージ回復量
		STAR_WAIT_HEAL,						// 回復待機カウント
		this,								// 親オブジェクト
		STAR_ADDPOS,						// 表示位置の加算量
		STAR_DIS_CENTER,					// 中心からの距離
		STAR_FLICKER						// 揺らめき量
	);
	if (UNUSED(m_pDash))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, PLAY_SHADOW_SIZE, this);
	if (UNUSED(m_pShadow))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 軌跡の生成
	m_pOrbit = CObjectOrbit::Create(GetMultiModel(MODEL_ROD)->GetPtrMtxWorld(), PLAY_ORBIT_COL, CObjectOrbit::OFFSET_ROD, PLAY_ORBIT_PART);
	if (UNUSED(m_pOrbit))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを読込・割当
	m_pOrbit->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_ORBIT]));

	// 優先順位を設定
	m_pOrbit->SetPriority(PLAY_ORBIT_PRIO);

	// 透明度を設定
	SetAlpha(0.0f);

	// 表示の設定
	SetDisp(false);

	// モーションの設定
	SetMotion(MOTION_MOVE);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayer::Uninit(void)
{
	// 魔法マネージャーを破棄
	if (FAILED(m_pMagic->Release(m_pMagic)))
	{ // 破棄に失敗した場合

		// 例外処理
		assert(false);
	}

	// 経験値マネージャーを破棄
	if (FAILED(m_pExp->Release(m_pExp)))
	{ // 破棄に失敗した場合

		// 例外処理
		assert(false);
	}

	// 影を破棄
	m_pShadow->Uninit();

	// 軌跡を破棄
	m_pOrbit->Uninit();

	// オブジェクトキャラクターの終了
	CObjectChara::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayer::Update(void)
{
	// 魔法マネージャーの更新
	m_pMagic->Update();

	// 経験値マネージャーの更新
	m_pExp->Update();

	// 影の更新
	m_pShadow->Update();

	// 軌跡の更新
	m_pOrbit->Update();
}

//============================================================
//	描画処理
//============================================================
void CPlayer::Draw(void)
{
	// オブジェクトキャラクターの描画
	CObjectChara::Draw();
}

//============================================================
//	ヒット処理
//============================================================
void CPlayer::Hit(const int nDmg)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// プレイヤー位置
	int nRevDamage = 0;	// 補正後のダメージ量

	if (IsDeath() != true)
	{ // 死亡フラグが立っていない場合

		if (m_state == STATE_NORMAL)
		{ // 通常状態の場合

			// ダメージ量を補正
			nRevDamage = nDmg - (int)(m_aStatusInfo[m_level.nDefense].nDefense * 0.25f);
			useful::LimitNum(nRevDamage, 0, m_aStatusInfo[m_level.nLife].nLife);	// ダメージ量制限

			// 体力からダメージ分減算
			m_pLife->AddNum(-nRevDamage);

			if (m_pLife->GetNum() > 0)
			{ // 生きている場合

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を変更
				m_state = STATE_DAMAGE;	// ダメージ状態
			}
			else
			{ // 死んでいる場合

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を変更
				m_state = STATE_DEATH;	// 死亡状態

				// モーションの設定
				SetMotion(MOTION_DEATH);	// 死亡モーション

				// サウンドの再生
				CManager::GetSound()->Play(CSound::LABEL_SE_BREAK);	// 破壊音
			}

			// パーティクル3Dオブジェクトを生成
			CParticle3D::Create(CParticle3D::TYPE_PLAYER_DAMAGE, pos);
		}
	}
}

//============================================================
//	生成処理
//============================================================
CPlayer *CPlayer::Create
(
	const CScene::MODE mode,	// モード
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot		// 向き
)
{
	// 変数を宣言
	D3DXVECTOR3 pos = rPos;		// 座標設定用

	// ポインタを宣言
	CPlayer *pPlayer = NULL;	// プレイヤー生成用

	if (UNUSED(pPlayer))
	{ // 使用されていない場合

		switch (mode)
		{ // モードごとの処理
		case CScene::MODE_TITLE:

			// 無し

			break;

		case CScene::MODE_TUTORIAL:

			// メモリ確保
			pPlayer = new CPlayerTutorial;	// チュートリアルプレイヤー

			break;

		case CScene::MODE_GAME:

			// メモリ確保
			pPlayer = new CPlayerGame;	// ゲームプレイヤー

			break;

		case CScene::MODE_RESULT:

			// 無し

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pPlayer))
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

		// 位置を設定
		CScene::GetStage()->LimitPosition(pos, PLAY_RADIUS);	// ステージ範囲外補正
		pos.y = CScene::GetField()->GetPositionHeight(pos);		// 高さを地面に設定
		pPlayer->SetPosition(pos);

		// 向きを設定
		pPlayer->SetRotation(rRot);
	}

	// 確保したアドレスを返す
	return pPlayer;
}

//============================================================
//	ステータス情報取得処理
//============================================================
CPlayer::StatusInfo CPlayer::GetStatusInfo(const LEVEL level)
{
	// 引数のレベルのステータス情報を返す
	return m_aStatusInfo[level];
}

//============================================================
//	ステータスレベルの加算処理
//============================================================
void CPlayer::AddLevelStatus(const LEVELINFO level)
{
	// 変数を宣言
	int nLife = 0;	// 体力の保存用
	int nMana = 0;	// マナの保存用

	// 引数のレベルを加算
	switch (level)
	{ // レベル情報ごとの処理
	case LEVELINFO_LIFE:	// 体力

		// 体力のレベルを加算
		m_level.nLife++;

		// 現在の体力を取得
		nLife = m_pLife->GetNum();

		// 最大体力を更新
		m_pLife->SetMaxNum(m_aStatusInfo[m_level.nLife].nLife);

		// 体力の増加分を先ほどまでの体力に加算して設定
		m_pLife->SetNum(nLife + (m_aStatusInfo[m_level.nLife].nLife - m_aStatusInfo[m_level.nLife - 1].nLife));

		// 体力オーバー
		assert(m_level.nLife < LEVEL_MAX);

		break;

	case LEVELINFO_MANA:		// マナ

		// マナのレベルを加算
		m_level.nMana++;

		// 現在のマナを取得
		nMana = m_pMagic->GetMana();

		// 最大マナを更新
		m_pMagic->SetMaxMana(m_aStatusInfo[m_level.nMana].nMana);

		// マナの増加分を先ほどまでのマナに加算して設定
		m_pMagic->SetMana(nMana + (m_aStatusInfo[m_level.nMana].nMana - m_aStatusInfo[m_level.nMana - 1].nMana));

		// マナオーバー
		assert(m_level.nMana < LEVEL_MAX);

		break;

	case LEVELINFO_DASH:	// ダッシュ

		// ダッシュのレベルを加算
		m_level.nDash++;

		// 最大ダッシュを更新
		m_pDash->SetMaxNumGauge(m_aStatusInfo[m_level.nDash].nDash);

		// ダッシュオーバー
		assert(m_level.nDash < LEVEL_MAX);

		break;

	case LEVELINFO_DEFENSE:	// 防御力

		// 防御力のレベルを加算
		m_level.nDefense++;

		// 防御力オーバー
		assert(m_level.nDefense < LEVEL_MAX);

		break;

	case LEVELINFO_SPEED:	// 素早さ

		// 素早さのレベルを加算
		m_level.nSpeed++;

		// 素早さオーバー
		assert(m_level.nSpeed < LEVEL_MAX);

		break;

	case LEVELINFO_EXP_UP:	// 経験値ボーナス

		// 経験値ボーナスのレベルを加算
		m_level.nMulExp++;

		// 経験値ボーナスオーバー
		assert(m_level.nMulExp < LEVEL_MAX);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	レベルの加算処理
//============================================================
void CPlayer::AddLevel(const int nAdd)
{
	// レベルを加算
	m_pExp->AddLevel(nAdd);
}

//============================================================
//	経験値の加算処理
//============================================================
void CPlayer::AddExp(const int nAdd)
{
	// 経験値を加算
	m_pExp->AddExp(nAdd);
}

//============================================================
//	体力の加算処理
//============================================================
void CPlayer::AddLife(const int nAdd)
{
	// 体力を加算
	m_pLife->AddNum(nAdd);
}

//============================================================
//	再出現の設定処理
//============================================================
void CPlayer::SetRespawn(D3DXVECTOR3& rPos)
{
	// 情報を初期化
	SetMotion(CPlayer::MOTION_MOVE);	// 浮遊モーションを設定
	m_state = STATE_NORMAL;	// 通常状態を設定
	m_nCounterState = 0;	// カウンターを初期化

	// 位置を補正・設定
	CScene::GetStage()->LimitPosition(rPos, PLAY_RADIUS);	// ステージ範囲外の補正
	UpdateLanding(rPos);	// 着地判定
	SetPosition(rPos);		// 位置を設定

	// 向きを設定
	SetRotation(RESPAWN_ROT);

	// ターゲットとの距離を設定
	UpdateDisTarget();

	// マナゲージを全回復させる
	m_pMagic->SetMana(m_pMagic->GetMaxMana());

	// ダッシュゲージを全回復させる
	m_pDash->HealNumGauge();

	// 軌跡の初期化を行う状態にする
	m_pOrbit->SetEnableInit(false);

	// 表示する設定にする
	SetDisp(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を不透明に再設定
	SetAlpha(1.0f);

	// カメラ更新をONにする
	CManager::GetCamera()->SetEnableUpdate(true);

	// カメラ目標位置設定
	CManager::GetCamera()->SetDestBargaining();
}

//============================================================
//	マナの設定処理
//============================================================
void CPlayer::SetMana(const int nMana)
{
	// 引数のマナを設定
	m_pMagic->SetMana(nMana);
}

//============================================================
//	状態の設定処理
//============================================================
void CPlayer::SetState(const int nState)
{
	// 引数の状態を設定
	m_state = (STATE)nState;
}

//============================================================
//	更新状況の設定処理
//============================================================
void CPlayer::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);		// 自身
	m_pShadow->SetEnableUpdate(bUpdate);	// 影
	m_pOrbit->SetEnableUpdate(bUpdate);		// 軌跡
	m_pDash->SetEnableUpdate(bUpdate);		// ダッシュ
}

//============================================================
//	描画状況の設定処理
//============================================================
void CPlayer::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pShadow->SetEnableDraw(bDraw);	// 影
	m_pOrbit->SetEnableDraw(bDraw);		// 軌跡
}

//============================================================
//	マナの描画状況の設定処理
//============================================================
void CPlayer::SetEnableDrawMana(const bool bDraw)
{
	// 引数の描画状況をマナに設定
	m_pMagic->SetEnableManaDraw(bDraw);
}

//============================================================
//	経験値の描画状況の設定処理
//============================================================
void CPlayer::SetEnableDrawExp(const bool bDraw)
{
	// 引数の描画状況を経験値に設定
	m_pExp->SetEnableExpDraw(bDraw);
}

//============================================================
//	体力の描画状況の設定処理
//============================================================
void CPlayer::SetEnableDrawLife(const bool bDraw)
{
	// 引数の描画状況を体力に設定
	m_pLife->SetEnableDraw(bDraw);
}

//============================================================
//	ダッシュの描画状況の設定処理
//============================================================
void CPlayer::SetEnableDrawDash(const bool bDraw)
{
	// 引数の描画状況をダッシュに設定
	m_pDash->SetEnableDraw(bDraw);
}

//============================================================
//	マナの回復状況の設定処理
//============================================================
void CPlayer::SetEnableHealMana(const bool bHeal)
{
	// 引数の回復状況をマナの回復状況に設定
	m_pMagic->SetEnableHealMana(bHeal);
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CPlayer::GetMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX  mtxRot, mtxTrans, mtxWorld;	// 計算用マトリックス
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetRotation();	// プレイヤー向き

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
//	ターゲットとの距離取得処理
//============================================================
float CPlayer::GetDistanceTarget(void) const
{
	// ターゲットとの距離を返す
	return m_fDisTarget;
}

//============================================================
//	ステータスレベル取得処理
//============================================================
int CPlayer::GetLevelStatus(const LEVELINFO level) const
{
	// 引数のレベルを返す
	switch (level)
	{ // レベル情報ごとの処理
	case LEVELINFO_LIFE:	// 体力

		// 体力のレベルを返す
		return m_level.nLife;

	case LEVELINFO_MANA:	// マナ

		// マナのレベルを返す
		return m_level.nMana;

	case LEVELINFO_DASH:	// ダッシュ

		// ダッシュのレベルを返す
		return m_level.nDash;

	case LEVELINFO_DEFENSE:	// 防御力

		// 防御力のレベルを返す
		return m_level.nDefense;

	case LEVELINFO_SPEED:	// 素早さ

		// 素早さのレベルを返す
		return m_level.nSpeed;

	case LEVELINFO_EXP_UP:	// 経験値ボーナス

		// 経験値ボーナスのレベルを返す
		return m_level.nMulExp;

	default:	// 例外処理
		assert(false);
		return NONE_IDX;
	}
}

//============================================================
//	レベル取得処理
//============================================================
int CPlayer::GetLevel(void) const
{
	// レベルを返す
	return m_pExp->GetLevel();
}

//============================================================
//	体力取得処理
//============================================================
int CPlayer::GetLife(void) const
{
	// 体力を返す
	return m_pLife->GetNum();
}

//============================================================
//	最大体力取得処理
//============================================================
int CPlayer::GetMaxLife(void) const
{
	// 最大体力を返す
	return m_pLife->GetMaxNum();
}

//============================================================
//	マナ取得処理
//============================================================
int CPlayer::GetMana(void) const
{
	// マナを返す
	return m_pMagic->GetMana();
}

//============================================================
//	最大マナ取得処理
//============================================================
int CPlayer::GetMaxMana(void) const
{
	// 最大マナを返す
	return m_pMagic->GetMaxMana();
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
	return PLAY_RADIUS;
}

//============================================================
//	吹っ飛びの設定処理
//============================================================
void CPlayer::SetBlowAway(const D3DXVECTOR3& rVecAway)
{
	// 移動量を設定
	m_move = rVecAway * AWAY_SIDE_MOVE;
	m_move.y = AWAY_UP_MOVE;
}

//============================================================
//	渦巻きこまれの設定処理
//============================================================
void CPlayer::SetVortex(const D3DXVECTOR3& rPlayerPos, const D3DXVECTOR3& rHitPos)
{
	// 巻き込まれ始めの向きを設定
	m_fVortexRot = atan2f(rPlayerPos.x - rHitPos.x, rPlayerPos.z - rHitPos.z);

	// 渦との距離を設定
	m_fVortexDis = sqrtf((rPlayerPos.x - rHitPos.x) * (rPlayerPos.x - rHitPos.x) + (rPlayerPos.z - rHitPos.z) * (rPlayerPos.z - rHitPos.z)) * 0.5f;
}

//============================================================
//	過去位置の更新処理
//============================================================
void CPlayer::UpdateOldPosition(void)
{
	// 過去位置を更新
	m_oldPos = GetPosition();
}

//============================================================
//	ターゲット距離の更新処理
//============================================================
void CPlayer::UpdateDisTarget(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置
	D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置

	// ターゲットとの距離を設定
	m_fDisTarget = sqrtf((posPlayer.x - posTarget.x) * (posPlayer.x - posTarget.x)+ (posPlayer.z - posTarget.z) * (posPlayer.z - posTarget.z)) * 0.5f;
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

	// モーションの遷移
	if (IsMotionFinish())
	{ // モーションが終了していた場合

		switch (GetMotionType())
		{ // モーションの種類ごとの処理
		case MOTION_BLOW_AWAY:	// 吹っ飛び状態

			// 無し

			// 処理を抜ける
			break;
		}
	}
}

//============================================================
//	吸い込みの更新処理
//============================================================
void CPlayer::UpdateAbsorb(D3DXVECTOR3& rVecTarg, D3DXVECTOR3& rVecSide)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置
	D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
	float fDisTargRate = (1.0f / CScene::GetStage()->GetStageLimit().fRadius) * m_fDisTarget;	// ターゲット距離の割合

	// ターゲット逆方向のベクトルを計算
	rVecTarg = posTarget - posPlayer;
	rVecTarg.y = 0.0f;							// ベクトルの縦方向を無視
	D3DXVec3Normalize(&rVecTarg, &rVecTarg);	// ベクトルの正規化

	// ターゲット横方向ベクトルを計算
	rVecSide = D3DXVECTOR3(rVecTarg.z, 0.0f, -rVecTarg.x);

	// 目標向きを設定
	m_destRot.y = atan2f(posTarget.x - posPlayer.x, posTarget.z - posPlayer.z);
	m_destRot.y -= D3DX_PI * 0.5f;	// 左を向かせる
	m_destRot.y -= ALWAYS_ADDROT;	// 向きをほんのり内側よりにする

	// 内側への移動量を設定
	m_move += rVecTarg * (MOVE_INSIDE + (fDisTargRate * PLUS_INSIDE_MOVE));

	// 左側への移動量を設定
	m_move += rVecSide * MOVE_LEFT;
}

//============================================================
//	ターゲット逆方向への加減速の操作処理
//============================================================
CPlayer::MOTION CPlayer::ControlTargAccel(const D3DXVECTOR3& rVecTarg, bool *pMove)
{
	// 変数を宣言
	float fDisTargRate = (1.0f / CScene::GetStage()->GetStageLimit().fRadius) * m_fDisTarget;	// ターゲット距離の割合

	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetPad();		// パッド

	if (pKeyboard->GetPress(DIK_W) || pPad->GetPressLStickY() > 0.0f)
	{
		// 外側への移動量を追加
		m_move -= rVecTarg * (m_aStatusInfo[m_level.nSpeed].speed.fOutside + (fDisTargRate * PLUS_OUTSIDE_MOVE));

		// 外側を向かせる
		m_destRot.y += INSIDE_ADDROT;

		if (USED(pMove))
		{ // 移動判定の引数が使用されていた場合

			// 移動している状態にする
			*pMove = true;
		}
	}
	else if (pKeyboard->GetPress(DIK_S) || pPad->GetPressLStickY() < 0.0f)
	{
		// 内側への移動量を追加
		m_move += rVecTarg * m_aStatusInfo[m_level.nSpeed].speed.fInside;

		// 内側を向かせる
		m_destRot.y -= OUTSIDE_SUBROT;

		if (USED(pMove))
		{ // 移動判定の引数が使用されていた場合

			// 移動している状態にする
			*pMove = true;
		}
	}
	else
	{
		if (USED(pMove))
		{ // 移動判定の引数が使用されていた場合

			// 移動していない状態にする
			*pMove = false;
		}
	}

	// 浮遊モーションを返す
	return MOTION_MOVE;
}

//============================================================
//	ターゲット横方向への加減速の操作処理
//============================================================
CPlayer::MOTION CPlayer::ControlSideAccel(const D3DXVECTOR3& rVecSide, bool *pMove)
{
	// 変数を宣言
	MOTION currentMotion = MOTION_MOVE;	// 現在のモーション

	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetPad();		// パッド

	if (pKeyboard->GetPress(DIK_A) || pPad->GetPress(CInputPad::KEY_L1))
	{
		if (m_pDash->UseGauge())
		{ // ゲージが使用できた場合

			// 左移動量を加速
			m_move += rVecSide * m_aStatusInfo[m_level.nSpeed].speed.fLeftside;

			// 現在のモーションを設定
			currentMotion = MOTION_ACCELE;	// 加速モーション

			if (USED(pMove))
			{ // 移動判定の引数が使用されていた場合

				// 移動している状態にする
				*pMove = true;
			}
		}
	}
	else if (pKeyboard->GetPress(DIK_D) || pPad->GetPress(CInputPad::KEY_R1))
	{
		// 左移動量を減速
		m_move -= rVecSide * MOVE_LEFT_DECELE;

		// 現在のモーションを設定
		currentMotion = MOTION_DECELE;	// 減速モーション

		if (USED(pMove))
		{ // 移動判定の引数が使用されていた場合

			// 移動している状態にする
			*pMove = true;
		}
	}
	else
	{
		if (USED(pMove))
		{ // 移動判定の引数が使用されていた場合

			// 移動していない状態にする
			*pMove = false;
		}
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	魔法攻撃の操作処理
//============================================================
CPlayer::MOTION CPlayer::ControlShotMagic(MOTION motion, bool *pShot)
{
	// 変数を宣言
	MOTION currentMotion = motion;	// 現在のモーション
	CMagicManager::Shot shot;		// 魔法の発射状況

	// 魔法の発射
	shot = m_pMagic->ShotMagic();	// 発射情報を代入

	if (shot.bControl && m_pMagic->GetMana() > 0)
	{ // 発射操作をしている且つ、マナ残量がある場合

		// 射撃向きをの値を0～2πにする
		shot.fRot += D3DX_PI;

		if (shot.fRot >= QRTR_PI && shot.fRot <= D3DX_PI - QRTR_PI)
		{
			// 右のアクションモーションを設定
			currentMotion = MOTION_ACTION_RIGHT;
		}
		else if (shot.fRot >= D3DX_PI - QRTR_PI && shot.fRot <= D3DX_PI + QRTR_PI)
		{
			// 手前のアクションモーションを設定
			currentMotion = MOTION_ACTION_NEAR;
		}
		else if (shot.fRot >= D3DX_PI + QRTR_PI && shot.fRot <= (D3DX_PI * 2) - QRTR_PI)
		{
			// 左のアクションモーションを設定
			currentMotion = MOTION_ACTION_LEFT;
		}
		else if (shot.fRot >= (D3DX_PI * 2) - QRTR_PI && shot.fRot <= (D3DX_PI * 2) || shot.fRot >= 0.0f && shot.fRot <= QRTR_PI)
		{
			// 奥のアクションモーションを設定
			currentMotion = MOTION_ACTION_FAR;
		}
		else
		{
			// 例外処理
			assert(false);	// 向きの正規化ミス
		}

		if (USED(pShot))
		{ // 射撃判定の引数が使用されていた場合

			// 射撃している状態にする
			*pShot = true;
		}
	}
	else
	{ // 発射できていない場合

		if (USED(pShot))
		{ // 射撃判定の引数が使用されていた場合

			// 射撃していない状態にする
			*pShot = false;
		}
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	着地状況の更新処理
//============================================================
bool CPlayer::UpdateLanding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// 着地判定
	if (CScene::GetField()->LandPosition(rPos, m_move)
	||  CScene::GetStage()->LandPosition(rPos, m_move, 0.0f))
	{ // プレイヤーが着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;
	}
	else
	{ // 着地していない場合

		// ジャンプしている状態にする
		m_bJump = true;
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	位置の更新処理
//============================================================
void CPlayer::UpdatePosition(D3DXVECTOR3& rPos)
{
	// 重力を加算
	m_move.y -= PLAY_GRAVITY;

	// 移動量を加算
	rPos += m_move;

	// 移動量を減衰
	if (m_state == STATE_BLOW_AWAY)
	{ // 空中の場合

		m_move.x += (0.0f - m_move.x) * PLAY_AWAY_REV;
		m_move.z += (0.0f - m_move.z) * PLAY_AWAY_REV;
	}
	else
	{ // 地上の場合

		m_move.x += (0.0f - m_move.x) * PLAY_LAND_REV;
		m_move.z += (0.0f - m_move.z) * PLAY_LAND_REV;
	}
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
	rRot.y += fDiffRot * PLAY_REV_ROTA;

	// 向きの正規化
	useful::NormalizeRot(rRot.y);
}

//============================================================
//	ターゲットとの当たり判定
//============================================================
void CPlayer::CollisionTarget(D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (pTarget->GetState() != CTarget::STATE_DESTROY)
	{ // ターゲットが破壊されていない場合

		// ターゲットとの衝突判定
		collision::CirclePillar
		( // 引数
			rPos,					// 判定位置
			pTarget->GetPosition(),	// 判定目標位置
			PLAY_RADIUS,			// 判定半径
			pTarget->GetRadius()	// 判定目標半径
		);
	}
}

//============================================================
//	敵との当たり判定
//============================================================
void CPlayer::CollisionEnemy(D3DXVECTOR3& rPos)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (USED(pObjectTop))
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (USED(pObjCheck))
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_ENEMY)
				{ // オブジェクトラベルが敵ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() == CEnemy::STATE_SPAWN
				||  pObjCheck->GetState() == CEnemy::STATE_DEATH)
				{ // 敵の状態がスポーン状態か死亡状態の場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 敵との当たり判定
				if (collision::CirclePillar
				( // 引数
					rPos,						// 判定位置
					pObjCheck->GetPosition(),	// 判定目標位置
					PLAY_RADIUS,				// 判定半径
					CEnemy::GetStatusInfo(pObjCheck->GetType()).fRadius	// 判定目標半径
				))
				{ // 当たっていた場合

					// プレイヤーのヒット処理
					HitBlowAway(rPos, pObjCheck->GetPosition(), ENE_HIT_DMG);
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	ダメージ状態時の更新処理
//============================================================
CPlayer::MOTION CPlayer::UpdateDamage(void)
{
	// 変数を宣言
	MOTION currentMotion;	// 現在のモーション

	// 通常状態時の更新
	currentMotion = UpdateNormal();

	if (m_nCounterState < INVULN_CNT)
	{ // カウンターが一定値より小さい場合

		// カウンターを加算
		m_nCounterState++;

		// マテリアルを設定
		SetMaterial(material::Red());	// 赤
	}
	else
	{ // カウンターが一定値以上の場合

		// カウンターを初期化
		m_nCounterState = 0;

		// マテリアル再設定
		ResetMaterial();

		// 状態を変更
		m_state = STATE_INVULN;	// 無敵状態
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	無敵状態時の更新処理
//============================================================
CPlayer::MOTION CPlayer::UpdateInvuln(void)
{
	// 変数を宣言
	MOTION currentMotion;	// 現在のモーション

	// 通常状態時の更新
	currentMotion = UpdateNormal();

	if (m_nCounterState < NORMAL_CNT)
	{ // カウンターが一定値より小さい場合

		// カウンターを加算
		m_nCounterState++;

		if (m_nCounterState % CHANGE_ALPHA_CNT == 0)
		{ // 透明度の変更タイミングだった場合

			if (GetAlpha() > INVULN_MIN_ALPHA)
			{ // 透明度が上がっていない場合

				// 透明度を上げる
				SetAlpha(INVULN_MIN_ALPHA);
			}
			else
			{ // 透明度が上がっている場合

				// 透明度を下げる
				SetAlpha(INVULN_MAX_ALPHA);
			}
		}
	}
	else
	{ // カウンターが一定値以上の場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 透明度を不透明に設定
		SetAlpha(1.0f);

		// 状態を変更
		m_state = STATE_NORMAL;	// 通常状態
	}

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	吹っ飛び状態時の更新処理
//============================================================
void CPlayer::UpdateBlowAway(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置

	// 位置更新
	UpdatePosition(posPlayer);

	// ターゲットとの当たり判定
	CollisionTarget(posPlayer);

	// ステージ範囲外の補正
	CScene::GetStage()->LimitPosition(posPlayer, PLAY_RADIUS);

	// 着地判定
	if (UpdateLanding(posPlayer))
	{ // 着地していた場合

		// 浮遊モーションに移行
		SetMotion(MOTION_MOVE);

		// 状態を設定
		m_state = STATE_DAMAGE;	// ダメージ状態
	}

	// 位置を反映
	SetPosition(posPlayer);

	// 向きを設定
	SetRotation(D3DXVECTOR3(0.0f, atan2f(m_move.x, m_move.z), 0.0f));
}

//============================================================
//	渦巻きこまれ状態時の更新処理
//============================================================
void CPlayer::UpdateVortex(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置

	// 向きを減算
	m_fVortexRot -= VORTEX_ADDROT;
	useful::NormalizeRot(m_fVortexRot);

	// 距離を加算
	m_fVortexDis += VORTEX_ADDDIS;

	// 位置を設定
	posPlayer.x = sinf(m_fVortexRot) * m_fVortexDis;
	posPlayer.y += VORTEX_ADDPOSY;
	posPlayer.z = cosf(m_fVortexRot) * m_fVortexDis;

	// 位置を反映
	SetPosition(posPlayer);

	// フェードイン状態時の更新
	UpdateFadeIn();

	if (m_state == STATE_NONE)
	{ // 透明になり切った場合

		// プレイヤーを再出現させる
		SetRespawn(PLAY_SPAWN_POS);
	}
}

//============================================================
//	フェードアウト状態時の更新処理
//============================================================
void CPlayer::UpdateFadeOut(void)
{
	// 変数を宣言
	float fAlpha = GetAlpha();	// 透明度

	// プレイヤー自身の描画を再開
	CObject::SetEnableDraw(true);

	// 透明度を上げる
	fAlpha += FADE_LEVEL;

	if (fAlpha >= GetMaxAlpha())
	{ // 透明度が上がり切った場合

		// 透明度を補正
		fAlpha = GetMaxAlpha();

		// 状態を設定
		m_state = STATE_INVULN;	// 無敵状態
	}

	// 透明度を設定
	SetAlpha(fAlpha);
}

//============================================================
//	フェードイン状態時の更新処理
//============================================================
void CPlayer::UpdateFadeIn(void)
{
	// 変数を宣言
	float fAlpha = GetAlpha();	// 透明度

	// 透明度を下げる
	fAlpha -= FADE_LEVEL;

	if (fAlpha <= 0.0f)
	{ // 透明度が下がり切った場合

		// 透明度を補正
		fAlpha = 0.0f;

		// プレイヤー自身の描画を停止
		CObject::SetEnableDraw(false);

		// 状態を設定
		m_state = STATE_NONE;	// 何もしない状態
	}

	// 透明度を設定
	SetAlpha(fAlpha);
}

//============================================================
//	セットアップ処理
//============================================================
void CPlayer::LoadSetup(void)
{
	// 変数を宣言
	CMotion::MotionInfo info;		// ポーズの代入用
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nLevel		= 0;	// レベルの代入用
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

	// 静的メンバ変数の情報をクリア
	memset(&m_aStatusInfo[0], 0, sizeof(m_aStatusInfo));	// ステータス情報

	// ファイルを読み込み形式で開く
	pFile = fopen(PLAYER_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// ステータスの設定
			if (strcmp(&aString[0], "STATUSSET") == 0)
			{ // 読み込んだ文字列が STATUSSET の場合

				do
				{ // 読み込んだ文字列が END_STATUSSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "LEVELSET") == 0)
					{ // 読み込んだ文字列が LEVELSET の場合

						do
						{ // 読み込んだ文字列が END_LEVELSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "LEVEL") == 0)
							{ // 読み込んだ文字列が LEVEL の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nLevel);		// レベルを読み込む
							}
							else if (strcmp(&aString[0], "LIFE") == 0)
							{ // 読み込んだ文字列が LIFE の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nLevel].nLife);		// 体力を読み込む
							}
							else if (strcmp(&aString[0], "MANA") == 0)
							{ // 読み込んだ文字列が MANA の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nLevel].nMana);		// マナを読み込む
							}
							else if (strcmp(&aString[0], "DASH") == 0)
							{ // 読み込んだ文字列が DASH の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nLevel].nDash);		// ダッシュを読み込む
							}
							else if (strcmp(&aString[0], "DEFENSE") == 0)
							{ // 読み込んだ文字列が DEFENSE の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nLevel].nDefense);	// 防御力を読み込む
							}
							else if (strcmp(&aString[0], "LEFTSIDE") == 0)
							{ // 読み込んだ文字列が LEFTSIDE の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nLevel].speed.fLeftside);	// 左側への加速量を読み込む
							}
							else if (strcmp(&aString[0], "INSIDE") == 0)
							{ // 読み込んだ文字列が INSIDE の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nLevel].speed.fInside);		// 内側への加速量を読み込む
							}
							else if (strcmp(&aString[0], "OUTSIDE") == 0)
							{ // 読み込んだ文字列が OUTSIDE の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nLevel].speed.fOutside);		// 外側への加速量を読み込む
							}
							else if (strcmp(&aString[0], "EXPBONUS") == 0)
							{ // 読み込んだ文字列が EXPBONUS の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nLevel].fMulExp);	// 経験値ボーナスを読み込む
							}
						} while (strcmp(&aString[0], "END_LEVELSET") != 0);	// 読み込んだ文字列が END_LEVELSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);		// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}

			// キャラクターの設定
			else if (strcmp(&aString[0], "CHARACTERSET") == 0)
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
