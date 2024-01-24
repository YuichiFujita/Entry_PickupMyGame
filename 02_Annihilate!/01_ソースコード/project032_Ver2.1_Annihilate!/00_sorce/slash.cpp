//======================================================================================================================
//
//	斬撃処理 [slash.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "slash.h"

#include "bullet.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "object.h"
#include "sound.h"
#include "weed.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define MAX_SLASH		(32)		// 使用するポリゴン数 (斬撃の最大数)

#define REV_SLASH_ROT	(0.09f)		// 斬撃の回転量の補正係数
#define HIT_SLA_HEIGHT	(5.0f)		// 斬撃の縦幅 / 2

#define SLA_DMG_PLAY	(25)		// 斬撃の攻撃力 (プレイヤー)
#define SLA_DMG_ENE		(15)		// 斬撃の攻撃力 (敵)

#define REF_BULL_PLUS	(0.1f)		// 弾の反射時の追加弾の方向加算量

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureSlash[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\slash000.png",	// 斬撃 (プレイヤー) のテクスチャ相対パス
	"02_data\\02_TEXTURE\\slash001.png",	// 斬撃 (敵) のテクスチャ相対パス
};

//**********************************************************************************************************************
//	構造体定義 (Slash)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 rot;				// 向き
	D3DXMATRIX  mtxWorld;			// ワールドマトリックス
	SLASHTYPE   type;				// 種類
	int         nLife;				// 寿命
	int         nMaxLife;			// 総寿命
	float       fRadius;			// 半径
	float       fAlpha;				// α値
	float       fChange;			// 1Fごとの薄くなる量
	int        *pSlashIDParent;		// 斬撃の親の斬撃インデックス
	bool       *pUseParent;			// 斬撃の親の使用状況
	bool        bUse;				// 使用状況
}Slash;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void CollisionPlayer(Slash *pSlash);	// プレイヤーと斬撃の当たり判定
void CollisionEnemy(Slash *pSlash);		// 敵と斬撃の当たり判定
void CollisionWeed(Slash *pSlash);		// 雑草と斬撃の当たり判定
void CollisionObject(Slash *pSlash);	// オブジェクトと斬撃の当たり判定
void CollisionBullet(Slash *pSlash);	// 弾と斬撃の当たり判定

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9		g_apTextureSlash[SLASHTYPE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSlash = NULL;					// 頂点バッファへのポインタ

Slash g_aSlash[MAX_SLASH];	// 斬撃の情報

//======================================================================================================================
//	斬撃の初期化処理
//======================================================================================================================
void InitSlash(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntSlash = 0; nCntSlash < SLASHTYPE_MAX; nCntSlash++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureSlash[nCntSlash], &g_apTextureSlash[nCntSlash]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_SLASH,	// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,						// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffSlash,
		NULL
	);

	// 斬撃の情報の初期化
	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		g_aSlash[nCntSlash].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aSlash[nCntSlash].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_aSlash[nCntSlash].type           = SLASHTYPE_PLAYER;				// 種類
		g_aSlash[nCntSlash].nLife          = 0;								// 寿命
		g_aSlash[nCntSlash].nMaxLife       = 0;								// 総寿命
		g_aSlash[nCntSlash].fRadius        = 0.0f;							// 半径
		g_aSlash[nCntSlash].fAlpha         = 1.0f;							// α値
		g_aSlash[nCntSlash].fChange        = 0.0f;							// 1Fごとの薄くなる量
		g_aSlash[nCntSlash].pSlashIDParent = NULL;							// 斬撃の親の斬撃インデックス
		g_aSlash[nCntSlash].pUseParent     = NULL;							// 斬撃の親の使用状況
		g_aSlash[nCntSlash].bUse           = false;							// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

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
	g_pVtxBuffSlash->Unlock();
}

//======================================================================================================================
//	斬撃の終了処理
//======================================================================================================================
void UninitSlash(void)
{
	// テクスチャの破棄
	for (int nCntSlash = 0; nCntSlash < SLASHTYPE_MAX; nCntSlash++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureSlash[nCntSlash] != NULL)
		{ // 変数 (g_apTextureSlash) がNULLではない場合

			g_apTextureSlash[nCntSlash]->Release();
			g_apTextureSlash[nCntSlash] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffSlash != NULL)
	{ // 変数 (g_pVtxBuffSlash) がNULLではない場合

		g_pVtxBuffSlash->Release();
		g_pVtxBuffSlash = NULL;
	}
}

//======================================================================================================================
//	斬撃の更新処理
//======================================================================================================================
void UpdateSlash(void)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		if (g_aSlash[nCntSlash].bUse == true)
		{ // 斬撃が使用されている場合

			//----------------------------------------------------------------------------------------------------------
			//	当たり判定
			//----------------------------------------------------------------------------------------------------------
			switch (g_aSlash[nCntSlash].type)
			{ // 種類ごとの判定
			case SLASHTYPE_PLAYER:	// プレイヤーの斬撃

				// 敵と斬撃の当たり判定
				CollisionEnemy(&g_aSlash[nCntSlash]);

				// 雑草と斬撃の当たり判定
				CollisionWeed(&g_aSlash[nCntSlash]);

				// オブジェクトと斬撃の当たり判定
				CollisionObject(&g_aSlash[nCntSlash]);

				// 弾と斬撃の当たり判定
				CollisionBullet(&g_aSlash[nCntSlash]);

				// 処理を抜ける
				break;

			case SLASHTYPE_ENEMY:	// 敵の斬撃

				// プレイヤーと斬撃の当たり判定
				CollisionPlayer(&g_aSlash[nCntSlash]);

				// 処理を抜ける
				break;
			}

			//----------------------------------------------------------------------------------------------------------
			//	斬撃の透明化
			//----------------------------------------------------------------------------------------------------------
			// α値を減算
			g_aSlash[nCntSlash].fAlpha -= g_aSlash[nCntSlash].fChange;

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aSlash[nCntSlash].fAlpha);

			//----------------------------------------------------------------------------------------------------------
			//	斬撃の削除
			//----------------------------------------------------------------------------------------------------------
			// 寿命を減らす
			g_aSlash[nCntSlash].nLife--;

			if (g_aSlash[nCntSlash].nLife <= 0)
			{ // 斬撃の寿命が尽きた場合

				// 使用していない状態にする
				g_aSlash[nCntSlash].bUse = false;

				if (g_aSlash[nCntSlash].pSlashIDParent != NULL)
				{ // 斬撃の親の斬撃インデックスアドレスが設定されている場合

					// 斬撃インデックスの初期化
					*g_aSlash[nCntSlash].pSlashIDParent = NONE_SLASH;
				}
			}

			if (g_aSlash[nCntSlash].pUseParent != NULL)
			{ // 斬撃の親の使用状況アドレスが設定されている場合

				if (*g_aSlash[nCntSlash].pUseParent == false)
				{ // 斬撃の親が使用されていない場合

					// 斬撃を使用していない状態にする
					g_aSlash[nCntSlash].bUse = false;

					if (g_aSlash[nCntSlash].pSlashIDParent != NULL)
					{ // 斬撃の親の斬撃インデックスアドレスが設定されている場合

						// 斬撃インデックスの初期化
						*g_aSlash[nCntSlash].pSlashIDParent = NONE_SLASH;
					}
				}
			}
		}

		// 頂点データのポインタを 4つ分進める
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffSlash->Unlock();
}

//======================================================================================================================
//	斬撃の描画処理
//======================================================================================================================
void DrawSlash(void)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// カリングを無効にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		if (g_aSlash[nCntSlash].bUse == true)
		{ // 斬撃が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aSlash[nCntSlash].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aSlash[nCntSlash].rot.y, g_aSlash[nCntSlash].rot.x, g_aSlash[nCntSlash].rot.z);
			D3DXMatrixMultiply(&g_aSlash[nCntSlash].mtxWorld, &g_aSlash[nCntSlash].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aSlash[nCntSlash].pos.x, g_aSlash[nCntSlash].pos.y, g_aSlash[nCntSlash].pos.z);
			D3DXMatrixMultiply(&g_aSlash[nCntSlash].mtxWorld, &g_aSlash[nCntSlash].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aSlash[nCntSlash].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffSlash, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureSlash[g_aSlash[nCntSlash].type]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntSlash * 4, 2);
		}
	}

	// カリングを有効にする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//======================================================================================================================
//	斬撃の設定処理
//======================================================================================================================
int SetSlash(SLASHTYPE type, int nLife, float fRadius, int *pSlashID, bool *pUse)
{
	// ポインタを宣言
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		if (g_aSlash[nCntSlash].bUse == false)
		{ // 斬撃が使用されていない場合

			// 引数を代入
			g_aSlash[nCntSlash].type     = type;	// 種類
			g_aSlash[nCntSlash].nLife    = nLife;	// 寿命
			g_aSlash[nCntSlash].nMaxLife = nLife;	// 総寿命
			g_aSlash[nCntSlash].fRadius  = fRadius;	// 半径

			// 引数のアドレスを代入
			g_aSlash[nCntSlash].pSlashIDParent = pSlashID;	// 斬撃の親の斬撃インデックス
			g_aSlash[nCntSlash].pUseParent     = pUse;		// 斬撃の親の使用状況

			// 1Fごとの薄くなる量を設定
			g_aSlash[nCntSlash].fChange = 0.8f / g_aSlash[nCntSlash].nMaxLife;

			// 斬撃の情報を初期化
			g_aSlash[nCntSlash].pos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
			g_aSlash[nCntSlash].rot    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
			g_aSlash[nCntSlash].fAlpha = 1.0f;							// α値

			// 使用している状態にする
			g_aSlash[nCntSlash].bUse = true;

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffSlash->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点データのポインタを使用する斬撃のポインタまで進める
			pVtx += nCntSlash * 4;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-fRadius, 0.0f,  fRadius);
			pVtx[1].pos = D3DXVECTOR3( fRadius, 0.0f,  fRadius);
			pVtx[2].pos = D3DXVECTOR3(-fRadius, 0.0f, -fRadius);
			pVtx[3].pos = D3DXVECTOR3( fRadius, 0.0f, -fRadius);

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// 頂点バッファをアンロックする
			g_pVtxBuffSlash->Unlock();

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_SLASH);	// SE (斬撃)

			// 使用できる斬撃のIDを返す
			return nCntSlash;
		}
	}

	// 斬撃の総数が足りない場合 -1を返す
	return -1;
}

//======================================================================================================================
//	斬撃の位置・向きの設定処理
//======================================================================================================================
void SetPositionSlash(int nSlashID, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (nSlashID > NONE_SLASH)
	{ // 使用できるIDの場合

		// 引数の斬撃を設定
		g_aSlash[nSlashID].pos = pos;	// 位置
		g_aSlash[nSlashID].rot = rot;	// 向き

		// 斬撃を回転
		g_aSlash[nSlashID].rot.y -= (g_aSlash[nSlashID].nMaxLife - g_aSlash[nSlashID].nLife) * REV_SLASH_ROT;

		// 向きの正規化
		if      (g_aSlash[nSlashID].rot.y >  D3DX_PI) { g_aSlash[nSlashID].rot.y -= D3DX_PI * 2; }
		else if (g_aSlash[nSlashID].rot.y < -D3DX_PI) { g_aSlash[nSlashID].rot.y += D3DX_PI * 2; }
	}
}

//======================================================================================================================
//	プレイヤーと斬撃の当たり判定
//======================================================================================================================
void CollisionPlayer(Slash *pSlash)
{
	// 変数を宣言
	float fLength = 0.0f;				// 斬撃とプレイヤーとの間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// 斬撃とプレイヤーとの間の距離を求める
		fLength = (pSlash->pos.x - pPlayer->pos.x) * (pSlash->pos.x - pPlayer->pos.x)
				+ (pSlash->pos.z - pPlayer->pos.z) * (pSlash->pos.z - pPlayer->pos.z);

		if (fLength < ((pSlash->fRadius + PLAY_WIDTH) * (pSlash->fRadius + PLAY_WIDTH))	// 円の当たり判定
		&&  pSlash->pos.y + HIT_SLA_HEIGHT > pPlayer->pos.y								// 縦の判定 (下)
		&&  pSlash->pos.y - HIT_SLA_HEIGHT < pPlayer->pos.y + PLAY_HEIGHT)				// 縦の判定 (上)
		{ // 斬撃がプレイヤーに当たっている場合

			// プレイヤーのダメージ判定
			HitPlayer(pPlayer, SLA_DMG_ENE);
		}
	}
}

//======================================================================================================================
//	敵と斬撃の当たり判定
//======================================================================================================================
void CollisionEnemy(Slash *pSlash)
{
	// 変数を宣言
	float fLength = 0.0f;			// 斬撃と敵との間の距離

	// ポインタを宣言
	Enemy *pEnemy = GetEnemy();		// 敵の情報

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (pEnemy->bUse == true)
		{ // 敵が使用されている場合

			// 斬撃と敵との間の距離を求める
			fLength = (pSlash->pos.x - pEnemy->pos.x) * (pSlash->pos.x - pEnemy->pos.x)
					+ (pSlash->pos.z - pEnemy->pos.z) * (pSlash->pos.z - pEnemy->pos.z);

			if (fLength < ((pSlash->fRadius + pEnemy->pModelData->fRadius) * (pSlash->fRadius + pEnemy->pModelData->fRadius))	// 円の当たり判定
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pEnemy->pos.y																	// 縦の判定 (下)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)									// 縦の判定 (上)
			{ // 斬撃が敵に当たっている場合

				// 敵のダメージ判定
				HitEnemy(pEnemy, SLA_DMG_PLAY, DAMAGETYPE_SLASH);
			}
		}
	}
}

//======================================================================================================================
//	雑草と斬撃の当たり判定
//======================================================================================================================
void CollisionWeed(Slash *pSlash)
{
	// 変数を宣言
	float fLength = 0.0f;		// 斬撃と雑草との間の距離

	// ポインタを宣言
	Weed *pWeed = GetWeed();	// 雑草の情報

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++, pWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (pWeed->bUse == true)
		{ // 雑草が使用されている場合

			// 斬撃と雑草との間の距離を求める
			fLength = (pSlash->pos.x - pWeed->pos.x) * (pSlash->pos.x - pWeed->pos.x)
					+ (pSlash->pos.z - pWeed->pos.z) * (pSlash->pos.z - pWeed->pos.z);

			if (fLength < ((pSlash->fRadius + WEED_WIDTH) * (pSlash->fRadius + WEED_WIDTH))	// 円の当たり判定
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pWeed->pos.y								// 縦の判定 (下)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pWeed->pos.y + WEED_HEIGHT)				// 縦の判定 (上)
			{ // 斬撃が雑草に当たっている場合

				// 雑草のダメージ判定
				HitWeed(pWeed, SLA_DMG_WEED);
			}
		}
	}
}

//======================================================================================================================
//	オブジェクトと斬撃の当たり判定
//======================================================================================================================
void CollisionObject(Slash *pSlash)
{
	// 変数を宣言
	float fLength = 0.0f;				// 斬撃とオブジェクトとの間の距離

	// ポインタを宣言
	Object *pObject = GetObjectData();	// オブジェクトの情報

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // オブジェクトの最大表示数分繰り返す

		if (pObject->bUse == true && pObject->nBreakType != BREAKTYPE_NONE)
		{ // オブジェクトが使用されている且つ、オブジェクトが壊れない設定ではない場合

			// 斬撃とオブジェクトとの間の距離を求める
			fLength = (pSlash->pos.x - pObject->pos.x) * (pSlash->pos.x - pObject->pos.x)
					+ (pSlash->pos.z - pObject->pos.z) * (pSlash->pos.z - pObject->pos.z);

			if (fLength < ((pSlash->fRadius + pObject->pModelData->fRadius) * (pSlash->fRadius + pObject->pModelData->fRadius))	// 円の当たり判定
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pObject->pos.y																	// 縦の判定 (下)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pObject->pos.y + pObject->pModelData->fHeight)									// 縦の判定 (上)
			{ // 斬撃がオブジェクトに当たっている場合

				// オブジェクトのダメージ判定
				HitObject(pObject, SLA_DMG_PLAY);
			}
		}
	}
}

//======================================================================================================================
//	弾と斬撃の当たり判定
//======================================================================================================================
void CollisionBullet(Slash *pSlash)
{
	// 変数を宣言
	float fLength = 0.0f;			// 斬撃と弾との間の距離
	float fRot    = 0.0f;			// 斬撃から見た弾の方向

	// ポインタを宣言
	Bullet *pBullet = GetBullet();	// 弾の情報

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++, pBullet++)
	{ // 弾の最大表示数分繰り返す

		if (pBullet->bUse == true && pBullet->type == BULLETTYPE_ENEMY)
		{ // 弾が使用されている且つ、弾の種類が敵の弾の場合

			// 斬撃と弾との間の距離を求める
			fLength = (pSlash->pos.x - pBullet->pos.x) * (pSlash->pos.x - pBullet->pos.x)
					+ (pSlash->pos.z - pBullet->pos.z) * (pSlash->pos.z - pBullet->pos.z);

			if (fLength < ((pSlash->fRadius + BULL_WIDTH) * (pSlash->fRadius + BULL_WIDTH))	// 円の当たり判定
			&&  pSlash->pos.y + HIT_SLA_HEIGHT > pBullet->pos.y - BULL_HEIGHT				// 縦の判定 (下)
			&&  pSlash->pos.y - HIT_SLA_HEIGHT < pBullet->pos.y + BULL_HEIGHT)				// 縦の判定 (上)
			{ // 斬撃が弾に当たっている場合

				// 斬撃から見た弾の方向を求める
				fRot = atan2f(pBullet->pos.x - pSlash->pos.x, pBullet->pos.z - pSlash->pos.z);

				// 弾の情報の再設定
				pBullet->move = D3DXVECTOR3
				( // 引数
					sinf(fRot) * PLAY_MOVE_BULLET,	// 移動量 (x)
					0.0f,							// 移動量 (y)
					cosf(fRot) * PLAY_MOVE_BULLET	// 移動量 (z)
				);
				pBullet->type  = BULLETTYPE_PLAYER;	// 種類をプレイヤーの弾に変更
				pBullet->nLife = LIFE_BULLET;		// 弾の寿命を初期化

				// 弾の設定
				SetBullet
				( // 引数
					pBullet->pos,		// 位置
					D3DXVECTOR3
					( // 引数
						sinf(fRot + (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET,	// 移動量 (x)
						0.0f,														// 移動量 (y)
						cosf(fRot + (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET	// 移動量 (z)
					),
					BULLETTYPE_PLAYER,	// 種類
					false				// 効果音
				);

				// 弾の設定
				SetBullet
				( // 引数
					pBullet->pos,		// 位置
					D3DXVECTOR3
					( // 引数
						sinf(fRot - (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET,	// 移動量 (x)
						0.0f,														// 移動量 (y)
						cosf(fRot - (D3DX_PI * REF_BULL_PLUS)) * PLAY_MOVE_BULLET	// 移動量 (z)
					),
					BULLETTYPE_PLAYER,	// 種類
					false				// 効果音
				);

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_REF);	// SE (反射)
			}
		}
	}
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	斬撃の総数取得処理
//**********************************************************************************************************************
int GetNumSlash(void)
{
	// 変数を宣言
	int nNumSlash = 0;	// 斬撃の総数の確認用

	for (int nCntSlash = 0; nCntSlash < MAX_SLASH; nCntSlash++)
	{ // 斬撃の最大表示数分繰り返す

		if (g_aSlash[nCntSlash].bUse == true)
		{ // 斬撃が使用されている場合

			// カウンターを加算
			nNumSlash++;
		}
	}

	// 変数の値を返す
	return nNumSlash;	// 斬撃の総数
}
#endif