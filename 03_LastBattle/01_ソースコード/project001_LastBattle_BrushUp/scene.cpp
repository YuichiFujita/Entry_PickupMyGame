//============================================================
//
//	シーン処理 [scene.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "scene.h"
#include "manager.h"
#include "renderer.h"
#include "light.h"
#include "camera.h"
#include "sceneTitle.h"
#include "sceneTutorial.h"
#include "sceneGame.h"
#include "sceneResult.h"
#include "sceneRanking.h"

#include "stage.h"
#include "player.h"
#include "enemy.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	// ボスの情報
	namespace boss
	{
		const D3DXVECTOR3 SPAWN_POS = D3DXVECTOR3(0.0f, 0.0f, 600.0f);	// スポーン位置
		const D3DXVECTOR3 SPAWN_ROT = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// スポーン向き
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CStage *CScene::m_pStage = nullptr;	// ステージの情報

//************************************************************
//	親クラス [CScene] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScene::CScene(const EMode mode) : m_mode(mode)
{

}

//============================================================
//	デストラクタ
//============================================================
CScene::~CScene()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScene::Init(void)
{
	// ステージの生成
	m_pStage = CStage::Create(m_mode);
	if (m_pStage == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	if (m_mode == MODE_GAME)
	{ // ゲーム画面の場合

		// ボスの生成
		CEnemy::Create
		( // 引数
			CEnemy::TYPE_BOSS_DRAGON,	// 種類
			boss::SPAWN_POS,			// 位置
			boss::SPAWN_ROT				// 向き
		);
	}

	// プレイヤーの生成
	CPlayer::Create(m_mode);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScene::Uninit(void)
{
	// ステージの破棄
	SAFE_REF_RELEASE(m_pStage);
}

//============================================================
//	更新処理
//============================================================
void CScene::Update(void)
{
	// ポインタを宣言
	CManager	*pManager	= GET_MANAGER;				// マネージャー
	CLight		*pLight		= pManager->GetLight();		// ライト
	CCamera		*pCamera	= pManager->GetCamera();	// カメラ
	CRenderer	*pRenderer	= pManager->GetRenderer();	// レンダラー

	// ステージの更新
	assert(m_pStage != nullptr);
	m_pStage->Update();

	// ライトの更新
	assert(pLight != nullptr);
	pLight->Update();

	// カメラの更新
	assert(pCamera != nullptr);
	pCamera->Update();

	// レンダラーの更新
	assert(pRenderer != nullptr);
	pRenderer->Update();
}

//============================================================
//	生成処理
//============================================================
CScene *CScene::Create(EMode mode)
{
	// ポインタを宣言
	CScene *pScene = nullptr;	// シーン情報

	// シーンの生成
	switch (mode)
	{ // モードごとの処理
	case MODE_TITLE:
		pScene = new CSceneTitle(mode);
		break;

	case MODE_TUTORIAL:
		pScene = new CSceneTutorial(mode);
		break;

	case MODE_GAME:
		pScene = new CSceneGame(mode);
		break;

	case MODE_RESULT:
		pScene = new CSceneResult(mode);
		break;

	case MODE_RANKING:
		pScene = new CSceneRanking(mode);
		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (pScene == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 確保したアドレスを返す
		return pScene;
	}
}

//============================================================
//	破棄処理
//============================================================
void CScene::Release(CScene *&prScene)
{
	// シーンの終了
	assert(prScene != nullptr);
	prScene->Uninit();

	// メモリ開放
	SAFE_DELETE(prScene);
}

//============================================================
//	ステージ取得処理
//============================================================
CStage *CScene::GetStage(void)
{
	// インスタンス未使用
	assert(m_pStage != nullptr);

	// ステージのポインタを返す
	return m_pStage;
}

//============================================================
//	プレイヤー取得処理
//============================================================
CPlayer *CScene::GetPlayer(void)
{
	CListManager<CPlayer> *pListManager = CPlayer::GetList();	// プレイヤーリストマネージャー
	if (pListManager == nullptr)		 { return nullptr; }	// リスト未使用の場合抜ける
	if (pListManager->GetNumAll() != 1)	 { return nullptr; }	// プレイヤーが1人ではない場合抜ける
	CPlayer *pPlayer = pListManager->GetList().front();			// プレイヤーの情報

	// プレイヤーのポインタを返す
	return pPlayer;
}

//============================================================
//	ボス取得処理
//============================================================
CEnemy *CScene::GetBoss(void)
{
	CListManager<CEnemy> *pListManager = CEnemy::GetList();	// 敵リストマネージャー
	if (pListManager == nullptr) { return nullptr; }		// リスト未使用

	std::list<CEnemy*> listEnemy = pListManager->GetList();	// 敵リスト
	for (CEnemy *pEnemy : listEnemy)
	{ // 要素数分繰り返す

		if (pEnemy->GetType() == CEnemy::TYPE_BOSS_DRAGON)
		{ // 敵がボスの場合

			// 敵のポインタを返す
			return pEnemy;
		}
	}

	return nullptr;
}

//============================================================
//	モードの設定処理
//============================================================
void CScene::SetMode(const EMode mode)
{
	// 引数のモードを設定
	m_mode = mode;
}

//============================================================
//	モード取得処理
//============================================================
CScene::EMode CScene::GetMode(void) const
{
	// 現在のモードを返す
	return m_mode;
}
