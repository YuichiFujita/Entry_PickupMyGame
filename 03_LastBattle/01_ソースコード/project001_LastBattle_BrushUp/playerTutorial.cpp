//============================================================
//
//	プレイヤーチュートリアル処理 [playerTutorial.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "playerTutorial.h"
#include "manager.h"
#include "camera.h"
#include "sceneTutorial.h"
#include "tutorialManager.h"
#include "renderer.h"
#include "sound.h"
#include "stage.h"
#include "sword.h"
#include "shadow.h"
#include "anim2D.h"
#include "effect3D.h"
#include "particle3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{

}

//************************************************************
//	子クラス [CPlayerTutorial] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayerTutorial::CPlayerTutorial() : m_oldCamRot(VEC3_ZERO)
{

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
	m_oldCamRot = VEC3_ZERO;	// カメラの過去向き

	// プレイヤーの初期化
	if (FAILED(CPlayer::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 通常状態の初期化
	InitNormal();

	// 自動更新・自動描画をONにする
	SetEnableUpdate(true);
	SetEnableDraw(true);

	// カメラを追従状態に設定
	CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報
	pCamera->SetState(CCamera::STATE_FOLLOW);		// カメラを追従状態に設定
	pCamera->SetDestFollow();						// カメラ目標位置の初期化

	// カメラの過去向きを保存
	m_oldCamRot = pCamera->GetRotation();

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
	// 状態が通常以外ならエラー
	assert(GetState() == STATE_NORMAL);

	CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報
	CTutorialManager *pTutorialManager = CSceneTutorial::GetTutorialManager();	// チュートリアルマネージャー情報
	if (pTutorialManager->GetLesson() == CTutorialManager::LESSON_01)
	{ // レッスン01：移動・カメラ操作に挑戦中の場合

		D3DXVECTOR3 curCamRot = pCamera->GetRotation();	// 現在のカメラ向き
		if (fabsf(m_oldCamRot.x - curCamRot.x) >= 0.001f
		||  fabsf(m_oldCamRot.y - curCamRot.y) >= 0.001f
		||  fabsf(m_oldCamRot.z - curCamRot.z) >= 0.001f)
		{ // カメラの向きが動いていた場合

			// レッスンカウンター加算
			pTutorialManager->AddLessonCounter();
		}
	}

	// カメラの過去向きを保存
	m_oldCamRot = pCamera->GetRotation();

	// プレイヤーの更新
	CPlayer::Update();
}

//============================================================
//	描画処理
//============================================================
void CPlayerTutorial::Draw(CShader * /*pShader*/)
{
	// プレイヤーの描画
	CPlayer::Draw();
}

//============================================================
//	通常状態時の更新処理
//============================================================
void CPlayerTutorial::UpdateNormal(int *pLowMotion, int *pUpMotion)
{
	// ポインタを宣言
	CTutorialManager *pTutorialManager = CSceneTutorial::GetTutorialManager();	// チュートリアルマネージャー情報
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetVec3Position();		// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();		// プレイヤー向き
	int nLesson = pTutorialManager->GetLesson();	// レッスン番号

	// 重力の更新
	UpdateGravity();

	if (nLesson >= CTutorialManager::LESSON_04)
	{ // レッスン04：攻撃操作の挑戦中、またはクリア済みの場合

		// 攻撃操作
		if (UpdateAttack(posPlayer, rotPlayer))
		{ // 攻撃していた場合

			if (nLesson == CTutorialManager::LESSON_04)
			{ // レッスン04：ジャンプ操作に挑戦中の場合

				// レッスンカウンター加算
				pTutorialManager->AddLessonCounter();
			}
		}
	}

	if (nLesson >= CTutorialManager::LESSON_03)
	{ // レッスン03：回避操作の挑戦中、またはクリア済みの場合

		// 回避操作
		if (UpdateDodge(rotPlayer))
		{ // 回避していた場合

			if (nLesson == CTutorialManager::LESSON_03)
			{ // レッスン03：ジャンプ操作に挑戦中の場合

				// レッスンカウンター加算
				pTutorialManager->AddLessonCounter();
			}
		}
	}

	if (nLesson >= CTutorialManager::LESSON_01)
	{ // レッスン01：移動・カメラ操作の挑戦中、またはクリア済みの場合

		// 移動操作・目標向き設定
		if (UpdateMove(pLowMotion, pUpMotion))
		{ // 移動していた場合

			if (nLesson == CTutorialManager::LESSON_01)
			{ // レッスン01：移動・カメラ操作に挑戦中の場合

				// レッスンカウンター加算
				pTutorialManager->AddLessonCounter();
			}
		}
	}

	if (nLesson >= CTutorialManager::LESSON_02)
	{ // レッスン02：ジャンプ操作の挑戦中、またはクリア済みの場合

		// ジャンプ操作
		if (UpdateJump(pLowMotion, pUpMotion))
		{ // ジャンプしていた場合

			if (nLesson == CTutorialManager::LESSON_02)
			{ // レッスン02：ジャンプ操作に挑戦中の場合

				// レッスンカウンター加算
				pTutorialManager->AddLessonCounter();
			}
		}
	}

	// 位置更新
	UpdatePosition(&posPlayer);

	// 敵との当たり判定の更新
	UpdateCollEnemy(&posPlayer);

	// 着地判定
	UpdateLanding(&posPlayer);

	// 向き更新
	UpdateRotation(&rotPlayer);

	// ステージ範囲外の補正
	pStage->LimitPosition(posPlayer, GetRadius());

	// 位置を反映
	SetVec3Position(posPlayer);

	// 向きを反映
	SetVec3Rotation(rotPlayer);
}
