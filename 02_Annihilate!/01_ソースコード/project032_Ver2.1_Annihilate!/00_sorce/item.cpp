//======================================================================================================================
//
//	アイテム処理 [item.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "input.h"

#include "item.h"
#include "particle.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"
#include "tutorial.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define ITEM_HEAL			(100)	// アイテムの回復量
#define EFFECT_TIME_ITEM	(160)	// パーティクルを出す間隔
#define MOVE_ROT_ITEM		(0.03f)	// アイテムの回転量

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apModelItem[] =		// モデルの相対パス
{
	"02_data\\03_MODEL\\04_ITEM\\heal000.x",	// 回復アイテムのモデル相対パス
};

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void TutorialItem(void);			// チュートリアルのアイテムの更新処理
void GameItem(void);				// ゲームのアイテムの更新処理

void CollisionPlayer(Item *pItem);	// アイテムとプレイヤーの当たり判定

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
Model g_aModelItem[ITEMTYPE_MAX];	// アイテムのモデル情報
Item  g_aItem[MAX_ITEM];			// アイテムの情報

//======================================================================================================================
//	アイテムの初期化処理
//======================================================================================================================
void InitItem(void)
{
	// 変数を宣言
	int         nNumVtx;		// モデルの頂点数
	DWORD       dwSizeFVF;		// モデルの頂点フォーマットのサイズ
	BYTE        *pVtxBuff;		// モデルの頂点バッファへのポインタ
	D3DXVECTOR3 vtx;			// モデルの頂点座標
	D3DXVECTOR3 size;			// モデルの大きさ

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATERIAL      *pMat;					// マテリアルへのポインタ

	// アイテムのモデル情報の初期化
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		g_aModelItem[nCntItem].apTexture[MAX_MODEL_TEXTURE] = {};						// テクスチャへのポインタ
		g_aModelItem[nCntItem].pMesh    = NULL;											// メッシュ (頂点情報) へのポインタ
		g_aModelItem[nCntItem].pBuffMat = NULL;											// マテリアルへのポインタ
		g_aModelItem[nCntItem].dwNumMat = 0;											// マテリアルの数
		g_aModelItem[nCntItem].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// 最小の頂点座標
		g_aModelItem[nCntItem].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// 最大の頂点座標
		g_aModelItem[nCntItem].fHeight  = 0.0f;											// 縦幅
		g_aModelItem[nCntItem].fRadius  = 0.0f;											// 半径
	}

	// アイテムの情報の初期化
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		g_aItem[nCntItem].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_aItem[nCntItem].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_aItem[nCntItem].pModelData     = NULL;							// モデル情報
		g_aItem[nCntItem].nType          = 0;								// 種類
		g_aItem[nCntItem].nCounterEffect = 0;								// エフェクト管理カウンター
		g_aItem[nCntItem].nShadowID      = NONE_SHADOW;						// 影のインデックス
		g_aItem[nCntItem].bUse           = false;							// 使用状況
	}

	// xファイルの読み込み
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		D3DXLoadMeshFromX
		( // 引数
			apModelItem[nCntItem],				// モデルの相対パス
			D3DXMESH_SYSTEMMEM,
			pDevice,							// デバイスへのポインタ
			NULL,
			&g_aModelItem[nCntItem].pBuffMat,	// マテリアルへのポインタ
			NULL,
			&g_aModelItem[nCntItem].dwNumMat,	// マテリアルの数
			&g_aModelItem[nCntItem].pMesh		// メッシュ (頂点情報) へのポインタ
		);
	}

	// 当たり判定の作成
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		// モデルの頂点数を取得
		nNumVtx = g_aModelItem[nCntItem].pMesh->GetNumVertices();

		// モデルの頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(g_aModelItem[nCntItem].pMesh->GetFVF());

		// モデルの頂点バッファをロック
		g_aModelItem[nCntItem].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // モデルの頂点数分繰り返す

			// モデルの頂点座標を代入
			vtx = *(D3DXVECTOR3*)pVtxBuff;

			// 頂点座標 (x) の設定
			if (vtx.x < g_aModelItem[nCntItem].vtxMin.x)
			{ // 今回の頂点座標 (x) が、現状の頂点座標 (x) よりも小さい場合

				// 今回の頂点情報 (x) を代入
				g_aModelItem[nCntItem].vtxMin.x = vtx.x;
			}
			else if (vtx.x > g_aModelItem[nCntItem].vtxMax.x)
			{ // 今回の頂点座標 (x) が、現状の頂点座標 (x) よりも大きい場合

				// 今回の頂点情報 (x) を代入
				g_aModelItem[nCntItem].vtxMax.x = vtx.x;
			}

			// 頂点座標 (y) の設定
			if (vtx.y < g_aModelItem[nCntItem].vtxMin.y)
			{ // 今回の頂点座標 (y) が、現状の頂点座標 (y) よりも小さい場合

				// 今回の頂点情報 (y) を代入
				g_aModelItem[nCntItem].vtxMin.y = vtx.y;
			}
			else if (vtx.y > g_aModelItem[nCntItem].vtxMax.y)
			{ // 今回の頂点座標 (y) が、現状の頂点座標 (y) よりも大きい場合

				// 今回の頂点情報 (y) を代入
				g_aModelItem[nCntItem].vtxMax.y = vtx.y;
			}

			// 頂点座標 (z) の設定
			if (vtx.z < g_aModelItem[nCntItem].vtxMin.z)
			{ // 今回の頂点座標 (z) が、現状の頂点座標 (z) よりも小さい場合

				// 今回の頂点情報 (z) を代入
				g_aModelItem[nCntItem].vtxMin.z = vtx.z;
			}
			else if (vtx.z > g_aModelItem[nCntItem].vtxMax.z)
			{ // 今回の頂点座標 (z) が、現状の頂点座標 (z) よりも大きい場合

				// 今回の頂点情報 (z) を代入
				g_aModelItem[nCntItem].vtxMax.z = vtx.z;
			}

			// 頂点フォーマットのサイズ分ポインタを進める
			pVtxBuff += dwSizeFVF;
		}

		// モデルの頂点バッファをアンロック
		g_aModelItem[nCntItem].pMesh->UnlockVertexBuffer();

		// モデルサイズを求める
		size = g_aModelItem[nCntItem].vtxMax - g_aModelItem[nCntItem].vtxMin;

		// モデルの縦幅を代入
		g_aModelItem[nCntItem].fHeight = size.y;

		// モデルの円の当たり判定を作成
		g_aModelItem[nCntItem].fRadius = ((size.x * 0.5f) + (size.z * 0.5f)) * 0.5f;
	}

	// テクスチャの読み込み
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		// マテリアル情報に対するポインタを取得
		pMat = (D3DXMATERIAL*)g_aModelItem[nCntItem].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelItem[nCntItem].dwNumMat; nCntMat++)
		{ // マテリアルの数分繰り返す

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // テクスチャファイルが存在する場合

				// テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelItem[nCntItem].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	アイテムの終了処理
//======================================================================================================================
void UninitItem(void)
{
	// テクスチャの破棄
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		for (int nCntMat = 0; nCntMat < MAX_MODEL_TEXTURE; nCntMat++)
		{ // 使用するテクスチャ数分繰り返す

			if (g_aModelItem[nCntItem].apTexture[nCntMat] != NULL)
			{ // 変数 (g_aModelItem[nCntItem].apTexture) がNULLではない場合

				g_aModelItem[nCntItem].apTexture[nCntMat]->Release();
				g_aModelItem[nCntItem].apTexture[nCntMat] = NULL;
			}
		}
	}

	// メッシュの破棄
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		if (g_aModelItem[nCntItem].pMesh != NULL)
		{ // 変数 (g_aModelItem[nCntItem].pMesh) がNULLではない場合

			g_aModelItem[nCntItem].pMesh->Release();
			g_aModelItem[nCntItem].pMesh = NULL;
		}
	}

	// マテリアルの破棄
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{ // アイテムの種類分繰り返す

		if (g_aModelItem[nCntItem].pBuffMat != NULL)
		{ // 変数 (g_aModelItem[nCntItem].pBuffMat) がNULLではない場合

			g_aModelItem[nCntItem].pBuffMat->Release();
			g_aModelItem[nCntItem].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	アイテムの更新処理
//======================================================================================================================
void UpdateItem(void)
{
	switch (GetMode())
	{ // モードごとの更新
	case MODE_TUTORIAL:	// チュートリアル画面

		// チュートリアルのアイテムの更新
		TutorialItem();

		// 処理を抜ける
		break;

	case MODE_GAME:		// ゲーム画面

		// ゲームのアイテムの更新
		GameItem();

		// 処理を抜ける
		break;
	}
}

//======================================================================================================================
//	アイテムの描画処理
//======================================================================================================================
void DrawItem(void)
{
	// 変数を宣言
	D3DXMATRIX   mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATERIAL     *pMat;						// マテリアルデータへのポインタ

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		if (g_aItem[nCntItem].bUse == true)
		{ // アイテムが使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aItem[nCntItem].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aItem[nCntItem].rot.y, g_aItem[nCntItem].rot.x, g_aItem[nCntItem].rot.z);
			D3DXMatrixMultiply(&g_aItem[nCntItem].mtxWorld, &g_aItem[nCntItem].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aItem[nCntItem].pos.x, g_aItem[nCntItem].pos.y, g_aItem[nCntItem].pos.z);
			D3DXMatrixMultiply(&g_aItem[nCntItem].mtxWorld, &g_aItem[nCntItem].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aItem[nCntItem].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_aItem[nCntItem].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aItem[nCntItem].pModelData->dwNumMat; nCntMat++)
			{ // マテリアルの数分繰り返す

				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aItem[nCntItem].pModelData->apTexture[nCntMat]);

				// モデルの描画
				g_aItem[nCntItem].pModelData->pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//======================================================================================================================
//	アイテムの設定処理
//======================================================================================================================
void SetItem(D3DXVECTOR3 pos, int nType)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		if (g_aItem[nCntItem].bUse == false)
		{ // アイテムが使用されていない場合

			// 引数を代入
			g_aItem[nCntItem].pos    = pos;		// 位置
			g_aItem[nCntItem].nType  = nType;	// 種類

			// アイテムの情報を初期化
			g_aItem[nCntItem].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
			g_aItem[nCntItem].nCounterEffect = 0;					// エフェクト管理カウンター

			// 使用している状態にする
			g_aItem[nCntItem].bUse = true;

			// モデル情報を設定
			g_aItem[nCntItem].pModelData = &g_aModelItem[nType];	// モデル情報

			// 影のインデックスを設定
			g_aItem[nCntItem].nShadowID = SetShadow
			( // 引数
				0.5f,																									// α値
				fabsf(g_aModelItem[g_aItem[nCntItem].nType].vtxMax.x - g_aModelItem[g_aItem[nCntItem].nType].vtxMin.x),	// 半径
				&g_aItem[nCntItem].nShadowID,																			// 影の親の影インデックス
				&g_aItem[nCntItem].bUse																					// 影の親の使用状況
			);

			// 影の位置設定
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_SPAWN);	// SE (出現)

			// 処理を抜ける
			break;
		}
	}
}

//======================================================================================================================
//	アイテムとプレイヤーの当たり判定
//======================================================================================================================
void CollisionPlayer(Item *pItem)
{
	// 変数を宣言
	float fLength = 0.0f;				// アイテムとプレイヤーとの間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// アイテムとプレイヤーとの間の距離を求める
		fLength = (pItem->pos.x - pPlayer->pos.x) * (pItem->pos.x - pPlayer->pos.x)
				+ (pItem->pos.z - pPlayer->pos.z) * (pItem->pos.z - pPlayer->pos.z);

		if (fLength < ((pItem->pModelData->fRadius + PLAY_WIDTH) * (pItem->pModelData->fRadius + PLAY_WIDTH)))
		{ // アイテムがプレイヤーに当たっている場合

			// パーティクルの設定
			SetParticle
			( // 引数
				pItem->pos,							// 位置
				D3DXCOLOR(0.6f, 0.2f, 0.55f, 1.0f),	// 色
				PARTICLETYPE_GETHEAL,				// 種類
				SPAWN_PARTICLE_GETHEAL,				// エフェクト数
				1									// 寿命
			);

			// プレイヤーの回復判定
			HealPlayer(pPlayer, ITEM_HEAL);

			// 使用していない状態にする
			pItem->bUse = false;
		}
	}
}

//======================================================================================================================
//	チュートリアルのアイテムの更新処理
//======================================================================================================================
void TutorialItem(void)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		if (g_aItem[nCntItem].bUse == true)
		{ // アイテムが使用されている場合

			//--------------------------------------------------------------------------------------------------------------
			//	向きの更新
			//--------------------------------------------------------------------------------------------------------------
			// 向きの更新
			g_aItem[nCntItem].rot.y += MOVE_ROT_ITEM;

			// 向きの正規化
			if      (g_aItem[nCntItem].rot.y >  D3DX_PI) { g_aItem[nCntItem].rot.y -= D3DX_PI * 2; }
			else if (g_aItem[nCntItem].rot.y < -D3DX_PI) { g_aItem[nCntItem].rot.y += D3DX_PI * 2; }

			//--------------------------------------------------------------------------------------------------------------
			//	パーティクルの更新
			//--------------------------------------------------------------------------------------------------------------
			if (g_aItem[nCntItem].nCounterEffect < EFFECT_TIME_ITEM)
			{ // カウンターが一定値より小さい場合

				// カウンターを加算
				g_aItem[nCntItem].nCounterEffect++;
			}
			else
			{ // カウンターが一定値以上の場合

				// カウンターを初期化
				g_aItem[nCntItem].nCounterEffect = 0;

				// パーティクルの設定
				SetParticle
				( // 引数
					g_aItem[nCntItem].pos,				// 位置
					D3DXCOLOR(1.0f, 0.2f, 0.6f, 1.0f),	// 色
					PARTICLETYPE_HEAL,					// 種類
					SPAWN_PARTICLE_HEAL,				// エフェクト数
					1									// 寿命
				);
			}

			//------------------------------------------------------------------------------------------------------
			//	当たり判定
			//------------------------------------------------------------------------------------------------------
			if (GetLessonState() >= LESSON_05)
			{ // レッスン5に挑戦中、またはクリアしている場合

				// アイテムとプレイヤーの当たり判定
				CollisionPlayer(&g_aItem[nCntItem]);
			}

			//------------------------------------------------------------------------------------------------------
			//	影の更新
			//------------------------------------------------------------------------------------------------------
			// 影の位置設定
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);
		}
	}
}

//======================================================================================================================
//	ゲームのアイテムの更新処理
//======================================================================================================================
void GameItem(void)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		if (g_aItem[nCntItem].bUse == true)
		{ // アイテムが使用されている場合

			//--------------------------------------------------------------------------------------------------------------
			//	向きの更新
			//--------------------------------------------------------------------------------------------------------------
			// 向きの更新
			g_aItem[nCntItem].rot.y += MOVE_ROT_ITEM;

			// 向きの正規化
			if      (g_aItem[nCntItem].rot.y >  D3DX_PI) { g_aItem[nCntItem].rot.y -= D3DX_PI * 2; }
			else if (g_aItem[nCntItem].rot.y < -D3DX_PI) { g_aItem[nCntItem].rot.y += D3DX_PI * 2; }

			//--------------------------------------------------------------------------------------------------------------
			//	パーティクルの更新
			//--------------------------------------------------------------------------------------------------------------
			if (g_aItem[nCntItem].nCounterEffect < EFFECT_TIME_ITEM)
			{ // カウンターが一定値より小さい場合

				// カウンターを加算
				g_aItem[nCntItem].nCounterEffect++;
			}
			else
			{ // カウンターが一定値以上の場合

				// カウンターを初期化
				g_aItem[nCntItem].nCounterEffect = 0;

				// パーティクルの設定
				SetParticle
				( // 引数
					g_aItem[nCntItem].pos,				// 位置
					D3DXCOLOR(1.0f, 0.2f, 0.6f, 1.0f),	// 色
					PARTICLETYPE_HEAL,					// 種類
					SPAWN_PARTICLE_HEAL,				// エフェクト数
					1									// 寿命
				);
			}

			//------------------------------------------------------------------------------------------------------
			//	当たり判定
			//------------------------------------------------------------------------------------------------------
			// アイテムとプレイヤーの当たり判定
			CollisionPlayer(&g_aItem[nCntItem]);

			//------------------------------------------------------------------------------------------------------
			//	影の更新
			//------------------------------------------------------------------------------------------------------
			// 影の位置設定
			SetPositionShadow(g_aItem[nCntItem].nShadowID, g_aItem[nCntItem].pos);
		}
	}
}

//======================================================================================================================
//	アイテムの取得処理
//======================================================================================================================
Item *GetItem(void)
{
	// アイテムの情報の先頭アドレスを返す
	return &g_aItem[0];
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	アイテムの総数取得処理
//**********************************************************************************************************************
int GetNumItem(void)
{
	// 変数を宣言
	int nNumItem = 0;	// アイテムの総数の確認用

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{ // アイテムの最大表示数分繰り返す

		if (g_aItem[nCntItem].bUse == true)
		{ // アイテムが使用されている場合

			// カウンターを加算
			nNumItem++;
		}
	}

	// 変数の値を返す
	return nNumItem;	// アイテムの総数
}
#endif