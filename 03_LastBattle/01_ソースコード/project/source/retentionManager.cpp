//============================================================
//
//	データ保存マネージャー処理 [retentionManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "retentionManager.h"
#include "manager.h"

//************************************************************
//	親クラス [CRetentionManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CRetentionManager::CRetentionManager()
{
	// メンバ変数をクリア
	memset(&m_result, 0, sizeof(m_result));	// リザルト情報
}

//============================================================
//	デストラクタ
//============================================================
CRetentionManager::~CRetentionManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CRetentionManager::Init(void)
{
	// メンバ変数を初期化
	m_result.win	= WIN_NONE;	// 勝利状況
	m_result.nTime	= 0;		// 経過タイム

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CRetentionManager::Uninit(void)
{

}

//============================================================
//	生成処理
//============================================================
CRetentionManager *CRetentionManager::Create(void)
{
	// データ保存マネージャーの生成
	CRetentionManager *pRetentionManager = new CRetentionManager;
	if (pRetentionManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// データ保存マネージャーの初期化
		if (FAILED(pRetentionManager->Init()))
		{ // 初期化に失敗した場合

			// データ保存マネージャーの破棄
			SAFE_DELETE(pRetentionManager);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pRetentionManager;
	}
}

//============================================================
//	破棄処理
//============================================================
void CRetentionManager::Release(CRetentionManager *&prRetentionManager)
{
	// データ保存マネージャーの終了
	assert(prRetentionManager != nullptr);
	prRetentionManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prRetentionManager);
}

//============================================================
//	リザルト情報の設定処理
//============================================================
void CRetentionManager::SetResult(const EWin win, const long nTime)
{
	if (win <= WIN_NONE || win >= WIN_MAX) { assert(false); return; }	// 勝利が正規ではない

	// 引数のクリア状況を設定
	m_result.win = win;

	// 引数の経過タイムを設定
	m_result.nTime = nTime;
}

//============================================================
//	勝利状況の取得処理
//============================================================
CRetentionManager::EWin CRetentionManager::GetWin(void) const
{
	// 勝利状況を返す
	return m_result.win;
}

//============================================================
//	経過タイムの取得処理
//============================================================
long CRetentionManager::GetTime(void) const
{
	// 経過タイムを返す
	return m_result.nTime;
}
