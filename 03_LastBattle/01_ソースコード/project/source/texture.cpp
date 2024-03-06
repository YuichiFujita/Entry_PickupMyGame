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
//	定数宣言
//************************************************************
namespace
{
	const char *SETUP_TXT = "data\\TXT\\texture.txt";	// セットアップテキスト相対パス
}

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
	// テクスチャへのポインタをクリア
	memset(&m_apTexture[0], 0, sizeof(m_apTexture));

	// 全ファイル名を削除
	m_sFileName.clear();
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
	// テクスチャへのポインタを初期化
	for (int nCntTexture = 0; nCntTexture < texture::MAX_NUM; nCntTexture++)
	{ // テクスチャの最大数分繰り返す

		// nullを代入
		m_apTexture[nCntTexture] = nullptr;
	}

	// 全ファイル名を削除
	m_sFileName.clear();

	// セットアップの読込
	LoadSetup();

	// 成功を返す
	return S_OK;
}

//============================================================
//	テクスチャ破棄処理
//============================================================
void CTexture::Unload(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < texture::MAX_NUM; nCntTexture++)
	{ // テクスチャの最大数分繰り返す

		// テクスチャの破棄
		SAFE_RELEASE(m_apTexture[nCntTexture]);
	}

	// 全ファイル名を削除
	m_sFileName.clear();
}

//============================================================
//	テクスチャ登録処理 (生成)
//============================================================
int CTexture::Regist(const SInfo info)
{
	// 変数を宣言
	HRESULT hr;				// 異常終了の確認用
	int nID = m_nNumAll;	// テクスチャ読込番号

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	if (m_nNumAll >= texture::MAX_NUM)
	{ // テクスチャオーバーの場合

		// 失敗を返す
		assert(false);
		return NONE_IDX;
	}

	// 空のテクスチャを生成
	hr = D3DXCreateTexture
	( // 引数
		pDevice,			// Direct3Dデバイス
		info.Width,			// テクスチャ横幅
		info.Height,		// テクスチャ縦幅
		info.MipLevels,		// ミップマップレベル
		info.Usage,			// 性質・確保オプション
		info.Format,		// ピクセルフォーマット
		info.Pool,			// 格納メモリ
		&m_apTexture[nID]	// テクスチャへのポインタ
	);
	if (FAILED(hr))
	{ // テクスチャの生成に失敗した場合

		// 失敗を返す
		assert(false);
		return NONE_IDX;
	}

	// テクスチャファイル名なしを保存
	m_sFileName.push_back(NONE_STRING);

	// テクスチャの総数を加算
	m_nNumAll++;

	// 読み込んだテクスチャの配列番号を返す
	return nID;
}

//============================================================
//	テクスチャ登録処理 (パス)
//============================================================
int CTexture::Regist(const char *pFileName)
{
	// 変数を宣言
	int nID = m_nNumAll;	// テクスチャ読込番号

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GET_DEVICE;	// デバイスのポインタ

	if (pFileName != nullptr)
	{ // ポインタが使用されている場合

		int nCntTexture = 0;	// テクスチャ番号
		for (auto sFileName : m_sFileName)
		{ // 最後尾まで繰り返す

			if (sFileName.compare(pFileName) == 0)
			{ // 文字列が一致した場合

				// すでに読み込んでいるテクスチャの配列番号を返す
				return nCntTexture;
			}

			// 次のテクスチャ番号にする
			nCntTexture++;
		}

		if (m_nNumAll >= texture::MAX_NUM)
		{ // テクスチャオーバーの場合

			// 失敗を返す
			assert(false);
			return NONE_IDX;
		}

		// テクスチャの読み込み
		if (FAILED(D3DXCreateTextureFromFile(pDevice, pFileName, &m_apTexture[nID])))
		{ // テクスチャの読み込みに失敗した場合

			// 失敗を返す
			assert(false);
			return NONE_IDX;
		}

		// テクスチャのファイル名を保存
		m_sFileName.push_back(pFileName);

		// テクスチャの総数を加算
		m_nNumAll++;

		// 読み込んだテクスチャの配列番号を返す
		return nID;
	}
	else
	{ // ポインタが使用されていない場合

		// テクスチャ非使用を返す
		return NONE_IDX;
	}
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

		// nullptrを返す
		return nullptr;
	}
	else
	{ // 引数のインデックスが使用不可の場合

		// nullptrを返す
		assert(false);
		return nullptr;
	}
}

//============================================================
//	生成処理
//============================================================
CTexture *CTexture::Create(void)
{
	// テクスチャの生成
	CTexture *pTexture = new CTexture;
	if (pTexture == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// テクスチャの読込
		if (FAILED(pTexture->Load()))
		{ // テクスチャ読込に失敗した場合

			// テクスチャの破棄
			SAFE_DELETE(pTexture);
			return nullptr;
		}

		// 確保したアドレスを返す
		return pTexture;
	}
}

//============================================================
//	破棄処理
//============================================================
void CTexture::Release(CTexture *&prTexture)
{
	// テクスチャの破棄
	assert(prTexture != nullptr);
	prTexture->Unload();

	// メモリ開放
	SAFE_DELETE(prTexture);
}

//============================================================
//	セットアップ処理
//============================================================
void CTexture::LoadSetup(void)
{
	// 変数を宣言
	int nEnd = 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT, "r");

	if (pFile != nullptr)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			if (strcmp(&aString[0], "FILENAME") == 0)
			{ // 読み込んだ文字列が FILENAME の場合

				// = を読み込む (不要)
				fscanf(pFile, "%s", &aString[0]);

				// ファイルパスを読み込む
				fscanf(pFile, "%s", &aString[0]);

				// テクスチャを登録
				Regist(&aString[0]);
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "テクスチャセットアップファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
