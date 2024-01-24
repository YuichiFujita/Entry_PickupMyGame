//======================================================================================================================
//
//	ランキング処理 [ranking.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "ranking.h"
#include "result.h"
#include "score.h"
#include "value.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define RANK_SETUP_BIN	"02_data\\01_BIN\\ranking.bin"	// ランキングセットアップ用のバイナリファイルの相対パス

#define MAX_RANKING		(1)			// 使用するポリゴン数
#define MAX_VAL_RANK	(5)			// ランキングの数値表示の最大数
#define NONE_RANK		(-1)		// 順位更新の初期値

#define RANKING_POS_X	(995.0f)	// ランキングの絶対座標 (x)
#define RANKING_POS_Y	(360.0f)	// ランキングの絶対座標 (y)
#define RANKING_WIDTH	(260.0f)	// ランキングの横幅 / 2
#define RANKING_HEIGHT	(330.0f)	// ランキングの縦幅 / 2

#define VAL_RANK_WIDTH	(42.0f)		// ランキングの数値の横幅 / 2
#define VAL_RANK_HEIGHT	(42.0f)		// ランキングの数値の縦幅 / 2
#define VAL_RANK_SPACE	(66.0f)		// ランキングの数値間の幅 (y)

#define RANK_SCO_POS_X	(1205.0f)	// スコアの絶対座標 (x)
#define RANK_SCO_POS_Y	(212.0f)	// スコアの絶対座標 (y)
#define RANK_SCO_SPACE	(96.0f)		// スコア間の幅 (y)

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureRanking[] =	// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\ranking000.png",	// ランキング背景のテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_RANKING)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_RANKING_BG,				// ランキングの背景
	TEXTURE_RANKING_MAX,			// この列挙型の総数
} TEXTURE_RANKING;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void WirteRanking(void);			// ランキングの書き出し処理
void ReadRanking(void);				// ランキングの読み込み処理
void SortRanking(void);				// ランキングのソート処理
void CurrentRanking(void);			// 今回のスコアの順位判定

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureRanking[TEXTURE_RANKING_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking = NULL;						// 頂点バッファへのポインタ

int g_nCurrentRankID;		// 今回のスコアの順位のインデックス
int g_aRank[MAX_VAL_RANK];	// ランキングの値

//======================================================================================================================
//	ランキングの初期化処理
//======================================================================================================================
void InitRanking(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D        *pVtx;						// 頂点情報へのポインタ

	for (int nCntRanking = 0; nCntRanking < TEXTURE_RANKING_MAX; nCntRanking++)
	{ // 使用するテクスチャ数分繰り返す

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, apTextureRanking[nCntRanking], &g_apTextureRanking[nCntRanking]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_RANKING,	// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffRanking,
		NULL
	);

	// グローバル変数を初期化
	g_nCurrentRankID = NONE_RANK;	// 今回のスコアの順位のインデックス

	// グローバル変数配列を初期化
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // ランキングの数値表示の最大数分繰り返す

		g_aRank[nCntRanking] = 0;	// ランキングの値
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(RANKING_POS_X - RANKING_WIDTH, RANKING_POS_Y - RANKING_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(RANKING_POS_X + RANKING_WIDTH, RANKING_POS_Y - RANKING_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(RANKING_POS_X - RANKING_WIDTH, RANKING_POS_Y + RANKING_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(RANKING_POS_X + RANKING_WIDTH, RANKING_POS_Y + RANKING_HEIGHT, 0.0f);

	// rhw の設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffRanking->Unlock();
}

//======================================================================================================================
//	ランキングの終了処理
//======================================================================================================================
void UninitRanking(void)
{
	// テクスチャの破棄
	for (int nCntRanking = 0; nCntRanking < TEXTURE_RANKING_MAX; nCntRanking++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureRanking[nCntRanking] != NULL)
		{ // 変数 (g_apTextureRanking) がNULLではない場合

			g_apTextureRanking[nCntRanking]->Release();
			g_apTextureRanking[nCntRanking] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffRanking != NULL)
	{ // 変数 (g_pVtxBuffRanking) がNULLではない場合

		g_pVtxBuffRanking->Release();
		g_pVtxBuffRanking = NULL;
	}
}

//======================================================================================================================
//	ランキングの更新処理
//======================================================================================================================
void UpdateRanking(float fAlpha)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

	// 頂点バッファをアンロックする
	g_pVtxBuffRanking->Unlock();
}

//======================================================================================================================
//	ランキングの描画処理
//======================================================================================================================
void DrawRanking(float fAlpha)
{
	// 変数を宣言
	D3DXVECTOR3 posValue;	// ランキング表示の位置用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffRanking, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//------------------------------------------------------------------------------------------------------------------
	//	背景の描画
	//------------------------------------------------------------------------------------------------------------------
	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureRanking[TEXTURE_RANKING_BG]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//------------------------------------------------------------------------------------------------------------------
	//	数値の描画
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // ランキングの数値表示の最大数分繰り返す

		// ランキング表示の位置を設定
		posValue = D3DXVECTOR3(RANK_SCO_POS_X, RANK_SCO_POS_Y + (nCntRanking * RANK_SCO_SPACE), 0.0f);

		// 数値の設定
		SetValue
		( // 引数
			posValue,				// 位置
			g_aRank[nCntRanking],	// 値
			VAL_MAX_SCORE,			// 値の最大値
			VAL_RANK_WIDTH,			// 横幅
			VAL_RANK_HEIGHT,		// 縦幅
			VAL_RANK_SPACE,			// 数値間の幅
			fAlpha					// α値
		);

		if (g_nCurrentRankID == nCntRanking)
		{ // 順位が更新されている場合

			// 数値の描画
			DrawValue(VAL_SCO_DIGIT, VALUETYPE_GREEN);
		}
		else
		{ // 順位が更新されていない場合

			// 数値の描画
			DrawValue(VAL_SCO_DIGIT, VALUETYPE_BORDER);
		}
	}
}

//======================================================================================================================
//	ランキングの設定処理
//======================================================================================================================
void SetRanking(void)
{
	// ランキングの読み込み
	ReadRanking();

	if (GetTotalScore() >= g_aRank[MAX_VAL_RANK - 1])
	{ // 今回のスコアがランキング5位のスコアより大きい場合

		// ランキングのソート
		SortRanking();

		// ランキングの書き出し
		WirteRanking();

		// 今回のスコアの順位判定
		CurrentRanking();
	}
}

//======================================================================================================================
//	ランキングの書き出し処理
//======================================================================================================================
void WirteRanking(void)
{
	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// バイナリファイルを書き出し方式で開く
	pFile = fopen(RANK_SETUP_BIN, "wb");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		// ファイルに数値を書き出す
		fwrite(&g_aRank[0], sizeof(int), MAX_VAL_RANK, pFile);

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ランキングファイルの書き出しに失敗！", "警告！", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	ランキングの読み込み処理
//======================================================================================================================
void ReadRanking(void)
{
	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// バイナリファイルを読み込み方式で開く
	pFile = fopen(RANK_SETUP_BIN, "rb");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		// ファイルの数値を読み込む
		fread(&g_aRank[0], sizeof(int), MAX_VAL_RANK, pFile);

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ランキングファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	ランキングのソート処理
//======================================================================================================================
void SortRanking(void)
{
	// 変数を宣言
	int nNowMaxID = 0;	// 最大値が入っている配列の要素番号の記憶用
	int nSortKeep = 0;	// ソート入れ替え時の代入用

	// 今回のスコアをランキング5位に代入
	g_aRank[MAX_VAL_RANK - 1] = GetTotalScore();

	for (int nCntRanking = 0; nCntRanking < (MAX_VAL_RANK - 1); nCntRanking++)
	{ // ランキングの総数 -1回分繰り返す

		// 現在の繰り返し数を代入 (要素1とする)
		nNowMaxID = nCntRanking;

		for (int nCntSort = nCntRanking + 1; nCntSort < MAX_VAL_RANK; nCntSort++)
		{ // ランキングの数値表示の最大数分繰り返す

			if (g_aRank[nNowMaxID] < g_aRank[nCntSort])
			{ // 最大値に設定されている値より、現在の値のほうが大きい場合

				// 現在の要素番号を最大値に設定
				nNowMaxID = nCntSort;
			}
		}

		if (nCntRanking != nNowMaxID)
		{ // 最大値の要素番号に変動があった場合

			// 値の入れ替え
			nSortKeep            = g_aRank[nCntRanking];
			g_aRank[nCntRanking] = g_aRank[nNowMaxID];
			g_aRank[nNowMaxID]   = nSortKeep;
		}
	}
}

//======================================================================================================================
//	今回のスコアの順位判定
//======================================================================================================================
void CurrentRanking(void)
{
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // ランキングの数値表示の最大数分繰り返す

		if (GetTotalScore() == g_aRank[nCntRanking] && g_aRank[nCntRanking] > 0)
		{ // 今回のスコアと同じ値且つ、今回のスコアが 0より大きい場合

			// 今回のスコアの順位のインデックスを設定
			g_nCurrentRankID = nCntRanking;

			// 処理を抜ける
			break;
		}
	}
}