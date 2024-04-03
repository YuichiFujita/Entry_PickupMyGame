//============================================================
//
//	敵攻撃05処理 [enemyAttack05.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack05.h"
#include "enemyBossDragon.h"
#include "manager.h"
#include "sound.h"
#include "player.h"
#include "stage.h"
#include "multiModel.h"
#include "collision.h"
#include "collSphere.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const CEnemyBossDragon::EModel COLL_PARTS[] =	// 攻撃判定を持たせるパーツ
	{
		CEnemyBossDragon::MODEL_ARM_UL,	// 左上腕
		CEnemyBossDragon::MODEL_ARM_DL,	// 左下腕
		CEnemyBossDragon::MODEL_HAND_L,	// 左手
	};

	const float FIND_RADIUS		 = 250.0f;	// プレイヤー検知半径
	const float TELEPORT_POS_DIS = 125.0f;	// テレポート時のプレイヤー位置から遠ざける距離
	const float	SCALE_LEFT_ARM	 = 1.25f;	// 左腕の拡大率
	const float	SCALE_LEFT_HAND	 = 1.4f;	// 左手の拡大率
	const float	SCALE_COLL_PLUS	 = 2.25f;	// 判定の拡大率
	const int	SCALE_MOTION_KEY = 1;		// 左腕拡大キー
	const int	DMG_CLAW		 = 15;		// 爪のダメージ量
}

//************************************************************
//	子クラス [CEnemyAttack05] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyAttack05::CEnemyAttack05(CEnemyBossDragon *pBoss) : CEnemyAttack(pBoss),
	m_state	(STATE_INIT_TELEPORT)	// 状態
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyAttack05::~CEnemyAttack05()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyAttack05::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_INIT_TELEPORT;	// 状態

	// 敵攻撃の初期化
	if (FAILED(CEnemyAttack::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テレポートするかを設定
	SetEnableTeleport();

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemyAttack05::Uninit(void)
{
	// 敵攻撃の終了
	CEnemyAttack::Uninit();
}

//============================================================
//	更新処理
//============================================================
bool CEnemyAttack05::Update(void)
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

			// 攻撃の初期化状態にする
			m_state = STATE_INIT_ATTACK;
		}

		break;
	}
	case STATE_INIT_ATTACK:	// 攻撃の初期化
	{
		// ひっかき攻撃の行動を設定
		pBoss->SetActClawAttack();

		// 攻撃状態にする
		m_state = STATE_ATTACK;

		// 処理は抜けず攻撃の状態更新に移行
	}
	case STATE_ATTACK:	// 攻撃
	{
		// 攻撃の更新
		UpdateAttack();

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
void CEnemyAttack05::SetRandomArray
(
	CRandom<EAttack> *pRandom,	// ランダム攻撃クラス
	EAttack oldAtk,				// 前回の攻撃
	int nSameAct				// 同じ行動の連続数
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

		// 同一攻撃を追加
		if (oldAtk == ATTACK_05)
		{ // 前回攻撃が自分と同一の場合

			pRandom->AddList(ATTACK_05, 5 - nSameAct);	// 攻撃05(ひっかき攻撃)
		}

		// 近距離攻撃を追加
		pRandom->AddList(ATTACK_06, 2);	// 攻撃06(しっぽ攻撃)

		// 遠距離攻撃を追加
		pRandom->AddList(ATTACK_02, 1);	// 攻撃02(雷外周向かい生成)
		pRandom->AddList(ATTACK_03, 1);	// 攻撃03(雷プレイヤー位置生成)
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

//============================================================
//	テレポートするかの設定処理
//============================================================
void CEnemyAttack05::SetEnableTeleport(void)
{
	// ポインタを宣言
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

		// 攻撃の初期化状態にする
		m_state = STATE_INIT_ATTACK;
	}
	else
	{ // プレイヤーとの距離が遠い場合

		// テレポートの初期化状態にする
		m_state = STATE_INIT_TELEPORT;
	}
}

//============================================================
//	テレポートの初期化処理
//============================================================
void CEnemyAttack05::InitTeleport(void)
{
	// ポインタを宣言
	CStage  *pStage  = CScene::GetStage();	// ステージの情報
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	CStage::SStageLimit stageLimit = pStage->GetStageLimit();	// ステージ範囲情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();			// プレイヤーの位置
	D3DXVECTOR3 rotPlayer = pPlayer->GetVec3Rotation();			// プレイヤーの向き
	D3DXVECTOR3 posEnemy = VEC3_ZERO;	// 敵の設定位置
	D3DXVECTOR3 rotEnemy = VEC3_ZERO;	// 敵の設定向き

	// プレイヤーの背後に位置を設定
	posEnemy.x += posPlayer.x + sinf(rotPlayer.y) * TELEPORT_POS_DIS;
	posEnemy.z += posPlayer.z + cosf(rotPlayer.y) * TELEPORT_POS_DIS;

	// プレイヤー方向を向かせる
	D3DXVECTOR3 vecPlayer = posEnemy - posPlayer;
	rotEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

	// ボスをテレポートさせる
	pBoss->SetTeleport(posEnemy, rotEnemy);

	// テレポート状態にする
	m_state = STATE_TELEPORT;
}

//============================================================
//	攻撃の更新処理
//============================================================
void CEnemyAttack05::UpdateAttack(void)
{
	// ポインタを宣言
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	// 攻撃の終了判定
	if (pBoss->GetMotionType() != CEnemyBossDragon::MOTION_CLAW_ATTACK)
	{ // ひっかきモーションが終わった場合

		// 手のサイズを元に戻す
		pBoss->GetMultiModel(CEnemyBossDragon::MODEL_ARM_UL)->SetVec3Scaling(VEC3_ALL(1.0f));
		pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_L)->SetVec3Scaling(VEC3_ALL(1.0f));

		// 終了状態にする
		m_state = STATE_END;

		// 処理を抜ける
		return;
	}

	// プレイヤー方向を向かせる
	if (pBoss->GetMotionKey() < SCALE_MOTION_KEY)
	{ // 手を振り降ろす前のタイミングの場合

		// プレイヤーからボスへのベクトルを求める
		D3DXVECTOR3 vecPlayer = pBoss->GetVec3Position() - pPlayer->GetVec3Position();

		// 目標向きを取得
		D3DXVECTOR3 rotDestEnemy = pBoss->GetDestRotation();

		// 目標向きをプレイヤー方向にする
		rotDestEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

		// 目標向きを設定
		pBoss->SetDestRotation(rotDestEnemy);
	}

	// 手の巨大化判定
	if (pBoss->GetMotionKey() == SCALE_MOTION_KEY && pBoss->GetMotionKeyCounter() == 0)
	{ // 手を振り降ろし始めたタイミングの場合

		// 手のサイズを拡大
		pBoss->GetMultiModel(CEnemyBossDragon::MODEL_ARM_UL)->SetVec3Scaling(VEC3_ALL(SCALE_LEFT_ARM));
		pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_L)->SetVec3Scaling(VEC3_ALL(SCALE_LEFT_HAND));

		// ひっかき音の再生
		PLAY_SOUND(CSound::LABEL_SE_CLAW);
	}

	// 攻撃判定
	if (pBoss->IsLeftWeaponCollision())
	{ // 攻撃判定がONの場合

		// プレイヤー判定位置を求める
		D3DXVECTOR3 heightPlayer  = D3DXVECTOR3(0.0f, pPlayer->GetHeight(), 0.0f);		// プレイヤーの縦幅
		D3DXVECTOR3 posCentPlayer = pPlayer->GetVec3Position() + (heightPlayer * 0.5f);	// プレイヤーの中心位置

		for (int nCntColl = 0; nCntColl < NUM_ARRAY(COLL_PARTS); nCntColl++)
		{ // 判定をとるパーツ数分繰り返す

			// パーツ拡大率を取得
			D3DXMATRIX  mtxParts = pBoss->GetMultiModel(COLL_PARTS[nCntColl])->GetMtxWorld();	// パーツマトリックス
			D3DXVECTOR3 scaleParts = useful::GetMatrixScaling(mtxParts);	// パーツ拡大率

			// 判定配列を取得
			CCollSphere *pColl = pBoss->GetCollision(COLL_PARTS[nCntColl]);	// 円判定情報
			std::vector<CCollSphere::SInfo> vector = pColl->GetVector();	// 円判定配列

			// 判定情報の要素番号
			int nCntArray = 0;

			for (auto& rColl : vector)
			{ // 配列の要素数分繰り返す

				// 敵の判定位置を計算
				D3DXVECTOR3 posCollEnemy = pColl->CalcWorldPosition(nCntArray);

				// 要素番号を次に
				nCntArray++;

				// 攻撃の当たり判定
				bool bHit = collision::Circle3D
				( // 引数
					posCollEnemy,	// 判定位置
					posCentPlayer,	// 判定目標位置
					rColl.fRadius * scaleParts.x * SCALE_COLL_PLUS,	// 判定半径
					pPlayer->GetRadius()							// 判定目標半径
				);
				if (bHit)
				{ // 攻撃が当たった場合

					// プレイヤーのヒット処理
					D3DXVECTOR3 vecKnock = posCentPlayer - pBoss->GetVec3Position();
					pPlayer->HitKnockBack(DMG_CLAW, vecKnock);
				}
			}
		}
	}
}
