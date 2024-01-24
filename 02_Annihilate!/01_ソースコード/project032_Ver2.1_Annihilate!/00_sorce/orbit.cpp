//======================================================================================================================
//
//	軌跡処理 [orbit.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "orbit.h"
#include "game.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_ORBIT	(128)	// 軌跡の最大数
#define MAX_OFFSET	(2)		// オフセットの数
#define MAX_VERTEX	(40)	// 維持する頂点の最大数

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureOrbit[] =	// テクスチャの相対パス
{
	NULL,									// NULL
	"02_data\\02_TEXTURE\\orbit000.jpg",	// 軌跡のテクスチャの相対パス
};

const D3DXVECTOR3 aOffset[][MAX_OFFSET]	// オフセットの位置変更量
{
	{ D3DXVECTOR3(0.0f, 0.0f, 0.0f),   D3DXVECTOR3(0.0f, 10.0f, 0.0f) },	// 敵のオフセット (縦)
	{ D3DXVECTOR3(-6.5f, 0.0f, 0.0f),  D3DXVECTOR3(6.5f, 0.0f, 0.0f)  },	// 敵のオフセット (横)
};

//**********************************************************************************************************************
//	構造体定義 (Orbit)
//**********************************************************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファ
	D3DXMATRIX *pMtxParent;					// 親のマトリックス
	D3DXVECTOR3 aOffset[MAX_OFFSET];		// 両端のオフセット
	D3DXCOLOR   aCol[MAX_OFFSET];			// 両端の基準色
	D3DXMATRIX  aMtxWorldPoint[MAX_OFFSET];	// 両端のワールドマトリックス
	D3DXVECTOR3 aPosPoint[MAX_VERTEX];		// 計算後の各頂点座標
	D3DXCOLOR   aColPoint[MAX_VERTEX];		// 各頂点カラー
	ORBITTYPE   type;						// 種類
	int        *pOrbitIDParent;				// 軌跡の親の軌跡インデックス
	bool       *pUseParent;					// 軌跡の親の使用状況
	bool        bInit;						// 初期化状況
	bool        bUse;						// 使用状況
}Orbit;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureOrbit[ORBITTYPE_MAX] = {};	// テクスチャへのポインタ

Orbit g_aOrbit[MAX_ORBIT];	// 軌跡の情報

//======================================================================================================================
//	軌跡の初期化処理
//======================================================================================================================
void InitOrbit(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntOrbit = 0; nCntOrbit < ORBITTYPE_MAX; nCntOrbit++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureOrbit[nCntOrbit], &g_apTextureOrbit[nCntOrbit]);
	}

	// 軌跡の情報の初期化
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // 軌跡の最大表示数分繰り返す

		//--------------------------------------------------------------------------------------------------------------
		//	頂点情報の初期化
		//--------------------------------------------------------------------------------------------------------------
		// 頂点バッファの生成
		pDevice->CreateVertexBuffer
		( // 引数
			sizeof(VERTEX_3D) * MAX_VERTEX,		// 必要頂点数
			D3DUSAGE_WRITEONLY,					// 使用方法
			FVF_VERTEX_3D,						// 頂点フォーマット
			D3DPOOL_MANAGED,					// メモリの指定
			&g_aOrbit[nCntOrbit].pVtxBuff,		// 頂点バッファへのポインタ
			NULL
		);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_aOrbit[nCntOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntWidth = 0; nCntWidth < (int)(MAX_VERTEX * 0.5f); nCntWidth++)
		{ // 維持する横幅の頂点の最大数分繰り返す

			for (int nCntHeight = 0; nCntHeight < MAX_OFFSET; nCntHeight++)
			{ // 維持する縦幅の頂点の最大数分繰り返す

				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// 法線ベクトルの設定
				pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2
				( // 引数
					1.0f * (nCntWidth % MAX_OFFSET),	// u
					1.0f * nCntHeight					// v
				);

				// 頂点データのポインタを進める
				pVtx += 1;
			}
		}

		// 頂点バッファをアンロックする
		g_aOrbit[nCntOrbit].pVtxBuff->Unlock();

		//--------------------------------------------------------------------------------------------------------------
		//	基本情報の初期化
		//--------------------------------------------------------------------------------------------------------------
		// アドレスを初期化
		g_aOrbit[nCntOrbit].pOrbitIDParent = NULL;		// 軌跡の親の軌跡インデックス
		g_aOrbit[nCntOrbit].pMtxParent     = NULL;		// 親のマトリックス
		g_aOrbit[nCntOrbit].pUseParent     = NULL;		// 軌跡の親の使用状況

		// 真偽状況を初期化
		g_aOrbit[nCntOrbit].bInit = false;		// 初期化状況
		g_aOrbit[nCntOrbit].bUse  = false;		// 使用状況

		// 種類を初期化
		g_aOrbit[nCntOrbit].type = ORBITTYPE_NONE;

		for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
		{ // オフセットの数分繰り返す

			g_aOrbit[nCntOrbit].aOffset[nCntOff] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 両端のオフセット
			g_aOrbit[nCntOrbit].aCol[nCntOff]    = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 両端の基準色
		}

		for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
		{ // 維持する頂点の最大数分繰り返す

			g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 計算後の各頂点座標
			g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 各頂点カラー
		}
	}
}

//======================================================================================================================
//	軌跡の終了処理
//======================================================================================================================
void UninitOrbit(void)
{
	// テクスチャの破棄
	for (int nCntOrbit = 0; nCntOrbit < ORBITTYPE_MAX; nCntOrbit++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureOrbit[nCntOrbit] != NULL)
		{ // 変数 (g_apTextureOrbit) がNULLではない場合

			g_apTextureOrbit[nCntOrbit]->Release();
			g_apTextureOrbit[nCntOrbit] = NULL;
		}
	}

	// 頂点バッファの破棄
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // 軌跡の最大表示数分繰り返す

		if (g_aOrbit[nCntOrbit].pVtxBuff != NULL)
		{ // 変数 (g_aOrbit[nCntOrbit].pVtxBuff) がNULLではない場合

			g_aOrbit[nCntOrbit].pVtxBuff->Release();
			g_aOrbit[nCntOrbit].pVtxBuff = NULL;
		}
	}
}

//======================================================================================================================
//	軌跡の更新処理
//======================================================================================================================
void UpdateOrbit(void)
{
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // 軌跡の最大表示数分繰り返す

		if (g_aOrbit[nCntOrbit].bUse == true && g_aOrbit[nCntOrbit].pUseParent != NULL)
		{ // 軌跡が使用されている且つ、軌跡の親の使用状況アドレスが設定されている場合

			if (*g_aOrbit[nCntOrbit].pUseParent == false)
			{ // 軌跡の親が使用されていない場合

				// 軌跡を使用していない状態にする
				g_aOrbit[nCntOrbit].bUse = false;
			}
			else
			{ // 軌跡の親が使用されている場合

				if (g_aOrbit[nCntOrbit].pOrbitIDParent != NULL)
				{ // 軌跡の親の軌跡インデックスアドレスが設定されている場合

					if (*g_aOrbit[nCntOrbit].pOrbitIDParent != nCntOrbit)
					{ // 軌跡の親の軌跡インデックスと、現在の軌跡インデックスの値が違う場合

						// 軌跡を使用していない状態にする
						g_aOrbit[nCntOrbit].bUse = false;

						// [※] 使用しない状態になった親が同フレーム内で同じインデックスが使われて
						//      また使用された場合に軌跡が一つの親に二つ生成されてしまうため、その防止。
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	軌跡の描画処理
//======================================================================================================================
void DrawOrbit(void)
{
	// 変数を宣言
	D3DXMATRIX mtxIdent;	// 単位マトリックス設定用

	// 単位マトリックスの初期化
	D3DXMatrixIdentity(&mtxIdent);

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ
	
	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ポリゴンの両面を表示状態にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // 軌跡の最大表示数分繰り返す

		if (g_aOrbit[nCntOrbit].bUse == true)
		{ // 軌跡が使用されている場合

			//----------------------------------------------------------------------------------------------------------
			//	オフセットの初期化
			//----------------------------------------------------------------------------------------------------------
			for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
			{ // オフセットの数分繰り返す

				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]);

				// 位置を反映
				D3DXMatrixTranslation(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], g_aOrbit[nCntOrbit].aOffset[nCntOff].x, g_aOrbit[nCntOrbit].aOffset[nCntOff].y, g_aOrbit[nCntOrbit].aOffset[nCntOff].z);

				// 親のマトリックスと掛け合わせる
				D3DXMatrixMultiply(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], &g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], g_aOrbit[nCntOrbit].pMtxParent);
			}

			if (GetPause() == false)
			{ // ポーズ中ではない場合

				//------------------------------------------------------------------------------------------------------
				//	頂点座標と頂点カラーの情報をずらす
				//------------------------------------------------------------------------------------------------------
				for (int nCntVtx = MAX_VERTEX - 1; nCntVtx >= MAX_OFFSET; nCntVtx--)
				{ // 維持する頂点の最大数分繰り返す (オフセット分は含まない)

					// 頂点情報をずらす
					g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = g_aOrbit[nCntOrbit].aPosPoint[nCntVtx - MAX_OFFSET];
					g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = g_aOrbit[nCntOrbit].aColPoint[nCntVtx - MAX_OFFSET];
				}

				//------------------------------------------------------------------------------------------------------
				//	最新の頂点座標と頂点カラーの情報を設定
				//------------------------------------------------------------------------------------------------------
				for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
				{ // オフセットの数分繰り返す

					// 頂点座標の設定
					g_aOrbit[nCntOrbit].aPosPoint[nCntOff] = D3DXVECTOR3
					( // 引数
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._41,	// x
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._42,	// y
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._43		// z
					);

					// 頂点カラーの設定
					g_aOrbit[nCntOrbit].aColPoint[nCntOff] = g_aOrbit[nCntOrbit].aCol[nCntOff];
				}
			}

			//----------------------------------------------------------------------------------------------------------
			//	頂点座標と頂点カラーの情報を初期化
			//----------------------------------------------------------------------------------------------------------
			if (g_aOrbit[nCntOrbit].bInit == false)
			{ // 初期化済みではない場合

				for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
				{ // 維持する頂点の最大数分繰り返す

					// 頂点座標の設定
					g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3
					( // 引数
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._41,	// x
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._42,	// y
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._43	// z
					);

					// 頂点カラーの設定
					g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = g_aOrbit[nCntOrbit].aCol[nCntVtx % MAX_OFFSET];
				}

				// 初期化済みにする
				g_aOrbit[nCntOrbit].bInit = true;
			}

			//----------------------------------------------------------------------------------------------------------
			//	頂点情報の設定
			//----------------------------------------------------------------------------------------------------------
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aOrbit[nCntOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
			{ // 維持する頂点の最大数分繰り返す

				// 頂点座標の設定
				pVtx[0].pos = g_aOrbit[nCntOrbit].aPosPoint[nCntVtx];

				// 頂点カラーの設定
				pVtx[0].col = g_aOrbit[nCntOrbit].aColPoint[nCntVtx];

				// 頂点データのポインタを進める
				pVtx += 1;
			}

			// 頂点バッファをアンロックする
			g_aOrbit[nCntOrbit].pVtxBuff->Unlock();

			//----------------------------------------------------------------------------------------------------------
			//	ポリゴンの描画
			//----------------------------------------------------------------------------------------------------------
			// 単位マトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &mtxIdent);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aOrbit[nCntOrbit].pVtxBuff, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureOrbit[g_aOrbit[nCntOrbit].type]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, MAX_VERTEX - 2);
		}
	}

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ポリゴンの表面のみを表示状態にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//======================================================================================================================
//	軌跡の設定処理
//======================================================================================================================
int SetOrbit(D3DXMATRIX *pMtxParent, bool *pUse, int *pOrbitID, D3DXCOLOR col, OFFSETTYPE offType, ORBITTYPE texType)
{
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // 軌跡の最大表示数分繰り返す

		if (g_aOrbit[nCntOrbit].bUse == false)
		{ // 軌跡が使用されていない場合

			// 引数のアドレスを設定
			g_aOrbit[nCntOrbit].pOrbitIDParent = pOrbitID;		// 軌跡の親の軌跡インデックス
			g_aOrbit[nCntOrbit].pMtxParent     = pMtxParent;	// 親のマトリックス
			g_aOrbit[nCntOrbit].pUseParent     = pUse;			// 軌跡の親の使用状況

			// 引数の種類を設定
			g_aOrbit[nCntOrbit].type = texType;

			// 初期化状況を初期化
			g_aOrbit[nCntOrbit].bInit = false;

			// オフセットの頂点情報の初期化
			for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
			{ // オフセットの数分繰り返す

				g_aOrbit[nCntOrbit].aOffset[nCntOff] = aOffset[(int)offType][nCntOff];	// 両端のオフセット
				g_aOrbit[nCntOrbit].aCol[nCntOff]    = col;								// 両端の基準色
			}

			// 保持する頂点情報の初期化
			for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
			{ // 頂点の最大数分繰り返す

				g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 計算後の各頂点座標
				g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 各頂点カラー
			}

			// 使用している状態にする
			g_aOrbit[nCntOrbit].bUse = true;

			// 使用できる軌跡のIDを返す
			return nCntOrbit;
		}
	}

	// 軌跡の総数が足りない場合 -1を返す
	return NONE_ORBIT;
}

#ifdef _DEBUG	// デバッグ処理
#endif