//============================================================
//
//	経験値マネージャー処理 [expManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "expManager.h"
#include "manager.h"
#include "sound.h"
#include "objectGauge2D.h"
#include "multiValue.h"

//************************************************************
//	マクロ定義
//************************************************************
#define MAX_EXP			(100)	// 最大経験値
#define EXP_FRAME		(10)	// 経験値変動フレーム
#define GAUGE_POS		(D3DXVECTOR3(SCREEN_CENT.x, 680.0f, 0.0f))	// 位置
#define GAUGE_GAUGESIZE	(D3DXVECTOR3(600.0f, 10.0f, 0.0f))			// ゲージ大きさ
#define GAUGE_FRONTCOL	(D3DXCOLOR(0.0f, 0.8f, 0.35f, 1.0f))		// 表ゲージ色
#define GAUGE_BACKCOL	(D3DXCOLOR(0.0f, 0.1f, 0.2f, 1.0f))			// 裏ゲージ色
#define GAUGE_FRAMESIZE	(D3DXVECTOR3(602.0f, 12.0f, 0.0f))			// ゲージ枠大きさ

#define FIRST_LV	(0)	// レベル初期値
#define LV_DIGIT	(2)	// レベル桁数
#define LV_SIZE		(D3DXVECTOR3(55.0f, 65.0f, 0.0f))	// 数値大きさ
#define LV_SPACE	(D3DXVECTOR3(55.0f, 0.0f, 0.0f))	// 行間
#define LV_POS		(D3DXVECTOR3(SCREEN_CENT.x - (LV_SPACE.x * 0.5f), 655.0f, 0.0f))	// 位置

//************************************************************
//	親クラス [CExpManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CExpManager::CExpManager()
{
	// メンバ変数をクリア
	m_pExp = NULL;		// 経験値の情報
	m_pLevel = NULL;	// レベルの情報
}

//============================================================
//	デストラクタ
//============================================================
CExpManager::~CExpManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CExpManager::Init(void)
{
	// メンバ変数を初期化
	m_pExp = NULL;		// 経験値の情報
	m_pLevel = NULL;	// レベルの情報

	// 経験値の生成
	m_pExp = CObjectGauge2D::Create
	( // 引数
		CObject::LABEL_GAUGE,		// オブジェクトラベル
		MAX_EXP,					// 最大経験値
		EXP_FRAME,					// 経験値変動フレーム
		GAUGE_POS,					// 位置
		GAUGE_GAUGESIZE,			// ゲージ大きさ
		GAUGE_FRONTCOL,				// 表ゲージ色
		GAUGE_BACKCOL,				// 裏ゲージ色
		true,						// 枠描画状況
		CObjectGauge2D::TYPE_EXP,	// 枠種類
		GAUGE_FRAMESIZE				// 枠大きさ
	);
	if (UNUSED(m_pExp))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ゲージの設定
	m_pExp->SetNum(0);

	// レベルの生成
	m_pLevel = CMultiValue::Create
	( // 引数
		CValue::TEXTURE_MAGIC_GREEN,	// テクスチャ
		FIRST_LV,	// レベル初期値
		LV_DIGIT,	// レベル桁数
		LV_POS,		// 位置
		LV_SIZE,	// 数値大きさ
		LV_SPACE	// 行間
	);
	if (UNUSED(m_pLevel))
	{ // 非使用中の場合

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
void CExpManager::Uninit(void)
{

}

//============================================================
//	更新処理
//============================================================
void CExpManager::Update(void)
{

}

//============================================================
//	レベルの加算処理
//============================================================
void CExpManager::AddLevel(const int nAdd)
{
	// 引数分レベルを加算
	m_pLevel->AddNum(nAdd);
}

//============================================================
//	経験値の加算処理
//============================================================
void CExpManager::AddExp(const int nAdd)
{
	// 変数を宣言
	int nCurrentExp = m_pExp->GetNum();	// 現在の経験値

	if (nCurrentExp + nAdd >= MAX_EXP)
	{ // 経験値がオーバーする場合

		// レベルを加算
		m_pLevel->AddNum(1);

		// 経験値のあまりを設定
		m_pExp->SetNum((nCurrentExp + nAdd) - MAX_EXP);

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_LEVELUP);	// レベルアップ音
	}
	else
	{ // 経験値がオーバーしない場合

		// 経験値を設定
		m_pExp->SetNum(nCurrentExp + nAdd);
	}

	// 例外処理
	assert(nAdd <= MAX_EXP + 1);	// 加算量オーバー
}

//============================================================
//	レベル取得処理
//============================================================
int CExpManager::GetLevel(void) const
{
	// レベルを返す
	return m_pLevel->GetNum();
}

//============================================================
//	経験値描画状況の設定処理
//============================================================
void CExpManager::SetEnableExpDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	m_pExp->SetEnableDraw(bDraw);	// ゲージ
	m_pLevel->SetEnableDraw(bDraw);	// 数値
}

//============================================================
//	生成処理
//============================================================
CExpManager *CExpManager::Create()
{
	// ポインタを宣言
	CExpManager *pExpManager = NULL;	// 経験値マネージャー生成用

	if (UNUSED(pExpManager))
	{ // 使用されていない場合

		// メモリ確保
		pExpManager = new CExpManager;	// 経験値マネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pExpManager))
	{ // 使用されている場合
		
		// 経験値マネージャーの初期化
		if (FAILED(pExpManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pExpManager;
			pExpManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pExpManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CExpManager::Release(CExpManager *&prExpManager)
{
	if (USED(prExpManager))
	{ // 使用中の場合

		// 経験値マネージャーの終了
		prExpManager->Uninit();

		// メモリ開放
		delete prExpManager;
		prExpManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}
