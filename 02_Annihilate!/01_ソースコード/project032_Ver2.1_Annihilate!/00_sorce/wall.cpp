//======================================================================================================================
//
//	壁処理 [wall.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "wall.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_WALL		(4)			// 使用するポリゴン数 (壁の最大数)

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureWall[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\pillar000.jpg",	// 壁のテクスチャの相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_WALL)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_WALL_NORMAL = 0,		// 壁 (通常)
	TEXTURE_WALL_MAX,				// この列挙型の総数
} TEXTURE_WALL;

//**********************************************************************************************************************
//	構造体定義 (Wall)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 rot;				// 向き
	D3DXMATRIX  mtxWorld;			// ワールドマトリックス
	bool        bUse;				// 使用状況
} Wall;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureWall[TEXTURE_WALL_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWall = NULL;					// 頂点バッファへのポインタ

Wall g_aWall[MAX_WALL];				// 壁の情報

//======================================================================================================================
//	壁の初期化処理
//======================================================================================================================
void InitWall(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntWall = 0; nCntWall < TEXTURE_WALL_MAX; nCntWall++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureWall[nCntWall], &g_apTextureWall[nCntWall]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_WALL,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffWall,
		NULL
	);

	// 壁の情報の初期化
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // 壁の最大表示数分繰り返す

		g_aWall[nCntWall].pos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
		g_aWall[nCntWall].rot  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
		g_aWall[nCntWall].bUse = false;								// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // 壁の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-400.0f, 200.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( 400.0f, 200.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-400.0f, 0.0f,   0.0f);
		pVtx[3].pos = D3DXVECTOR3( 400.0f, 0.0f,   0.0f);

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
	g_pVtxBuffWall->Unlock();
}

//======================================================================================================================
//	壁の終了処理
//======================================================================================================================
void UninitWall(void)
{
	// テクスチャの破棄
	for (int nCntWall = 0; nCntWall < TEXTURE_WALL_MAX; nCntWall++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureWall[nCntWall] != NULL)
		{ // 変数 (g_apTextureWall) がNULLではない場合

			g_apTextureWall[nCntWall]->Release();
			g_apTextureWall[nCntWall] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffWall != NULL)
	{ // 変数 (g_pVtxBuffWall) がNULLではない場合

		g_pVtxBuffWall->Release();
		g_pVtxBuffWall = NULL;
	}
}

//======================================================================================================================
//	壁の更新処理
//======================================================================================================================
void UpdateWall(void)
{

}

//======================================================================================================================
//	壁の描画処理
//======================================================================================================================
void DrawWall(void)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // 壁の最大表示数分繰り返す

		if (g_aWall[nCntWall].bUse == true)
		{ // 壁が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aWall[nCntWall].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aWall[nCntWall].rot.y, g_aWall[nCntWall].rot.x, g_aWall[nCntWall].rot.z);
			D3DXMatrixMultiply(&g_aWall[nCntWall].mtxWorld, &g_aWall[nCntWall].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aWall[nCntWall].pos.x, g_aWall[nCntWall].pos.y, g_aWall[nCntWall].pos.z);
			D3DXMatrixMultiply(&g_aWall[nCntWall].mtxWorld, &g_aWall[nCntWall].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aWall[nCntWall].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffWall, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureWall[TEXTURE_WALL_NORMAL]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWall * 4, 2);
		}
	}
}

//======================================================================================================================
//	壁の設定処理
//======================================================================================================================
void SetWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // 壁の最大表示数分繰り返す

		if (g_aWall[nCntWall].bUse == false)
		{ // 壁が使用されていない場合

			// 引数の位置を代入
			g_aWall[nCntWall].pos = pos;

			// 引数の向きを代入
			g_aWall[nCntWall].rot = rot;

			// 使用している状態にする
			g_aWall[nCntWall].bUse = true;

			// 処理を抜ける
			break;
		}
	}
}