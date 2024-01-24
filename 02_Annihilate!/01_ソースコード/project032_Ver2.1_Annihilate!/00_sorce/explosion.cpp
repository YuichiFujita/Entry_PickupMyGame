//======================================================================================================================
//
//	爆発処理 [explosion.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "explosion.h"
#include "sound.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_EXPLOSION	(64)	// 使用するポリゴン数 (爆発の最大数)

#define EXP_WIDTH		(45)	// 爆発の横幅 / 2
#define EXP_HEIGHT		(45)	// 爆発の縦幅 / 2
#define ANIM_TIME		(6)		// 爆発のアニメーション待機時間
#define ANIM_MAX		(7)		// 爆発の最大アニメーション

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureExplosion[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\explosion000.png",	// 爆発のテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_EXPLOSION)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_EXPLOSION_NORMAL = 0,		// 爆発 (通常)
	TEXTURE_EXPLOSION_MAX,				// この列挙型の総数
} TEXTURE_EXPLOSION;

//**********************************************************************************************************************
//	構造体定義 (Explosion)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;					// 位置
	D3DXMATRIX  mtxWorld;				// ワールドマトリックス
	int         nCounterAnim;			// アニメーションカウンター
	int         nPatternAnim;			// アニメーションパターン
	bool        bUse;					// 使用状況
}Explosion;

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9		g_apTextureExplosion[TEXTURE_EXPLOSION_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffExplosion = NULL;							// 頂点バッファへのポインタ

Explosion g_aExplosion[MAX_EXPLOSION];	// 爆発の情報

//======================================================================================================================
//	爆発の初期化処理
//======================================================================================================================
void InitExplosion(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntExplosion = 0; nCntExplosion < TEXTURE_EXPLOSION_MAX; nCntExplosion++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureExplosion[nCntExplosion], &g_apTextureExplosion[nCntExplosion]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_EXPLOSION,	// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffExplosion,
		NULL
	);

	// 爆発の情報の初期化
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		g_aExplosion[nCntExplosion].pos          = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aExplosion[nCntExplosion].nCounterAnim = 0;								// アニメーションカウンター
		g_aExplosion[nCntExplosion].nPatternAnim = 0;								// アニメーションパターン
		g_aExplosion[nCntExplosion].bUse         = false;							// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-EXP_WIDTH,  EXP_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( EXP_WIDTH,  EXP_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-EXP_WIDTH, -EXP_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3( EXP_WIDTH, -EXP_HEIGHT, 0.0f);

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
		pVtx[0].tex = D3DXVECTOR2(0.0f,   0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.125f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f,   1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.125f, 1.0f);

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}

//======================================================================================================================
//	爆発の終了処理
//======================================================================================================================
void UninitExplosion(void)
{
	// テクスチャの破棄
	for (int nCntExplosion = 0; nCntExplosion < TEXTURE_EXPLOSION_MAX; nCntExplosion++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureExplosion[nCntExplosion] != NULL)
		{ // 変数 (g_apTextureExplosion) がNULLではない場合

			g_apTextureExplosion[nCntExplosion]->Release();
			g_apTextureExplosion[nCntExplosion] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffExplosion != NULL)
	{ // 変数 (g_pVtxBuffExplosion) がNULLではない場合

		g_pVtxBuffExplosion->Release();
		g_pVtxBuffExplosion = NULL;
	}
}

//======================================================================================================================
//	爆発の更新処理
//======================================================================================================================
void UpdateExplosion(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // 爆発が使用されている場合

			// カウンターを加算
			g_aExplosion[nCntExplosion].nCounterAnim++;

			if ((g_aExplosion[nCntExplosion].nCounterAnim % ANIM_TIME) == 0)
			{ // カウンターがパターンを切り替えるタイミングになった場合

				// アニメーションのパターンを加算
				g_aExplosion[nCntExplosion].nPatternAnim++;
			}

			if (g_aExplosion[nCntExplosion].nPatternAnim >= ANIM_MAX)
			{ // アニメーションのパターンがパターンの総数以上の場合

				// 使用していない状態にする
				g_aExplosion[nCntExplosion].bUse = false;
			}

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2( g_aExplosion[nCntExplosion].nPatternAnim      * 0.125f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * 0.125f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2( g_aExplosion[nCntExplosion].nPatternAnim      * 0.125f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * 0.125f, 1.0f);
		}

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}

//======================================================================================================================
//	爆発の描画処理
//======================================================================================================================
void DrawExplosion(void)
{
		// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス取得用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);		// Zテストの設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の有効 / 無効の設定

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// αテストの参照値設定

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // 爆発が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aExplosion[nCntExplosion].mtxWorld);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aExplosion[nCntExplosion].mtxWorld, NULL, &mtxView);	// 逆行列を求める
			g_aExplosion[nCntExplosion].mtxWorld._41 = 0.0f;
			g_aExplosion[nCntExplosion].mtxWorld._42 = 0.0f;
			g_aExplosion[nCntExplosion].mtxWorld._43 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aExplosion[nCntExplosion].pos.x, g_aExplosion[nCntExplosion].pos.y, g_aExplosion[nCntExplosion].pos.z);
			D3DXMatrixMultiply(&g_aExplosion[nCntExplosion].mtxWorld, &g_aExplosion[nCntExplosion].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aExplosion[nCntExplosion].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffExplosion, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureExplosion[TEXTURE_EXPLOSION_NORMAL]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntExplosion * 4, 2);
		}
	}

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);		// Zテストの設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);			// Zバッファ更新の有効 / 無効の設定

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//======================================================================================================================
//	爆発の設定処理
//======================================================================================================================
void SetExplosion(D3DXVECTOR3 pos, SOUNDTYPE sound)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		if (g_aExplosion[nCntExplosion].bUse == false)
		{ // 爆発が使用されていない場合

			// 引数の位置を代入
			g_aExplosion[nCntExplosion].pos = pos;

			// 爆発の構造体の要素を初期化
			g_aExplosion[nCntExplosion].nCounterAnim = 0;
			g_aExplosion[nCntExplosion].nPatternAnim = 0;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f,   0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.125f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f,   1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.125f, 1.0f);

			// 使用している状態にする
			g_aExplosion[nCntExplosion].bUse = true;

			switch (sound)
			{ // 効果音の種類ごとの処理
			case SOUNDTYPE_NONE:		// 効果音無し

				// 無し

				// 処理を抜ける
				break;

			case SOUNDTYPE_EXPLOSION:	// 爆発音

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_DEATH);	// SE (死亡)

				// 処理を抜ける
				break;

			case SOUNDTYPE_BREAK:		// 破壊音

				
				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_BREAK);	// SE (破壊)

				// 処理を抜ける
				break;
			}

			// 処理を抜ける
			break;
		}

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	爆発の総数取得処理
//**********************************************************************************************************************
int GetNumExplosion(void)
{
	// 変数を宣言
	int nNumExplosion = 0;	// 爆発の総数の確認用

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // 爆発の最大表示数分繰り返す

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // 爆発が使用されている場合

			// カウンターを加算
			nNumExplosion++;
		}
	}

	// 変数の値を返す
	return nNumExplosion;	// 爆発の総数
}
#endif