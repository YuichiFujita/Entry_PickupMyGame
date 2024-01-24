//============================================================
//
//	ショップ省き情報処理 [omitShop.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "omitShop.h"
#include "manager.h"

//************************************************************
//	親クラス [COmitShop] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
COmitShop::COmitShop()
{
	// メンバ変数をクリア
	memset(&m_aOmitShop[0], 0, sizeof(m_aOmitShop));	// 購入品の省き状況
	m_nNumOmit = 0;	// 省く購入品数
	m_pOmit = NULL;	// 省く購入品情報
}

//============================================================
//	デストラクタ
//============================================================
COmitShop::~COmitShop()
{

}

//============================================================
//	初期化処理
//============================================================
void COmitShop::Init(void)
{
	// メンバ変数を初期化
	m_nNumOmit = 0;	// 省く購入品数
	m_pOmit = NULL;	// 省く購入品情報

	for (int nCntOmit = 0; nCntOmit < CShop::BUY_MAX; nCntOmit++)
	{ // 購入品の最大数分繰り返す

		// 省かない状態に設定
		m_aOmitShop[nCntOmit] = false;
	}
}

//============================================================
//	終了処理
//============================================================
void COmitShop::Uninit(void)
{
	if (USED(m_pOmit))
	{ // 省く購入品情報が使われている場合

		// シャッフルを省く数分のメモリ開放
		delete[] m_pOmit;
		m_pOmit = NULL;
	}
}

//============================================================
//	省く購入品生成処理
//============================================================
int *COmitShop::CreateOmit(void)
{
	// 変数を宣言
	int nOmitID = 0;	// 現在のシャッフル省き配列の要素番号

	if (UNUSED(m_pOmit))
	{ // 省く購入品情報が使われていない場合

		// シャッフルを省く数分のメモリ生成
		m_pOmit = new int[m_nNumOmit];

		if (UNUSED(m_pOmit))
		{ // 確保に失敗した場合

			// 失敗を返す
			return NULL;
		}
	}
	else { assert(false); return NULL; }	// 使用中

	for (int nCntOmit = 0; nCntOmit < CShop::BUY_MAX; nCntOmit++)
	{ // 購入品の最大数分繰り返す

		if (m_aOmitShop[nCntOmit])
		{ // 省く場合

			// 現在のインデックスを省くデータに代入
			m_pOmit[nOmitID] = nCntOmit;

			// 省くデータを入れるインデックスを加算
			nOmitID++;
		}
	}

	// 省く購入品情報を返す
	return m_pOmit;
}

//============================================================
//	省く購入品破棄処理
//============================================================
HRESULT COmitShop::ReleaseOmit(void)
{
	if (USED(m_pOmit))
	{ // 省く購入品情報が使われている場合

		// シャッフルを省く数分のメモリ開放
		delete[] m_pOmit;
		m_pOmit = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	省く購入品数取得処理
//============================================================
int COmitShop::GetNumOmit(void) const
{
	// 省く購入品数を返す
	return m_nNumOmit;
}

//============================================================
//	省き状況取得処理
//============================================================
bool COmitShop::IsOmit(const CShop::BUY buy) const
{
	// 引数の購入品の省き状況を返す
	return m_aOmitShop[buy];
}

//============================================================
//	省き状況の設定処理
//============================================================
void COmitShop::SetEnableOmit(const CShop::BUY buy, const bool bOmit)
{
	// 引数の購入品の省き状況を設定
	m_aOmitShop[buy] = bOmit;

	// 省く購入品数を初期化
	m_nNumOmit = 0;

	// 省く購入品数を設定
	for (int nCntOmit = 0; nCntOmit < CShop::BUY_MAX; nCntOmit++)
	{ // 購入品の最大数分繰り返す

		if (m_aOmitShop[nCntOmit])
		{ // 省く状態だった場合

			// 省く購入品数を加算
			m_nNumOmit++;
		}
	}
}

//============================================================
//	生成処理
//============================================================
COmitShop *COmitShop::Create(void)
{
	// ポインタを宣言
	COmitShop *pOmitShop = NULL;	// ショップ省き情報生成用

	if (UNUSED(pOmitShop))
	{ // 使用されていない場合

		// メモリ確保
		pOmitShop = new COmitShop;	// ショップ省き情報
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pOmitShop))
	{ // 使用されている場合
		
		// ショップ省き情報の初期化
		pOmitShop->Init();

		// 確保したアドレスを返す
		return pOmitShop;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT COmitShop::Release(COmitShop *&prOmitShop)
{
	if (USED(prOmitShop))
	{ // 使用中の場合

		// ショップ省き情報の終了
		prOmitShop->Uninit();

		// メモリ開放
		delete prOmitShop;
		prOmitShop = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}
