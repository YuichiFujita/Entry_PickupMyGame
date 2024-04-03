//============================================================
//
//	タイマーUI処理 [timerUI.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "timerUI.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\timer001.png",	// 背景テクスチャ
	};

	const int PRIORITY = 5;	// タイマーUIの優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CTimerUI::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CTimerUI] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTimerUI::CTimerUI() :
	m_pBG		(nullptr),	// 背景の情報
	m_offsetBG	(VEC3_ZERO)	// 背景のオフセット
{

}

//============================================================
//	デストラクタ
//============================================================
CTimerUI::~CTimerUI()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CTimerUI::Init(void)
{
	// メンバ変数を初期化
	m_pBG = nullptr;		// 背景の情報
	m_offsetBG = VEC3_ZERO;	// 背景のオフセット

	// 背景の生成
	m_pBG = CObject2D::Create(VEC3_ZERO);
	if (m_pBG == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pBG->BindTexture(TEXTURE_FILE[TEXTURE_BG]);

	// タイマーマネージャーの初期化
	if (FAILED(CTimerManager::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	SetPriority(PRIORITY);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CTimerUI::Uninit(void)
{
	// 背景の終了
	SAFE_UNINIT(m_pBG);

	// タイマーマネージャーの終了
	CTimerManager::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CTimerUI::Update(void)
{
	// 背景の更新
	m_pBG->Update();

	// タイマーマネージャーの更新
	CTimerManager::Update();
}

//============================================================
//	位置の設定処理
//============================================================
void CTimerUI::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	CTimerManager::SetPosition(rPos);

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	優先順位の設定処理
//============================================================
void CTimerUI::SetPriority(const int nPriority)
{
	// 背景の優先順位を設定
	m_pBG->SetPriority(nPriority);

	// 優先順位の設定
	CTimerManager::SetPriority(nPriority);
}

//============================================================
//	更新状況の設定処理
//============================================================
void CTimerUI::SetEnableUpdate(const bool bUpdate)
{
	// 背景の更新状況を設定
	m_pBG->SetEnableUpdate(bUpdate);

	// 更新状況の設定
	CTimerManager::SetEnableUpdate(bUpdate);
}

//============================================================
//	描画状況の設定処理
//============================================================
void CTimerUI::SetEnableDraw(const bool bDraw)
{
	// 背景の更新状況を設定
	m_pBG->SetEnableDraw(bDraw);

	// 描画状況の設定
	CTimerManager::SetEnableDraw(bDraw);
}

//============================================================
//	生成処理
//============================================================
CTimerUI *CTimerUI::Create
(
	const ETime time,				// 設定タイム
	const long nTime,				// 制限時間
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSizeValue,	// 数字の大きさ
	const D3DXVECTOR3& rSizePart,	// 区切りの大きさ
	const D3DXVECTOR3& rSpaceValue,	// 数字の空白
	const D3DXVECTOR3& rSpacePart,	// 区切りの空白
	const D3DXVECTOR3& rOffsetBG,	// 背景のオフセット
	const D3DXVECTOR3& rSizeBG		// 背景の大きさ
)
{
	// タイマーUIの生成
	CTimerUI *pTimerUI = new CTimerUI;
	if (pTimerUI == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// タイマーUIの初期化
		if (FAILED(pTimerUI->Init()))
		{ // 初期化に失敗した場合

			// タイマーUIの破棄
			SAFE_DELETE(pTimerUI);
			return nullptr;
		}

		// 制限時間を設定
		pTimerUI->SetLimit(time, nTime);

		// 位置を設定
		pTimerUI->SetPosition(rPos);

		// 数字の大きさを設定
		pTimerUI->SetSizingValue(rSizeValue);

		// 区切りの大きさを設定
		pTimerUI->SetSizingPart(rSizePart);

		// 数字の空白を設定
		pTimerUI->SetSpaceValue(rSpaceValue);

		// 区切りの空白を設定
		pTimerUI->SetSpacePart(rSpacePart);

		// オフセットを設定
		pTimerUI->SetOffset(rOffsetBG);

		// 背景の大きさを設定
		pTimerUI->SetSizingBG(rSizeBG);

		// 確保したアドレスを返す
		return pTimerUI;
	}
}

//============================================================
//	破棄処理
//============================================================
void CTimerUI::Release(CTimerUI *&prTimerUI)
{
	// タイマーUIの終了
	assert(prTimerUI != nullptr);
	prTimerUI->Uninit();

	// メモリ開放
	SAFE_DELETE(prTimerUI);
}

//============================================================
//	オフセットの設定処理
//============================================================
void CTimerUI::SetOffset(const D3DXVECTOR3& rOffset)
{
	// オフセットの設定
	m_offsetBG = rOffset;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	背景の大きさ設定処理
//============================================================
void CTimerUI::SetSizingBG(const D3DXVECTOR3& rSize)
{
	// 背景の大きさを設定
	assert(m_pBG != nullptr);
	m_pBG->SetVec3Sizing(rSize);
}

//============================================================
//	相対位置の設定処理
//============================================================
void CTimerUI::SetPositionRelative(void)
{
	// 背景位置を設定
	m_pBG->SetVec3Position(GetPosition() + m_offsetBG);
}
