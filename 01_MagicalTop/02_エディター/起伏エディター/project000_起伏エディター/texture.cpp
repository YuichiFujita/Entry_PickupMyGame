//============================================================
//
//	テクスチャ処理 [texture.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
int CTexture::m_nNumAll = 0;	// テクスチャの総数

//************************************************************
//	親クラス [CTexture] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTexture::CTexture()
{
	// メンバ変数をクリア
	memset(&m_apTexture[0], 0, sizeof(m_apTexture));	// テクスチャへのポインタ
	memset(&m_pFileName[0][0], 0, sizeof(m_pFileName));	// 読み込んだテクスチャファイル名
}

//============================================================
//	デストラクタ
//============================================================
CTexture::~CTexture()
{

}

//============================================================
//	テクスチャ生成処理
//============================================================
HRESULT CTexture::Load(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// メンバ変数を初期化
	for (int nCntTexture = 0; nCntTexture < MAX_TEXTURE; nCntTexture++)
	{ // テクスチャの最大数分繰り返す

		// テクスチャへのポインタを初期化
		m_apTexture[nCntTexture] = NULL;
	}

	// 読み込んだテクスチャファイル名を初期化
	memset(&m_pFileName[0][0], 0, sizeof(m_pFileName));

	// 成功を返す
	return S_OK;
}

//============================================================
//	テクスチャ破棄処理
//============================================================
void CTexture::Unload(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEXTURE; nCntTexture++)
	{ // テクスチャの最大数分繰り返す

		// テクスチャの破棄
		if (USED(m_apTexture[nCntTexture]))
		{ // テクスチャが使用中の場合

			// メモリ開放
			m_apTexture[nCntTexture]->Release();
			m_apTexture[nCntTexture] = NULL;
		}
	}
}

//============================================================
//	テクスチャ登録処理
//============================================================
int CTexture::Regist(const char *pFileName)
{
	// 変数を宣言
	int nID = m_nNumAll;	// テクスチャ読込番号

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	for (int nCntTexture = 0; nCntTexture < m_nNumAll; nCntTexture++)
	{ // テクスチャの総数分繰り返す

		if (strcmp(&m_pFileName[nCntTexture][0], pFileName) == 0)
		{ // 文字列が一致した場合

			// すでに読み込んでいるテクスチャの配列番号を返す
			return nCntTexture;
		}
	}

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice, pFileName, &m_apTexture[nID])))
	{ // テクスチャの読み込みに失敗した場合

		// 失敗を返す
		assert(false);
		return -1;
	}

	// テクスチャのファイル名を保存
	strcpy(&m_pFileName[nID][0], pFileName);

	// テクスチャの総数を加算
	m_nNumAll++;

	// 読み込んだテクスチャの配列番号を返す
	return nID;
}

//============================================================
//	テクスチャ情報の取得処理
//============================================================
LPDIRECT3DTEXTURE9 CTexture::GetTexture(int nID)
{
	if (nID >= 0 && nID < m_nNumAll)
	{ // 引数のインデックスが範囲内の場合

		// 引数のテクスチャポインタを返す
		return m_apTexture[nID];
	}
	else if (nID == NONE_IDX)
	{ // 引数のインデックスが -1の場合

		// NULLを返す
		return NULL;
	}
	else
	{ // 引数のインデックスが使用不可の場合

		// NULLを返す
		assert(false);
		return NULL;
	}
}

//============================================================
//	生成処理
//============================================================
CTexture *CTexture::Create(void)
{
	// ポインタを宣言
	CTexture *pTexture = NULL;	// テクスチャ生成用

	if (UNUSED(pTexture))
	{ // 使用されていない場合

		// メモリを確保
		pTexture = new CTexture;	// テクスチャ
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pTexture))
	{ // 確保に成功している場合

		// テクスチャの読込
		if (FAILED(pTexture->Load()))
		{ // テクスチャ読み込みに失敗した場合

			// メモリ開放
			delete pTexture;
			pTexture = NULL;

			// 失敗を返す
			assert(false);
			return NULL;
		}

		// 確保したアドレスを返す
		return pTexture;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CTexture::Release(CTexture *&prTexture)
{
	if (USED(prTexture))
	{ // 使用中の場合

		// テクスチャの破棄
		prTexture->Unload();

		// メモリ開放
		delete prTexture;
		prTexture = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

// TODO：動的に使うやつRegistしておく
