//======================================================================================================================
//
//	パーティクル処理 [particle.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "particle.h"
#include "effect.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_PARTICLE	(128)		// 使用するポリゴン数 (パーティクルの最大数)

//**********************************************************************************************************************
//	構造体定義 (Particle)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3  pos;				// 位置
	D3DXCOLOR    col;				// 色
	PARTICLETYPE type;				// 種類
	int          nSpawn;			// エフェクト数
	int          nLife;				// 寿命
	bool         bUse;				// 使用状況
}Particle;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
Particle g_aParticle[MAX_PARTICLE];	// パーティクルの情報

//======================================================================================================================
//	パーティクルの初期化処理
//======================================================================================================================
void InitParticle(void)
{
	// パーティクルの情報の初期化
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{ // パーティクルの最大表示数分繰り返す

		g_aParticle[nCntParticle].pos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
		g_aParticle[nCntParticle].col    = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色
		g_aParticle[nCntParticle].type   = PARTICLETYPE_NONE;					// 種類
		g_aParticle[nCntParticle].nSpawn = 0;									// エフェクト数
		g_aParticle[nCntParticle].nLife  = 0;									// 寿命
		g_aParticle[nCntParticle].bUse   = false;								// 使用状況
	}
}

//======================================================================================================================
//	パーティクルの終了処理
//======================================================================================================================
void UninitParticle(void)
{
	
}

//======================================================================================================================
//	パーティクルの更新処理
//======================================================================================================================
void UpdateParticle(void)
{
	// 変数を宣言
	D3DXVECTOR3 move;	// エフェクトの移動量の代入用

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{ // パーティクルの最大表示数分繰り返す

		if (g_aParticle[nCntParticle].bUse == true)
		{ // パーティクルが使用されている場合

			for (int nCntAppear = 0; nCntAppear < g_aParticle[nCntParticle].nSpawn; nCntAppear++)
			{ // パーティクルの 1Fで生成されるエフェクト数分繰り返す

				switch (g_aParticle[nCntParticle].type)
				{ // パーティクルの種類ごとの更新
				case PARTICLETYPE_EXPLOSION:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 4.0f;
					move.y *= 4.0f;
					move.z *= 4.0f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_NORMAL,						// 種類
						30,										// 寿命
						40.0f,									// 半径
						1.2f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_DAMAGE:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 2.0f;
					move.y *= 2.0f;
					move.z *= 2.0f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_NORMAL,						// 種類
						26,										// 寿命
						20.0f,									// 半径
						0.5f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_SLASH:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 1.8f;
					move.y *= 1.8f;
					move.z *= 1.8f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_NORMAL,						// 種類
						20,										// 寿命
						16.0f,									// 半径
						0.3f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_WEED:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 1.4f;
					move.y *= 1.4f;
					move.z *= 1.4f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_WEED,						// 種類
						20,										// 寿命
						16.0f,									// 半径
						0.3f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_SLASHWEED:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 2.0f;
					move.y *= 2.0f;
					move.z *= 2.0f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_WEED,						// 種類
						26,										// 寿命
						20.0f,									// 半径
						0.5f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_HEAL:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 2.0f;
					move.y *= 2.0f;
					move.z *= 2.0f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_HEAL,						// 種類
						26,										// 寿命
						20.0f,									// 半径
						0.5f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;

				case PARTICLETYPE_GETHEAL:

					// ベクトルをランダムに設定
					move.x = sinf((float)(rand() % 629 - 314) / 100.0f);
					move.y = cosf((float)(rand() % 629 - 314) / 100.0f);
					move.z = cosf((float)(rand() % 629 - 314) / 100.0f);

					// ベクトルを正規化
					D3DXVec3Normalize(&move, &move);

					// 移動量を乗算
					move.x *= 3.0f;
					move.y *= 3.0f;
					move.z *= 3.0f;

					// エフェクトの設定
					SetEffect
					( // 引数
						g_aParticle[nCntParticle].pos,			// 位置
						move,									// 移動量
						g_aParticle[nCntParticle].col,			// 色
						EFFECTTYPE_HEAL,						// 種類
						30,										// 寿命
						40.0f,									// 半径
						1.2f									// 減算量 (半径)
					);

					// 処理を抜ける
					break;
				}
			}

			// 寿命を減らす
			g_aParticle[nCntParticle].nLife--;

			if (g_aParticle[nCntParticle].nLife <= 0)
			{ // 寿命が尽きた場合

				// 使用されていない状態にする
				g_aParticle[nCntParticle].bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	パーティクルの描画処理
//======================================================================================================================
void DrawParticle(void)
{
	
}

//======================================================================================================================
//	パーティクルの設定処理
//======================================================================================================================
void SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, PARTICLETYPE type, int nSpawn, int nLife)
{
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{ // パーティクルの最大表示数分繰り返す

		if (g_aParticle[nCntParticle].bUse == false)
		{ // パーティクルが使用されていない場合

			// 引数を代入
			g_aParticle[nCntParticle].pos    = pos;		// 位置
			g_aParticle[nCntParticle].col    = col;		// 色
			g_aParticle[nCntParticle].type   = type;	// 種類
			g_aParticle[nCntParticle].nSpawn = nSpawn;	// エフェクト数
			g_aParticle[nCntParticle].nLife  = nLife;	// 寿命

			// 使用している状態にする
			g_aParticle[nCntParticle].bUse = true;

			// 処理を抜ける
			break;
		}
	}
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	パーティクルの総数取得処理
//**********************************************************************************************************************
int GetNumParticle(void)
{
	// 変数を宣言
	int nNumParticle = 0;	// パーティクルの総数の確認用

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{ // パーティクルの最大表示数分繰り返す

		if (g_aParticle[nCntParticle].bUse == true)
		{ // パーティクルが使用されている場合

			// カウンターを加算
			nNumParticle++;
		}
	}

	// 変数の値を返す
	return nNumParticle;	// パーティクルの総数
}
#endif