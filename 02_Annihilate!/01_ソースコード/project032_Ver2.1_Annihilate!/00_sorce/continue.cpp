//======================================================================================================================
//
//	コンテニュー処理 [continue.cpp]
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

#include "continue.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_CONTINUE		(3)			// 使用するポリゴン数

#define CONTINUE_POS_X		(365.0f)	// コンテニューの絶対座標 (x)
#define CONTINUE_POS_Y		(575.5f)	// コンテニューの絶対座標 (y)
#define CONTINUE_WIDTH		(340.0f)	// コンテニューの横幅 / 2
#define CONTINUE_HEIGHT		(115.0f)	// コンテニューの縦幅 / 2

#define CON_YESNO_POS_X		(220.0f)	// 選択肢の絶対座標 (x)
#define CON_YESNO_POS_Y		(630.0f)	// 選択肢の絶対座標 (y)
#define CON_YESNO_SPACE		(290.0f)	// 選択肢間の幅 (x)
#define CON_YESNO_WIDTH		(120.0f)	// 選択肢の横幅 / 2
#define CON_YESNO_HEIGHT	(50.0f)		// 選択肢の縦幅 / 2

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureContinue[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\continue000.png",	// コンテニュー背景のテクスチャ相対パス
	"02_data\\02_TEXTURE\\continue001.png",	// YESのテクスチャ相対パス
	"02_data\\02_TEXTURE\\continue002.png",	// NOのテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (CONTINUESELECT)
//**********************************************************************************************************************
typedef enum
{
	CONTINUESELECT_YES = 0,				// YES状態
	CONTINUESELECT_NO,					// NO状態
	CONTINUESELECT_MAX					// この列挙型の総数
} CONTINUESELECT;

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_CONTINUE)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_CONTINUE_BG,				// コンテニューの背景
	TEXTURE_CONTINUE_YES,				// YES
	TEXTURE_CONTINUE_NO,				// NO
	TEXTURE_CONTINUE_MAX,				// この列挙型の総数
} TEXTURE_CONTINUE;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void SelectContinue(void);	// コンテニューの操作処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureContinue[TEXTURE_CONTINUE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffContinue = NULL;						// 頂点バッファへのポインタ

int  g_nSelectContinue;		// 現在の選択中メニュー
int  g_nOldSelectContinue;	// 前回の選択中メニュー

//======================================================================================================================
//	コンテニューの初期化処理
//======================================================================================================================
void InitContinue(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D        *pVtx;						// 頂点情報へのポインタ

	for (int nCntContinue = 0; nCntContinue < TEXTURE_CONTINUE_MAX; nCntContinue++)
	{ // 使用するテクスチャ数分繰り返す

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, apTextureContinue[nCntContinue], &g_apTextureContinue[nCntContinue]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_CONTINUE,	// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffContinue,
		NULL
	);

	// グローバル変数の値を初期化
	g_nSelectContinue    = CONTINUESELECT_YES;	// 現在の選択中メニューを初期化
	g_nOldSelectContinue = CONTINUESELECT_YES;	// 前回の選択中メニューを初期化

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	//------------------------------------------------------------------------------------------------------------------
	//	背景の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(CONTINUE_POS_X - CONTINUE_WIDTH, CONTINUE_POS_Y - CONTINUE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(CONTINUE_POS_X + CONTINUE_WIDTH, CONTINUE_POS_Y - CONTINUE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(CONTINUE_POS_X - CONTINUE_WIDTH, CONTINUE_POS_Y + CONTINUE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(CONTINUE_POS_X + CONTINUE_WIDTH, CONTINUE_POS_Y + CONTINUE_HEIGHT, 0.0f);

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

	//------------------------------------------------------------------------------------------------------------------
	//	セレクトメニューの初期化
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
	{ // 選択肢の項目数分繰り返す

		// 頂点座標を設定
		pVtx[4].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) - CON_YESNO_WIDTH, CON_YESNO_POS_Y - CON_YESNO_HEIGHT, 0.0f);
		pVtx[5].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) + CON_YESNO_WIDTH, CON_YESNO_POS_Y - CON_YESNO_HEIGHT, 0.0f);
		pVtx[6].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) - CON_YESNO_WIDTH, CON_YESNO_POS_Y + CON_YESNO_HEIGHT, 0.0f);
		pVtx[7].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) + CON_YESNO_WIDTH, CON_YESNO_POS_Y + CON_YESNO_HEIGHT, 0.0f);

		// rhw の設定
		pVtx[4].rhw = 1.0f;
		pVtx[5].rhw = 1.0f;
		pVtx[6].rhw = 1.0f;
		pVtx[7].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);

		// テクスチャ座標の設定
		pVtx[4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[5].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[6].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[7].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	コンテニューの終了処理
//======================================================================================================================
void UninitContinue(void)
{
	// テクスチャの破棄
	for (int nCntContinue = 0; nCntContinue < TEXTURE_CONTINUE_MAX; nCntContinue++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureContinue[nCntContinue] != NULL)
		{ // 変数 (g_apTextureContinue) がNULLではない場合

			g_apTextureContinue[nCntContinue]->Release();
			g_apTextureContinue[nCntContinue] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffContinue != NULL)
	{ // 変数 (g_pVtxBuffContinue) がNULLではない場合

		g_pVtxBuffContinue->Release();
		g_pVtxBuffContinue = NULL;
	}
}

//======================================================================================================================
//	コンテニューの更新処理
//======================================================================================================================
void UpdateContinue(float fAlpha, ALPHASTATE state)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	switch (state)
	{ // 透明度の変更状態ごとの処理
	case ALPHASTATE_CHANGE:	// 透明度の変更状態

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

		for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
		{ // 選択肢の項目数分繰り返す

			// 頂点カラーの設定
			pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);

			// 頂点データのポインタを 4つ分進める
			pVtx += 4;
		}

		// 処理を抜ける
		break;

	case ALPHASTATE_NONE:	// 何もしていない状態

		// コンテニューの操作処理
		SelectContinue();

		// 頂点データのポインタを前フレームにセレクトしていた要素の頂点に移動
		pVtx += (g_nOldSelectContinue * 4);

		// 頂点カラーの更新
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// 頂点データのポインタを先頭アドレスに戻す
		pVtx -= (g_nOldSelectContinue * 4);

		// 頂点データのポインタを今フレームでセレクトしている要素の頂点に移動
		pVtx += (g_nSelectContinue * 4);

		// 頂点カラーの更新
		pVtx[4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[6].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[7].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// 現在セレクト中の要素を代入
		g_nOldSelectContinue = g_nSelectContinue;

		// 処理を抜ける
		break;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	コンテニューの描画処理
//======================================================================================================================
void DrawContinue(float fAlpha)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffContinue, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntContinue = 0; nCntContinue < MAX_CONTINUE; nCntContinue++)
	{ // 使用するポリゴン数分繰り返す

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureContinue[nCntContinue]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntContinue * 4, 2);
	}
}

//======================================================================================================================
//	コンテニューのα値の補正処理
//======================================================================================================================
void RevAlphaContinue(void)
{
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
	{ // 選択肢の項目数分繰り返す

		// 頂点カラーの設定
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	コンテニューの操作処理
//======================================================================================================================
void SelectContinue(void)
{
	if (GetFade() == FADE_NONE)
	{ // フェードしていない場合

		if (GetKeyboardTrigger(DIK_A) == true || GetKeyboardTrigger(DIK_LEFT) == true || GetJoyKeyTrigger(JOYKEY_LEFT, 0) == true)
		{ // 左移動の操作が行われた場合

			// 変数 (g_nSelectContinue) を 1ずつ引いていく ( 0の場合 1に戻す)
			g_nSelectContinue = (g_nSelectContinue + (CONTINUESELECT_MAX - 1)) % CONTINUESELECT_MAX;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (カーソル移動)
		}
		else if (GetKeyboardTrigger(DIK_D) == true || GetKeyboardTrigger(DIK_RIGHT) == true || GetJoyKeyTrigger(JOYKEY_RIGHT, 0) == true)
		{ // 右移動の操作が行われた場合

			// 変数 (g_nSelectContinue) を 1ずつ足していく ( 1の場合 0に戻す)
			g_nSelectContinue = (g_nSelectContinue + 1) % CONTINUESELECT_MAX;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (カーソル移動)
		}

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // 決定の操作が行われた場合

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_DEC_00);	// SE (決定00)

			switch (g_nSelectContinue)
			{ // 画面の移行先の確認
			case CONTINUESELECT_YES:	// YESの場合

				// モード選択 (ゲーム画面に移行)
				SetFade(MODE_GAME);

				// 処理を抜ける
				break;

			case CONTINUESELECT_NO:		// NOの場合

				// モード選択 (タイトル画面に移行)
				SetFade(MODE_TITLE);

				// 処理を抜ける
				break;
			}
		}
	}
}