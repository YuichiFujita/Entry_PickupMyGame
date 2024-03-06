//============================================================
//
//	敵攻撃02処理 [enemyAttack02.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack02.h"
#include "enemyBossDragon.h"
#include "manager.h"
#include "camera.h"
#include "player.h"
#include "stage.h"
#include "collision.h"
#include "attackThunder.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int	ATK_MOTION_KEY	= 2;		// 攻撃生成キー
	const int	INIT_WAIT_FRAME	= 26;		// 初期攻撃待機フレーム
	const int	NUM_ATTACK		= 8;		// 攻撃の生成数
	const int	ADD_WAIT_FRAME	= 4;		// 一攻撃ごとの攻撃待機フレーム加算量
	const int	MUL_NUM_CREATE	= 9;		// 一攻撃ごとの攻撃生成数の乗算量
	const float	CREATE_LENGTH	= 240.0f;	// 一攻撃ごとの距離の乗算量
	const float	FIND_RADIUS		= 250.0f;	// プレイヤー検知半径
	const float	TELEPORT_POSY	= 170.0f;	// テレポート後のY座標
}

//************************************************************
//	子クラス [CEnemyAttack02] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyAttack02::CEnemyAttack02(CEnemyBossDragon *pBoss) : CEnemyAttack(pBoss),
	m_state				(STATE_INIT_TELEPORT),	// 状態
	m_nCounterWait		(0),					// 余韻管理カウンター
	m_nCounterNumAtk	(0),					// 攻撃回数カウンター
	m_nWaitFrame		(0)						// 余韻フレーム数
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyAttack02::~CEnemyAttack02()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyAttack02::Init(void)
{
	// メンバ変数を初期化
	m_state				= STATE_INIT_TELEPORT;	// 状態
	m_nCounterWait		= 0;					// 余韻管理カウンター
	m_nCounterNumAtk	= 0;					// 攻撃回数カウンター
	m_nWaitFrame		= INIT_WAIT_FRAME;		// 余韻フレーム数

	// 敵攻撃の初期化
	if (FAILED(CEnemyAttack::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemyAttack02::Uninit(void)
{
	// 敵攻撃の終了
	CEnemyAttack::Uninit();
}

//============================================================
//	更新処理
//============================================================
bool CEnemyAttack02::Update(void)
{
	// ポインタを宣言
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_INIT_TELEPORT:	// テレポートの初期化
	{
		// テレポートの初期化
		InitTeleport();

		// 処理は抜けずテレポートの状態更新に移行
	}
	case STATE_TELEPORT:	// テレポート
	{
		if (pBoss->GetAction() == CEnemyBossDragon::ACT_NONE)
		{ // 何も行動していない場合

			// 攻撃待機の初期化状態にする
			m_state = STATE_INIT_WAIT;
		}

		break;
	}
	case STATE_INIT_WAIT:	// 攻撃待機の初期化
	{
		// 空中攻撃の行動を設定
		pBoss->SetActFlyAttack();

		// 攻撃待機状態にする
		m_state = STATE_WAIT;

		// 処理は抜けず攻撃待機の状態更新に移行
	}
	case STATE_WAIT:	// 攻撃待機
	{
		if (pBoss->GetMotionKey() == ATK_MOTION_KEY
		&&  pBoss->GetMotionKeyCounter() == 1)
		{ // 手を振り上げたタイミングの場合

			// 雷発射状態にする
			m_state = STATE_THUNDER;
		}

		break;
	}
	case STATE_THUNDER:	// 雷発射
	{
		// 雷発射の更新
		UpdateThunder();

		break;
	}
	case STATE_END:	// 終了
	{
		// 攻撃終了を返す
		return true;
	}
	default:	// 例外処理
		assert(false);
		break;
	}

	// 攻撃非終了を返す
	return false;
}

//============================================================
//	攻撃選択肢の要素設定処理
//============================================================
void CEnemyAttack02::SetRandomArray
(
	CRandom<EAttack> *pRandom,	// ランダム攻撃クラス
	EAttack /*oldAtk*/,			// 前回の攻撃
	int /*nSameAct*/			// 同じ行動の連続数
)
{
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	// XZ平面の円の当たり判定
	bool bHit = collision::Circle2D
	( // 引数
		pPlayer->GetVec3Position(),	// 判定位置
		pBoss->GetVec3Position(),	// 判定目標位置
		pPlayer->GetRadius(),		// 判定半径
		FIND_RADIUS					// 判定目標半径
	);
	if (bHit)
	{ // プレイヤーとの距離が近い場合

		// 近距離攻撃を追加
		pRandom->AddList(ATTACK_05, 5);	// 攻撃05(ひっかき攻撃)
		pRandom->AddList(ATTACK_06, 2);	// 攻撃06(しっぽ攻撃)

		// 遠距離攻撃を追加
		pRandom->AddList(ATTACK_00, 1);	// 攻撃00(地面殴り波動)
		pRandom->AddList(ATTACK_04, 1);	// 攻撃04(炎外周吐き出し)
	}
	else
	{ // プレイヤーとの距離が遠い場合

		// 遠距離攻撃を追加
		pRandom->AddList(ATTACK_00, 1);	// 攻撃00(地面殴り波動)
		pRandom->AddList(ATTACK_04, 1);	// 攻撃04(炎外周吐き出し)
	}
}

//============================================================
//	テレポートの初期化処理
//============================================================
void CEnemyAttack02::InitTeleport(void)
{
	// ポインタを宣言
	CStage  *pStage  = CScene::GetStage();	// ステージの情報
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	CStage::SStageLimit stageLimit = pStage->GetStageLimit();	// ステージ範囲情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();			// プレイヤーの位置
	D3DXVECTOR3 posEnemy = VEC3_ZERO;	// 敵の設定位置
	D3DXVECTOR3 rotEnemy = VEC3_ZERO;	// 敵の設定向き

	// 敵の位置を中央に設定
	posEnemy.x += stageLimit.center.x;
	posEnemy.y  = TELEPORT_POSY;
	posEnemy.z += stageLimit.center.z;

	// プレイヤー方向を向かせる
	D3DXVECTOR3 vecPlayer = posEnemy - posPlayer;
	rotEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

	// ボスをテレポートさせる
	pBoss->SetTeleport(posEnemy, rotEnemy, CEnemyBossDragon::MOTION_FLY_IDOL);

	// テレポート状態にする
	m_state = STATE_TELEPORT;
}

//============================================================
//	雷発射の更新処理
//============================================================
void CEnemyAttack02::UpdateThunder(void)
{
	// ポインタを宣言
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	// プレイヤーからボスへのベクトルを求める
	D3DXVECTOR3 vecPlayer = pBoss->GetVec3Position() - pPlayer->GetVec3Position();

	// 目標向きを取得
	D3DXVECTOR3 rotDestEnemy = pBoss->GetDestRotation();

	// 目標向きをプレイヤー方向にする
	rotDestEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

	// 目標向きを設定
	pBoss->SetDestRotation(rotDestEnemy);

	// カウンターを加算
	m_nCounterWait++;
	if (m_nCounterWait > m_nWaitFrame)
	{ // 余韻フレームが終了した場合

		// カウンターを初期化
		m_nCounterWait = 0;

		// 余韻の長さを延長
		m_nWaitFrame += ADD_WAIT_FRAME;

		int   nLoop = m_nCounterNumAtk * MUL_NUM_CREATE + 1;	// 攻撃生成数
		float fRotRate = (D3DX_PI * 2.0f) / nLoop;				// 攻撃方向の割合
		for (int nCntAttack = 0; nCntAttack < nLoop; nCntAttack++)
		{ // 雷生成数分繰り返す

			float fThunderRot = fRotRate * nCntAttack;	// 雷の生成方向
			D3DXVECTOR3 posThunder = VEC3_ZERO;			// 雷の生成位置
			posThunder.x = sinf(fThunderRot) * m_nCounterNumAtk * CREATE_LENGTH;
			posThunder.z = cosf(fThunderRot) * m_nCounterNumAtk * CREATE_LENGTH;

			// 雷攻撃をランダム位置に生成
			CAttackThunder::Create(posThunder, (nCntAttack == 0));
		}

		// 攻撃回数を加算
		m_nCounterNumAtk++;
		if (m_nCounterNumAtk >= NUM_ATTACK)
		{ // 攻撃がステージ端に到達した場合

			// 終了状態にする
			m_state = STATE_END;
		}
	}
}
