//============================================================
//
//	敵攻撃00処理 [enemyAttack00.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack00.h"
#include "enemyBossDragon.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"
#include "stage.h"
#include "player.h"
#include "multiModel.h"
#include "impact.h"
#include "collision.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int	ATK_1ST_MOTION_KEY	= 3;		// 攻撃生成キー:一段階目
	const int	ATK_2ND_MOTION_KEY	= 7;		// 攻撃生成キー:二段階目
	const int	ATK_3RD_MOTION_KEY	= 13;		// 攻撃生成キー:三段階目
	const int	ATTACK_WAIT_FRAME	= 0;		// 攻撃後の硬直フレーム
	const float	FIND_RADIUS			= 500.0f;	// プレイヤー検知半径

	namespace impact
	{
		namespace little
		{
			const CCamera::SSwing	PUNCH_SWING	= CCamera::SSwing(14.0f, 1.5f, 0.5f);	// 地面殴り時のカメラ揺れ (小)
			const CWave::SGrow		GROW		= CWave::SGrow(5.5f, 0.0f, 0.0f);		// 成長量
			const CWave::SGrow		ADDGROW		= CWave::SGrow(0.05f, 0.0f, 0.0f);		// 成長加速量

			const D3DXCOLOR COL = XCOL_YELLOW;	// 色
			const float	HOLE_RADIUS	= 0.0f;		// 穴の半径
			const float	THICKNESS	= 0.0f;		// 太さ
			const float	OUTER_PLUSY	= 80.0f;	// 外周のY座標加算量
			const float	MAX_RADIUS	= 1400.0f;	// 半径の最大成長量
		}

		namespace big
		{
			const CCamera::SSwing	PUNCH_SWING	= CCamera::SSwing(34.0f, 1.5f, 1.5f);	// 地面殴り時のカメラ揺れ (大)
			const CWave::SGrow		GROW		= CWave::SGrow(6.5f, 0.0f, 0.0f);		// 成長量
			const CWave::SGrow		ADDGROW		= CWave::SGrow(0.05f, 0.0f, 0.0f);		// 成長加速量

			const D3DXCOLOR COL = XCOL_YELLOW;	// 色
			const float	HOLE_RADIUS	= 0.0f;		// 穴の半径
			const float	THICKNESS	= 0.0f;		// 太さ
			const float	OUTER_PLUSY	= 140.0f;	// 外周のY座標加算量
			const float	MAX_RADIUS	= 1600.0f;	// 半径の最大成長量
		}
	}
}

//************************************************************
//	子クラス [CEnemyAttack00] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyAttack00::CEnemyAttack00(CEnemyBossDragon *pBoss) : CEnemyAttack(pBoss),
	m_state			(STATE_INIT_TELEPORT),	// 状態
	m_nCounterState	(0)						// 状態管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyAttack00::~CEnemyAttack00()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyAttack00::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_INIT_TELEPORT;	// 状態
	m_nCounterState = 0;			// 状態管理カウンター

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
void CEnemyAttack00::Uninit(void)
{
	// 敵攻撃の終了
	CEnemyAttack::Uninit();
}

//============================================================
//	更新処理
//============================================================
bool CEnemyAttack00::Update(void)
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

			// 波動発射の初期化状態にする
			m_state = STATE_INIT_WAVE;
		}

		break;
	}
	case STATE_INIT_WAVE:	// 波動発射の初期化
	{
		// 地面殴りの行動を設定
		pBoss->SetActPunchGround();

		// 波動発射状態にする
		m_state = STATE_WAVE;

		// 処理は抜けず波動発射の状態更新に移行
	}
	case STATE_WAVE:	// 波動発射
	{
		// 波動発射の更新
		UpdateWave();

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
void CEnemyAttack00::SetRandomArray
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
//	テレポートの初期化処理
//============================================================
void CEnemyAttack00::InitTeleport(void)
{
	// ポインタを宣言
	CStage  *pStage  = CScene::GetStage();	// ステージの情報
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	CStage::SStageLimit stageLimit = pStage->GetStageLimit();	// ステージ範囲情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();			// プレイヤーの位置
	D3DXVECTOR3 posEnemy = VEC3_ZERO;	// 敵の設定位置
	D3DXVECTOR3 rotEnemy = VEC3_ZERO;	// 敵の設定向き

	// ステージ中央との当たり判定
	bool bHit = collision::Circle2D
	( // 引数
		posPlayer,					// 判定位置
		stageLimit.center,			// 判定目標位置
		pPlayer->GetRadius(),		// 判定半径
		stageLimit.fRadius * 0.5f	// 判定目標半径
	);
	if (bHit)
	{ // ステージの中央にいた場合

		// 中心からプレイヤーへの向きを求める
		D3DXVECTOR3 vec = stageLimit.center - posPlayer;
		float fRot = atan2f(vec.x, vec.z);

		// 最大限遠ざけた位置を設定
		posEnemy.x += stageLimit.center.x + sinf(fRot) * stageLimit.fRadius;
		posEnemy.z += stageLimit.center.z + cosf(fRot) * stageLimit.fRadius;
	}
	else
	{ // ステージの外周にいた場合

		// 中央位置を設定
		posEnemy.x += stageLimit.center.x;
		posEnemy.z += stageLimit.center.z;
	}

	// プレイヤー方向を向かせる
	D3DXVECTOR3 vecPlayer = posEnemy - posPlayer;
	rotEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

	// ボスをテレポートさせる
	pBoss->SetTeleport(posEnemy, rotEnemy);

	// テレポート状態にする
	m_state = STATE_TELEPORT;
}

//============================================================
//	波動発射の更新処理
//============================================================
void CEnemyAttack00::UpdateWave(void)
{
	// ポインタを宣言
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	if (pBoss->GetMotionType() != CEnemyBossDragon::MOTION_PUNCH_GROUND)
	{ // 地面殴りモーションではない場合

		// 待機状態にする
		m_state = STATE_WAIT;
	}

	// マトリックスがキーの開始位置になっていない場合抜ける
	if (pBoss->GetMotionKeyCounter() != 1) { return; }

	switch (pBoss->GetMotionKey())
	{ // モーションキーごとの処理
	case ATK_1ST_MOTION_KEY:	// 殴り一回目：右手
	{
		D3DXVECTOR3 posHand;	// 手のワールド座標

		// 左手のマトリックスを取得
		D3DXMATRIX mtxHandL = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_L)->GetMtxWorld();

		// 左手のワールド座標を設定
		posHand = useful::GetMatrixPosition(mtxHandL);

		// Y座標は足元にする
		posHand.y = pBoss->GetVec3Position().y;

		// カメラ揺れを設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, impact::little::PUNCH_SWING);

		// 衝撃波の生成
		CImpact::Create
		( // 引数
			CWave::TEXTURE_IMPACT,			// 種類
			posHand,						// 位置
			impact::little::COL,			// 色
			impact::little::GROW,			// 成長量
			impact::little::ADDGROW,		// 成長加速量
			impact::little::HOLE_RADIUS,	// 穴の半径
			impact::little::THICKNESS,		// 太さ
			impact::little::OUTER_PLUSY,	// 外周のY座標加算量
			impact::little::MAX_RADIUS		// 半径の最大成長量
		);

		// パンチ音の再生
		PLAY_SOUND(CSound::LABEL_SE_PUNCH);

		break;
	}
	case ATK_2ND_MOTION_KEY:	// 殴り二回目：左手
	{
		D3DXVECTOR3 posHand;	// 手のワールド座標

		// 右手のマトリックスを取得
		D3DXMATRIX mtxHandR = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_R)->GetMtxWorld();

		// 右手のワールド座標を設定
		posHand = useful::GetMatrixPosition(mtxHandR);

		// Y座標は足元にする
		posHand.y = pBoss->GetVec3Position().y;

		// カメラ揺れを設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, impact::little::PUNCH_SWING);

		// 衝撃波の生成
		CImpact::Create
		( // 引数
			CWave::TEXTURE_IMPACT,			// 種類
			posHand,						// 位置
			impact::little::COL,			// 色
			impact::little::GROW,			// 成長量
			impact::little::ADDGROW,		// 成長加速量
			impact::little::HOLE_RADIUS,	// 穴の半径
			impact::little::THICKNESS,		// 太さ
			impact::little::OUTER_PLUSY,	// 外周のY座標加算量
			impact::little::MAX_RADIUS		// 半径の最大成長量
		);

		// パンチ音の再生
		PLAY_SOUND(CSound::LABEL_SE_PUNCH);

		break;
	}
	case ATK_3RD_MOTION_KEY:	// 殴り三回目：両手
	{
		D3DXMATRIX  mtxHandL = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_L)->GetMtxWorld();	// 左手のマトリックス
		D3DXMATRIX  mtxHandR = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_HAND_R)->GetMtxWorld();	// 右手のマトリックス
		D3DXVECTOR3 posHandL = useful::GetMatrixPosition(mtxHandL);	// 左手のワールド座標
		D3DXVECTOR3 posHandR = useful::GetMatrixPosition(mtxHandR);	// 右手のワールド座標
		D3DXVECTOR3 posHand;	// 手のワールド座標

		// 両手の中間ワールド座標を設定
		D3DXVec3Lerp(&posHand, &posHandL, &posHandR, 0.5f);

		// Y座標は足元にする
		posHand.y = pBoss->GetVec3Position().y;

		// カメラ揺れを設定
		GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, impact::big::PUNCH_SWING);

		// 衝撃波の生成
		CImpact::Create
		( // 引数
			CWave::TEXTURE_IMPACT,		// 種類
			posHand,					// 位置
			impact::big::COL,			// 色
			impact::big::GROW,			// 成長量
			impact::big::ADDGROW,		// 成長加速量
			impact::big::HOLE_RADIUS,	// 穴の半径
			impact::big::THICKNESS,		// 太さ
			impact::big::OUTER_PLUSY,	// 外周のY座標加算量
			impact::big::MAX_RADIUS		// 半径の最大成長量
		);

		// パンチ音の再生
		PLAY_SOUND(CSound::LABEL_SE_PUNCH);

		break;
	}
	default:
		break;
	}
}
