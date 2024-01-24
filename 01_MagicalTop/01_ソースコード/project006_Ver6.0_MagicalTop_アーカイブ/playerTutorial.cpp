//============================================================
//
//	チュートリアルプレイヤー処理 [playerTutorial.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "playerTutorial.h"
#include "sceneTutorial.h"
#include "tutorialManager.h"
#include "manager.h"
#include "camera.h"
#include "sound.h"
#include "collision.h"
#include "stage.h"
#include "enemy.h"

//************************************************************
//	子クラス [CPlayerTutorial] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayerTutorial::CPlayerTutorial()
{
	// メンバ変数をクリア
	m_nOldNumEnemy = 0;	// 過去の敵の総数
}

//============================================================
//	デストラクタ
//============================================================
CPlayerTutorial::~CPlayerTutorial()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayerTutorial::Init(void)
{
	// メンバ変数を初期化
	m_nOldNumEnemy = CTutorialManager::GetNextLessonCounter(CTutorialManager::LESSON_06);	// 過去の敵の総数

	// プレイヤーの初期化
	if (FAILED(CPlayer::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// 経験値の描画を停止
	SetEnableDrawExp(false);

	// ダッシュの描画を停止
	SetEnableDrawDash(false);

	// マナの描画を停止
	SetEnableDrawMana(false);

	// マナの回復をできないように変更
	SetEnableHealMana(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayerTutorial::Uninit(void)
{
	// プレイヤーの終了
	CPlayer::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayerTutorial::Update(void)
{
	// 変数を宣言
	int nCurrentMotion = NONE_IDX;	// 現在のモーション
	int nNumEnemy = 0;	// 現在の敵の総数
	int nOldMana = 0;	// 回復前のマナ

	if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_06
	&&  CSceneTutorial::GetTutorialManager()->GetState()  == CTutorialManager::STATE_PROGRESSION)
	{ // チュートリアルが進行状態且つ、レッスン06：敵への攻撃の場合

		// 現在の敵の総数を取得
		nNumEnemy = CEnemy::GetNumAll();

		if (nNumEnemy < m_nOldNumEnemy)
		{ // 敵を倒せている場合

			for (int nCntPlayer = 0; nCntPlayer < m_nOldNumEnemy - nNumEnemy; nCntPlayer++)
			{ // 同フレームに倒した敵の数分繰り返す

				// レッスンカウンター加算
				CSceneTutorial::GetTutorialManager()->AddLessonCounter();
			}
		}

		// 過去の敵の総数を更新
		m_nOldNumEnemy = nNumEnemy;
	}

	// 過去位置の更新
	UpdateOldPosition();

	// ターゲット距離の更新
	UpdateDisTarget();

	switch (GetState())
	{ // 状態ごとの処理
	case STATE_NONE:		// 何もしない状態

		// 無し

		break;

	case STATE_FADEOUT:		// フェードアウト状態

		// フェードアウト状態時の更新
		UpdateFadeOut();

		break;

	case STATE_NORMAL:		// 通常状態

		// 通常状態時の更新
		nCurrentMotion = UpdateNormal();

		break;

	case STATE_DAMAGE:		// ダメージ状態

		// ダメージ状態時の更新
		nCurrentMotion = UpdateDamage();

		break;

	case STATE_INVULN:		// 無敵状態

		// 無敵状態時の更新
		nCurrentMotion = UpdateInvuln();

		break;

	case STATE_BLOW_AWAY:	// 吹っ飛び状態

		// 吹っ飛び状態時の更新
		UpdateBlowAway();

		break;

	case STATE_VORTEX:		// 渦巻きこまれ状態

		// 渦巻きこまれ状態時の更新
		UpdateVortex();

		if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_01)
		{ // レッスン01：吸い込まれるの場合

			// レッスンカウンター加算
			CSceneTutorial::GetTutorialManager()->AddLessonCounter();
		}

		break;

	case STATE_FADEIN:		// フェードイン状態

		// フェードイン状態時の更新
		UpdateFadeIn();

		break;

	case STATE_DEATH:		// 死亡状態

		// 現在のモーションに死亡モーションを設定
		nCurrentMotion = CPlayer::MOTION_DEATH;

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 回復前のマナを設定
	nOldMana = GetMana();

	// プレイヤーの更新
	CPlayer::Update();

	if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_05)
	{ // レッスン05：マナ回復の場合

		if (GetMana() > nOldMana)
		{ // マナが回復できている場合

			// レッスンカウンター加算
			CSceneTutorial::GetTutorialManager()->AddLessonCounter();
		}
	}

	// モーション・オブジェクトキャラクターの更新
	UpdateMotion(nCurrentMotion);
}

//============================================================
//	吹っ飛びヒット処理
//============================================================
void CPlayerTutorial::HitBlowAway
(
	const D3DXVECTOR3& rPlayerPos,	// プレイヤー位置
	const D3DXVECTOR3& rHitObjPos,	// オブジェクト位置
	const int nDmg					// ダメージ量
)
{
	// 変数を宣言
	D3DXVECTOR3 vecAway = VEC3_ZERO;	// 吹っ飛びベクトル

	if (GetState() == STATE_NORMAL)
	{ // 通常状態の場合

		// 吹っ飛びベクトルを求める
		vecAway = rPlayerPos - rHitObjPos;
		vecAway.y = 0.0f;						// 縦ベクトルを削除
		D3DXVec3Normalize(&vecAway, &vecAway);	// ベクトルを正規化

		// 吹っ飛びの設定
		SetBlowAway(vecAway);

		// 状態を変更
		SetState(STATE_BLOW_AWAY);	// 吹っ飛び状態

		// 吹っ飛びモーションに移行
		SetMotion(MOTION_BLOW_AWAY);
	}
}

//============================================================
//	渦巻きこまれヒット処理
//============================================================
void CPlayerTutorial::HitVortex
(
	D3DXVECTOR3& rPlayerPos,	// プレイヤー位置
	const D3DXVECTOR3& rHitPos,	// 当たり判定位置
	const int nDmg				// ダメージ量
)
{
	// バリアの衝突判定
	collision::CirclePillar
	( // 引数
		rPlayerPos,										// 判定位置
		CScene::GetStage()->GetStageBarrierPosition(),	// 判定目標位置
		GetRadius(),									// 判定半径
		CScene::GetStage()->GetStageBarrier().fRadius	// 判定目標半径
	);

	if (GetState() == STATE_NORMAL)
	{ // 通常状態の場合

		// 渦巻きこまれの設定処理
		SetVortex(rPlayerPos, rHitPos);

		// カメラ更新をOFFにする
		CManager::GetCamera()->SetEnableUpdate(false);

		// 状態を設定
		SetState(STATE_VORTEX);	// 渦巻きこまれ状態

		// 吹っ飛びモーションに移行
		SetMotion(MOTION_BLOW_AWAY);

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_HURRICANE);	// ハリケーン巻き込まれ音
	}
}

//============================================================
//	表示の設定処理
//============================================================
void CPlayerTutorial::SetDisp(const bool bDisp)
{
	if (bDisp)
	{ // 表示する状態の場合

		// 状態を設定
		SetState(STATE_FADEOUT);	// フェードアウト状態

		// 描画する設定にする
		SetEnableDraw(true);
	}
	else
	{ // 表示しない状態の場合

		// 状態を設定
		SetState(STATE_FADEIN);		// フェードイン状態

		// 描画しない設定にする
		SetEnableDraw(false);
		CObject::SetEnableDraw(true);	// プレイヤーの描画はONにする
	}
}

//============================================================
//	描画状況の設定処理
//============================================================
void CPlayerTutorial::SetEnableDraw(const bool bDraw)
{
	// 変数を宣言
	bool bMana = (CSceneTutorial::GetTutorialManager()->GetLesson() >= CTutorialManager::LESSON_04) ? bDraw : false;	// マナの描画状況
	bool bDash = (CSceneTutorial::GetTutorialManager()->GetLesson() >= CTutorialManager::LESSON_03) ? bDraw : false;	// ダッシュの描画状況

	// 引数の描画状況を設定
	CPlayer::SetEnableDraw(bDraw);	// 自身
	SetEnableDrawLife(bDraw);		// 体力

	// レッスンに応じた描画状況を設定
	SetEnableDrawMana(bMana);	// マナ
	SetEnableDrawDash(bDash);	// ダッシュ
}

//============================================================
//	通常状態時の更新処理
//============================================================
CPlayer::MOTION CPlayerTutorial::UpdateNormal(void)
{
	// 変数を宣言
	MOTION currentMotion = MOTION_MOVE;		// 現在のモーション
	D3DXVECTOR3 posPlayer = GetPosition();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetRotation();	// プレイヤー向き
	float fRadiusPlayer = GetRadius();		// プレイヤー半径

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報

	if (USED(pStage))
	{ // ステージが使用されている場合

		// 移動操作
		currentMotion = UpdateMove();

		// 向き更新
		UpdateRotation(rotPlayer);

		// 位置更新
		UpdatePosition(posPlayer);

		// ステージ範囲外の補正
		pStage->LimitPosition(posPlayer, fRadiusPlayer);

		// 着地判定
		UpdateLanding(posPlayer);

		// 魔法発射操作
		if (CSceneTutorial::GetTutorialManager()->GetLesson() > CTutorialManager::LESSON_03)
		{ // レッスン03：左右加速を終了している場合

			// 変数を宣言
			bool bShot = false;	// 魔法の発射状況

			// 魔法発射操作
			currentMotion = ControlShotMagic(currentMotion, &bShot);

			if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_04)
			{ // レッスン04：攻撃の場合

				if (bShot)
				{ // 攻撃操作が行われていた場合

					// レッスンカウンター加算
					CSceneTutorial::GetTutorialManager()->AddLessonCounter();
				}
			}
		}

		// 当たり判定
		CollisionTarget(posPlayer);	// ターゲット
		CollisionEnemy(posPlayer);	// 敵

		// バリアとの当たり判定
		if (pStage->CollisionBarrier(posPlayer, fRadiusPlayer))
		{ // 当たっていた場合

			// 渦巻きこまれヒット
			HitVortex(posPlayer, pStage->GetStageBarrierPosition(), 0);
		}

		// 位置を更新
		SetPosition(posPlayer);

		// 向きを更新
		SetRotation(rotPlayer);
	}
	else { assert(false); }	// 非使用中

	// 現在のモーションを返す
	return currentMotion;
}

//============================================================
//	移動量・目標向きの更新処理
//============================================================
CPlayer::MOTION CPlayerTutorial::UpdateMove(void)
{
	// 変数を宣言
	MOTION currentMotion = MOTION_MOVE;	// 現在のモーション
	D3DXVECTOR3 vecTarg = VEC3_ZERO;	// ターゲット逆方向ベクトル
	D3DXVECTOR3 vecSide = VEC3_ZERO;	// ターゲット横方向ベクトル
	bool bTargMove = false;	// ターゲット逆方向への操作状況
	bool bSideMove = false;	// ターゲット横方向への操作状況

	// 吸い込みの更新
	UpdateAbsorb(vecTarg, vecSide);

	if (CSceneTutorial::GetTutorialManager()->GetLesson() > CTutorialManager::LESSON_01)
	{ // レッスン01：吸い込まれるを終了している場合

		// ターゲット逆方向への加減速の操作
		currentMotion = ControlTargAccel(vecTarg, &bTargMove);

		if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_02)
		{ // レッスン02：前後加速の場合

			if (bTargMove)
			{ // ターゲット逆方向への操作が行われていた場合

				// レッスンカウンター加算
				CSceneTutorial::GetTutorialManager()->AddLessonCounter();
			}
		}
	}

	if (CSceneTutorial::GetTutorialManager()->GetLesson() > CTutorialManager::LESSON_02)
	{ // レッスン02：前後加速を終了している場合

		// ターゲット横方向への加減速の操作
		currentMotion = ControlSideAccel(vecSide, &bSideMove);

		if (CSceneTutorial::GetTutorialManager()->GetLesson() == CTutorialManager::LESSON_03)
		{ // レッスン03：左右加速の場合

			if (bSideMove)
			{ // ターゲット横方向への操作が行われていた場合

				// レッスンカウンター加算
				CSceneTutorial::GetTutorialManager()->AddLessonCounter();
			}
		}
	}

	// 現在のモーションを返す
	return currentMotion;
}
