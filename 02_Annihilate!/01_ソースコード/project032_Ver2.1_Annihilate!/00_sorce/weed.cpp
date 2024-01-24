//======================================================================================================================
//
//	雑草処理 [weed.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "weed.h"
#include "enemy.h"
#include "particle.h"
#include "player.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define TIP_MOVE		(0.6f)		// 雑草の先端の移動量
#define TIP_POS_DIS		(20.0f)		// 雑草が踏まれた時の先端位置の距離

#define WEED_GROW_CNT	(500)		// 雑草の成長までの時間
#define WEED_SIZE_LEVEL	(0.4f)		// 成長段階のサイズ加算量の倍率
#define SHADOW_WEED		(35.0f)		// 雑草の影の半径
#define SCORE_WEED		(10)		// 雑草のスコア加算量

#define WEED_LIFE		((int)(SLA_DMG_WEED * 0.5f))	// 雑草の体力
#define WEED_TIP_LIMIT	(WEED_WIDTH * 1.5f)				// 先端位置の移動量の反転位置
#define ADD_WEED_WIDTH	(0.2f)							// 1Fごとのサイズ変更量 (横幅)
#define ADD_WEED_HEIGHT	(ADD_WEED_WIDTH * 2)			// 1Fごとのサイズ変更量 (縦幅)

#define DAMAGE_TIME_WEED	(PLAY_SLASH_LIFE)			// ダメージ状態を保つ時間
#define UNR_TIME_WEED		(PLAY_SLASH_LIFE - 10)		// 無敵状態に変更する時間

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureWeed[] =	// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\weed000.png",	// 雑草 (通常) のテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_WEED)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_WEED_NORMAL = 0,		// 雑草 (通常)
	TEXTURE_WEED_MAX,				// この列挙型の総数
} TEXTURE_WEED;

//**********************************************************************************************************************
//	列挙型定義 (GROWSTATE)
//**********************************************************************************************************************
typedef enum
{
	GROWSTATE_SMALL = 0,			// 第一状態 (小)
	GROWSTATE_MEDIUM,				// 第二状態 (中)
	GROWSTATE_LARGE,				// 第三状態 (大)
	GROWSTATE_MAX,					// この列挙型の総数
} GROWSTATE;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
bool CollisionPlayer(Weed *pWeed);				// 雑草とプレイヤーの当たり判定
void TreadWeed(Weed *pWeed, int nCntWeed);		// 雑草が踏まれている際の処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureWeed[TEXTURE_WEED_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWeed = NULL;					// 頂点バッファへのポインタ

Weed  g_aWeed[MAX_WEED];	// 雑草の情報
float g_fMoveTip;			// 先端の移動量
float g_fPosTip;			// 先端の位置
int   g_nNumWeed;			// 雑草の総数
int   g_nKillWeed;			// 雑草の刈り取り数

//======================================================================================================================
//	雑草の初期化処理
//======================================================================================================================
void InitWeed(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntWeed = 0; nCntWeed < TEXTURE_WEED_MAX; nCntWeed++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureWeed[nCntWeed], &g_apTextureWeed[nCntWeed]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_WEED,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffWeed,
		NULL
	);

	// グローバル変数の初期化
	g_fMoveTip  = TIP_MOVE;		// 先端の移動量
	g_fPosTip   = 0.0f;			// 先端の位置
	g_nNumWeed  = 0;			// 雑草の総数
	g_nKillWeed = 0;			// 雑草の刈り取り数

	// 雑草の情報の初期化
	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		g_aWeed[nCntWeed].pos           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aWeed[nCntWeed].state         = ACTIONSTATE_SPAWN;				// 状態
		g_aWeed[nCntWeed].nGrowState    = GROWSTATE_SMALL;					// 成長状態
		g_aWeed[nCntWeed].nLife         = WEED_LIFE;						// 体力
		g_aWeed[nCntWeed].nCounterState = 0;								// 状態管理カウンター
		g_aWeed[nCntWeed].nCounterGrow  = 0;								// 成長管理カウンター
		g_aWeed[nCntWeed].fWidth        = WEED_WIDTH;						// 横幅
		g_aWeed[nCntWeed].fHeight       = WEED_HEIGHT;						// 縦幅
		g_aWeed[nCntWeed].nShadowID     = NONE_SHADOW;						// 影のインデックス
		g_aWeed[nCntWeed].bUse          = false;							// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-WEED_WIDTH, WEED_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( WEED_WIDTH, WEED_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-WEED_WIDTH, 0.0f,        0.0f);
		pVtx[3].pos = D3DXVECTOR3( WEED_WIDTH, 0.0f,        0.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	雑草の終了処理
//======================================================================================================================
void UninitWeed(void)
{
	// テクスチャの破棄
	for (int nCntWeed = 0; nCntWeed < TEXTURE_WEED_MAX; nCntWeed++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureWeed[nCntWeed] != NULL)
		{ // 変数 (g_apTextureWeed) がNULLではない場合

			g_apTextureWeed[nCntWeed]->Release();
			g_apTextureWeed[nCntWeed] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffWeed != NULL)
	{ // 変数 (g_pVtxBuffWeed) がNULLではない場合

		g_pVtxBuffWeed->Release();
		g_pVtxBuffWeed = NULL;
	}
}

//======================================================================================================================
//	雑草の更新処理
//======================================================================================================================
void UpdateWeed(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	// 雑草の先端位置に移動量を加算
	g_fPosTip += g_fMoveTip;

	if (g_fPosTip - WEED_WIDTH < -WEED_TIP_LIMIT
	||  g_fPosTip + WEED_WIDTH >  WEED_TIP_LIMIT)
	{ // 先端位置が右、または左に行き過ぎた場合

		// 移動量を反転
		g_fMoveTip *= -1;
	}

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (g_aWeed[nCntWeed].bUse == true)
		{ // 雑草が使用されている場合

			switch (g_aWeed[nCntWeed].state)
			{ // 状態ごとの処理
			case ACTIONSTATE_SPAWN:		// 出現状態

				if (g_aWeed[nCntWeed].pos.y < 0.0f)
				{ // 出現中の場合

					// 地中からスポーンさせる
					g_aWeed[nCntWeed].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // 出現しきった場合

					// 敵の座標 (y) を補正
					g_aWeed[nCntWeed].pos.y = 0.0f;

					// 通常状態にする
					g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
				}

				// 処理を抜ける
				break;

			case ACTIONSTATE_GROW:		// 成長状態

				// 雑草のサイズを加算
				g_aWeed[nCntWeed].fWidth  += ADD_WEED_WIDTH;	// 横幅
				g_aWeed[nCntWeed].fHeight += ADD_WEED_HEIGHT;	// 縦幅

				if (g_aWeed[nCntWeed].fWidth >= WEED_WIDTH * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1))))
				{ // 横幅が一定サイズ以上になった場合

					// 雑草のサイズを補正
					g_aWeed[nCntWeed].fWidth  = WEED_WIDTH  * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1)));
					g_aWeed[nCntWeed].fHeight = WEED_HEIGHT * (1.0f + (WEED_SIZE_LEVEL * (g_aWeed[nCntWeed].nGrowState + 1)));

					// 成長段階を加算
					g_aWeed[nCntWeed].nGrowState++;

					// 体力を加算
					g_aWeed[nCntWeed].nLife += WEED_LIFE;

					// 通常状態にする
					g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
				}

				// 処理を抜ける
				break;

			default:					// それ以外の状態

				//--------------------------------------------------------------------------------------------------
				//	状態の更新
				//--------------------------------------------------------------------------------------------------
				if (g_aWeed[nCntWeed].nCounterState > 0)
				{ // カウンターが 0より大きい場合

					// カウンターを減算
					g_aWeed[nCntWeed].nCounterState--;

					if (g_aWeed[nCntWeed].nCounterState == UNR_TIME_WEED)
					{ // カウンターが一定値の場合

						// 無敵状態にする
						g_aWeed[nCntWeed].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aWeed[nCntWeed].nCounterState <= 0)
					{ // カウンターが 0以下の場合

						// 通常状態にする
						g_aWeed[nCntWeed].state = ACTIONSTATE_NORMAL;
					}
				}

				//--------------------------------------------------------------------------------------------------
				//	成長の更新
				//--------------------------------------------------------------------------------------------------
				if (g_aWeed[nCntWeed].nGrowState < GROWSTATE_LARGE)
				{ // 成長段階が最大成長より小さい場合

					// カウンターを加算
					g_aWeed[nCntWeed].nCounterGrow++;

					if (g_aWeed[nCntWeed].nCounterGrow >= WEED_GROW_CNT)
					{ // カウンターが一定値以上の場合

						// カウンターを初期化
						g_aWeed[nCntWeed].nCounterGrow = 0;

						// 成長状態にする
						g_aWeed[nCntWeed].state = ACTIONSTATE_GROW;
					}
				}

				// 処理を抜ける
				break;
			}

			//------------------------------------------------------------------------------------------------------
			//	雑草の踏まれ・揺らめき処理
			//------------------------------------------------------------------------------------------------------
			if (CollisionPlayer(&g_aWeed[nCntWeed]) == true)
			{ // プレイヤーに踏まれている場合

				// 雑草の踏まれ処理
				TreadWeed(&g_aWeed[nCntWeed], nCntWeed);
			}
			else
			{ // プレイヤーに踏まれていない場合

				// 雑草の先端位置を動かす (揺らめき)
				pVtx[0].pos = D3DXVECTOR3(g_fPosTip - g_aWeed[nCntWeed].fWidth, g_aWeed[nCntWeed].fHeight, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(g_fPosTip + g_aWeed[nCntWeed].fWidth, g_aWeed[nCntWeed].fHeight, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-g_aWeed[nCntWeed].fWidth,            0.0f,                      0.0f);
				pVtx[3].pos = D3DXVECTOR3( g_aWeed[nCntWeed].fWidth,            0.0f,                      0.0f);
			}

			//--------------------------------------------------------------------------------------------------------------
			//	影の更新
			//--------------------------------------------------------------------------------------------------------------
			// 影の位置設定
			SetPositionShadow
			( // 引数
				g_aWeed[nCntWeed].nShadowID,	// 影のインデックス
				g_aWeed[nCntWeed].pos			// 位置
			);
		}

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	雑草の描画処理
//======================================================================================================================
void DrawWeed(void)
{
	// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス取得用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// カリングを無効にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// αテストの参照値設定

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (g_aWeed[nCntWeed].bUse == true)
		{ // 雑草が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aWeed[nCntWeed].mtxWorld);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aWeed[nCntWeed].mtxWorld, NULL, &mtxView);	// 逆行列を求める
			g_aWeed[nCntWeed].mtxWorld._41 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._42 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._43 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._21 = 0.0f;
			g_aWeed[nCntWeed].mtxWorld._23 = 0.3f;
			g_aWeed[nCntWeed].mtxWorld._24 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aWeed[nCntWeed].pos.x, g_aWeed[nCntWeed].pos.y, g_aWeed[nCntWeed].pos.z);
			D3DXMatrixMultiply(&g_aWeed[nCntWeed].mtxWorld, &g_aWeed[nCntWeed].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aWeed[nCntWeed].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffWeed, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureWeed[TEXTURE_WEED_NORMAL]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWeed * 4, 2);
		}
	}

	// カリングを有効にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//======================================================================================================================
//	雑草の設定処理
//======================================================================================================================
void SetWeed(D3DXVECTOR3 pos)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (g_aWeed[nCntWeed].bUse == false)
		{ // 雑草が使用されていない場合

			// 引数を代入
			g_aWeed[nCntWeed].pos   = pos;						// 位置
			g_aWeed[nCntWeed].state = ACTIONSTATE_SPAWN;		// 状態

			// 構造体の要素を初期化
			g_aWeed[nCntWeed].nGrowState    = GROWSTATE_SMALL;	// 成長状態
			g_aWeed[nCntWeed].nLife         = WEED_LIFE;		// 体力
			g_aWeed[nCntWeed].nCounterState = 0;				// 状態管理カウンター
			g_aWeed[nCntWeed].nCounterGrow  = 0;				// 成長管理カウンター
			g_aWeed[nCntWeed].fWidth        = WEED_WIDTH;		// 横幅
			g_aWeed[nCntWeed].fHeight       = WEED_HEIGHT;		// 縦幅

			// 使用している状態にする
			g_aWeed[nCntWeed].bUse = true;

			// 影のインデックスを設定
			g_aWeed[nCntWeed].nShadowID = SetShadow(0.2f, SHADOW_WEED, &g_aWeed[nCntWeed].nShadowID, &g_aWeed[nCntWeed].bUse);

			// 影の位置設定
			SetPositionShadow(g_aWeed[nCntWeed].nShadowID, g_aWeed[nCntWeed].pos);

			// 雑草の総数を加算
			g_nNumWeed++;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-WEED_WIDTH, WEED_HEIGHT, 0.0f);
			pVtx[1].pos = D3DXVECTOR3( WEED_WIDTH, WEED_HEIGHT, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-WEED_WIDTH, 0.0f,        0.0f);
			pVtx[3].pos = D3DXVECTOR3( WEED_WIDTH, 0.0f,        0.0f);

			// 処理を抜ける
			break;
		}

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	雑草のダメージ判定
//======================================================================================================================
void HitWeed(Weed *pWeed, int nDamage)
{
	if (pWeed->state == ACTIONSTATE_NORMAL || pWeed->state == ACTIONSTATE_GROW)
	{ // 雑草が通常状態、または成長状態の場合

		// 引数のダメージ分を体力から減算
		pWeed->nLife -= nDamage;

		if (pWeed->nLife > 0)
		{ // 体力が残っている場合

			// ダメージ状態にする
			pWeed->state = ACTIONSTATE_DAMAGE;

			// パーティクルの設定
			SetParticle
			( // 引数
				pWeed->pos,							// 位置
				D3DXCOLOR(0.0f, 0.9f, 0.4f, 1.0f),	// 色
				PARTICLETYPE_WEED,					// 種類
				SPAWN_PARTICLE_WEED,				// エフェクト数
				1									// 寿命
			);

			// カウンターを設定
			pWeed->nCounterState = DAMAGE_TIME_WEED;
		}
		else
		{ // 体力が尽きた場合

			// スコアの加算
			AddScore(SCORE_WEED);

			// パーティクルの設定
			SetParticle
			( // 引数
				pWeed->pos,							// 位置
				D3DXCOLOR(0.6f, 1.0f, 0.4f, 1.0f),	// 色
				PARTICLETYPE_SLASHWEED,				// 種類
				SPAWN_PARTICLE_SLASHWEED,			// エフェクト数
				1									// 寿命
			);

			// 使用していない状態にする
			pWeed->bUse = false;

			// 雑草の総数を減算
			g_nNumWeed--;

			// 雑草の刈り取り数を加算
			g_nKillWeed++;
		}
	}
}

//======================================================================================================================
//	雑草とプレイヤーの当たり判定
//======================================================================================================================
bool CollisionPlayer(Weed *pWeed)
{
	// 変数を宣言
	float fLength = 0.0f;				// 雑草とプレイヤーとの間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// 雑草とプレイヤーとの間の距離を求める
		fLength = (pWeed->pos.x - pPlayer->pos.x) * (pWeed->pos.x - pPlayer->pos.x)
				+ (pWeed->pos.z - pPlayer->pos.z) * (pWeed->pos.z - pPlayer->pos.z);

		if (fLength < ((pWeed->fWidth + PLAY_WIDTH) * (pWeed->fWidth + PLAY_WIDTH))	// 円の当たり判定
		&&  pWeed->pos.y + pWeed->fHeight > pPlayer->pos.y							// 縦の判定 (下)
		&&  pWeed->pos.y                  < pPlayer->pos.y + PLAY_HEIGHT)			// 縦の判定 (上)
		{ // 雑草がプレイヤーに当たっている場合

			// 踏まれている場合 true を返す
			return true;
		}
	}

	// 踏まれていない場合 false を返す
	return false;
}

//======================================================================================================================
//	雑草が踏まれている際の処理
//======================================================================================================================
void TreadWeed(Weed *pWeed, int nCntWeed)
{
	// 変数を宣言
	D3DXVECTOR3 tipPos;			// 先端位置の代入用
	float       fRot = 0.0f;	// プレイヤーから見た雑草の方向

	// ポインタを宣言
	VERTEX_3D  *pVtx;					// 頂点情報へのポインタ
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWeed->Lock(0, 0, (void**)&pVtx, 0);

	// ポインタのアドレスを移動
	pVtx += nCntWeed * 4;

	// プレイヤーから見た雑草の方向を求める
	fRot = atan2f(pWeed->pos.x - pPlayer->pos.x, pWeed->pos.z - pPlayer->pos.z);

	// 先端の絶対位置を設定
	tipPos.x = sinf(fRot) * TIP_POS_DIS;
	tipPos.z = cosf(fRot) * TIP_POS_DIS;

	// 雑草の先端位置を動かす
	pVtx[0].pos = D3DXVECTOR3(tipPos.x - pWeed->fWidth, pWeed->fHeight, tipPos.z - pWeed->fWidth);
	pVtx[1].pos = D3DXVECTOR3(tipPos.x + pWeed->fWidth, pWeed->fHeight, tipPos.z + pWeed->fWidth);
	pVtx[2].pos = D3DXVECTOR3(-pWeed->fWidth,           0.0f,           0.0f);
	pVtx[3].pos = D3DXVECTOR3( pWeed->fWidth,           0.0f,           0.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffWeed->Unlock();
}

//======================================================================================================================
//	雑草の余り数の取得処理
//======================================================================================================================
int GetLeaveWeed(void)
{
	// 雑草の総数を返す
	return g_nNumWeed;
}

//======================================================================================================================
//	雑草の取得処理
//======================================================================================================================
Weed *GetWeed(void)
{
	// 雑草の情報の先頭アドレスを返す
	return &g_aWeed[0];
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	雑草の総数取得処理
//**********************************************************************************************************************
int GetNumWeed(void)
{
	// 変数を宣言
	int nNumWeed = 0;	// 雑草の総数の確認用

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (g_aWeed[nCntWeed].bUse == true)
		{ // 雑草が使用されている場合

			// カウンターを加算
			nNumWeed++;
		}
	}

	// 変数の値を返す
	return nNumWeed;	// 雑草の総数
}
#endif