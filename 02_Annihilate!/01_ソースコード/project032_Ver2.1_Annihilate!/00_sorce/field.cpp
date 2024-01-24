//======================================================================================================================
//
//	床処理 [field.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "field.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_FIELD	(4)	// 使用するポリゴン数 (床の最大数)

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureField[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\field000.jpg",	// 床のテクスチャの相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_FIELD)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_FIELD_NORMAL = 0,		// 床 (通常)
	TEXTURE_FIELD_MAX,				// この列挙型の総数
} TEXTURE_FIELD;

//**********************************************************************************************************************
//	構造体定義 (Field)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 rot;				// 向き
	D3DXMATRIX  mtxWorld;			// ワールドマトリックス
	bool        bUse;				// 使用状況
} Field;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureField[TEXTURE_FIELD_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;						// 頂点バッファへのポインタ

Field g_aField[MAX_FIELD];			// 床の情報

//======================================================================================================================
//	床の初期化処理
//======================================================================================================================
void InitField(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntField = 0; nCntField < TEXTURE_FIELD_MAX; nCntField++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureField[nCntField], &g_apTextureField[nCntField]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_FIELD,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL
	);

	// 床の情報の初期化
	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // 床の最大表示数分繰り返す

		g_aField[nCntField].pos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aField[nCntField].rot  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_aField[nCntField].bUse = false;							// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // 床の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-200.0f, 0.0f,  200.0f);
		pVtx[1].pos = D3DXVECTOR3( 200.0f, 0.0f,  200.0f);
		pVtx[2].pos = D3DXVECTOR3(-200.0f, 0.0f, -200.0f);
		pVtx[3].pos = D3DXVECTOR3( 200.0f, 0.0f, -200.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
	g_pVtxBuffField->Unlock();
}

//======================================================================================================================
//	床の終了処理
//======================================================================================================================
void UninitField(void)
{
	// テクスチャの破棄
	for (int nCntField = 0; nCntField < TEXTURE_FIELD_MAX; nCntField++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureField[nCntField] != NULL)
		{ // 変数 (g_apTextureField) がNULLではない場合

			g_apTextureField[nCntField]->Release();
			g_apTextureField[nCntField] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffField != NULL)
	{ // 変数 (g_pVtxBuffField) がNULLではない場合

		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
}

//======================================================================================================================
//	床の更新処理
//======================================================================================================================
void UpdateField(void)
{

}

//======================================================================================================================
//	床の描画処理
//======================================================================================================================
void DrawField(void)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // 床の最大表示数分繰り返す

		if (g_aField[nCntField].bUse == true)
		{ // 床が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aField[nCntField].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aField[nCntField].rot.y, g_aField[nCntField].rot.x, g_aField[nCntField].rot.z);
			D3DXMatrixMultiply(&g_aField[nCntField].mtxWorld, &g_aField[nCntField].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aField[nCntField].pos.x, g_aField[nCntField].pos.y, g_aField[nCntField].pos.z);
			D3DXMatrixMultiply(&g_aField[nCntField].mtxWorld, &g_aField[nCntField].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aField[nCntField].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureField[TEXTURE_FIELD_NORMAL]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntField * 4, 2);
		}
	}
}

//======================================================================================================================
//	床の設定処理
//======================================================================================================================
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // 壁の最大表示数分繰り返す

		if (g_aField[nCntField].bUse == false)
		{ // 壁が使用されていない場合

			// 引数の位置を代入
			g_aField[nCntField].pos = pos;

			// 引数の向きを代入
			g_aField[nCntField].rot = rot;

			// 使用している状態にする
			g_aField[nCntField].bUse = true;

			// 処理を抜ける
			break;
		}
	}
}