//======================================================================================================================
//
//	弾処理 [bullet.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"

#include "game.h"
#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "explosion.h"
#include "meshwall.h"
#include "object.h"
#include "particle.h"
#include "shadow.h"
#include "sound.h"
#include "weed.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define BULL_DMG_PLAY	(100)		// 弾の攻撃力 (プレイヤー)
#define BULL_DMG_ENE	(10)		// 弾の攻撃力 (敵)

#define SHADOW_BULL		(30.0f)		// 弾の影の半径

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apTextureBullet[] =		// テクスチャの相対パス
{
	"02_data\\02_TEXTURE\\bullet000.png",	// 弾 (通常) のテクスチャ相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (TEXTURE_BULLET)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_BULLET_NORMAL,			// 弾 (通常)
	TEXTURE_BULLET_MAX,				// この列挙型の総数
} TEXTURE_BULLET;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void CollisionPlayer(Bullet *pBullet);			// プレイヤーと弾の当たり判定
void CollisionEnemy(Bullet *pBullet);			// 敵と弾の当たり判定
void CollisionWeed(Bullet *pBullet);			// 雑草と弾の当たり判定
void CollisionObject(Bullet *pBullet);			// オブジェクトと弾の当たり判定
void CollisionMeshWall(Bullet *pBullet);		// メッシュウォールと弾の当たり判定

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureBullet[TEXTURE_BULLET_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBullet = NULL;					// 頂点バッファへのポインタ

Bullet g_aBullet[MAX_BULLET];		// 弾の情報

//======================================================================================================================
//	弾の初期化処理
//======================================================================================================================
void InitBullet(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntBullet = 0; nCntBullet < TEXTURE_BULLET_MAX; nCntBullet++)
	{ // 使用するテクスチャ数分繰り返す

		D3DXCreateTextureFromFile(pDevice, apTextureBullet[nCntBullet], &g_apTextureBullet[nCntBullet]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer
	( // 引数
		sizeof(VERTEX_3D) * 4 * MAX_BULLET,		// 必要頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// 頂点フォーマット
		D3DPOOL_MANAGED,
		&g_pVtxBuffBullet,
		NULL
	);

	// 弾の情報の初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		g_aBullet[nCntBullet].pos       = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の位置
		g_aBullet[nCntBullet].oldPos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 前回の位置
		g_aBullet[nCntBullet].move      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
		g_aBullet[nCntBullet].type      = BULLETTYPE_PLAYER;				// 種類
		g_aBullet[nCntBullet].nLife     = 0;								// 寿命
		g_aBullet[nCntBullet].nShadowID = NONE_SHADOW;						// 影のインデックス
		g_aBullet[nCntBullet].bUse      = false;							// 使用状況
	}

	//------------------------------------------------------------------------------------------------------------------
	//	頂点情報の初期化
	//------------------------------------------------------------------------------------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffBullet->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-BULL_WIDTH,  BULL_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( BULL_WIDTH,  BULL_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-BULL_WIDTH, -BULL_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3( BULL_WIDTH, -BULL_HEIGHT, 0.0f);

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
	g_pVtxBuffBullet->Unlock();
}

//======================================================================================================================
//	弾の終了処理
//======================================================================================================================
void UninitBullet(void)
{
	// テクスチャの破棄
	for (int nCntBullet = 0; nCntBullet < TEXTURE_BULLET_MAX; nCntBullet++)
	{ // 使用するテクスチャ数分繰り返す

		if (g_apTextureBullet[nCntBullet] != NULL)
		{ // 変数 (g_apTextureBullet) がNULLではない場合

			g_apTextureBullet[nCntBullet]->Release();
			g_apTextureBullet[nCntBullet] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffBullet != NULL)
	{ // 変数 (g_pVtxBuffBullet) がNULLではない場合

		g_pVtxBuffBullet->Release();
		g_pVtxBuffBullet = NULL;
	}
}

//======================================================================================================================
//	弾の更新処理
//======================================================================================================================
void UpdateBullet(void)
{
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		if (g_aBullet[nCntBullet].bUse == true)
		{ // 弾が使用されている場合

			//----------------------------------------------------------------------------------------------------------
			//	前回位置の更新
			//----------------------------------------------------------------------------------------------------------
			g_aBullet[nCntBullet].oldPos = g_aBullet[nCntBullet].pos;

			//----------------------------------------------------------------------------------------------------------
			//	位置の更新
			//----------------------------------------------------------------------------------------------------------
			g_aBullet[nCntBullet].pos.x += g_aBullet[nCntBullet].move.x;
			g_aBullet[nCntBullet].pos.z += g_aBullet[nCntBullet].move.z;

			//----------------------------------------------------------------------------------------------------------
			//	当たり判定
			//----------------------------------------------------------------------------------------------------------
			switch (g_aBullet[nCntBullet].type)
			{ // 種類ごとの処理
			case BULLETTYPE_PLAYER:		// ブレイヤーの弾

				// 敵と弾の当たり判定
				CollisionEnemy(&g_aBullet[nCntBullet]);

				// 雑草と弾の当たり判定
				CollisionWeed(&g_aBullet[nCntBullet]);

				// 処理を抜ける
				break;

			case BULLETTYPE_ENEMY:		// 敵の弾

				// プレイヤーと弾の当たり判定
				CollisionPlayer(&g_aBullet[nCntBullet]);

				// 処理を抜ける
				break;
			}

			// オブジェクトと弾の当たり判定
			CollisionObject(&g_aBullet[nCntBullet]);

			// メッシュウォールと弾の当たり判定
			CollisionMeshWall(&g_aBullet[nCntBullet]);

			//----------------------------------------------------------------------------------------------------------
			//	エフェクトの更新
			//----------------------------------------------------------------------------------------------------------
			switch (g_aBullet[nCntBullet].type)
			{ // 種類ごとの処理
			case BULLETTYPE_PLAYER:		// ブレイヤーの弾

				// エフェクトの設定
				SetEffect
				( // 引数
					g_aBullet[nCntBullet].pos,			// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 移動量
					D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),	// 色
					EFFECTTYPE_NORMAL,					// 種類
					14,									// 寿命
					20.0f,								// 半径
					1.0f								// 減算量 (半径)
				);

				// エフェクトの設定
				SetEffect
				( // 引数
					g_aBullet[nCntBullet].pos,			// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 移動量
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),	// 色
					EFFECTTYPE_NORMAL,					// 種類
					14,									// 寿命
					20.0f,								// 半径
					1.5f								// 減算量 (半径)
				);

				// 処理を抜ける
				break;

			case BULLETTYPE_ENEMY:		// 敵の弾

				// エフェクトの設定
				SetEffect
				( // 引数
					g_aBullet[nCntBullet].pos,			// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 移動量
					D3DXCOLOR(0.3f, 0.6f, 1.0f, 1.0f),	// 色
					EFFECTTYPE_NORMAL,					// 種類
					14,									// 寿命
					20.0f,								// 半径
					1.0f								// 減算量 (半径)
				);

				// エフェクトの設定
				SetEffect
				( // 引数
					g_aBullet[nCntBullet].pos,			// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 移動量
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),	// 色
					EFFECTTYPE_NORMAL,					// 種類
					14,									// 寿命
					20.0f,								// 半径
					1.5f								// 減算量 (半径)
				);

				// 処理を抜ける
				break;
			}

			//----------------------------------------------------------------------------------------------------------
			//	影の更新
			//----------------------------------------------------------------------------------------------------------
			// 影の位置設定
			SetPositionShadow(g_aBullet[nCntBullet].nShadowID, g_aBullet[nCntBullet].pos);

			//----------------------------------------------------------------------------------------------------------
			//	弾の削除
			//----------------------------------------------------------------------------------------------------------
			// 寿命を減らす
			g_aBullet[nCntBullet].nLife--;

			if (g_aBullet[nCntBullet].pos.z > GetLimitStage().fNear
			||  g_aBullet[nCntBullet].pos.z < GetLimitStage().fFar
			||  g_aBullet[nCntBullet].pos.x > GetLimitStage().fRight
			||  g_aBullet[nCntBullet].pos.x < GetLimitStage().fLeft
			||  g_aBullet[nCntBullet].nLife <= 0)
			{ // 弾が範囲外、または寿命が尽きた場合

				// 爆発の設定
				SetExplosion(g_aBullet[nCntBullet].pos, SOUNDTYPE_EXPLOSION);

				// パーティクルの設定
				SetParticle
				( // 引数
					g_aBullet[nCntBullet].pos,			// 位置
					D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// 色
					PARTICLETYPE_EXPLOSION,				// 種類
					SPAWN_PARTICLE_EXPLOSION,			// エフェクト数
					2									// 寿命
				);

				// 使用していない状態にする
				g_aBullet[nCntBullet].bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	弾の描画処理
//======================================================================================================================
void DrawBullet(void)
{
	// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス取得用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// αテストの参照値設定

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		if (g_aBullet[nCntBullet].bUse == true)
		{ // 弾が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aBullet[nCntBullet].mtxWorld);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aBullet[nCntBullet].mtxWorld, NULL, &mtxView);	// 逆行列を求める
			g_aBullet[nCntBullet].mtxWorld._41 = 0.0f;
			g_aBullet[nCntBullet].mtxWorld._42 = 0.0f;
			g_aBullet[nCntBullet].mtxWorld._43 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aBullet[nCntBullet].pos.x, g_aBullet[nCntBullet].pos.y, g_aBullet[nCntBullet].pos.z);
			D3DXMatrixMultiply(&g_aBullet[nCntBullet].mtxWorld, &g_aBullet[nCntBullet].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aBullet[nCntBullet].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffBullet, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureBullet[TEXTURE_BULLET_NORMAL]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntBullet * 4, 2);
		}
	}

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//======================================================================================================================
//	弾の設定処理
//======================================================================================================================
void SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 move, BULLETTYPE type, bool bSound)
{
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		if (g_aBullet[nCntBullet].bUse == false)
		{ // 弾が使用されていない場合

			// 引数を代入
			g_aBullet[nCntBullet].pos    = pos;		// 現在の位置
			g_aBullet[nCntBullet].oldPos = pos;		// 前回の位置
			g_aBullet[nCntBullet].move   = move;	// 移動量
			g_aBullet[nCntBullet].type   = type;	// 種類

			// 寿命を設定
			g_aBullet[nCntBullet].nLife = LIFE_BULLET;

			// 使用している状態にする
			g_aBullet[nCntBullet].bUse = true;

			// 影のインデックスを設定
			g_aBullet[nCntBullet].nShadowID = SetShadow(0.5f, SHADOW_BULL, &g_aBullet[nCntBullet].nShadowID, &g_aBullet[nCntBullet].bUse);

			// 影の位置設定
			SetPositionShadow(g_aBullet[nCntBullet].nShadowID, g_aBullet[nCntBullet].pos);

			if (bSound == true)
			{ // 効果音の再生が ON の場合

				// サウンドの再生
				PlaySound(SOUND_LABEL_SE_SHOT);		// SE (発射)
			}

			// 処理を抜ける
			break;
		}
	}
}

//======================================================================================================================
//	プレイヤーと弾の当たり判定
//======================================================================================================================
void CollisionPlayer(Bullet *pBullet)
{
	// 変数を宣言
	float fLength = 0.0f;				// 弾とプレイヤーとの間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// 弾とプレイヤーとの間の距離を求める
		fLength = (pBullet->pos.x - pPlayer->pos.x) * (pBullet->pos.x - pPlayer->pos.x)
				+ (pBullet->pos.z - pPlayer->pos.z) * (pBullet->pos.z - pPlayer->pos.z);

		if (fLength < ((BULL_WIDTH + PLAY_WIDTH) * (BULL_WIDTH + PLAY_WIDTH))	// 円の当たり判定
		&&  pBullet->pos.y + BULL_HEIGHT > pPlayer->pos.y						// 縦の判定 (下)
		&&  pBullet->pos.y - BULL_HEIGHT < pPlayer->pos.y + PLAY_HEIGHT)		// 縦の判定 (上)
		{ // 弾がプレイヤーに当たっている場合

			// プレイヤーのダメージ判定
			HitPlayer(pPlayer, BULL_DMG_ENE);

			// 使用していない状態にする
			pBullet->bUse = false;
		}
	}
}

//======================================================================================================================
//	敵と弾の当たり判定
//======================================================================================================================
void CollisionEnemy(Bullet *pBullet)
{
	// 変数を宣言
	float fLength = 0.0f;			// 弾と敵との間の距離

	// ポインタを宣言
	Enemy *pEnemy = GetEnemy();		// 敵の情報

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (pEnemy->bUse == true)
		{ // 敵が使用されている場合

			// 弾と敵との間の距離を求める
			fLength = (pBullet->pos.x - pEnemy->pos.x) * (pBullet->pos.x - pEnemy->pos.x)
					+ (pBullet->pos.z - pEnemy->pos.z) * (pBullet->pos.z - pEnemy->pos.z);

			if (fLength < ((BULL_WIDTH + pEnemy->pModelData->fRadius) * (BULL_WIDTH + pEnemy->pModelData->fRadius))	// 円の当たり判定
			&&  pBullet->pos.y + BULL_HEIGHT > pEnemy->pos.y														// 縦の判定 (下)
			&&  pBullet->pos.y - BULL_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)							// 縦の判定 (上)
			{ // 弾が敵に当たっている場合

				// 敵のダメージ判定
				HitEnemy(pEnemy, BULL_DMG_PLAY, DAMAGETYPE_BULLET);

				// 使用していない状態にする
				pBullet->bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	雑草と弾の当たり判定
//======================================================================================================================
void CollisionWeed(Bullet *pBullet)
{
	// 変数を宣言
	float fLength = 0.0f;		// 弾と雑草との間の距離

	// ポインタを宣言
	Weed *pWeed = GetWeed();	// 雑草の情報

	for (int nCntWeed = 0; nCntWeed < MAX_WEED; nCntWeed++, pWeed++)
	{ // 雑草の最大表示数分繰り返す

		if (pWeed->bUse == true)
		{ // 雑草が使用されている場合

			// 弾と雑草との間の距離を求める
			fLength = (pBullet->pos.x - pWeed->pos.x) * (pBullet->pos.x - pWeed->pos.x)
					+ (pBullet->pos.z - pWeed->pos.z) * (pBullet->pos.z - pWeed->pos.z);

			if (fLength < ((BULL_WIDTH + WEED_WIDTH) * (BULL_WIDTH + WEED_WIDTH)))
			{ // 弾が雑草に当たっている場合

				// 雑草のダメージ判定
				HitWeed(pWeed, BULL_DMG_PLAY);
			}
		}
	}
}

//======================================================================================================================
//	オブジェクトと弾の当たり判定
//======================================================================================================================
void CollisionObject(Bullet *pBullet)
{
	// ポインタを宣言
	Object *pObject = GetObjectData();	// オブジェクトの情報

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{ // オブジェクトの最大表示数分繰り返す

		if (pObject->bUse == true)
		{ // オブジェクトが使用されている場合

			if (pBullet->pos.x + BULL_WIDTH  > pObject->pos.x + pObject->pModelData->vtxMin.x
			&&  pBullet->pos.y + BULL_HEIGHT > pObject->pos.y + pObject->pModelData->vtxMin.y
			&&  pBullet->pos.z + BULL_DEPTH  > pObject->pos.z + pObject->pModelData->vtxMin.z
			&&  pBullet->pos.x - BULL_WIDTH  < pObject->pos.x + pObject->pModelData->vtxMax.x
			&&  pBullet->pos.y - BULL_HEIGHT < pObject->pos.y + pObject->pModelData->vtxMax.y
			&&  pBullet->pos.z - BULL_DEPTH  < pObject->pos.z + pObject->pModelData->vtxMax.z)
			{ // 弾がオブジェクトに当たっている場合

				if (pBullet->type == BULLETTYPE_PLAYER && pObject->nBreakType != BREAKTYPE_NONE)
				{ // 弾の種類がプレイヤー且つ、オブジェクトの壊れ方の種類が壊れない種類ではない場合

					// オブジェクトのダメージ判定
					HitObject(pObject, BULL_DMG_PLAY);
				}

				// 爆発の設定
				SetExplosion(pBullet->pos, SOUNDTYPE_EXPLOSION);

				// パーティクルの設定
				SetParticle
				( // 引数
					pBullet->pos,						// 位置
					D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// 色
					PARTICLETYPE_EXPLOSION,				// 種類
					SPAWN_PARTICLE_EXPLOSION,			// エフェクト数
					2									// 寿命
				);

				// 使用していない状態にする
				pBullet->bUse = false;
			}
		}
	}
}

//======================================================================================================================
//	メッシュウォールと弾の当たり判定
//======================================================================================================================
void CollisionMeshWall(Bullet *pBullet)
{
	// 変数を宣言
	D3DXVECTOR3 posLeft;		// メッシュウォールの左端の座標
	D3DXVECTOR3 posRight;		// メッシュウォールの右端の座標
	D3DXVECTOR3 vecMove;		// 移動ベクトル
	D3DXVECTOR3 vecLine;		// 境界線ベクトル
	D3DXVECTOR3 vecToPos;		// メッシュウォールの左端と弾の現在位置のベクトル
	D3DXVECTOR3 vecToOldPos;	// メッシュウォールの左端と弾の過去位置のベクトル
	D3DXVECTOR3 posCloss;		// 爆発の位置
	float       fSmallArea;		// 現在の面積
	float       fBigArea;		// 最大の面積
	float       fRate;			// 面積の割合

	// ポインタを宣言
	MeshWall *pMeshWall = GetMeshWall();	// メッシュウォールの情報

	for (int nCntObject = 0; nCntObject < MAX_MESHWALL; nCntObject++, pMeshWall++)
	{ // メッシュウォールの最大表示数分繰り返す

		if (pMeshWall->bUse == true)
		{ // メッシュウォールが使用されている場合
	
			// メッシュウォールの左端の座標を求める
			posLeft = D3DXVECTOR3
			( // 引数
				pMeshWall->pos.x - sinf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f),	// x
				0.0f,																						// y
				pMeshWall->pos.z - cosf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f)	// z
			);

			// メッシュウォールの右端の座標を求める
			posRight = D3DXVECTOR3
			( // 引数
				pMeshWall->pos.x + sinf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f),	// x
				0.0f,																						// y
				pMeshWall->pos.z + cosf(pMeshWall->rot.y + (D3DX_PI * 0.5f)) * (pMeshWall->fWidth * 0.5f)	// z
			);

			// 移動ベクトルを求める
			vecMove = pBullet->pos - pBullet->oldPos;

			// 境界線ベクトルを求める
			vecLine = posRight - posLeft;

			// メッシュウォールの左端と弾の現在位置のベクトルを求める
			vecToPos = pBullet->pos - posLeft;

			// メッシュウォールの左端と弾の現在位置のベクトルを求める
			vecToOldPos = pBullet->oldPos - posLeft;

			// vecToPos と vecMove でできた平行四辺形の面積を求める
			fSmallArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

			// vecLine と vecMove でできた平行四辺形の面積を求める
			fBigArea = (vecLine.z  * vecMove.x) - (vecLine.x  * vecMove.z);

			// 面積の割合を求める
			fRate = fSmallArea / fBigArea;

			if ((vecLine.z * vecToOldPos.x) - (vecLine.x * vecToOldPos.z) > 0			// 弾の前回の位置が境界線ベクトルから見て右にいる場合
			&&  (vecLine.z * vecToPos.x   ) - (vecLine.x * vecToPos.z   ) < 0			// 弾の現在の位置が境界線ベクトルから見て左にいる場合
			&&  pBullet->pos.y - BULL_HEIGHT < pMeshWall->pos.y + pMeshWall->fHeight	// 弾の現在の位置 (y) が壁の位置 (y) より下にいる場合
			&&  pBullet->pos.y + BULL_HEIGHT > pMeshWall->pos.y)						// 弾の現在の位置 (y) が壁の位置 (y) より上にいる場合
			{ // 弾が境界線に当たっている場合

				if (fRate >= 0.0f && fRate <= 1.0f)
				{ // 弾がメッシュウォールに当たっている場合 (割合が 0.0f ～ 1.0f の範囲内)

					// 爆発の位置を設定
					posCloss = D3DXVECTOR3
					( // 引数
						posLeft.x + (vecLine.x * fRate),	// 移動ベクトルと境界線ベクトルの交差座標 (x)
						pBullet->pos.y,						// 弾の座標 (y)
						posLeft.z + (vecLine.z * fRate)		// 移動ベクトルと境界線ベクトルの交差座標 (z)
					);

					// 爆発の設定
					SetExplosion(posCloss, SOUNDTYPE_EXPLOSION);

					// パーティクルの設定
					SetParticle
					( // 引数
						posCloss,							// 位置
						D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),	// 色
						PARTICLETYPE_EXPLOSION,				// 種類
						SPAWN_PARTICLE_EXPLOSION,			// エフェクト数
						2									// 寿命
					);

					// 使用していない状態にする
					pBullet->bUse = false;
				}
			}
		}
	}
}

//======================================================================================================================
//	弾の取得処理
//======================================================================================================================
Bullet *GetBullet(void)
{
	// 弾の情報の先頭アドレスを返す
	return &g_aBullet[0];
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	弾の総数取得処理
//**********************************************************************************************************************
int GetNumBullet(void)
{
	// 変数を宣言
	int nNumBullet = 0;	// 弾の総数の確認用

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{ // 弾の最大表示数分繰り返す

		if (g_aBullet[nCntBullet].bUse == true)
		{ // 弾が使用されている場合

			// カウンターを加算
			nNumBullet++;
		}
	}

	// 変数の値を返す
	return nNumBullet;	// 弾の総数
}
#endif