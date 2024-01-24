//============================================================
//
//	ゲームプレイヤー処理 [playerGame.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "playerGame.h"
#include "manager.h"
#include "camera.h"
#include "sound.h"
#include "collision.h"
#include "stage.h"
#include "levelupManager.h"

//************************************************************
//	マクロ定義
//************************************************************
#define HURRICANE_DMG	(50)	// ハリケーンのダメージ量

//************************************************************
//	子クラス [CPlayerGame] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayerGame::CPlayerGame()
{
	// メンバ変数をクリア
	m_pLevelup = NULL;	// 強化マネージャーの情報
}

//============================================================
//	デストラクタ
//============================================================
CPlayerGame::~CPlayerGame()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayerGame::Init(void)
{
	// メンバ変数を初期化
	m_pLevelup = NULL;	// 強化マネージャーの情報

	// 強化マネージャーの生成
	m_pLevelup = CLevelupManager::Create();
	if (UNUSED(m_pLevelup))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// プレイヤーの初期化
	if (FAILED(CPlayer::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayerGame::Uninit(void)
{
	// 強化マネージャーを破棄
	if (FAILED(m_pLevelup->Release(m_pLevelup)))
	{ // 破棄に失敗した場合

		// 例外処理
		assert(false);
	}

	// プレイヤーの終了
	CPlayer::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayerGame::Update(void)
{
	// 変数を宣言
	int nCurrentMotion = NONE_IDX;	// 現在のモーション

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

	// プレイヤーの更新
	CPlayer::Update();

	// 強化マネージャーの更新
	m_pLevelup->Update();

	// モーション・オブジェクトキャラクターの更新
	UpdateMotion(nCurrentMotion);
}

//============================================================
//	吹っ飛びヒット処理
//============================================================
void CPlayerGame::HitBlowAway
(
	const D3DXVECTOR3& rPlayerPos,	// プレイヤー位置
	const D3DXVECTOR3& rHitObjPos,	// オブジェクト位置
	const int nDmg					// ダメージ量
)
{
	// 変数を宣言
	D3DXVECTOR3 vecAway = VEC3_ZERO;	// 吹っ飛びベクトル
	STATE oldState = (STATE)GetState();	// 過去の状態

	// ヒット処理
	Hit(nDmg);

	if (oldState   == STATE_NORMAL
	&&  GetState() == STATE_DAMAGE)
	{ // 元の状態が通常状態且つ、現在の状態がダメージ状態の場合

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
void CPlayerGame::HitVortex
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

		// ヒット処理
		Hit(nDmg);

		if (GetState() != STATE_DEATH)
		{ // 死亡状態の場合

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
}

//============================================================
//	表示の設定処理
//============================================================
void CPlayerGame::SetDisp(const bool bDisp)
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
void CPlayerGame::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CPlayer::SetEnableDraw(bDraw);	// 自身
	SetEnableDrawMana(bDraw);		// マナ
	SetEnableDrawLife(bDraw);		// 体力
	SetEnableDrawDash(bDraw);		// ダッシュ
}

//============================================================
//	通常状態時の更新処理
//============================================================
CPlayer::MOTION CPlayerGame::UpdateNormal(void)
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
		currentMotion = ControlShotMagic(currentMotion);

		// 当たり判定
		CollisionTarget(posPlayer);	// ターゲット
		CollisionEnemy(posPlayer);	// 敵

		// バリアとの当たり判定
		if (pStage->CollisionBarrier(posPlayer, fRadiusPlayer))
		{ // 当たっていた場合

			// 渦巻きこまれヒット
			HitVortex(posPlayer, pStage->GetStageBarrierPosition(), HURRICANE_DMG);
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
CPlayer::MOTION CPlayerGame::UpdateMove(void)
{
	// 変数を宣言
	MOTION currentMotion = MOTION_MOVE;	// 現在のモーション
	D3DXVECTOR3 vecTarg = VEC3_ZERO;	// ターゲット逆方向ベクトル
	D3DXVECTOR3 vecSide = VEC3_ZERO;	// ターゲット横方向ベクトル

	// 吸い込みの更新
	UpdateAbsorb(vecTarg, vecSide);

	// ターゲット逆方向への加減速の操作
	currentMotion = ControlTargAccel(vecTarg);

	// ターゲット横方向への加減速の操作
	currentMotion = ControlSideAccel(vecSide);

	// 現在のモーションを返す
	return currentMotion;
}
