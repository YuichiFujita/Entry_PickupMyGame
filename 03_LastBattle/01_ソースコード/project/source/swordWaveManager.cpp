//============================================================
//
//	剣波動マネージャー処理 [swordWaveManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "swordWaveManager.h"
#include "swordWave.h"
#include "scene.h"
#include "player.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const CWave::SGrow GROW		= CWave::SGrow(5.5f, 0.0f, 0.06f);	// 成長量
	const CWave::SGrow ADD_GROW	= CWave::SGrow(0.12f, 0.0f, 0.0f);	// 成長加速量
	const float HOLE_RAD	= 60.0f;	// 穴の半径
	const float THICKNESS	= 35.0f;	// 太さ
}

//************************************************************
//	親クラス [CSwordWaveManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSwordWaveManager::CSwordWaveManager()
{
	// メンバ変数をクリア
	m_spawnTiming.clear();	// 生成タイミング
}

//============================================================
//	デストラクタ
//============================================================
CSwordWaveManager::~CSwordWaveManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSwordWaveManager::Init(void)
{
	// メンバ変数を初期化
	m_spawnTiming.clear();	// 生成タイミング

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSwordWaveManager::Uninit(void)
{
	// 要素をクリア
	m_spawnTiming.clear();	// 生成タイミング
}

//============================================================
//	更新処理
//============================================================
void CSwordWaveManager::Update(void)
{
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報
	assert(pPlayer != nullptr);

	for (auto spawn : m_spawnTiming)
	{ // 全要素分繰り返す

		// 生成タイミングではない場合次に移行
		if (spawn.nMotion != pPlayer->GetMotionType(CPlayer::BODY_LOWER))		  { continue; }
		if (spawn.nFrame  != pPlayer->GetMotionWholeCounter(CPlayer::BODY_LOWER)) { continue; }

		D3DXMATRIX  mtxPlayer = pPlayer->CalcMtxWorld();	// プレイヤーマトリックス
		D3DXMATRIX  mtxSpawn, mtxOffset;	// 生成位置・オフセットマトリックス
		D3DXVECTOR3 posSpawn, rotSpawn;		// 生成位置・向き

		// 生成位置を設定
		D3DXMatrixTranslation(&mtxOffset, spawn.posOffset.x, spawn.posOffset.y, spawn.posOffset.z);	// オフセットマトリックスを作成
		D3DXMatrixMultiply(&mtxSpawn, &mtxOffset, &mtxPlayer);	// プレイヤーマトリックスと掛け合わせる
		posSpawn = useful::GetMatrixPosition(mtxSpawn);			// 生成ワールド座標を取得

		// 生成向きを設定
		rotSpawn = pPlayer->GetVec3Rotation() + spawn.rotOffset;
		useful::Vec3NormalizeRot(rotSpawn);	// 向き正規化

		// 剣波動の生成
		CSwordWave::Create
		( // 引数
			posSpawn,	// 位置
			rotSpawn,	// 向き
			GROW,		// 成長量
			ADD_GROW,	// 成長加速量
			HOLE_RAD,	// 穴の半径
			THICKNESS	// 太さ
		);
	}
}

//============================================================
//	生成タイミングの追加処理
//============================================================
void CSwordWaveManager::AddTiming
(
	const D3DXVECTOR3& rPosOffset,	// 生成位置オフセット
	const D3DXVECTOR3& rRotOffset,	// 生成向きオフセット
	const int nMotion,	// 生成モーション
	const int nFrame	// 生成フレーム
)
{
	// 情報を設定
	STiming tempTiming;	// 新規タイミング設定用
	tempTiming.posOffset = rPosOffset;	// 生成位置オフセット
	tempTiming.rotOffset = rRotOffset;	// 生成向きオフセット
	tempTiming.nMotion	 = nMotion;		// 生成モーション
	tempTiming.nFrame	 = nFrame;		// 生成フレーム

	// 配列最後尾に生成タイミングを追加
	m_spawnTiming.push_back(tempTiming);
}

//============================================================
//	生成タイミングの追加処理
//============================================================
void CSwordWaveManager::AddTiming(const STiming& rTiming)
{
	// 配列最後尾に生成タイミングを追加
	m_spawnTiming.push_back(rTiming);
}

//============================================================
//	生成処理
//============================================================
CSwordWaveManager *CSwordWaveManager::Create(void)
{
	// 剣波動マネージャーの生成
	CSwordWaveManager *pWaveManager = new CSwordWaveManager;
	if (pWaveManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 剣波動マネージャーの初期化
		if (FAILED(pWaveManager->Init()))
		{ // 初期化に失敗した場合

			// 剣波動マネージャーの破棄
			SAFE_DELETE(pWaveManager);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pWaveManager;
	}
}

//============================================================
//	破棄処理
//============================================================
void CSwordWaveManager::Release(CSwordWaveManager *&prSwordWaveManager)
{
	// 剣波動マネージャーの終了
	assert(prSwordWaveManager != nullptr);
	prSwordWaveManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prSwordWaveManager);
}
