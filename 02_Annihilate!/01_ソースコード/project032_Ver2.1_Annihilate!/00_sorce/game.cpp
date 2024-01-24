//======================================================================================================================
//
//	ゲーム画面処理 [game.cpp]
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

#include "game.h"

#include "billboard.h"
#include "bullet.h"
#include "camera.h"
#include "continue.h"
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
#include "pause.h"
#include "player.h"
#include "ranking.h"
#include "result.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "timer.h"
#include "wall.h"
#include "weed.h"

#include "orbit.h"
#include "tornado.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_GAME		(1)		// 使用するポリゴン数
#define RESULT_TIME		(40)	// リザルトまでの余韻フレーム

#define ADD_ALPHA_GAME	(0.04f)	// α値の加算量
#define MUL_FADE_ALPHA	(0.5f)	// フェード背景の透明度の減算倍率

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGame = NULL;	// 頂点バッファへのポインタ

GAMESTATE  g_gameState;			// ゲームの状態
Alpha      g_alphaGame;			// 透明度の状態
int        g_nCounterGameState;	// 状態管理カウンター
bool       g_bPause;			// ポーズ状態の ON / OFF
bool       g_bGameEnd;			// モードの遷移状況

//======================================================================================================================
//	ゲーム画面の初期化処理
//======================================================================================================================
void InitGame(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	ゲームの初期化
	//------------------------------------------------------------------------------------------------------------------
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D        *pVtx;						// 頂点情報へのポインタ

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_2D) * 4 * MAX_GAME,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffGame,
		NULL
	);

	// グローバル変数を初期化
	g_gameState         = GAMESTATE_NORMAL;		// ゲームの状態
	g_nCounterGameState = RESULT_TIME;			// 状態管理カウンター
	g_bPause            = false;				// ポーズ状態の ON / OFF
	g_bGameEnd          = false;				// モードの遷移状況

	// 透明度の状態を初期化
	g_alphaGame.state  = ALPHASTATE_WAIT;		// 透明度の変更状態
	g_alphaGame.fAlpha = 0.0f;					// 背景の透明度

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffGame->Lock(0, 0, (void**)&pVtx, 0);

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
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha);

	// 頂点バッファをアンロックする
	g_pVtxBuffGame->Unlock();

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

	// プレイヤーの初期化
	InitPlayer();

	// 敵の初期化
	InitEnemy();

	// 雑草の初期化
	InitWeed();

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

	// 軌跡の初期化
	InitOrbit();

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

	// ポーズの初期化
	InitPause();

	// リザルトの初期化
	InitResult();

	// ランキングの初期化
	InitRanking();

	// コンテニューの初期化
	InitContinue();

	// ステージのセットアップ
	TxtSetStage();

	// サウンドの再生
	PlaySound(SOUND_LABEL_BGM_002);	// BGM (ゲーム画面)
}

//======================================================================================================================
//	ゲーム画面の終了処理
//======================================================================================================================
void UninitGame(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	ゲームの終了
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファの破棄
	if (g_pVtxBuffGame != NULL)
	{ // 変数 (g_pVtxBuffGame) がNULLではない場合

		g_pVtxBuffGame->Release();
		g_pVtxBuffGame = NULL;
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

	// 軌跡の終了
	UninitOrbit();

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

	// ポーズの終了
	UninitPause();

	// リザルトの終了
	UninitResult();

	// ランキングの終了
	UninitRanking();

	// コンテニューの終了
	UninitContinue();

	// サウンドの停止
	StopSound();
}

//======================================================================================================================
//	ゲーム画面の更新処理
//======================================================================================================================
void UpdateGame(void)
{
	// ポインタを宣言
	VERTEX_2D  *pVtx;						// 頂点情報へのポインタ
	Player *pPlayer = GetPlayer();		// プレイヤーの情報

	if (g_bGameEnd == false)
	{ // 遷移設定がされていない場合

		if (GetTime() == 0 || pPlayer->bUse == false)
		{ // タイマーが 0になった、もしくはプレイヤーが死亡した場合

			// 遷移設定がされた状態にする
			g_bGameEnd = true;

			// トータルスコアの設定
			SetTotalScore();

			// ランキングの設定
			SetRanking();

			// ゲーム画面の状態設定
			SetGameState(GAMESTATE_END, RESULT_TIME);	// 終了状態

			if (GetTime() == 0)
			{ // タイマーが 0の場合

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_RES_00);		// SE (リザルト移行00)
			}
			else if (pPlayer->bUse == false)
			{ // プレイヤーが死亡している場合

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_RES_01);		// SE (リザルト移行01)
			}
		}
	}

	switch (g_gameState)
	{ // 状態ごとの処理
	case GAMESTATE_NORMAL:		// 通常状態

		if (GetFade() == FADE_NONE)
		{ // フェードしていない場合

			if (GetKeyboardTrigger(DIK_P) == true || GetJoyKeyTrigger(JOYKEY_START, 0) == true)
			{ // ポーズの操作が行われた場合

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_DEC_01);				// SE (決定01)

				// ポーズの選択項目の再設定
				ResetPauseSelect();

				// 変数 (g_bPause) 真偽を代入する (三項演算子)	// 条件式が真なら true 、偽なら false を代入
				g_bPause = (g_bPause == false) ? true : false;	// ※ 条件式：() の中
			}
		}

		// 処理を抜ける
		break;

	case GAMESTATE_END:			// 終了状態

		switch (g_alphaGame.state)
		{ // 透明度の変更状態ごとの処理
		case ALPHASTATE_WAIT:	// 透明度の変更待機状態

			if (g_nCounterGameState > 0)
			{ // カウンターが 0より大きい場合

				// カウンターを減算
				g_nCounterGameState--;
			}
			else
			{ // カウンターが 0以下の場合

				// 透明度の変更状態にする
				g_alphaGame.state = ALPHASTATE_CHANGE;
			}

			// 処理を抜ける
			break;

		case ALPHASTATE_CHANGE:	// 透明度の変更状態

			if (g_alphaGame.fAlpha < 1.0f)
			{ // 不透明度が 1.0f より小さい場合

				// 不透明度を上げる
				g_alphaGame.fAlpha += ADD_ALPHA_GAME;
			}
			else
			{ // 不透明度が 1.0f 以上の場合

				// 不透明度を補正
				g_alphaGame.fAlpha = 1.0f;

				// 何もしない状態にする
				g_alphaGame.state = ALPHASTATE_NONE;
			}

			// 処理を抜ける
			break;

		case ALPHASTATE_NONE:	// 何もしていない状態

			// 無し

			// 処理を抜ける
			break;
		}

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffGame->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点カラーの更新
		pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, g_alphaGame.fAlpha * MUL_FADE_ALPHA);

		// 頂点バッファをアンロックする
		g_pVtxBuffGame->Unlock();

		// リザルトの更新
		UpdateResult(g_alphaGame.fAlpha);

		// ランキングの更新
		UpdateRanking(g_alphaGame.fAlpha);

		// コンテニューの更新
		UpdateContinue(g_alphaGame.fAlpha, g_alphaGame.state);

		// 処理を抜ける
		break;
	}

	if (g_bPause == false)
	{ // ポーズ状態ではない場合

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

		if (g_gameState == GAMESTATE_NORMAL)
		{ // ゲーム画面が通常状態の場合

			// プレイヤーの更新
			UpdatePlayer();

			// 敵の更新
			UpdateEnemy();
		}

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

		// 斬撃の更新
		UpdateSlash();

		// 竜巻の更新
		UpdateTornado();

		// エフェクトの更新
		UpdateEffect();

		// パーティクルの更新
		UpdateParticle();

		// 体力ゲージの更新
		UpdateLife();

		// タイマーの更新
		UpdateTimer();

		// スコアの更新
		UpdateScore();

		// 影の更新
		UpdateShadow();

		// 軌跡の更新
		UpdateOrbit();
	}
	else
	{ // ポーズ状態の場合

		// ポーズの更新
		UpdatePause();
	}
}

//======================================================================================================================
//	ゲーム画面の描画処理
//======================================================================================================================
void DrawGame(void)
{
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

	// 軌跡の描画
	DrawOrbit();

	// 竜巻の描画
	DrawTornado();

	// 斬撃の描画
	DrawSlash();

	// 体力ゲージの描画
	DrawLife();

	// タイマーの描画
	DrawTimer();

	// スコアの描画
	DrawScore();

	if (g_bPause == true)
	{ // ポーズ状態の場合

		// ポーズの描画
		DrawPause();
	}

	switch (g_gameState)
	{ // 状態ごとの処理
	case GAMESTATE_NORMAL:

		// 無し

		// 処理を抜ける
		break;

	case GAMESTATE_END:

		//--------------------------------------------------------------------------------------------------------------
		//	背景の描画
		//--------------------------------------------------------------------------------------------------------------
		// ポインタを宣言
		LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffGame, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//--------------------------------------------------------------------------------------------------------------
		//	リザルト・ランキング・コンテニューの描画
		//--------------------------------------------------------------------------------------------------------------
		// リザルトの描画
		DrawResult(g_alphaGame.fAlpha);

		// ランキングの描画
		DrawRanking(g_alphaGame.fAlpha);

		// コンテニューの描画
		DrawContinue(g_alphaGame.fAlpha);

		// 処理を抜ける
		break;
	}
}

//======================================================================================================================
//	ゲーム画面の状態設定処理
//======================================================================================================================
void SetGameState(GAMESTATE state, int nCounter)
{
	// 引数の状態を代入
	g_gameState = state;

	// 引数の余韻を代入
	g_nCounterGameState = nCounter;
}

//======================================================================================================================
//	ポーズ状態の設定処理
//======================================================================================================================
void SetEnablePause(bool bPause)
{
	// 現在のポーズ状態に引数のポーズ状態を代入
	g_bPause = bPause;
}

//======================================================================================================================
//	ゲーム画面の状態取得処理
//======================================================================================================================
GAMESTATE GetGameState(void)
{
	// ゲームの状態を返す
	return g_gameState;
}

//======================================================================================================================
//	ポーズ状態の取得処理
//======================================================================================================================
bool GetPause(void)
{
	// ポーズ状態の真偽を返す
	return g_bPause;
}