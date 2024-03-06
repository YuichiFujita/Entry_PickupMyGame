//============================================================
//
//	敵攻撃01処理 [enemyAttack01.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack01.h"
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
	const float	TELEPORT_POS_DIS	= 3600.0f;	// テレポート時のステージ中心位置から遠ざける距離
	const float	TELEPORT_POSY		= 800.0f;	// テレポート時のY座標
	const float FIND_RADIUS			= 250.0f;	// プレイヤー検知半径
	const int	ATK_MOTION_KEY		= 1;		// 攻撃生成キー
	const int	ATTACK_WAIT_FRAME	= 30;		// 攻撃後の硬直フレーム
	const int	NUM_THUNDER			= 4;		// 一回の攻撃で生成する雷の数 (中心にも生成するためプラス1される)
	const int	DIV_LENRAND			= 201;		// ランダム距離の剰余算の値
	const int	ADD_LENRAND			= 250;		// ランダム距離の加算の値
	const int	MAX_ATTACK			= 3;		// 攻撃回数
}

//************************************************************
//	子クラス [CEnemyAttack01] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyAttack01::CEnemyAttack01(CEnemyBossDragon *pBoss) : CEnemyAttack(pBoss),
	m_state				(STATE_INIT_TELEPORT),	// 状態
	m_nCounterState		(0),					// 状態管理カウンター
	m_nCounterNumAtk	(0)						// 攻撃回数カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyAttack01::~CEnemyAttack01()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyAttack01::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_INIT_TELEPORT;	// 状態
	m_nCounterState  = 0;			// 状態管理カウンター
	m_nCounterNumAtk = 0;			// 攻撃回数カウンター

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
void CEnemyAttack01::Uninit(void)
{
	// 敵攻撃の終了
	CEnemyAttack::Uninit();
}

//============================================================
//	更新処理
//============================================================
bool CEnemyAttack01::Update(void)
{
	// ポインタを宣言
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報
	CStage *pStage = CScene::GetStage();	// ステージの情報

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_INIT_TELEPORT:	// テレポートの初期化
	{
		D3DXVECTOR3 posEnemy = VEC3_ZERO;	// 敵の設定位置
		D3DXVECTOR3 rotEnemy = VEC3_ZERO;	// 敵の設定向き

		// ランダム方向上空に敵の位置を設定
		float fRandRot = useful::RandomRot();
		posEnemy.x = sinf(fRandRot) * TELEPORT_POS_DIS;
		posEnemy.y = TELEPORT_POSY;
		posEnemy.z = cosf(fRandRot) * TELEPORT_POS_DIS;

		// ステージ中心方向に敵の向きを設定
		D3DXVECTOR3 vec = posEnemy - pStage->GetStageLimit().center;
		rotEnemy.y = atan2f(vec.x, vec.z);

		// ボスをテレポートさせる
		pBoss->SetTeleport(posEnemy, rotEnemy, CEnemyBossDragon::MOTION_FLY_IDOL);

		// テレポート状態にする
		m_state = STATE_TELEPORT;

		// 処理は抜けずテレポートの状態更新に移行
	}
	case STATE_TELEPORT:	// テレポート
	{
		if (pBoss->GetAction() == CEnemyBossDragon::ACT_NONE)
		{ // 何も行動していない場合

			// 雷発射の初期化状態にする
			m_state = STATE_INIT_THUNDER;
		}

		break;
	}
	case STATE_INIT_THUNDER:	// 波動発射の初期化
	{
		// 空中攻撃の行動を設定
		pBoss->SetActFlyAttack();

		// 波動発射状態にする
		m_state = STATE_THUNDER;

		// 処理は抜けず波動発射の状態更新に移行
	}
	case STATE_THUNDER:	// 波動発射
	{
		if (pBoss->GetMotionType() != CEnemyBossDragon::MOTION_FLY_ATTACK)
		{ // 空中攻撃モーションではない場合

			// 待機状態にする
			m_state = STATE_WAIT;
		}

		if (pBoss->GetMotionKey() == ATK_MOTION_KEY && pBoss->GetMotionKeyCounter() == 0)
		{ // モーションが天を仰いだタイミングの場合

			CListManager<CPlayer> *pList = CPlayer::GetList();				// プレイヤーリスト
			if (pList == nullptr)		 { assert(false); return false; }	// リスト未使用
			if (pList->GetNumAll() != 1) { assert(false); return false; }	// プレイヤーが1人じゃない
			auto player = pList->GetList().front();							// プレイヤー情報

			D3DXVECTOR3 posPlayer = player->GetVec3Position();	// プレイヤーの位置
			D3DXVECTOR3 posThunder = VEC3_ZERO;					// 雷の位置

			// 雷攻撃をプレイヤー位置に生成
			CAttackThunder::Create(posPlayer, true);

			for (int nCntAttack = 0; nCntAttack < NUM_THUNDER; nCntAttack++)
			{ // 雷生成数分繰り返す

				// プレイヤーからランダム方向・距離に遠ざけた位置を設定
				float fRandRot = useful::RandomRot();	// ランダム向き
				posThunder.x = posPlayer.x + sinf(fRandRot) * (rand() % DIV_LENRAND + ADD_LENRAND);
				posThunder.z = posPlayer.z + cosf(fRandRot) * (rand() % DIV_LENRAND + ADD_LENRAND);

				// 雷攻撃をランダム位置に生成
				CAttackThunder::Create(posThunder, true);
			}

			// 攻撃回数を加算
			m_nCounterNumAtk++;
		}

		break;
	}
	case STATE_WAIT:	// 待機
	{
		// カウンターを加算
		m_nCounterState++;
		if (m_nCounterState >= ATTACK_WAIT_FRAME)
		{ // 待機が完了した場合

			// カウンターを初期化
			m_nCounterState = 0;

			if (m_nCounterNumAtk < MAX_ATTACK)
			{ // 攻撃回数が総数に到達していない場合

				// 雷発射の初期化状態にする
				m_state = STATE_INIT_THUNDER;
			}
			else
			{ // 攻撃回数が総数に到達した場合

				// 攻撃回数を初期化
				m_nCounterNumAtk = 0;

				// 中央テレポートの初期化状態にする
				m_state = STATE_CENTER_TELEPORT_INIT;
			}
		}

		break;
	}
	case STATE_CENTER_TELEPORT_INIT:	// 中央テレポートの初期化
	{
		CListManager<CPlayer> *pList = CPlayer::GetList();				// プレイヤーリスト
		if (pList == nullptr)		 { assert(false); return false; }	// リスト未使用
		if (pList->GetNumAll() != 1) { assert(false); return false; }	// プレイヤーが1人じゃない
		auto player = pList->GetList().front();							// プレイヤー情報

		D3DXVECTOR3 posPlayer = player->GetVec3Position();		// プレイヤーの位置
		D3DXVECTOR3 posEnemy  = pStage->GetStageLimit().center;	// 敵の位置
		D3DXVECTOR3 rotEnemy  = VEC3_ZERO;	// 敵の設定向き

		// プレイヤー方向を設定
		D3DXVECTOR3 vec = posEnemy - posPlayer;
		rotEnemy.y = atan2f(vec.x, vec.z);

		// ボスをテレポートさせる
		pBoss->SetTeleport(posEnemy, rotEnemy, CEnemyBossDragon::MOTION_IDOL);

		// 中央テレポート状態にする
		m_state = STATE_CENTER_TELEPORT;

		// 処理は抜けず中央テレポートの状態更新に移行

		break;
	}
	case STATE_CENTER_TELEPORT:	// 中央テレポート
	{
		if (pBoss->GetAction() == CEnemyBossDragon::ACT_NONE)
		{ // 何も行動していない場合

			// 終了状態にする
			m_state = STATE_END;
		}

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
void CEnemyAttack01::SetRandomArray
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
		pRandom->AddList(ATTACK_04, 1);	// 攻撃04(炎外周吐き出し)
	}
	else
	{ // プレイヤーとの距離が遠い場合

		// 遠距離攻撃を追加
		pRandom->AddList(ATTACK_02, 1);	// 攻撃02(雷外周向かい生成)
		pRandom->AddList(ATTACK_03, 1);	// 攻撃03(雷プレイヤー位置生成)
		pRandom->AddList(ATTACK_04, 1);	// 攻撃04(炎外周吐き出し)
	}
}
