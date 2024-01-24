//======================================================================================================================
//
//	リザルト処理 [result.cpp]
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

#include "enemy.h"
#include "result.h"
#include "score.h"
#include "timer.h"
#include "value.h"
#include "weed.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_RESULT		(1)			// 使用するポリゴン数
#define MAX_VAL_RES		(3)			// リザルトの数値表示の最大数

#define VAL_MAX_KILL	(999)		// 討伐数の最大値
#define VAL_KILL_DIGIT	(3)			// 討伐数の桁数
#define VAL_MAX_WEED	(999)		// 草刈り数の最大値
#define VAL_WEED_DIGIT	(3)			// 草刈り数の桁数

#define KILLENEMY_MUL	(150)		// 敵の討伐数のスコア加算時の倍率
#define KILLWEED_MUL	(200)		// 雑草の刈り取り数のスコア減算時の倍率

#define RESULT_POS_X	(365.0f)	// リザルトの絶対座標 (x)
#define RESULT_POS_Y	(229.5f)	// リザルトの絶対座標 (y)
#define RESULT_WIDTH	(340.0f)	// リザルトの横幅 / 2
#define RESULT_HEIGHT	(200.0f)	// リザルトの縦幅 / 2

#define VAL_RES_WIDTH	(42.0f)		// リザルトの数値の横幅 / 2
#define VAL_RES_HEIGHT	(42.0f)		// リザルトの数値の縦幅 / 2
#define VAL_RES_SPACE	(66.0f)		// リザルトの数値間の幅 (x)

#define RES_KILL_POS_X	(528.0f)	// 討伐数の絶対座標 (x)
#define RES_KILL_POS_Y	(212.0f)	// 討伐数の絶対座標 (y)
#define RES_WEED_POS_X	(528.0f)	// 草刈り数の絶対座標 (x)
#define RES_WEED_POS_Y	(292.0f)	// 草刈り数の絶対座標 (y)
#define RES_SCO_POS_X	(660.0f)	// スコアの絶対座標 (x)
#define RES_SCO_POS_Y	(372.0f)	// スコアの絶対座標 (y)

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureResult[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\result000.png",	// リザルト背景のテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_RESULT)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_RESULT_BG,				// リザルトの背景
	TEXTURE_RESULT_MAX,				// この列挙型の総数
} TEXTURE_RESULT;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureResult[TEXTURE_RESULT_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;					// 頂点バッファへのポインタ

int g_nTotalScore;		// トータルスコア

//======================================================================================================================
//	リザルトの初期化処理
//======================================================================================================================
void InitResult(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D        *pVtx;						// 頂点情報へのポインタ

	for (int nCntResult = 0; nCntResult < TEXTURE_RESULT_MAX; nCntResult++)
	{ // 使用するテクスチャ数分繰り返す

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, apTextureResult[nCntResult], &g_apTextureResult[nCntResult]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_RESULT,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffResult,
		NULL
	);

	// グローバル変数を初期化
	g_nTotalScore = 0;			// トータルスコア

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(RESULT_POS_X - RESULT_WIDTH, RESULT_POS_Y - RESULT_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(RESULT_POS_X + RESULT_WIDTH, RESULT_POS_Y - RESULT_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(RESULT_POS_X - RESULT_WIDTH, RESULT_POS_Y + RESULT_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(RESULT_POS_X + RESULT_WIDTH, RESULT_POS_Y + RESULT_HEIGHT, 0.0f);

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
	g_pVtxBuffResult->Unlock();
}

//======================================================================================================================
//	リザルトの終了処理
//======================================================================================================================
void UninitResult(void)
{
	// テクスチャの破棄
	for (int nCntResult = 0; nCntResult < TEXTURE_RESULT_MAX; nCntResult++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureResult[nCntResult] != NULL)
		{ // 変数 (g_apTextureResult) がNULLではない場合

			g_apTextureResult[nCntResult]->Release();
			g_apTextureResult[nCntResult] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffResult != NULL)
	{ // 変数 (g_pVtxBuffResult) がNULLではない場合

		g_pVtxBuffResult->Release();
		g_pVtxBuffResult = NULL;
	}
}

//======================================================================================================================
//	リザルトの更新処理
//======================================================================================================================
void UpdateResult(float fAlpha)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

	// 頂点バッファをアンロックする
	g_pVtxBuffResult->Unlock();

}

//======================================================================================================================
//	リザルトの描画処理
//======================================================================================================================
void DrawResult(float fAlpha)
{
	// 変数配列を宣言
	D3DXVECTOR3 aPosValue[MAX_VAL_RES] =					// リザルト表示の位置用
	{ // 初期値
		D3DXVECTOR3(RES_KILL_POS_X, RES_KILL_POS_Y, 0.0f),	// 討伐数の位置
		D3DXVECTOR3(RES_WEED_POS_X, RES_WEED_POS_Y, 0.0f),	// 討伐数の位置
		D3DXVECTOR3(RES_SCO_POS_X,  RES_SCO_POS_Y,  0.0f)	// スコアの位置
	};

	int aValue[MAX_VAL_RES]      = { GetKillEnemy(), GetLeaveWeed(), GetTotalScore() };	// リザルト表示の値用
	int aMaxValue[MAX_VAL_RES]   = { VAL_MAX_KILL,   VAL_MAX_WEED,   VAL_MAX_SCORE   };	// リザルト表示の値の制限用
	int aDigitValue[MAX_VAL_RES] = { VAL_KILL_DIGIT, VAL_WEED_DIGIT, VAL_SCO_DIGIT   };	// リザルト表示の桁数用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//------------------------------------------------------------------------------------------------------------------
	//	背景の描画
	//------------------------------------------------------------------------------------------------------------------
	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureResult[TEXTURE_RESULT_BG]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//------------------------------------------------------------------------------------------------------------------
	//	数値の描画
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntResult = 0; nCntResult < MAX_VAL_RES; nCntResult++)
	{ // リザルトの数値表示の最大数分繰り返す

		// 数値の設定
		SetValue
		( // 引数
			aPosValue[nCntResult],	// 位置
			aValue[nCntResult],		// 値
			aMaxValue[nCntResult],	// 値の最大値
			VAL_RES_WIDTH,			// 横幅
			VAL_RES_HEIGHT,			// 縦幅
			VAL_RES_SPACE,			// 数値間の幅
			fAlpha					// α値
		);

		// 数値の描画
		DrawValue(aDigitValue[nCntResult], VALUETYPE_BORDER);
	}
}

//======================================================================================================================
//	トータルスコアの設定処理
//======================================================================================================================
void SetTotalScore(void)
{
	// 変数を宣言
	int nKillEnemy = GetKillEnemy();	// 敵の討伐数
	int nKillWeed  = GetLeaveWeed();	// 雑草の余り数
	int nScore     = GetScore();		// 最終スコア

	// トータルスコアを求める
	g_nTotalScore = nScore + (nKillEnemy * KILLENEMY_MUL) - (nKillWeed * KILLWEED_MUL);

	// トータルスコアの補正
	if (g_nTotalScore < 0)
	{ // トータルスコアが 0より小さい場合

		// トータルスコアを 0に補正
		g_nTotalScore = 0;
	}
	else if (g_nTotalScore > VAL_MAX_SCORE)
	{ // トータルスコアが最大スコアより大きい場合

		// トータルスコアを最大スコアに補正
		g_nTotalScore = VAL_MAX_SCORE;
	}
}

//======================================================================================================================
//	トータルスコアの取得処理
//======================================================================================================================
int GetTotalScore(void)
{
	// トータルスコアの値を返す
	return g_nTotalScore;
}