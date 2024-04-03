//============================================================
//
//	敵攻撃04処理 [enemyAttack04.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyAttack04.h"
#include "enemyBossDragon.h"
#include "manager.h"
#include "sound.h"
#include "player.h"
#include "stage.h"
#include "multiModel.h"
#include "collision.h"
#include "attackMoveFire.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int	ATK_MOTION_KEY	= 3;		// 攻撃生成キー
	const int	NUM_FIRE		= 8;		// 炎の生成数
	const int	NUM_ATTACK		= 3;		// 攻撃の生成数
	const float FIND_RADIUS		= 250.0f;	// プレイヤー検知半径
	const int	BACK_ATTACK_END	= 2;		// 攻撃を辞められる攻撃数
	const float BACK_RADIUS		= 250.0f;	// 視界範囲
	const float BACK_ANGLE		= HALF_PI;	// 視野角

	namespace fire
	{
		const float	MOVE	= 16.5f;	// 移動量
		const float	ADDMOVE	= 0.09f;	// 加速量
		const int	LIFE	= 100;		// 寿命
	}
}

//************************************************************
//	子クラス [CEnemyAttack04] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyAttack04::CEnemyAttack04(CEnemyBossDragon *pBoss) : CEnemyAttack(pBoss),
	m_state				(STATE_INIT_TELEPORT),	// 状態
	m_nCounterWait		(0),					// 余韻管理カウンター
	m_nCounterNumAtk	(0)						// 攻撃回数カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyAttack04::~CEnemyAttack04()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyAttack04::Init(void)
{
	// メンバ変数を初期化
	m_state				= STATE_INIT_TELEPORT;	// 状態
	m_nCounterWait		= 0;					// 余韻管理カウンター
	m_nCounterNumAtk	= 0;					// 攻撃回数カウンター

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
void CEnemyAttack04::Uninit(void)
{
	// 敵攻撃の終了
	CEnemyAttack::Uninit();
}

//============================================================
//	更新処理
//============================================================
bool CEnemyAttack04::Update(void)
{
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

			// 炎発射の初期化状態にする
			m_state = STATE_INIT_FIRE;
		}

		break;
	}
	case STATE_INIT_FIRE:	// 炎発射の初期化
	{
		// 炎攻撃の行動を設定
		pBoss->SetActFireAttack();

		// 炎発射状態にする
		m_state = STATE_FIRE;

		// 処理は抜けず炎発射の状態更新に移行
	}
	case STATE_FIRE:	// 炎発射
	{
		// 炎発射の更新
		UpdateFire();

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
void CEnemyAttack04::SetRandomArray
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
		pRandom->AddList(ATTACK_02, 1);	// 攻撃02(雷外周向かい生成)
		pRandom->AddList(ATTACK_03, 1);	// 攻撃03(雷プレイヤー位置生成)
	}
	else
	{ // プレイヤーとの距離が遠い場合

		// 遠距離攻撃を追加
		pRandom->AddList(ATTACK_00, 1);	// 攻撃00(地面殴り波動)
		pRandom->AddList(ATTACK_02, 1);	// 攻撃02(雷外周向かい生成)
		pRandom->AddList(ATTACK_03, 1);	// 攻撃03(雷プレイヤー位置生成)
	}
}

//============================================================
//	テレポートの初期化処理
//============================================================
void CEnemyAttack04::InitTeleport(void)
{
	// ポインタを宣言
	CStage  *pStage  = CScene::GetStage();	// ステージの情報
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	CStage::SStageLimit stageLimit = pStage->GetStageLimit();	// ステージ範囲情報
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();			// プレイヤーの位置
	D3DXVECTOR3 posEnemy = VEC3_ZERO;	// 敵の設定位置
	D3DXVECTOR3 rotEnemy = VEC3_ZERO;	// 敵の設定向き

	// 中心からプレイヤーへの向きを求める
	D3DXVECTOR3 vec = stageLimit.center - posPlayer;
	float fRot = atan2f(vec.x, vec.z);

	// 最大限遠ざけた位置を設定
	posEnemy.x += stageLimit.center.x + sinf(fRot) * stageLimit.fRadius * 0.5f;
	posEnemy.z += stageLimit.center.z + cosf(fRot) * stageLimit.fRadius * 0.5f;

	// プレイヤー方向を向かせる
	D3DXVECTOR3 vecPlayer = posEnemy - posPlayer;
	rotEnemy.y = atan2f(vecPlayer.x, vecPlayer.z);

	// ボスをテレポートさせる
	pBoss->SetTeleport(posEnemy, rotEnemy);

	// テレポート状態にする
	m_state = STATE_TELEPORT;
}

//============================================================
//	炎発射の更新処理
//============================================================
void CEnemyAttack04::UpdateFire(void)
{
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報
	if (pBoss->GetMotionKey() == ATK_MOTION_KEY
	&&  pBoss->GetMotionKeyCounter() == 1)
	{ // 口を開けたタイミングの場合

		// 炎の生成
		CreateFire();

		// 攻撃回数を加算
		m_nCounterNumAtk++;
		if (m_nCounterNumAtk >= NUM_ATTACK)
		{ // 攻撃が生成総数に到達した場合

			// 終了状態にする
			m_state = STATE_END;
		}
		else
		{ // 攻撃がまだ続く場合

			if (m_nCounterNumAtk >= BACK_ATTACK_END
			&&  IsBackPlayer())
			{ // プレイヤーがボスの背後にいる場合

				// 終了状態にする
				m_state = STATE_END;
			}
			else
			{ // プレイヤーがボスの背後にいない場合

				// 炎発射の初期化状態にする
				m_state = STATE_INIT_FIRE;
			}
		}
	}
}

//============================================================
//	炎の生成処理
//============================================================
void CEnemyAttack04::CreateFire(void)
{
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報
	CMultiModel	*pObjJaw = pBoss->GetMultiModel(CEnemyBossDragon::MODEL_JAW);	// 顎モデル
	D3DXMATRIX	mtxJaw	 = pObjJaw->GetMtxWorld();				// 顎マトリックス
	D3DXVECTOR3	posJaw	 = useful::GetMatrixPosition(mtxJaw);	// 顎ワールド座標

	int   nNumFire	= NUM_FIRE + m_nCounterNumAtk * 3;	// 攻撃生成数
	float fSubRad	= D3DX_PI * 0.25f;					// 攻撃方向を狭める半径
	float fRotRate	= (D3DX_PI - fSubRad * 2.0f) / (nNumFire - 1);				// 攻撃方向の割合
	float fStartRot	= pBoss->GetVec3Rotation().y + (D3DX_PI * 0.5f) + fSubRad;	// 攻撃生成開始向き

	for (int nCntAttack = 0; nCntAttack < nNumFire; nCntAttack++)
	{ // 攻撃の生成数分繰り返す

		float fMoveRot = fStartRot + fRotRate * nCntAttack;	// 炎の生成方向
		D3DXVECTOR3 vecMove = D3DXVECTOR3(sinf(fMoveRot), 0.0f, cosf(fMoveRot));	// 炎の移動方向

		// 炎攻撃の生成
		CAttackMoveFire::Create
		( // 引数
			posJaw,			// 生成位置
			vecMove,		// 移動方向
			fire::MOVE,		// 移動量
			fire::ADDMOVE,	// 加速量
			fire::LIFE		// 寿命
		);
	}

	// 炎音の再生
	PLAY_SOUND(CSound::LABEL_SE_FIRE);
}

//============================================================
//	プレイヤーがボスの背後にいるかの取得処理
//============================================================
bool CEnemyAttack04::IsBackPlayer(void)
{
	// ポインタを宣言
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤーの情報
	CEnemyBossDragon *pBoss = GetBoss();	// ボスの情報

	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();	// プレイヤーの位置
	D3DXVECTOR3 posBoss = pBoss->GetVec3Position();		// ボスの位置
	D3DXVECTOR3 rotBoss = pBoss->GetVec3Rotation();		// ボスの向き

	// XZ平面の扇形の当たり判定
	bool bHit = collision::Sector
	( // 引数
		posBoss,		// 判定位置
		posPlayer,		// 判定目標位置
		rotBoss.y,		// 判定向き
		BACK_RADIUS,	// 視界範囲
		BACK_ANGLE		// 視野角
	);

	// 判定情報を返す
	return bHit;
}
