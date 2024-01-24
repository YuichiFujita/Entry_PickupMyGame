//============================================================
//
//	タイトル画面処理 [sceneTitle.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sceneTitle.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "titleManager.h"

#include "stage.h"
#include "target.h"
#include "field.h"
#include "sea.h"
#include "wall.h"
#include "scenery.h"
#include "sky.h"
#include "flower.h"
#include "weed.h"

//************************************************************
//	マクロ定義
//************************************************************
#define CREATE_FLOWER	(30)	// マナフラワーの生成数
#define CREATE_WEED		(200)	// 草の生成数

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CTitleManager *CSceneTitle::m_pTitleManager = NULL;	// タイトルマネージャー

//************************************************************
//	子クラス [CSceneTitle] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSceneTitle::CSceneTitle(const MODE mode) : CScene(mode)
{

}

//============================================================
//	デストラクタ
//============================================================
CSceneTitle::~CSceneTitle()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSceneTitle::Init(void)
{
	// 変数を宣言
	CWaveManager::SEASON season = (CWaveManager::SEASON)(rand() % CWaveManager::SEASON_MAX);	// 季節

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	//--------------------------------------------------------
	//	タイトルの初期化
	//--------------------------------------------------------
	// タイトルマネージャーの生成
	m_pTitleManager = CTitleManager::Create();
	if (UNUSED(m_pTitleManager))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// シーンの初期化
	CScene::Init();

	//--------------------------------------------------------
	//	オブジェクト生成・初期化
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
	// ステージエリア・バリアの描画を停止
	GetStage()->SetEnableDrawArea(false);
	GetStage()->SetEnableDrawBarrier(false);

	// ターゲット体力の描画を停止
	GetTarget()->SetEnableDrawLife(false);

	// マナフラワーセットアップの読込
	CFlower::LoadSetup();

	// 草セットアップの読込
	CWeed::LoadSetup();

	// マナフラワーランダム生成
	CFlower::RandomSpawn(CREATE_FLOWER, CFlower::TYPE_SPRING);

	// 草ランダム生成
	CWeed::RandomSpawn(CREATE_WEED, CWeed::TYPE_SPRING);

	// 季節を設定
	GetField()->SetSeason(season);	// 地面を変更
	CFlower::SetSeason(season);		// 花を変更
	CWeed::SetSeason(season);		// 草を変更

	// カメラを設定
	CManager::GetCamera()->SetState(CCamera::STATE_ROTATE);	// カメラを回転状態に設定
	CManager::GetCamera()->SetDestRotate();					// 目標位置を設定
	CManager::GetCamera()->SetEnableUpdate(false);			// 更新を停止

	// BGMの再生
	CManager::GetSound()->Play(CSound::LABEL_BGM_TITLE_RESULT);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
HRESULT CSceneTitle::Uninit(void)
{
	// タイトルマネージャーの破棄
	if (FAILED(CTitleManager::Release(m_pTitleManager)))
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
void CSceneTitle::Update(void)
{
	if (USED(m_pTitleManager))
	{ // 使用中の場合

		// タイトルマネージャーの更新
		m_pTitleManager->Update();
	}
	else { assert(false); }	// 非使用中

	// シーンの更新
	CScene::Update();
}

//============================================================
//	描画処理
//============================================================
void CSceneTitle::Draw(void)
{

}

//============================================================
//	タイトルマネージャー取得処理
//============================================================
CTitleManager *CSceneTitle::GetTitleManager(void)
{
	// タイトルマネージャーを返す
	return m_pTitleManager;
}
