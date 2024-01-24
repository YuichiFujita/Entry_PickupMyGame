//======================================================================================================================
//
//	チュートリアル画面処理 [tutorial.cpp]
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

#include "tutorial.h"

#include "billboard.h"
#include "bullet.h"
#include "camera.h"
#include "effect.h"
#include "enemy.h"
#include "explosion.h"
#include "field.h"
#include "item.h"
#include "life.h"
#include "light.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshwall.h"
#include "object.h"
#include "particle.h"
#include "player.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "timer.h"
#include "tornado.h"
#include "wall.h"
#include "weed.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_TUTORIAL		(1)			// 使用するポリゴン数
#define END_TUTORIAL_TIME	(30)		// チュートリアル終了までの余韻フレーム

#define TUTORIAL_POS_X		(640.0f)	// チュートリアルの絶対座標 (x)
#define TUTORIAL_POS_Y		(570.0f)	// チュートリアルの絶対座標 (y)
#define TUTORIAL_WIDTH		(500.0f)	// チュートリアルの幅 / 2 (横)
#define TUTORIAL_HEIGHT		(110.0f)	// チュートリアルの幅 / 2 (高さ)

#define LESSON_00_CNT		(120)		// レッスン00の加算タイミング
#define LESSON_01_CNT		(3)			// レッスン01の加算タイミング
#define LESSON_02_CNT		(3)			// レッスン02の加算タイミング
#define LESSON_03_CNT		(1)			// レッスン03の加算タイミング
#define LESSON_04_CNT		(1)			// レッスン04の加算タイミング
#define LESSON_05_CNT		(1)			// レッスン05の加算タイミング
#define LESSON_06_CNT		(0)			// レッスン06の加算タイミング

#define SET_ENEMY_X_00		(180.0f)	// 敵の位置 (x)
#define SET_ENEMY_Y_00		(-80.0f)	// 敵の位置 (y)
#define SET_ENEMY_Z_00		(-90.0f)	// 敵の位置 (z)
#define SET_ENEMY_ROT_00	(0)			// 敵の向き
#define SET_ENEMY_TYPE_00	(1)			// 敵の種類

#define SET_ENEMY_X_01		(0.0f)		// 敵の位置 (x)
#define SET_ENEMY_Y_01		(-80.0f)	// 敵の位置 (y)
#define SET_ENEMY_Z_01		(90.0f)		// 敵の位置 (z)
#define SET_ENEMY_ROT_01	(0)			// 敵の向き
#define SET_ENEMY_TYPE_01	(0)			// 敵の種類

#define SET_ENEMY_X_02		(-180.0f)	// 敵の位置 (x)
#define SET_ENEMY_Y_02		(-80.0f)	// 敵の位置 (y)
#define SET_ENEMY_Z_02		(-90.0f)	// 敵の位置 (z)
#define SET_ENEMY_ROT_02	(0)			// 敵の向き
#define SET_ENEMY_TYPE_02	(1)			// 敵の種類

#define SET_OBJECT_X_00		(180.0f)	// オブジェクトの位置 (x)
#define SET_OBJECT_Y_00		(0.0f)		// オブジェクトの位置 (y)
#define SET_OBJECT_Z_00		(90.0f)		// オブジェクトの位置 (z)
#define SET_OBJECT_ROT_00	(315)		// オブジェクトの向き
#define SET_OBJECT_BREAK_00	(1)			// オブジェクトの壊れ方の種類
#define SET_OBJECT_TYPE_00	(7)			// オブジェクトの種類

#define SET_OBJECT_X_01		(0.0f)		// オブジェクトの位置 (x)
#define SET_OBJECT_Y_01		(0.0f)		// オブジェクトの位置 (y)
#define SET_OBJECT_Z_01		(180.0f)	// オブジェクトの位置 (z)
#define SET_OBJECT_ROT_01	(0)			// オブジェクトの向き
#define SET_OBJECT_BREAK_01	(1)			// オブジェクトの壊れ方の種類
#define SET_OBJECT_TYPE_01	(6)			// オブジェクトの種類

#define SET_OBJECT_X_02		(-180.0f)	// オブジェクトの位置 (x)
#define SET_OBJECT_Y_02		(0.0f)		// オブジェクトの位置 (y)
#define SET_OBJECT_Z_02		(90.0f)		// オブジェクトの位置 (z)
#define SET_OBJECT_ROT_02	(45)		// オブジェクトの向き
#define SET_OBJECT_BREAK_02	(1)			// オブジェクトの壊れ方の種類
#define SET_OBJECT_TYPE_02	(7)			// オブジェクトの種類

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureTutorial[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\tutorial000.png",	// レッスン0 (移動) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial001.png",	// レッスン1 (ジャンプ) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial002.png",	// レッスン2 (攻撃) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial003.png",	// レッスン3 (討伐) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial004.png",	// レッスン4 (破壊) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial005.png",	// レッスン5 (回復) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\tutorial006.png",	// レッスン6 (時間) のテクスチャ相対パス
};

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
int CheckNumEnemy(void);				// 現在の敵の総数の確認処理
int CheckNumBreakObject(void);			// 現在の壊れるオブジェクトの総数の確認処理
int CheckNumItem(void);					// 現在のアイテムの総数の確認処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureTutorial[LESSON_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorial = NULL;				// 頂点バッファへのポインタ

TUTORIALSTATE g_tutorialState;			// チュートリアルの状態
int           g_nLessonState;			// レッスンの状態
int           g_nCounterTutorialState;	// チュートリアルの状態管理カウンター
int           g_nCounterLessonState;	// レッスンの状態管理カウンター
bool          g_bTutorialEnd;			// モードの遷移状況

//======================================================================================================================
//	チュートリアル画面の初期化処理
//======================================================================================================================
void InitTutorial(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	チュートリアルの初期化
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D        *pVtx;						// 頂点情報へのポインタ

	for (int nCntTutorial = 0; nCntTutorial < LESSON_MAX; nCntTutorial++)
	{ // 使用するテクスチャ数分繰り返す

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, apTextureTutorial[nCntTutorial], &g_apTextureTutorial[nCntTutorial]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_TUTORIAL,	// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorial,
		NULL
	);

	// グローバル変数を初期化
	g_tutorialState         = TUTORIALSTATE_NORMAL;		// チュートリアルの状態
	g_nLessonState          = LESSON_00;				// レッスンの状態
	g_nCounterTutorialState = 0;						// チュートリアルの状態管理カウンター
	g_nCounterLessonState   = 0;						// レッスンの状態管理カウンター
	g_bTutorialEnd          = false;					// モードの遷移状況

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorial->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を設定
	pVtx[0].pos = D3DXVECTOR3(TUTORIAL_POS_X - TUTORIAL_WIDTH, TUTORIAL_POS_Y - TUTORIAL_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TUTORIAL_POS_X + TUTORIAL_WIDTH, TUTORIAL_POS_Y - TUTORIAL_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TUTORIAL_POS_X - TUTORIAL_WIDTH, TUTORIAL_POS_Y + TUTORIAL_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TUTORIAL_POS_X + TUTORIAL_WIDTH, TUTORIAL_POS_Y + TUTORIAL_HEIGHT, 0.0f);

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

	// 頂点バッファをアンロックする
	g_pVtxBuffTutorial->Unlock();

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

	//// 床の初期化
	//InitField();

	//// 壁の初期化
	//InitWall();

	// メッシュフィールドの初期化
	InitMeshField();

	// メッシュウォールの初期化
	InitMeshWall();

	// 影の初期化
	InitShadow();

	// 雑草の初期化
	InitWeed();

	// プレイヤーの初期化
	InitPlayer();

	// 敵の初期化
	InitEnemy();

	// オブジェクトの初期化
	InitObject();

	// アイテムの初期化
	InitItem();

	//// ビルボードの初期化
	//InitBillboard();

	// 弾の初期化
	InitBullet();

	// 爆発の初期化
	InitExplosion();

	// 斬撃の初期化
	InitSlash();

	// 竜巻の初期化
	InitTornado();

	// エフェクトの初期化
	InitEffect();

	// パーティクルの初期化
	InitParticle();

	// 体力ゲージの初期化
	InitLife();

	// タイマーの初期化
	InitTimer();

	// スコアの初期化
	InitScore();

	// ステージのセットアップ
	TxtSetStage();

	// サウンドの再生
	PlaySound(SOUND_LABEL_BGM_001);	// BGM (チュートリアル画面)
}

//======================================================================================================================
//	チュートリアル画面の終了処理
//======================================================================================================================
void UninitTutorial(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	チュートリアルの終了
	//------------------------------------------------------------------------------------------------------------------
	// テクスチャの破棄
	for (int nCntTutorial = 0; nCntTutorial < LESSON_MAX; nCntTutorial++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureTutorial[nCntTutorial] != NULL)
		{ // 変数 (g_apTextureTutorial) がNULLではない場合

			g_apTextureTutorial[nCntTutorial]->Release();
			g_apTextureTutorial[nCntTutorial] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTutorial != NULL)
	{ // 変数 (g_pVtxBuffTutorial) がNULLではない場合

		g_pVtxBuffTutorial->Release();
		g_pVtxBuffTutorial = NULL;
	}

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

	//// 床の終了
	//UninitField();

	//// 壁の終了
	//UninitWall();

	// メッシュフィールドの終了
	UninitMeshField();

	// メッシュウォールの終了
	UninitMeshWall();

	// 影の終了
	UninitShadow();

	// プレイヤーの終了
	UninitPlayer();

	// 敵の終了
	UninitEnemy();

	// 雑草の終了
	UninitWeed();

	// オブジェクトの終了
	UninitObject();

	// アイテムの終了
	UninitItem();

	//// ビルボードの終了
	//UninitBillboard();

	// 弾の終了
	UninitBullet();

	// 爆発の終了
	UninitExplosion();

	// 斬撃の終了
	UninitSlash();

	// 竜巻の終了
	UninitTornado();

	// エフェクトの終了
	UninitEffect();

	// パーティクルの終了
	UninitParticle();

	// 体力ゲージの終了
	UninitLife();

	// タイマーの終了
	UninitTimer();

	// スコアの終了
	UninitScore();

	// サウンドの停止
	StopSound();
}

//======================================================================================================================
//	チュートリアル画面の更新処理
//======================================================================================================================
void UpdateTutorial(void)
{
	if (g_bTutorialEnd == false)
	{ // 遷移設定がされていない場合

		if (GetTime() == 0)
		{ // タイマーが 0になった場合

			// 遷移設定がされた状態にする
			g_bTutorialEnd = true;

			// ゲーム画面の状態設定
			SetTutorialState(TUTORIALSTATE_END, END_TUTORIAL_TIME);	// 終了状態
		}
	}

	switch (g_nLessonState)
	{ // レッスンごとの処理
	case LESSON_00:	// レッスン0 (移動)

		// 無し

		// 処理を抜ける
		break;

	case LESSON_01:	// レッスン1 (ジャンプ)

		// 無し

		// 処理を抜ける
		break;

	case LESSON_02:	// レッスン2 (攻撃)

		// 無し

		// 処理を抜ける
		break;

	case LESSON_03:	// レッスン3 (討伐)

		if (CheckNumEnemy() == 0)
		{ // 敵の総数が 0の場合

			// レッスンの状態の加算
			AddLessonState();
		}

		// 処理を抜ける
		break;

	case LESSON_04:	// レッスン4 (破壊)

		if (CheckNumBreakObject() == 0)
		{ // 壊れるオブジェクトの総数が 0の場合

			// レッスンの状態の加算
			AddLessonState();
		}

		// 処理を抜ける
		break;

	case LESSON_05:	// レッスン5 (回復)

		if (CheckNumItem() == 0)
		{ // アイテムの総数が 0の場合

			// レッスンの状態の加算
			AddLessonState();
		}

		// 処理を抜ける
		break;

	case LESSON_06:	// レッスン6 (時間)

		// 無し

		// 処理を抜ける
		break;
	}

	switch (g_tutorialState)
	{ // 状態ごとの処理
	case TUTORIALSTATE_NORMAL:		// 通常状態

		// 無し

		// 処理を抜ける
		break;

	case TUTORIALSTATE_END:			// 終了状態

		if (g_nCounterTutorialState > 0)
		{ // カウンターが 0より大きい場合

			// カウンターを減算
			g_nCounterTutorialState--;
		}
		else
		{ // カウンターが 0以下の場合

			// モード選択 (タイトル画面に移行)
			SetFade(MODE_TITLE);
		}

		// 処理を抜ける
		break;
	}

	// ライトの更新
	UpdateLight();

	// カメラの更新
	UpdateCamera();

	// メッシュドームの更新
	UpdateMeshDome();

	// メッシュシリンダーの更新
	UpdateMeshCylinder();

	//// 床の更新
	//UpdateField();
	
	//// 壁の更新
	//UpdateWall();

	// メッシュフィールドの更新
	UpdateMeshField();

	// メッシュウォールの更新
	UpdateMeshWall();

	// プレイヤーの更新
	UpdatePlayer();

	// 敵の更新
	UpdateEnemy();

	// 雑草の更新
	UpdateWeed();

	// オブジェクトの更新
	UpdateObject();

	// アイテムの更新
	UpdateItem();

	//// ビルボードの更新
	//UpdateBillboard();

	// 弾の更新
	UpdateBullet();

	// 爆発の更新
	UpdateExplosion();

	// 竜巻の更新
	UpdateTornado();

	// 斬撃の更新
	UpdateSlash();

	// エフェクトの更新
	UpdateEffect();

	// パーティクルの更新
	UpdateParticle();

	// 体力ゲージの更新
	UpdateLife();

	if (GetLessonState() >= LESSON_06)
	{ // レッスン6に挑戦中、またはクリアしている場合

		// タイマーの更新
		UpdateTimer();
	}

	// スコアの更新
	UpdateScore();

	// 影の更新
	UpdateShadow();
}

//======================================================================================================================
//	チュートリアル画面の描画処理
//======================================================================================================================
void DrawTutorial(void)
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

	//// 床の描画
	//DrawField();

	//// 壁の描画
	//DrawWall();

	// メッシュフィールドの描画
	DrawMeshField();

	// メッシュウォールの描画
	DrawMeshWall();

	// 影の描画
	DrawShadow();

	// プレイヤーの描画
	DrawPlayer();

	// 敵の描画
	DrawEnemy();

	// 雑草の描画
	DrawWeed();

	// オブジェクトの描画
	DrawObject();

	// アイテムの描画
	DrawItem();

	//// ビルボードの描画
	//DrawBillboard();

	// エフェクトの描画
	DrawEffect();

	// パーティクルの描画
	DrawParticle();

	// 弾の描画
	DrawBullet();

	// 爆発の描画
	DrawExplosion();

	// 竜巻の描画
	DrawTornado();

	// 斬撃の描画
	DrawSlash();

	// 体力ゲージの描画
	DrawLife();

	if (GetLessonState() >= LESSON_06)
	{ // レッスン6に挑戦中、またはクリアしている場合

		// タイマーの描画
		DrawTimer();
	}

	// スコアの描画
	DrawScore();

	//------------------------------------------------------------------------------------------------------------------
	//	チュートリアルの描画
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTutorial, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureTutorial[g_nLessonState]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//======================================================================================================================
//	レッスンの状態の加算処理
//======================================================================================================================
void AddLessonState(void)
{
	// 変数配列を宣言
	int aNextLesson[LESSON_MAX] = 
	{ // 初期値
		LESSON_00_CNT,	// レッスン00の加算タイミング
		LESSON_01_CNT,	// レッスン01の加算タイミング
		LESSON_02_CNT,	// レッスン02の加算タイミング
		LESSON_03_CNT,	// レッスン03の加算タイミング
		LESSON_04_CNT,	// レッスン04の加算タイミング
		LESSON_05_CNT,	// レッスン05の加算タイミング
		LESSON_06_CNT	// レッスン06の加算タイミング
	};

	if (g_nLessonState < LESSON_MAX - 1)
	{ // 現在のレッスンがレッスンの最大数 -1より小さい場合

		// レッスンカウンターを加算
		g_nCounterLessonState++;

		if (g_nCounterLessonState >= aNextLesson[g_nLessonState])
		{ // レッスンカウンターが次のレッスンに移行する値以上になった場合

			// カウンターを初期化
			g_nCounterLessonState = 0;

			// レッスンを加算 (次のレッスンに進める)
			g_nLessonState++;

			switch (g_nLessonState)
			{ // レッスンごとの処理
			case LESSON_00:	// レッスン0 (移動)

				// 無し

				// 処理を抜ける
				break;

			case LESSON_01:	// レッスン1 (ジャンプ)

				// 無し

				// 処理を抜ける
				break;

			case LESSON_02:	// レッスン2 (攻撃)

				// 無し

				// 処理を抜ける
				break;

			case LESSON_03:	// レッスン3 (討伐)

				// 敵の設定
				SetEnemy
				( // 引数
					D3DXVECTOR3(SET_ENEMY_X_00, SET_ENEMY_Y_00, SET_ENEMY_Z_00),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_00), 0.0f),		// 向き
					SET_ENEMY_TYPE_00												// 種類
				);

				// 敵の設定
				SetEnemy
				( // 引数
					D3DXVECTOR3(SET_ENEMY_X_01, SET_ENEMY_Y_01, SET_ENEMY_Z_01),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_01), 0.0f),		// 向き
					SET_ENEMY_TYPE_01												// 種類
				);

				// 敵の設定
				SetEnemy
				( // 引数
					D3DXVECTOR3(SET_ENEMY_X_02, SET_ENEMY_Y_02, SET_ENEMY_Z_02),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_ENEMY_ROT_02), 0.0f),		// 向き
					SET_ENEMY_TYPE_02												// 種類
				);

				// 処理を抜ける
				break;

			case LESSON_04:	// レッスン4 (破壊)

				// オブジェクトの設定
				SetObject
				( // 引数
					D3DXVECTOR3(SET_OBJECT_X_00, SET_OBJECT_Y_00, SET_OBJECT_Z_00),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_00), 0.0f),		// 向き
					SET_OBJECT_BREAK_00,											// 壊れ方の種類
					SET_OBJECT_TYPE_00												// オブジェクトの種類
				);

				// オブジェクトの設定
				SetObject
				( // 引数
					D3DXVECTOR3(SET_OBJECT_X_01, SET_OBJECT_Y_01, SET_OBJECT_Z_01),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_01), 0.0f),		// 向き
					SET_OBJECT_BREAK_01,											// 壊れ方の種類
					SET_OBJECT_TYPE_01												// オブジェクトの種類
				);

				// オブジェクトの設定
				SetObject
				( // 引数
					D3DXVECTOR3(SET_OBJECT_X_02, SET_OBJECT_Y_02, SET_OBJECT_Z_02),	// 位置
					D3DXVECTOR3(0.0f, D3DXToRadian(SET_OBJECT_ROT_02), 0.0f),		// 向き
					SET_OBJECT_BREAK_02,											// 壊れ方の種類
					SET_OBJECT_TYPE_02												// オブジェクトの種類
				);

				// 処理を抜ける
				break;

			case LESSON_05:	// レッスン5 (回復)

				// 無し

				// 処理を抜ける
				break;

			case LESSON_06:	// レッスン6 (時間)

				// 無し

				// 処理を抜ける
				break;
			}

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_DEC_00);	// SE (決定00)
		}
	}
}

//======================================================================================================================
//	チュートリアル画面の状態設定処理
//======================================================================================================================
void SetTutorialState(TUTORIALSTATE state, int nCounter)
{
	// 引数の状態を代入
	g_tutorialState = state;

	// 引数の余韻を代入
	g_nCounterTutorialState = nCounter;
}

//======================================================================================================================
//	レッスンの状態の取得処理
//======================================================================================================================
TUTORIALSTATE GetLessonState(void)
{
	// レッスンの状態を返す
	return (TUTORIALSTATE)g_nLessonState;

}

//======================================================================================================================
//	現在の敵の総数の確認処理
//======================================================================================================================
int CheckNumEnemy(void)
{
	// 変数を宣言
	int nNumEnemy = 0;	// 敵の総数の確認用

	// ポインタを宣言
	Enemy *pEnemy = GetEnemy();		// 敵の情報

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (pEnemy->bUse == true)
		{ // 敵が使用されている場合

			// カウンターを加算
			nNumEnemy++;
		}
	}

	// 変数の値を返す
	return nNumEnemy;	// 敵の総数
}

//======================================================================================================================
//	現在の壊れるオブジェクトの総数の確認処理
//======================================================================================================================
int CheckNumBreakObject(void)
{
	// 変数を宣言
	int nNumObject = 0;	// 壊れるオブジェクトの総数の確認用

	// ポインタを宣言
	Object *pObject = GetObjectData();	// オブジェクトの情報

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // オブジェクトの最大表示数分繰り返す

		if (pObject->bUse == true && pObject->nBreakType != BREAKTYPE_NONE)
		{ // オブジェクトが使用されている且つ、オブジェクトが壊れない設定ではない場合

			// カウンターを加算
			nNumObject++;
		}
	}

	// 変数の値を返す
	return nNumObject;	// 壊れるオブジェクトの総数
}

//======================================================================================================================
//	現在のアイテムの総数の確認処理
//======================================================================================================================
int CheckNumItem(void)
{
	// 変数を宣言
	int nNumItem = 0;	// アイテムの総数の確認用

	// ポインタを宣言
	Item *pItem = GetItem();	// アイテムの情報

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{ // アイテムの最大表示数分繰り返す

		if (pItem->bUse == true)
		{ // アイテムが使用されている場合

			// カウンターを加算
			nNumItem++;
		}
	}

	// 変数の値を返す
	return nNumItem;	// アイテムの総数
}