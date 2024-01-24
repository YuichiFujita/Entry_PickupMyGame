//======================================================================================================================
//
//	竜巻処理 [tornado.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "tornado.h"
#include "calculation.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_TORNADO	(128)	// 竜巻の最大数
#define MAX_OFFSET	(2)		// オフセットの数

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureTornado[] =	// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\orbit000.jpg",	// 竜巻のテクスチャの相対パス
};

//**********************************************************************************************************************
//	構造体定義 (Tornado)
//**********************************************************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファ
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	D3DXVECTOR3 direRot;	// トルネードの成長向き
	TORNADOTYPE type;		// 種類
	D3DXMATRIX  mtxWorld;	// ワールドマトリックス
	D3DXMATRIX *pMtxParent;	// 親のマトリックス
	int   nNumVtx;		// 必要頂点数
	int   nNumAround;	// 渦の周回数
	int   nPattern;		// 渦の分割数
	float fMoveRot;		// 向きの変更量
	float fThickness;	// ポリゴンの太さ
	float fOuterPlus;	// ポリゴン外周の y座標加算量
	float fSetWidth;	// 生成時の横ずれ量
	float fSetAlpha;	// 生成時の透明度
	float fAddWidth;	// 横ずれの加算量
	float fAddHeight;	// 縦ずれの加算量
	float fSubAlpha;	// 透明度の減算量
	float fGrowWidth;	// 横ずれの成長量
	float fGrowHeight;	// 縦ずれの成長量
	float fGrowAlpha;	// 透明度の成長量
	bool  bUse;			// 使用状況
}Tornado;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void DeleteTornado(Tornado *pTornado);	// 竜巻の削除処理
void SetVtxTornado(Tornado *pTornado);	// 竜巻の頂点情報の設定処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureTornado[TORNADOTYPE_MAX] = {};	// テクスチャへのポインタ

Tornado g_aTornado[MAX_TORNADO];	// 竜巻の情報

//======================================================================================================================
//	竜巻の初期化処理
//======================================================================================================================
void InitTornado(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// テクスチャの読み込み
	for (int nCntTornado = 0; nCntTornado < TORNADOTYPE_MAX; nCntTornado++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureTornado[nCntTornado], &g_apTextureTornado[nCntTornado]);
	}

	// 竜巻の情報の初期化
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // 竜巻の最大表示数分繰り返す

		// 基本情報の初期化
		g_aTornado[nCntTornado].pos		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aTornado[nCntTornado].rot		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_aTornado[nCntTornado].direRot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// トルネードの成長向き
		g_aTornado[nCntTornado].type	= TORNADOTYPE_NORMAL;				// 種類

		g_aTornado[nCntTornado].pMtxParent	= NULL;		// 親のマトリックス
		g_aTornado[nCntTornado].nNumVtx		= 0;		// 必要頂点数
		g_aTornado[nCntTornado].nNumAround	= 0;		// 渦の周回数
		g_aTornado[nCntTornado].nPattern	= 0;		// 渦の分割数
		g_aTornado[nCntTornado].fMoveRot	= 0.0f;		// 向きの変更量
		g_aTornado[nCntTornado].fThickness	= 0.0f;		// ポリゴンの太さ
		g_aTornado[nCntTornado].fOuterPlus	= 0.0f;		// ポリゴン外周の y座標加算量
		g_aTornado[nCntTornado].fSetWidth	= 0.0f;		// 生成時の横ずれ量
		g_aTornado[nCntTornado].fSetAlpha	= 0.0f;		// 生成時の透明度
		g_aTornado[nCntTornado].fAddWidth	= 0.0f;		// 横ずれの加算量
		g_aTornado[nCntTornado].fAddHeight	= 0.0f;		// 縦ずれの加算量
		g_aTornado[nCntTornado].fSubAlpha	= 0.0f;		// 透明度の減算量
		g_aTornado[nCntTornado].fGrowWidth	= 0.0f;		// 横ずれの成長量
		g_aTornado[nCntTornado].fGrowHeight	= 0.0f;		// 縦ずれの成長量
		g_aTornado[nCntTornado].fGrowAlpha	= 0.0f;		// 透明度の成長量
		g_aTornado[nCntTornado].bUse		= false;	// 使用状況

		// 頂点情報の初期化
		g_aTornado[nCntTornado].pVtxBuff = NULL;
	}
}

//======================================================================================================================
//	竜巻の終了処理
//======================================================================================================================
void UninitTornado(void)
{
	// テクスチャの破棄
	for (int nCntTornado = 0; nCntTornado < TORNADOTYPE_MAX; nCntTornado++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureTornado[nCntTornado] != NULL)
		{ // 変数 (g_apTextureTornado) がNULLではない場合

			g_apTextureTornado[nCntTornado]->Release();
			g_apTextureTornado[nCntTornado] = NULL;
		}
	}

	// 頂点バッファの破棄
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // 竜巻の最大表示数分繰り返す

		if (g_aTornado[nCntTornado].pVtxBuff != NULL)
		{ // 変数 (g_aTornado[nCntTornado].pVtxBuff) がNULLではない場合

			g_aTornado[nCntTornado].pVtxBuff->Release();
			g_aTornado[nCntTornado].pVtxBuff = NULL;
		}
	}
}

//======================================================================================================================
//	竜巻の更新処理
//======================================================================================================================
void UpdateTornado(void)
{
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // 竜巻の最大表示数分繰り返す

		if (g_aTornado[nCntTornado].bUse == true)
		{ // 竜巻が使用されている場合

			// 竜巻の向きを変更
			g_aTornado[nCntTornado].rot.y -= g_aTornado[nCntTornado].fMoveRot;

			// 向きを正規化
			NormalizeRot(&g_aTornado[nCntTornado].rot.y);

			// 竜巻を成長させる
			g_aTornado[nCntTornado].fSetWidth  += g_aTornado[nCntTornado].fGrowWidth * g_aTornado[nCntTornado].nNumVtx;	// 原点からの横ずれ量を加算
			g_aTornado[nCntTornado].fAddWidth  += g_aTornado[nCntTornado].fGrowWidth;	// 竜巻の横の広がりを加算
			g_aTornado[nCntTornado].fAddHeight += g_aTornado[nCntTornado].fGrowHeight;	// 竜巻の縦の広がりを加算
			g_aTornado[nCntTornado].fSetAlpha  -= g_aTornado[nCntTornado].fGrowAlpha;	// 竜巻の透明度を減算 (透明にしていく)

			// 竜巻の頂点情報の設定
			SetVtxTornado(&g_aTornado[nCntTornado]);

			if (g_aTornado[nCntTornado].fSetAlpha <= 0.0f)
			{ // 竜巻の透明になった場合

				// 竜巻の削除
				DeleteTornado(&g_aTornado[nCntTornado]);
			}
		}
	}
}

//======================================================================================================================
//	竜巻の描画処理
//======================================================================================================================
void DrawTornado(void)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxOrigin;			// 発生源のマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ポリゴンの両面を表示状態にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定

	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // 竜巻の最大表示数分繰り返す

		if (g_aTornado[nCntTornado].bUse == true)
		{ // 竜巻が使用されている場合

			//----------------------------------------------------------------------------------------------------------
			//	発生源のマトリックスを求める
			//----------------------------------------------------------------------------------------------------------
			// 発生源のマトリックスの初期化
			D3DXMatrixIdentity(&mtxOrigin);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aTornado[nCntTornado].direRot.y, g_aTornado[nCntTornado].direRot.x, g_aTornado[nCntTornado].direRot.z);
			D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxRot);	// 成長向き

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aTornado[nCntTornado].pos.x, g_aTornado[nCntTornado].pos.y, g_aTornado[nCntTornado].pos.z);
			D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxTrans);	// 発生位置

			//----------------------------------------------------------------------------------------------------------
			//	ワールドマトリックスを求める
			//----------------------------------------------------------------------------------------------------------
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aTornado[nCntTornado].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aTornado[nCntTornado].rot.y, g_aTornado[nCntTornado].rot.x, g_aTornado[nCntTornado].rot.z);
			D3DXMatrixMultiply(&g_aTornado[nCntTornado].mtxWorld, &g_aTornado[nCntTornado].mtxWorld, &mtxRot);	// 回転量

			//----------------------------------------------------------------------------------------------------------
			//	マトリックスを掛け合わせる
			//----------------------------------------------------------------------------------------------------------
			if (g_aTornado[nCntTornado].pMtxParent != NULL)
			{ // 親のマトリックスが存在する場合

				// 親のマトリックスと掛け合わせる
				D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, g_aTornado[nCntTornado].pMtxParent);
			}

			// 発生源のマトリックスと掛け合わせる
			D3DXMatrixMultiply(&g_aTornado[nCntTornado].mtxWorld, &g_aTornado[nCntTornado].mtxWorld, &mtxOrigin);

			//----------------------------------------------------------------------------------------------------------
			//	竜巻を描画
			//----------------------------------------------------------------------------------------------------------
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aTornado[nCntTornado].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aTornado[nCntTornado].pVtxBuff, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureTornado[g_aTornado[nCntTornado].type]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, g_aTornado[nCntTornado].nNumVtx - 2);
		}
	}

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ポリゴンの表面のみを表示状態にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//======================================================================================================================
//	竜巻の設定処理
//======================================================================================================================
int SetTornado
(
	D3DXVECTOR3 pos,		// 位置
	D3DXVECTOR3 direRot,	// 成長向き
	TORNADOTYPE type,		// 種類
	D3DXMATRIX *pMtxParent,	// 親のマトリックス
	int   nNumAround,		// 渦の周回数
	int   nPattern,			// 渦の分割数
	float fMoveRot,			// 向きの変更量
	float fThickness,		// ポリゴンの太さ
	float fOuterPlus,		// ポリゴン外周の y座標加算量
	float fSetWidth,		// 生成時の横ずれ量
	float fSetAlpha,		// 生成時の透明度
	float fAddWidth,		// 横ずれの加算量
	float fAddHeight,		// 縦ずれの加算量
	float fSubAlpha,		// 透明度の減算量
	float fGrowWidth,		// 横ずれの成長量
	float fGrowHeight,		// 縦ずれの成長量
	float fGrowAlpha		// 透明度の成長量
)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // 竜巻の最大表示数分繰り返す

		if (g_aTornado[nCntTornado].bUse == false)
		{ // 竜巻が使用されていない場合

			// 基本情報を設定
			g_aTornado[nCntTornado].pos		= pos;		// 位置
			g_aTornado[nCntTornado].direRot	= direRot;	// トルネードの成長向き
			g_aTornado[nCntTornado].type	= type;		// 種類

			g_aTornado[nCntTornado].pMtxParent	= pMtxParent;	// 親のマトリックス
			g_aTornado[nCntTornado].nNumAround	= nNumAround;	// 渦の周回数
			g_aTornado[nCntTornado].nPattern	= nPattern;		// 渦の分割数

			g_aTornado[nCntTornado].fMoveRot	= fMoveRot;		// 向きの変更量
			g_aTornado[nCntTornado].fThickness	= fThickness;	// ポリゴンの太さ
			g_aTornado[nCntTornado].fOuterPlus	= fOuterPlus;	// ポリゴン外周の y座標加算量

			g_aTornado[nCntTornado].fSetWidth	= fSetWidth;	// 生成時の横ずれ量
			g_aTornado[nCntTornado].fSetAlpha	= fSetAlpha;	// 生成時の透明度

			g_aTornado[nCntTornado].fAddWidth	= fAddWidth;	// 横ずれの加算量
			g_aTornado[nCntTornado].fAddHeight	= fAddHeight;	// 縦ずれの加算量
			g_aTornado[nCntTornado].fSubAlpha	= fSubAlpha;	// 透明度の減算量

			g_aTornado[nCntTornado].fGrowWidth	= fGrowWidth;	// 横ずれの成長量
			g_aTornado[nCntTornado].fGrowHeight	= fGrowHeight;	// 縦ずれの成長量
			g_aTornado[nCntTornado].fGrowAlpha	= fGrowAlpha;	// 透明度の成長量

			// 向きを初期化
			g_aTornado[nCntTornado].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			// 頂点情報を初期化
			if (g_aTornado[nCntTornado].pVtxBuff == NULL)
			{ // 頂点バッファが使用されていない場合

				// 必要頂点数を計算
				g_aTornado[nCntTornado].nNumVtx = nNumAround * nPattern * MAX_OFFSET;

				// 頂点バッファの生成
				pDevice->CreateVertexBuffer
				( // 引数
					sizeof(VERTEX_3D) * g_aTornado[nCntTornado].nNumVtx,	// 必要頂点数
					D3DUSAGE_WRITEONLY,					// 使用方法
					FVF_VERTEX_3D,						// 頂点フォーマット
					D3DPOOL_MANAGED,					// メモリの指定
					&g_aTornado[nCntTornado].pVtxBuff,	// 頂点バッファへのポインタ
					NULL
				);

				// 竜巻の頂点情報の設定
				SetVtxTornado(&g_aTornado[nCntTornado]);
			}
			else { assert(false); }	// 使用中

			// 使用している状態にする
			g_aTornado[nCntTornado].bUse = true;

			// 使用するインデックスを返す
			return nCntTornado;
		}
	}

	// 使用不可のインデックスを返す
	return NONE_IDX;
}

//======================================================================================================================
//	竜巻の成長設定処理
//======================================================================================================================
void SetGrowTornado
(
	int   nID,			// 変更インデックス
	float fMoveRot,		// 向きの変更量
	float fGrowWidth,	// 横ずれの成長量
	float fGrowHeight,	// 縦ずれの成長量
	float fGrowAlpha	// 透明度の成長量
)
{
	if (nID > NONE_IDX)
	{ // 使用可能なインデックスの場合

		// 引数の情報を設定
		g_aTornado[nID].fMoveRot	= fMoveRot;		// 向きの変更量
		g_aTornado[nID].fGrowWidth	= fGrowWidth;	// 横ずれの成長量
		g_aTornado[nID].fGrowHeight	= fGrowHeight;	// 縦ずれの成長量
		g_aTornado[nID].fGrowAlpha	= fGrowAlpha;	// 透明度の成長量
	}
}

//======================================================================================================================
//	竜巻の削除処理
//======================================================================================================================
void DeleteTornado(Tornado *pTornado)
{
	if (pTornado->pVtxBuff != NULL)
	{ // トルネードの頂点バッファが使用中の場合

		// メモリを開放
		pTornado->pVtxBuff->Release();
		pTornado->pVtxBuff = NULL;	// ポインタ初期化

		// 使用していない状態にする
		pTornado->bUse = false;
	}
	else { assert(false); }	// 非使用中
}

//======================================================================================================================
//	竜巻の頂点情報の設定処理
//======================================================================================================================
void SetVtxTornado(Tornado *pTornado)
{
	// 変数を宣言
	D3DXVECTOR3 vecPos;	// 竜巻の頂点方向ベクトル
	float fWidth  = pTornado->fSetWidth;	// 頂点方向の横ずれ量
	float fHeight = 0.0f;					// 頂点方向の縦ずれ量
	float fAlpha  = pTornado->fSetAlpha;	// 頂点カラーの透明度

	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	if (pTornado->pVtxBuff != NULL)
	{ // トルネードの頂点バッファが使用中の場合

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		pTornado->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntWidth = 0; nCntWidth < pTornado->nNumAround; nCntWidth++)
		{ // 渦の周回の総数分繰り返す

			for (int nCntHeight = 0; nCntHeight < pTornado->nPattern; nCntHeight++)
			{ // 渦の分割数分繰り返す

				// 頂点方向ベクトルを求める
				vecPos.x = sinf(nCntHeight * ((D3DX_PI * 2.0f) / pTornado->nPattern)) * 1.0f;
				vecPos.y = 1.0f;
				vecPos.z = cosf(nCntHeight * ((D3DX_PI * 2.0f) / pTornado->nPattern)) * 1.0f;

				for (int nCntSet = 0; nCntSet < MAX_OFFSET; nCntSet++)
				{ // オフセットの総数分繰り返す

					// 頂点座標の設定
					pVtx[0].pos.x = 0.0f + vecPos.x * (fWidth + (nCntSet * pTornado->fThickness));	// x
					pVtx[0].pos.y = 0.0f + vecPos.y * (fHeight + (nCntSet * pTornado->fOuterPlus));	// y
					pVtx[0].pos.z = 0.0f + vecPos.z * (fWidth + (nCntSet * pTornado->fThickness));	// z

					// 法線ベクトルの設定
					pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

					if (fAlpha < 0.0f)
					{ // 透明度が範囲外の場合

						// 透明度を補正
						fAlpha = 0.0f;
					}

					// 頂点カラーの設定
					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2
					( // 引数
						1.0f,			// u
						1.0f * nCntSet	// v
					);

					// 頂点データのポインタを進める
					pVtx += 1;
				}

				// 縦横を広げる
				fWidth  += pTornado->fAddWidth;
				fHeight += pTornado->fAddHeight;

				// α値を減算
				fAlpha -= pTornado->fSubAlpha;
			}
		}

		// 頂点バッファをアンロックする
		pTornado->pVtxBuff->Unlock();
	}
	else { assert(false); }	// 非使用中
}

#ifdef _DEBUG	// デバッグ処理
#endif