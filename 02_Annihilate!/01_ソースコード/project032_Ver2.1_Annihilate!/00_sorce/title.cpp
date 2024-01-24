//======================================================================================================================
//
//	タイトル画面処理 [title.cpp]
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

#include "title.h"

#include "camera.h"
#include "light.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshwall.h"
#include "object.h"
#include "shadow.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_TITLE			(5)			// 使用するポリゴン数
#define TITLE_MOVE			(1.2f)		// タイトルの移動量
#define SUB_ALPHA_TITLE		(0.006f)	// α値の減算量
#define FIRST_ALPHA_TITLE	(0.85f)		// タイトルのフェードの初期α値

#define TITLE_POS_X			(640.0f)	// タイトルの停止時の絶対座標 (x)
#define TITLE_POS_Y			(220.0f)	// タイトルの停止時の絶対座標 (y)
#define TITLE_WIDTH			(600.0f)	// タイトルの横幅 / 2
#define TITLE_HEIGHT		(120.0f)	// タイトルの縦幅 / 2

#define TIT_SELECT_POS_X	(350.0f)	// タイトルの選択肢の絶対座標 (x)
#define TIT_SELECT_POS_Y	(560.0f)	// タイトルの選択肢の絶対座標 (y)
#define TIT_SELECT_SPACE	(580.0f)	// タイトルの選択肢間の幅 (x)
#define TIT_SELECT_WIDTH	(210.0f)	// タイトルの選択肢の横幅 / 2
#define TIT_SELECT_HEIGHT	(70.0f)		// タイトルの選択肢の縦幅 / 2

#define TIT_SEL_BG_HEIGHT	(60.0f)		// タイトルの選択肢の背景の縦幅 / 2

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureTitle[] =	// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\title000.png",	// タイトルのテクスチャ相対パス
	NULL,									// NULL
	"02_data\\02_TEXTURE\\title001.png",	// ゲーム開始のテクスチャ相対パス
	"02_data\\02_TEXTURE\\title002.png",	// チュートリアル開始のテクスチャ相対パス
	NULL,									// NULL
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_TITLE)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_TITLE_TITLE = 0,			// タイトル
	TEXTURE_TITLE_NULL_00,				// NULL
	TEXTURE_TITLE_START_GAME,			// ゲーム開始
	TEXTURE_TITLE_START_TUTORIAL,		// チュートリアル開始
	TEXTURE_TITLE_NULL_01,				// NULL
	TEXTURE_TITLE_MAX,					// この列挙型の総数
} TEXTURE_TITLE;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void SelectTitle(void);			// タイトルの操作処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureTitle[TEXTURE_TITLE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;						// 頂点バッファへのポインタ

D3DXVECTOR3 g_posTitle;			// タイトルの絶対座標
TITLESTATE  g_stateTitle;		// タイトルの状態
Alpha       g_alphaTitle;		// 透明度の状態
int         g_nSelectTitle;		// 現在の選択中メニュー
int         g_nOldSelectTitle;	// 前回の選択中メニュー

//======================================================================================================================
//	タイトル画面の初期化処理
//======================================================================================================================
void InitTitle(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	使用するソースファイルの初期化
	//------------------------------------------------------------------------------------------------------------------
	// カメラの初期化
	InitCamera();

	// ライトの初期化
	InitLight();

	// メッシュドームの初期化
	InitMeshDome();

	// メッシュシリンダーの初期化
	InitMeshCylinder();

	// メッシュフィールドの初期化
	InitMeshField();

	// メッシュウォールの初期化
	InitMeshWall();

	// 影の初期化
	InitShadow();

	// オブジェクトの初期化
	InitObject();

	// ステージのセットアップ
	TxtSetStage();

	//------------------------------------------------------------------------------------------------------------------
	//	タイトルの初期化
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D         *pVtx;					// 頂点情報へのポインタ

	for (int nCntTitle = 0; nCntTitle < TEXTURE_TITLE_MAX; nCntTitle++)
	{ // 使用するテクスチャ数分繰り返す

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, apTextureTitle[nCntTitle], &g_apTextureTitle[nCntTitle]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_TITLE,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL
	);

	// グローバル変数を初期化
	g_posTitle        = D3DXVECTOR3(TITLE_POS_X, 0.0f, 0.0f);	// タイトルの絶対座標
	g_stateTitle      = TITLESTATE_MOVE;						// タイトルの状態
	g_nSelectTitle    = TITLESELECT_GAME;						// 現在の選択中メニュー
	g_nOldSelectTitle = TITLESELECT_GAME;						// 前回の選択中メニュー

	// 透明度の状態を初期化
	g_alphaTitle.state  = ALPHASTATE_CHANGE;	// 透明度の変更状態
	g_alphaTitle.fAlpha = FIRST_ALPHA_TITLE;	// 背景の透明度

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	//------------------------------------------------------------------------------------------------------------------
	//	タイトルアイコンの初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);

	// rhw の設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのポインタを 4つ分進める
	pVtx += 4;

	//------------------------------------------------------------------------------------------------------------------
	//	セレクト背景の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(0.0f,         TIT_SELECT_POS_Y - TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, TIT_SELECT_POS_Y - TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f,         TIT_SELECT_POS_Y + TIT_SEL_BG_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, TIT_SELECT_POS_Y + TIT_SEL_BG_HEIGHT, 0.0f);

	// rhw の設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.65f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのポインタを 4つ分進める
	pVtx += 4;

	//------------------------------------------------------------------------------------------------------------------
	//	セレクトメニューの初期化
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntTitle = 0; nCntTitle < TITLESELECT_MAX; nCntTitle++)
	{ // 選択肢の項目数分繰り返す

		// 頂点座標を設定
		pVtx[0].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) - TIT_SELECT_WIDTH, TIT_SELECT_POS_Y - TIT_SELECT_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) + TIT_SELECT_WIDTH, TIT_SELECT_POS_Y - TIT_SELECT_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) - TIT_SELECT_WIDTH, TIT_SELECT_POS_Y + TIT_SELECT_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(TIT_SELECT_POS_X + (nCntTitle * TIT_SELECT_SPACE) + TIT_SELECT_WIDTH, TIT_SELECT_POS_Y + TIT_SELECT_HEIGHT, 0.0f);

		// rhw の設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[1].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[2].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[3].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	//------------------------------------------------------------------------------------------------------------------
	//	フェードの初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(0.0f,         0.0f,          0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f,          0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f,         SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhw の設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, FIRST_ALPHA_TITLE);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();
}

//======================================================================================================================
//	タイトル画面の終了処理
//======================================================================================================================
void UninitTitle(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	使用したソースファイルの終了
	//------------------------------------------------------------------------------------------------------------------
	// カメラの終了
	UninitCamera();

	// ライトの終了
	UninitLight();

	// メッシュドームの終了
	UninitMeshDome();

	// メッシュシリンダーの終了
	UninitMeshCylinder();

	// メッシュフィールドの終了
	UninitMeshField();

	// メッシュウォールの終了
	UninitMeshWall();

	// 影の終了
	UninitShadow();

	// オブジェクトの終了
	UninitObject();

	//------------------------------------------------------------------------------------------------------------------
	//	タイトルの終了
	//------------------------------------------------------------------------------------------------------------------
	// テクスチャの破棄
	for (int nCntTitle = 0; nCntTitle < TEXTURE_TITLE_MAX; nCntTitle++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureTitle[nCntTitle] != NULL)
		{ // 変数 (g_apTextureTitle) がNULLではない場合

			g_apTextureTitle[nCntTitle]->Release();
			g_apTextureTitle[nCntTitle] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTitle != NULL)
	{ // 変数 (g_pVtxBuffTitle) がNULLではない場合

		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}

	//------------------------------------------------------------------------------------------------------------------
	//	サウンドの終了
	//------------------------------------------------------------------------------------------------------------------
	// サウンドの停止
	StopSound();
}

//======================================================================================================================
//	タイトル画面の更新処理
//======================================================================================================================
void UpdateTitle(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	使用するソースファイルの更新
	//------------------------------------------------------------------------------------------------------------------
	// ライトの更新
	UpdateLight();

	// カメラの更新
	UpdateCamera();

	// メッシュドームの更新
	UpdateMeshDome();

	// メッシュシリンダーの更新
	UpdateMeshCylinder();

	// メッシュフィールドの更新
	UpdateMeshField();

	// メッシュウォールの更新
	UpdateMeshWall();

	// 影の更新
	UpdateShadow();

	// オブジェクトの更新
	UpdateObject();

	//------------------------------------------------------------------------------------------------------------------
	//	タイトルの更新
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	VERTEX_2D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	// フェードの透明度
	switch (g_alphaTitle.state)
	{ // 透明度の変更状態ごとの処理
	case ALPHASTATE_WAIT:	// 透明度の変更待機状態

		// 無し

		// 処理を抜ける
		break;

	case ALPHASTATE_CHANGE:	// 透明度の変更状態

		if (g_alphaTitle.fAlpha > 0.0f)
		{ // 透明度が 0.0f より大きい場合

			// 透明度を上げる
			g_alphaTitle.fAlpha -= SUB_ALPHA_TITLE;
		}
		else
		{ // 透明度が 0.0f 以下の場合

			// 透明度を補正
			g_alphaTitle.fAlpha = 0.0f;

			// 何もしない状態にする
			g_alphaTitle.state = ALPHASTATE_NONE;
		}

		// 処理を抜ける
		break;

	case ALPHASTATE_NONE:	// 何もしていない状態

		// 無し

		// 処理を抜ける
		break;
	}

	// タイトルの動き
	switch (g_stateTitle)
	{ // 状態の確認
	case TITLESTATE_MOVE:	// 移動状態の場合

		// タイトルの絶対座標を移動 (下)
		g_posTitle.y += TITLE_MOVE;

		if (g_posTitle.y >= TITLE_POS_Y)
		{ // タイトルが下がり切った場合

			// y座標を補正
			g_posTitle.y = TITLE_POS_Y;

			// 状態を何もしないに設定
			g_stateTitle = TITLESTATE_NONE;

			// サウンドの再生
			PlaySound(SOUND_LABEL_BGM_000);	// BGM (タイトル画面)
		}

		// 処理を抜ける
		break;

	case TITLESTATE_NONE:	// 何もしていない状態の場合

		// タイトルの操作処理
		SelectTitle();

		// 頂点データのポインタを前フレームにセレクトしていた要素の頂点に移動
		pVtx += (g_nOldSelectTitle * 4);

		// 頂点カラーの更新 (セレクトメニュー)
		pVtx[8].col  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[9].col  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[10].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[11].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// 頂点データのポインタを先頭アドレスに戻す
		pVtx -= (g_nOldSelectTitle * 4);

		// 頂点データのポインタを今フレームでセレクトしている要素の頂点に移動
		pVtx += (g_nSelectTitle * 4);

		// 頂点カラーの更新 (セレクトメニュー)
		pVtx[8].col  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[9].col  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[10].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[11].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// 頂点データのポインタを先頭アドレスに戻す
		pVtx -= (g_nSelectTitle * 4);

		// 現在セレクト中の要素を代入
		g_nOldSelectTitle = g_nSelectTitle;

		// 処理を抜ける
		break;
	}

	// 演出のスキップ
	if (g_stateTitle != TITLESTATE_NONE && GetFade() == FADE_NONE)
	{ // タイトルが何もしない状態ではない且つ、フェードしていない場合

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // スキップ操作が行われた場合

			// 透明度を補正
			g_alphaTitle.fAlpha = 0.0f;

			// 透明度変更を何もしない状態にする
			g_alphaTitle.state = ALPHASTATE_NONE;

			// y座標を補正
			g_posTitle.y = TITLE_POS_Y;

			// 状態を何もしないに設定
			g_stateTitle = TITLESTATE_NONE;

			// サウンドの再生
			PlaySound(SOUND_LABEL_BGM_000);	// BGM (タイトル画面)
		}
	}

	// 頂点座標を更新 (タイトルアイコン)
	pVtx[0].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y - TITLE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_POS_X - TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_POS_X + TITLE_WIDTH, g_posTitle.y + TITLE_HEIGHT, 0.0f);

	// 頂点カラーの更新 (フェード)
	pVtx[16].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[17].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[18].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);
	pVtx[19].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaTitle.fAlpha);

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();
}

//======================================================================================================================
//	タイトル画面の描画処理
//======================================================================================================================
void DrawTitle(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	使用するソースファイルの描画
	//------------------------------------------------------------------------------------------------------------------
	// カメラの設定
	SetCamera();

	// メッシュドームの描画
	DrawMeshDome();

	// メッシュシリンダーの描画
	DrawMeshCylinder();

	// メッシュフィールドの描画
	DrawMeshField();

	// メッシュウォールの描画
	DrawMeshWall();

	// 影の描画
	DrawShadow();

	// オブジェクトの描画
	DrawObject();

	//------------------------------------------------------------------------------------------------------------------
	//	タイトルの描画
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{ // 使用するポリゴン数分繰り返す

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureTitle[nCntTitle]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTitle * 4, 2);
	}
}

//======================================================================================================================
//	タイトル画面の状態の取得処理
//======================================================================================================================
TITLESTATE GetTitleState(void)
{
	// タイトルの状態を返す
	return g_stateTitle;
}

//======================================================================================================================
//	タイトルの操作処理
//======================================================================================================================
void SelectTitle(void)
{
	if (GetFade() == FADE_NONE)
	{ // フェードしていない場合

		if (GetKeyboardTrigger(DIK_A) == true || GetKeyboardTrigger(DIK_LEFT) == true || GetJoyKeyTrigger(JOYKEY_LEFT, 0) == true)
		{ // 左移動の操作が行われた場合

			// 変数 (g_nSelectTitle) を 1ずつ引いていく ( 0の場合 1に戻す)
			g_nSelectTitle = (g_nSelectTitle + (TITLESELECT_MAX - 1)) % TITLESELECT_MAX;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (カーソル移動)
		}
		else if (GetKeyboardTrigger(DIK_D) == true || GetKeyboardTrigger(DIK_RIGHT) == true || GetJoyKeyTrigger(JOYKEY_RIGHT, 0) == true)
		{ // 右移動の操作が行われた場合

			// 変数 (g_nSelectTitle) を 1ずつ足していく ( 1の場合 0に戻す)
			g_nSelectTitle = (g_nSelectTitle + 1) % TITLESELECT_MAX;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (カーソル移動)
		}

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // 決定の操作が行われた場合

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_DEC_01);	// SE (決定01)

			switch (g_nSelectTitle)
			{ // 画面の移行先の確認
			case TITLESELECT_GAME:		// リトライの場合

				// モード選択 (ゲーム画面に移行)
				SetFade(MODE_GAME);

				// 処理を抜ける
				break;

			case TITLESELECT_TUTORIAL:		// 終了の場合

				// モード選択 (チュートリアル画面に移行)
				SetFade(MODE_TUTORIAL);

				// 処理を抜ける
				break;
			}
		}
	}
}