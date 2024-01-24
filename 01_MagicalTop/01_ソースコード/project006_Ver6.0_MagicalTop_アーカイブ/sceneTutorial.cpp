//============================================================
//
//	チュートリアル画面処理 [sceneTutorial.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sceneTutorial.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"
#include "tutorialManager.h"

#include "player.h"
#include "target.h"
#include "stage.h"
#include "enemy.h"
#include "magic.h"
#include "sea.h"
#include "wall.h"
#include "scenery.h"
#include "sky.h"
#include "flower.h"
#include "weed.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SCO_POS		(D3DXVECTOR3(825.0f, 50.0f, 0.0f))		// スコア位置
#define SCO_SIZE	(D3DXVECTOR3(60.0f, 80.0f, 0.0f))		// スコア大きさ
#define SCO_SPACE	(D3DXVECTOR3(SCO_SIZE.x, 0.0f, 0.0f))	// スコア空白

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CTutorialManager *CSceneTutorial::m_pTutorialManager = NULL;	// チュートリアルマネージャー

//************************************************************
//	子クラス [CSceneTutorial] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSceneTutorial::CSceneTutorial(const MODE mode) : CScene(mode)
{

}

//============================================================
//	デストラクタ
//============================================================
CSceneTutorial::~CSceneTutorial()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSceneTutorial::Init(void)
{
	//--------------------------------------------------------
	//	チュートリアルの初期化
	//--------------------------------------------------------
	// チュートリアルマネージャーの生成
	m_pTutorialManager = CTutorialManager::Create();
	if (UNUSED(m_pTutorialManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シーンの初期化
	CScene::Init();

	//--------------------------------------------------------
	//	オブジェクト生成
	//--------------------------------------------------------
	// 海オブジェクトの生成
	CSea::Create();

	// 壁オブジェクトの生成
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 0.0f,    0.0f, -3000.0f), D3DXToRadian(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),   D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3(-3000.0f, 0.0f,  0.0f),    D3DXToRadian(D3DXVECTOR3(0.0f, 90.0f, 0.0f)),  D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 0.0f,    0.0f,  3000.0f), D3DXToRadian(D3DXVECTOR3(0.0f, 180.0f, 0.0f)), D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));
	CWall::Create(CWall::TEXTURE_NORMAL, D3DXVECTOR3( 3000.0f, 0.0f,  0.0f),    D3DXToRadian(D3DXVECTOR3(0.0f, 270.0f, 0.0f)), D3DXVECTOR2(6000.0f, 400.0f), XCOL_WHITE, POSGRID2(18, 1));

	// 景色オブジェクトの生成
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, VEC3_ZERO,                                    XCOL_WHITE,                        POSGRID2(32, 1), 12000.0f, 1000.0f, D3DCULL_CW, false);
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, D3DXToRadian(D3DXVECTOR3(0.0f, 85.0f, 0.0f)), D3DXCOLOR(0.7f, 1.0f, 1.0f, 1.0f), POSGRID2(32, 1), 14000.0f, 1600.0f, D3DCULL_CW, false);
	CScenery::Create(CScenery::TEXTURE_NORMAL, VEC3_ZERO, D3DXToRadian(D3DXVECTOR3(0.0f, 35.0f, 0.0f)), D3DXCOLOR(0.4f, 1.0f, 0.7f, 1.0f), POSGRID2(32, 1), 16000.0f, 2200.0f, D3DCULL_CW, false);

	// 空オブジェクトの生成
	CSky::Create(CSky::TEXTURE_NORMAL, VEC3_ZERO, VEC3_ZERO, XCOL_WHITE, POSGRID2(32, 6), 18000.0f, D3DCULL_CW, false);

	//--------------------------------------------------------
	//	初期設定
	//--------------------------------------------------------
	// 敵セットアップの読込
	CEnemy::LoadSetup();

	// 魔法セットアップの読込
	CMagic::LoadSetup();

	// マナフラワーセットアップの読込
	CFlower::LoadSetup();

	// 草セットアップの読込
	CWeed::LoadSetup();

	// マナフラワーランダム生成
	CFlower::RandomSpawn(30, CFlower::TYPE_SPRING);	// TODO：定数変更

	// 草ランダム生成
	CWeed::RandomSpawn(200, CWeed::TYPE_SPRING);	// TODO：定数変更

	// カメラを設定
	CManager::GetCamera()->SetState(CCamera::STATE_BARGAINING);	// カメラを寄り引き状態に設定
	CManager::GetCamera()->SetDestBargaining();	// 目標位置を設定

	// BGMの再生
	CManager::GetSound()->Play(CSound::LABEL_BGM_TUTORIAL);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
HRESULT CSceneTutorial::Uninit(void)
{
	// チュートリアルマネージャーの破棄
	if (FAILED(CTutorialManager::Release(m_pTutorialManager)))
	{ // 破棄に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シーンの終了
	CScene::Uninit();

	// 成功を返す
	return S_OK;
}

//============================================================
//	更新処理
//============================================================
void CSceneTutorial::Update(void)
{
	if (USED(m_pTutorialManager))
	{ // 使用中の場合

		// チュートリアルマネージャーの更新
		m_pTutorialManager->Update();
	}
	else { assert(false); }	// 非使用中

	// プレイヤーのマナ消費制限
	UpdateLimitPlayerMana();

	// ターゲットの体力消費制限
	UpdateLimitTargetLife();

	// シーンの更新
	CScene::Update();
}

//============================================================
//	描画処理
//============================================================
void CSceneTutorial::Draw(void)
{

}

//============================================================
//	チュートリアルマネージャー取得処理
//============================================================
CTutorialManager *CSceneTutorial::GetTutorialManager(void)
{
	// チュートリアルマネージャーのポインタを返す
	return m_pTutorialManager;
}

//============================================================
//	プレイヤーのマナ消費制限処理
//============================================================
void CSceneTutorial::UpdateLimitPlayerMana(void)
{
	// ポインタを宣言
	CPlayer *pPlayer = GetPlayer();	// プレイヤー情報

	if (CSceneTutorial::GetTutorialManager()->GetLesson() < CTutorialManager::LESSON_05)
	{ // レッスン05：マナ回復まで到達していない場合

		if (pPlayer->GetMana() < pPlayer->GetMaxMana() / 2)
		{ // プレイヤーのマナが半分を切った場合

			// プレイヤーのマナを半分に設定
			pPlayer->SetMana(pPlayer->GetMaxMana() / 2);
		}
	}
}

//============================================================
//	ターゲットの体力消費制限処理
//============================================================
void CSceneTutorial::UpdateLimitTargetLife(void)
{
	// ポインタを宣言
	CTarget *pTarget = GetTarget();	// ターゲット情報

	if (pTarget->GetLife() < pTarget->GetMaxLife() / 2)
	{ // ターゲットの体力が半分を切った場合

		// ターゲットの体力を半分に設定
		pTarget->SetLife(pTarget->GetMaxLife() / 2);
	}
}
