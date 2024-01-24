//======================================================================================================================
//
//	敵処理 [enemy.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "input.h"

#include "bullet.h"
#include "enemy.h"
#include "explosion.h"
#include "game.h"
#include "object.h"
#include "particle.h"
#include "score.h"
#include "shadow.h"
#include "slash.h"
#include "sound.h"
#include "timer.h"
#include "tutorial.h"
#include "weed.h"

#include "orbit.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define ENEMY_SETUP_TXT	"02_data\\00_TXT\\enemy.txt"	// 敵セットアップ用のテキストファイルの相対パス

#define ENE_WIDTH		(15.0f)		// 敵の横幅 / 2
#define ENE_DEPTH		(15.0f)		// 敵の奥行 / 2

#define ENE_LIFE		(100)		// 敵の体力
#define ENE_BODY_DMG	(20)		// 敵本体のダメージ量

#define DAMAGE_TIME_ENE	(PLAY_SLASH_LIFE)			// ダメージ状態を保つ時間
#define UNR_TIME_ENE	(PLAY_SLASH_LIFE - 10)		// 無敵状態に変更する時間

#define NUM_SPAWN_POS	(8)			// 出現する中心位置の総数
#define NUM_ENE_NORMAL	(2)			// 通常敵の総数
#define SPAWN_MAT_ALPHA	(0.65f)		// 出現時のマテリアルのα値

#define PLAY_WIDTH_MUL	(0.25f)		// プレイヤーとの当たり判定時のプレイヤーサイズの乗算値

#define ENE_BULL_MOVE	(4.0f)		// 敵の弾の移動量

#define ENE_SLA_LIFE	(18)		// 敵の斬撃の寿命
#define ENE_SLA_RADIUS	(60.0f)		// 敵の斬撃の半径

#define BULL_SCORE_PLUS	(400)		// 弾で倒した時のプラススコア
#define KICK_SCORE_PLUS	(200)		// キックで倒した時のプラススコア

#define NOT_SPAWN_WEED	((int)((NUM_TIME_GAME / 60) * 0.1f))	// 雑草が生えなくなる時間

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apModelEnemy[] =	// モデルの相対パス
{
	"02_data\\03_MODEL\\03_ENEMY\\flower_face000.x",	// 花 (ブルー) のモデル相対パス
	"02_data\\03_MODEL\\03_ENEMY\\flower_face001.x",	// 花 (ピンク) のモデル相対パス
};

//**********************************************************************************************************************
//	列挙型定義 (ENEMYTYPE)
//**********************************************************************************************************************
typedef enum
{
	ENEMYTYPE_RANGE_SHORT = 0,		// 近距離型
	ENEMYTYPE_RANGE_LONG,			// 遠距離型
	ENEMYTYPE_MAX,					// この列挙型の総数
} ENEMYTYPE;

//**********************************************************************************************************************
//	構造体定義 (WeedSpawn)
//**********************************************************************************************************************
typedef struct
{
	int nCounterSpawn;				// 出現頻度
	int nMinSpawn;					// 最低出現数
	int nMaxSpawn;					// 最高出現数
	int nRandSpawnPos;				// 出現位置のブレの半径
}WeedSpawn;

//**********************************************************************************************************************
//	構造体定義 (EnemySpawn)
//**********************************************************************************************************************
typedef struct
{
	int         nMinSpawn;					// 最低出現数
	int         nMaxSpawn;					// 最高出現数
	int         nRandSpawnPos;				// 出現位置のブレの半径
	D3DXVECTOR3 aPosSpawn[NUM_SPAWN_POS];	// 出現の中心位置
}EnemySpawn;

//**********************************************************************************************************************
//	構造体定義 (EnemyStatus)
//**********************************************************************************************************************
typedef struct
{
	int       nLife;						// 体力
	int       nScore;						// スコア加算量
	int       nCounterAttack;				// 攻撃頻度
	float     fForwardMove;					// 前進の移動量
	float     fBackwardMove;				// 後退の移動量
	float     fLookRevision;				// プレイヤー方向を向く補正係数
	float     fFindRadius;					// 検知範囲
	float     fAttackRadius;				// 攻撃範囲
	float     fBackwardRadius;				// 後退範囲
	bool      bBackward;					// 後退の ON / OFF
	WeedSpawn weed;							// 雑草のスポーン情報
}EnemyStatus;

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void TutorialEnemy(void);					// チュートリアルの敵の更新処理
void GameEnemy(void);						// ゲームの敵の更新処理

void SpawnEnemy(void);												// 敵の出現処理
void SpawnWeed(Enemy *pEnemy);										// 雑草の出現処理
void CollisionPlayer(Enemy *pEnemy);								// 敵とプレイヤーの当たり判定
void CollisionEnemyFind(Enemy *pEnemy);								// 敵の検知範囲の当たり判定
bool CollisionEnemyAttack(Enemy *pEnemy, Player *pPlayer);		// 敵の攻撃範囲の当たり判定
bool CollisionEnemyBackward(Enemy *pEnemy, Player *pPlayer);	// 敵の後退範囲の当たり判定
void LookPlayer(Enemy *pEnemy, Player *pPlayer);				// プレイヤーの方向に向く処理
void RevPosEnemy(Enemy *pEnemy);									// 敵の位置の補正処理
void TxtSetEnemy(void);												// 敵のセットアップ処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
Model       g_aModelEnemy[ENEMYTYPE_MAX];	// 敵のモデル情報
Enemy       g_aEnemy[MAX_ENEMY];			// 敵の情報
EnemySpawn  g_enemySpawn;					// 敵のスポーン情報
EnemyStatus g_aEnemyStatus[ENEMYTYPE_MAX];	// 敵のステータス情報
int         g_nNumEnemy;					// 敵の総数
int         g_nKillEnemy;					// 敵の討伐数

//======================================================================================================================
//	敵の初期化処理
//======================================================================================================================
void InitEnemy(void)
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

	// グローバル変数を初期化
	g_nNumEnemy  = 0;			// 敵の総数
	g_nKillEnemy = 0;			// 敵の討伐数

	// 敵のモデル情報の初期化
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		g_aModelEnemy[nCntEnemy].apTexture[MAX_MODEL_TEXTURE] = {};						// テクスチャへのポインタ
		g_aModelEnemy[nCntEnemy].pMesh    = NULL;										// メッシュ (頂点情報) へのポインタ
		g_aModelEnemy[nCntEnemy].pBuffMat = NULL;										// マテリアルへのポインタ
		g_aModelEnemy[nCntEnemy].dwNumMat = 0;											// マテリアルの数
		g_aModelEnemy[nCntEnemy].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// 最小の頂点座標
		g_aModelEnemy[nCntEnemy].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// 最大の頂点座標
		g_aModelEnemy[nCntEnemy].fHeight  = 0.0f;										// 縦幅
		g_aModelEnemy[nCntEnemy].fRadius  = 0.0f;										// 半径
	}

	// 敵の情報の初期化
	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		g_aEnemy[nCntEnemy].pos            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の位置
		g_aEnemy[nCntEnemy].oldPos         = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 前回の位置
		g_aEnemy[nCntEnemy].rot            = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_aEnemy[nCntEnemy].pModelData     = NULL;							// モデル情報
		g_aEnemy[nCntEnemy].state          = ACTIONSTATE_NONE;				// 状態
		g_aEnemy[nCntEnemy].nLife          = 0;								// 体力
		g_aEnemy[nCntEnemy].nType          = 0;								// 種類
		g_aEnemy[nCntEnemy].nCounterState  = 0;								// 状態管理カウンター
		g_aEnemy[nCntEnemy].nCounterAttack = 0;								// 攻撃管理カウンター
		g_aEnemy[nCntEnemy].nCounterWeed   = 0;								// 雑草管理カウンター
		g_aEnemy[nCntEnemy].nShadowID      = NONE_SHADOW;					// 影のインデックス
		g_aEnemy[nCntEnemy].nSlashID       = NONE_SLASH;					// 斬撃のインデックス
		g_aEnemy[nCntEnemy].nOrbitID       = NONE_ORBIT;					// 軌跡のインデックス
		g_aEnemy[nCntEnemy].bUse           = false;							// 使用状況
	}

	// 敵のセットアップ処理
	TxtSetEnemy();			// ステータス情報・スポーン情報の初期化

	// xファイルの読み込み
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		D3DXLoadMeshFromX
		( // 引数
			apModelEnemy[nCntEnemy],			// モデルの相対パス
			D3DXMESH_SYSTEMMEM,
			pDevice,							// デバイスへのポインタ
			NULL,
			&g_aModelEnemy[nCntEnemy].pBuffMat,	// マテリアルへのポインタ
			NULL,
			&g_aModelEnemy[nCntEnemy].dwNumMat,	// マテリアルの数
			&g_aModelEnemy[nCntEnemy].pMesh		// メッシュ (頂点情報) へのポインタ
		);
	}

	// 当たり判定の作成
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		// モデルの頂点数を取得
		nNumVtx = g_aModelEnemy[nCntEnemy].pMesh->GetNumVertices();

		// モデルの頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(g_aModelEnemy[nCntEnemy].pMesh->GetFVF());

		// モデルの頂点バッファをロック
		g_aModelEnemy[nCntEnemy].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // モデルの頂点数分繰り返す

			// モデルの頂点座標を代入
			vtx = *(D3DXVECTOR3*)pVtxBuff;

			// 頂点座標 (x) の設定
			if (vtx.x < g_aModelEnemy[nCntEnemy].vtxMin.x)
			{ // 今回の頂点座標 (x) が、現状の頂点座標 (x) よりも小さい場合

				// 今回の頂点情報 (x) を代入
				g_aModelEnemy[nCntEnemy].vtxMin.x = vtx.x;
			}
			else if (vtx.x > g_aModelEnemy[nCntEnemy].vtxMax.x)
			{ // 今回の頂点座標 (x) が、現状の頂点座標 (x) よりも大きい場合

				// 今回の頂点情報 (x) を代入
				g_aModelEnemy[nCntEnemy].vtxMax.x = vtx.x;
			}

			// 頂点座標 (y) の設定
			if (vtx.y < g_aModelEnemy[nCntEnemy].vtxMin.y)
			{ // 今回の頂点座標 (y) が、現状の頂点座標 (y) よりも小さい場合

				// 今回の頂点情報 (y) を代入
				g_aModelEnemy[nCntEnemy].vtxMin.y = vtx.y;
			}
			else if (vtx.y > g_aModelEnemy[nCntEnemy].vtxMax.y)
			{ // 今回の頂点座標 (y) が、現状の頂点座標 (y) よりも大きい場合

				// 今回の頂点情報 (y) を代入
				g_aModelEnemy[nCntEnemy].vtxMax.y = vtx.y;
			}

			// 頂点座標 (z) の設定
			if (vtx.z < g_aModelEnemy[nCntEnemy].vtxMin.z)
			{ // 今回の頂点座標 (z) が、現状の頂点座標 (z) よりも小さい場合

				// 今回の頂点情報 (z) を代入
				g_aModelEnemy[nCntEnemy].vtxMin.z = vtx.z;
			}
			else if (vtx.z > g_aModelEnemy[nCntEnemy].vtxMax.z)
			{ // 今回の頂点座標 (z) が、現状の頂点座標 (z) よりも大きい場合

				// 今回の頂点情報 (z) を代入
				g_aModelEnemy[nCntEnemy].vtxMax.z = vtx.z;
			}

			// 頂点フォーマットのサイズ分ポインタを進める
			pVtxBuff += dwSizeFVF;
		}

		// モデルの頂点バッファをアンロック
		g_aModelEnemy[nCntEnemy].pMesh->UnlockVertexBuffer();

		// モデルサイズを求める
		size = g_aModelEnemy[nCntEnemy].vtxMax - g_aModelEnemy[nCntEnemy].vtxMin;

		// モデルの縦幅を代入
		g_aModelEnemy[nCntEnemy].fHeight = size.y;

		// モデルの円の当たり判定を作成
		g_aModelEnemy[nCntEnemy].fRadius = ((size.x * 0.5f) + (size.z * 0.5f)) * 0.5f;
	}

	// テクスチャの読み込み
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		// マテリアル情報に対するポインタを取得
		pMat = (D3DXMATERIAL*)g_aModelEnemy[nCntEnemy].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelEnemy[nCntEnemy].dwNumMat; nCntMat++)
		{ // マテリアルの数分繰り返す

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // テクスチャファイルが存在する場合

				// テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelEnemy[nCntEnemy].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	敵の終了処理
//======================================================================================================================
void UninitEnemy(void)
{
	// テクスチャの破棄
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		for (int nCntMat = 0; nCntMat < MAX_MODEL_TEXTURE; nCntMat++)
		{ // 使用するテクスチャ数分繰り返す

			if (g_aModelEnemy[nCntEnemy].apTexture[nCntMat] != NULL)
			{ // 変数 (g_aModelEnemy[nCntEnemy].apTexture) がNULLではない場合

				g_aModelEnemy[nCntEnemy].apTexture[nCntMat]->Release();
				g_aModelEnemy[nCntEnemy].apTexture[nCntMat] = NULL;
			}
		}
	}

	// メッシュの破棄
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		if (g_aModelEnemy[nCntEnemy].pMesh != NULL)
		{ // 変数 (g_aModelEnemy[nCntEnemy].pMesh) がNULLではない場合

			g_aModelEnemy[nCntEnemy].pMesh->Release();
			g_aModelEnemy[nCntEnemy].pMesh = NULL;
		}
	}

	// マテリアルの破棄
	for (int nCntEnemy = 0; nCntEnemy < ENEMYTYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		if (g_aModelEnemy[nCntEnemy].pBuffMat != NULL)
		{ // 変数 (g_aModelEnemy[nCntEnemy].pBuffMat) がNULLではない場合

			g_aModelEnemy[nCntEnemy].pBuffMat->Release();
			g_aModelEnemy[nCntEnemy].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	敵の更新処理
//======================================================================================================================
void UpdateEnemy(void)
{
	switch (GetMode())
	{ // モードごとの更新
	case MODE_TUTORIAL:	// チュートリアル画面

		// チュートリアルの敵の更新
		TutorialEnemy();

		// 処理を抜ける
		break;

	case MODE_GAME:		// ゲーム画面

		// ゲームの敵の更新
		GameEnemy();

		// 処理を抜ける
		break;
	}
}

//======================================================================================================================
//	敵の描画処理
//======================================================================================================================
void DrawEnemy(void)
{
	// 変数を宣言
	D3DXMATRIX   mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATERIAL     *pMat;						// マテリアルデータへのポインタ
	D3DXMATERIAL      redMat;					// マテリアルデータ (赤)
	D3DXMATERIAL      thinMat;					// マテリアルデータ (半透明)

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // 敵が使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aEnemy[nCntEnemy].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aEnemy[nCntEnemy].rot.y, g_aEnemy[nCntEnemy].rot.x, g_aEnemy[nCntEnemy].rot.z);
			D3DXMatrixMultiply(&g_aEnemy[nCntEnemy].mtxWorld, &g_aEnemy[nCntEnemy].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aEnemy[nCntEnemy].pos.x, g_aEnemy[nCntEnemy].pos.y, g_aEnemy[nCntEnemy].pos.z);
			D3DXMatrixMultiply(&g_aEnemy[nCntEnemy].mtxWorld, &g_aEnemy[nCntEnemy].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aEnemy[nCntEnemy].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_aEnemy[nCntEnemy].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aEnemy[nCntEnemy].pModelData->dwNumMat; nCntMat++)
			{ // マテリアルの数分繰り返す

				switch (g_aEnemy[nCntEnemy].state)
				{ // 状態ごとの処理
				case ACTIONSTATE_SPAWN:		// 出現状態

					// 読み込んだマテリアルを代入
					thinMat.MatD3D = pMat[nCntMat].MatD3D;

					// マテリアルのα値を下げる
					thinMat.MatD3D.Diffuse.a = SPAWN_MAT_ALPHA;

					// マテリアルの設定
					pDevice->SetMaterial(&thinMat.MatD3D);			// 半透明

					// 処理を抜ける
					break;

				case ACTIONSTATE_DAMAGE:	// ダメージ状態

					// 構造体の要素をクリア
					ZeroMemory(&redMat, sizeof(D3DXMATERIAL));

					// 拡散光・環境光・自己発光を赤にする
					redMat.MatD3D.Diffuse  = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
					redMat.MatD3D.Ambient  = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
					redMat.MatD3D.Emissive = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

					// マテリアルの設定
					pDevice->SetMaterial(&redMat.MatD3D);			// 赤

					// 処理を抜ける
					break;

				default:					// それ以外の状態

					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);	// 通常

					// 処理を抜ける
					break;
				}

				// テクスチャの設定
				pDevice->SetTexture(0, g_aEnemy[nCntEnemy].pModelData->apTexture[nCntMat]);

				// モデルの描画
				g_aEnemy[nCntEnemy].pModelData->pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//======================================================================================================================
//	敵の設定処理
//======================================================================================================================
void SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType)
{
	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (g_aEnemy[nCntEnemy].bUse == false)
		{ // 敵が使用されていない場合

			// 引数を代入
			g_aEnemy[nCntEnemy].pos    = pos;		// 現在の位置
			g_aEnemy[nCntEnemy].oldPos = pos;		// 前回の位置
			g_aEnemy[nCntEnemy].rot    = rot;		// 向き
			g_aEnemy[nCntEnemy].nType  = nType;		// 種類

			// 敵の情報を初期化
			g_aEnemy[nCntEnemy].state         = ACTIONSTATE_SPAWN;		// 状態
			g_aEnemy[nCntEnemy].nCounterState = 0;						// 状態管理カウンター
			g_aEnemy[nCntEnemy].nCounterWeed  = 0;						// 雑草管理カウンター
			g_aEnemy[nCntEnemy].nSlashID      = NONE_SLASH;				// 斬撃のインデックス

			// 体力を設定
			g_aEnemy[nCntEnemy].nLife = g_aEnemyStatus[nType].nLife;	// 体力

			// 攻撃管理カウンターを設定
			g_aEnemy[nCntEnemy].nCounterAttack = (int)(g_aEnemyStatus[nType].nCounterAttack * 0.5f);	// 初撃を早くする

			// 使用している状態にする
			g_aEnemy[nCntEnemy].bUse = true;

			// モデル情報を設定
			g_aEnemy[nCntEnemy].pModelData = &g_aModelEnemy[nType];		// モデル情報

			// 影のインデックスを設定
			g_aEnemy[nCntEnemy].nShadowID = SetShadow
			( // 引数
				0.5f,																											// α値
				fabsf(g_aModelEnemy[g_aEnemy[nCntEnemy].nType].vtxMax.x - g_aModelEnemy[g_aEnemy[nCntEnemy].nType].vtxMin.x),	// 半径
				&g_aEnemy[nCntEnemy].nShadowID,																					// 影の親の影インデックス
				&g_aEnemy[nCntEnemy].bUse																						// 影の親の使用状況
			);

			// 影の位置設定
			SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

			// 軌跡のインデックスを設定
			g_aEnemy[nCntEnemy].nOrbitID = SetOrbit
			( // 引数
				&g_aEnemy[nCntEnemy].mtxWorld,		// ワールドマトリックスのアドレス
				&g_aEnemy[nCntEnemy].bUse,			// 使用状況のアドレス
				&g_aEnemy[nCntEnemy].nOrbitID,		// インデックスのアドレス
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 色
				OFFSETTYPE_ENEMY_HEIGHT,			// オフセットの種類
				ORBITTYPE_NORMAL					// テクスチャの種類
			);

			// 敵の総数を加算
			g_nNumEnemy++;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_SPAWN);	// SE (出現)

			// 処理を抜ける
			break;
		}
	}
}

//======================================================================================================================
//	敵のダメージ判定
//======================================================================================================================
bool HitEnemy(Enemy *pEnemy, int nDamage, DAMAGETYPE type)
{
	if (pEnemy->state == ACTIONSTATE_NORMAL)
	{ // 敵が通常状態の場合

		// 引数のダメージ分を体力から減算
		pEnemy->nLife -= nDamage;

		if (pEnemy->nLife > 0)
		{ // 体力が残っている場合

			// ダメージ状態にする
			pEnemy->state = ACTIONSTATE_DAMAGE;

			// パーティクルの設定
			SetParticle
			( // 引数
				pEnemy->pos,						// 位置
				D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f),	// 色
				PARTICLETYPE_DAMAGE,				// 種類
				SPAWN_PARTICLE_DAMAGE,				// エフェクト数
				2									// 寿命
			);

			// カウンターを設定
			pEnemy->nCounterState = DAMAGE_TIME_ENE;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_DMG);			// SE (ダメージ)
		}
		else
		{ // 体力が尽きた場合

			// スコアの加算
			AddScore(g_aEnemyStatus[pEnemy->nType].nScore);

			// スコアアップ
			switch (type)
			{ // 受けた攻撃ごとの処理
			case DAMAGETYPE_SLASH:

				// 無し

				// 処理を抜ける
				break;

			case DAMAGETYPE_BULLET:

				// スコアの加算
				AddScore(BULL_SCORE_PLUS);

				// 処理を抜ける
				break;

			case DAMAGETYPE_KICK:

				// スコアの加算
				AddScore(KICK_SCORE_PLUS);

				// 処理を抜ける
				break;
			}

			// 爆発の設定
			SetExplosion(pEnemy->pos, SOUNDTYPE_EXPLOSION);

			// パーティクルの設定
			SetParticle
			( // 引数
				pEnemy->pos,						// 位置
				D3DXCOLOR(0.3f, 0.6f, 1.0f, 1.0f),	// 色
				PARTICLETYPE_EXPLOSION,				// 種類
				SPAWN_PARTICLE_EXPLOSION,			// エフェクト数
				2									// 寿命
			);

			// 使用していない状態にする
			pEnemy->bUse = false;

			// 敵の総数を減算
			g_nNumEnemy--;

			// 敵の討伐数を加算
			g_nKillEnemy++;

			// 死亡の真偽を返す
			return true;
		}
	}

	// 死亡の真偽を返す
	return false;
}

//======================================================================================================================
//	チュートリアルの敵の更新処理
//======================================================================================================================
void TutorialEnemy(void)
{
	if (GetLessonState() >= LESSON_06)
	{ // レッスン6に挑戦中、またはクリアしている場合

		// 敵の出現処理
		SpawnEnemy();
	}

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // 敵が使用されている場合

			switch (g_aEnemy[nCntEnemy].state)
			{ // 状態ごとの処理
			case ACTIONSTATE_SPAWN:		// 出現状態

				if (g_aEnemy[nCntEnemy].pos.y < 0.0f)
				{ // 出現中の場合

					// 地中からスポーンさせる
					g_aEnemy[nCntEnemy].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // 出現しきった場合

					// 敵の座標 (y) を補正
					g_aEnemy[nCntEnemy].pos.y = 0.0f;

					// 通常状態にする
					g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
				}

				// 処理を抜ける
				break;

			default:					// それ以外の状態

				// プレイヤーの方向を向かせる
				LookPlayer(&g_aEnemy[nCntEnemy], GetPlayer());

				// 状態の更新
				if (g_aEnemy[nCntEnemy].nCounterState > 0)
				{ // カウンターが 0より大きい場合

					// カウンターを減算
					g_aEnemy[nCntEnemy].nCounterState--;

					if (g_aEnemy[nCntEnemy].nCounterState == UNR_TIME_ENE)
					{ // カウンターが一定値の場合

						// 無敵状態にする
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aEnemy[nCntEnemy].nCounterState <= 0)
					{ // カウンターが 0以下の場合

						// 通常状態にする
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
					}
				}

				// 影の位置設定
				SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

				// 処理を抜ける
				break;
			}
		}
	}
}

//======================================================================================================================
//	ゲームの敵の更新処理
//======================================================================================================================
void GameEnemy(void)
{
	// 敵の出現処理
	SpawnEnemy();

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // 敵が使用されている場合

			switch (g_aEnemy[nCntEnemy].state)
			{ // 状態ごとの処理
			case ACTIONSTATE_SPAWN:		// 出現状態

				if (g_aEnemy[nCntEnemy].pos.y < 0.0f)
				{ // 出現中の場合

					// 地中からスポーンさせる
					g_aEnemy[nCntEnemy].pos.y += (float)((rand() % (SPAWN_MAX_SPEED - SPAWN_MIN_SPEED + 1)) + SPAWN_MIN_SPEED);
				}
				else
				{ // 出現しきった場合

					// 敵の座標 (y) を補正
					g_aEnemy[nCntEnemy].pos.y = 0.0f;

					// 通常状態にする
					g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
				}

				// 処理を抜ける
				break;

			default:					// それ以外の状態

				//------------------------------------------------------------------------------------------------------
				//	前回位置の更新
				//------------------------------------------------------------------------------------------------------
				g_aEnemy[nCntEnemy].oldPos = g_aEnemy[nCntEnemy].pos;

				//------------------------------------------------------------------------------------------------------
				//	状態の更新
				//------------------------------------------------------------------------------------------------------
				if (g_aEnemy[nCntEnemy].nCounterState > 0)
				{ // カウンターが 0より大きい場合

					// カウンターを減算
					g_aEnemy[nCntEnemy].nCounterState--;

					if (g_aEnemy[nCntEnemy].nCounterState == UNR_TIME_ENE)
					{ // カウンターが一定値の場合

						// 無敵状態にする
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_UNRIVALED;
					}
					else if (g_aEnemy[nCntEnemy].nCounterState <= 0)
					{ // カウンターが 0以下の場合

						// 通常状態にする
						g_aEnemy[nCntEnemy].state = ACTIONSTATE_NORMAL;
					}
				}

				//------------------------------------------------------------------------------------------------------
				//	攻撃の更新
				//------------------------------------------------------------------------------------------------------
				// 敵の検知範囲の当たり判定
				CollisionEnemyFind(&g_aEnemy[nCntEnemy]);

				switch (g_aEnemy[nCntEnemy].nType)
				{ // 種類ごとの処理
				case ENEMYTYPE_RANGE_SHORT:	// 近距離型の敵

					// 斬撃の位置・向きの設定
					SetPositionSlash
					( // 引数
						g_aEnemy[nCntEnemy].nSlashID,	// 斬撃のインデックス
						D3DXVECTOR3
						( // 引数
							g_aEnemy[nCntEnemy].pos.x,														// 位置 (x)
							g_aEnemy[nCntEnemy].pos.y + (g_aEnemy[nCntEnemy].pModelData->fHeight * 0.5f),	// 位置 (y)
							g_aEnemy[nCntEnemy].pos.z														// 位置 (z)
						),
						g_aEnemy[nCntEnemy].rot			// 向き
					);

					// 処理を抜ける
					break;

				case ENEMYTYPE_RANGE_LONG:	// 遠距離型の敵

					// 無し

					// 処理を抜ける
					break;
				}

				//------------------------------------------------------------------------------------------------------
				//	雑草の更新
				//------------------------------------------------------------------------------------------------------
				// カウンターを加算
				g_aEnemy[nCntEnemy].nCounterWeed++;

				// 雑草の出現処理
				SpawnWeed(&g_aEnemy[nCntEnemy]);

				//------------------------------------------------------------------------------------------------------
				//	当たり判定
				//------------------------------------------------------------------------------------------------------
				// 敵とプレイヤーの当たり判定
				CollisionPlayer(&g_aEnemy[nCntEnemy]);

				// オブジェクトとの当たり判定
				CollisionObject
				( // 引数
					&g_aEnemy[nCntEnemy].pos,		// 現在の位置
					&g_aEnemy[nCntEnemy].oldPos,	// 前回の位置
					ENE_WIDTH,						// 横幅
					ENE_DEPTH						// 奥行
				);

				//------------------------------------------------------------------------------------------------------
				//	影の更新
				//------------------------------------------------------------------------------------------------------
				// 影の位置設定
				SetPositionShadow(g_aEnemy[nCntEnemy].nShadowID, g_aEnemy[nCntEnemy].pos);

				//------------------------------------------------------------------------------------------------------
				//	軌跡の更新
				//------------------------------------------------------------------------------------------------------
				//// 軌跡の位置設定
				//SetPositionOrbit
				//( // 引数
				//	g_aEnemy[nCntEnemy].nOrbitID,		// 軌跡のインデックス
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 色
				//	OFFSETTYPE_NORMAL					// オフセットの種類
				//);

				// 処理を抜ける
				break;
			}
		}
	}
}

//======================================================================================================================
//	敵の出現処理
//======================================================================================================================
void SpawnEnemy(void)
{
	// 変数を宣言
	int         nSpawnEnemy;	// 出現数
	int         nSpawnPoint;	// 出現の中心位置
	D3DXVECTOR3 pos;			// 中心からのブレ位置
	D3DXVECTOR3 rot;			// 向き
	int         nType;			// 種類

	if (g_nNumEnemy < g_enemySpawn.nMinSpawn)
	{ // 敵の現在の総数が一定数より少なくなった場合

		// 敵の出現数を設定
		nSpawnEnemy = (rand() % (g_enemySpawn.nMaxSpawn - g_enemySpawn.nMinSpawn + 1)) + g_enemySpawn.nMinSpawn;

		for (int nCntEnemy = 0; nCntEnemy < nSpawnEnemy; nCntEnemy++)
		{ // 敵の出現数分繰り返す

			// 出現の中心位置をランダムに設定
			nSpawnPoint = rand() % NUM_SPAWN_POS;

			// 中心からのブレをランダムに設定
			pos = D3DXVECTOR3
			( // 引数
				(float)((rand() % (g_enemySpawn.nRandSpawnPos + 1)) - (g_enemySpawn.nRandSpawnPos * 0.5f)),	// 位置 (x)
				0.0f,																						// 位置 (y)
				(float)((rand() % (g_enemySpawn.nRandSpawnPos + 1)) - (g_enemySpawn.nRandSpawnPos * 0.5f))	// 位置 (z)
			);

			// 向きをランダムに設定
			rot = D3DXVECTOR3
			( // 引数
				0.0f,							// 向き (x)
				((rand() % 628) - 314) * 0.01f,	// 向き (y)
				0.0f							// 向き (z)
			);

			// 種類をランダムに設定
			nType = rand() % NUM_ENE_NORMAL;

			// 敵の設定
			SetEnemy
			( // 引数
				g_enemySpawn.aPosSpawn[nSpawnPoint] + pos,	// 位置
				rot,										// 向き
				nType										// 種類
			);
		}
	}
}

//======================================================================================================================
//	雑草の出現処理
//======================================================================================================================
void SpawnWeed(Enemy *pEnemy)
{
	// 変数を宣言
	int         nSpawnWeed;		// 出現数
	D3DXVECTOR3 pos;			// 敵座標からのブレ位置
	D3DXVECTOR3 spawnPos;		// 出現位置

	if (pEnemy->nCounterWeed >= g_aEnemyStatus[pEnemy->nType].weed.nCounterSpawn
	&&  GetTime() >= NOT_SPAWN_WEED)
	{ // カウンターが出現頻度の値以上且つ、タイマーが一定値以上の場合

		// カウンターを初期化
		pEnemy->nCounterWeed = 0;

		// 敵の出現数を設定
		nSpawnWeed = 
		(rand() % (g_aEnemyStatus[pEnemy->nType].weed.nMaxSpawn - g_aEnemyStatus[pEnemy->nType].weed.nMinSpawn + 1)) + g_aEnemyStatus[pEnemy->nType].weed.nMinSpawn;

		for (int nCntWeed = 0; nCntWeed < nSpawnWeed; nCntWeed++)
		{ // 雑草の出現数分繰り返す

			// 敵座標からのブレをランダムに設定
			pos = D3DXVECTOR3
			( // 引数
				(float)((rand() % (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos + 1)) - (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos * 0.5f)),	// 位置 (x)
				0.0f,																																	// 位置 (y)
				(float)((rand() % (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos + 1)) - (g_aEnemyStatus[pEnemy->nType].weed.nRandSpawnPos * 0.5f))	// 位置 (z)
			);

			// 出現位置を設定
			spawnPos = D3DXVECTOR3
			( // 引数
				pEnemy->pos.x + pos.x,	// 位置 (x)
				-WEED_HEIGHT,			// 位置 (y)
				pEnemy->pos.z + pos.z	// 位置 (z)
			);

			// 位置の補正
			if (spawnPos.z > GetLimitStage().fNear - WEED_SPAWN_REV)
			{ // 範囲外の場合 (手前)

				// 手前に位置を補正
				spawnPos.z = GetLimitStage().fNear - WEED_SPAWN_REV;
			}
			if (spawnPos.z < GetLimitStage().fFar + WEED_SPAWN_REV)
			{ // 範囲外の場合 (奥)

				// 奥に位置を補正
				spawnPos.z = GetLimitStage().fFar + WEED_SPAWN_REV;
			}
			if (spawnPos.x > GetLimitStage().fRight - WEED_SPAWN_REV)
			{ // 範囲外の場合 (右)

				// 右に位置を補正
				spawnPos.x = GetLimitStage().fRight - WEED_SPAWN_REV;
			}
			if (spawnPos.x < GetLimitStage().fLeft + WEED_SPAWN_REV)
			{ // 範囲外の場合 (左)

				// 左に位置を補正
				spawnPos.x = GetLimitStage().fLeft + WEED_SPAWN_REV;
			}

			// 雑草の設定
			SetWeed(spawnPos);
		}
	}
}

//======================================================================================================================
//	敵とプレイヤーの当たり判定
//======================================================================================================================
void CollisionPlayer(Enemy *pEnemy)
{
	// 変数を宣言
	float fLength = 0.0f;				// 敵とプレイヤーとの間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// 敵とプレイヤーとの間の距離を求める
		fLength = (pEnemy->pos.x - pPlayer->pos.x) * (pEnemy->pos.x - pPlayer->pos.x)
				+ (pEnemy->pos.z - pPlayer->pos.z) * (pEnemy->pos.z - pPlayer->pos.z);

		if (fLength < ((pEnemy->pModelData->fRadius + (PLAY_WIDTH * PLAY_WIDTH_MUL)) * (pEnemy->pModelData->fRadius + (PLAY_WIDTH * PLAY_WIDTH_MUL)))	// 円の当たり判定
		&&  pEnemy->pos.y + pEnemy->pModelData->fHeight > pPlayer->pos.y																				// 縦の判定 (下)
		&&  pEnemy->pos.y                               < pPlayer->pos.y + PLAY_HEIGHT)																	// 縦の判定 (上)
		{ // 敵がプレイヤーに当たっている場合

			// プレイヤーのダメージ判定
			HitPlayer(pPlayer, ENE_BODY_DMG);
		}
	}
}

//======================================================================================================================
//	敵の検知範囲の当たり判定
//======================================================================================================================
void CollisionEnemyFind(Enemy *pEnemy)
{
	// 変数を宣言
	float fLength = 0.0f;				// プレイヤーと敵との間の距離

	// ポインタを宣言
	Player *pPlayer = GetPlayer();	// プレイヤーの情報

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// プレイヤーと敵との間の距離を求める
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fFindRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fFindRadius)))
		{ // 敵の検知範囲内の場合

			// プレイヤーの方向を向かせる
			LookPlayer(pEnemy, pPlayer);

			if (CollisionEnemyAttack(pEnemy, pPlayer) == false)
			{ // 敵の攻撃範囲外の場合

				// プレイヤーの方向に移動 (前進)
				pEnemy->pos.x -= sinf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fForwardMove;
				pEnemy->pos.z -= cosf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fForwardMove;
			}
			else
			{ // 敵の攻撃範囲内の場合

				if (CollisionEnemyBackward(pEnemy, pPlayer) == true
				&&  g_aEnemyStatus[pEnemy->nType].bBackward == true)
				{ // 敵の後退範囲内且つ、後退が ON の場合

					// プレイヤーの逆方向に移動 (後退)
					pEnemy->pos.x += sinf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fBackwardMove;
					pEnemy->pos.z += cosf(pEnemy->rot.y) * g_aEnemyStatus[pEnemy->nType].fBackwardMove;

					// 敵の位置を補正
					RevPosEnemy(pEnemy);
				}

				// カウンターを加算
				pEnemy->nCounterAttack++;

				if (pEnemy->nCounterAttack >= g_aEnemyStatus[pEnemy->nType].nCounterAttack)
				{ // カウンターが一定値以上の場合

					// カウンターを初期化
					pEnemy->nCounterAttack = 0;

					switch (pEnemy->nType)
					{ // 種類ごとの処理
					case ENEMYTYPE_RANGE_SHORT:	// 近距離型の敵

						if (pEnemy->nSlashID == NONE_SLASH)
						{ // 斬撃のインデックスが初期化されている場合

							// 斬撃の設定
							pEnemy->nSlashID = SetSlash
							( // 引数
								SLASHTYPE_ENEMY,		// 種類
								ENE_SLA_LIFE,			// 寿命
								ENE_SLA_RADIUS,			// 半径
								&pEnemy->nSlashID,		// 斬撃の親の斬撃インデックス
								&pEnemy->bUse			// 斬撃の親の使用状況
							);
						}

						// 処理を抜ける
						break;

					case ENEMYTYPE_RANGE_LONG:	// 遠距離型の敵

						// 弾の設定
						SetBullet
						( // 引数
							D3DXVECTOR3
							( // 引数
								pEnemy->pos.x + sinf(pEnemy->rot.y + D3DX_PI),			// 位置 (x)
								pEnemy->pos.y + (pEnemy->pModelData->fHeight * 0.5f),	// 位置 (y)
								pEnemy->pos.z + cosf(pEnemy->rot.y + D3DX_PI)			// 位置 (z)
							),
							D3DXVECTOR3
							( // 引数
								sinf(pEnemy->rot.y + D3DX_PI) * ENE_BULL_MOVE,			// 移動量 (x)
								0.0f,													// 移動量 (y)
								cosf(pEnemy->rot.y + D3DX_PI) * ENE_BULL_MOVE			// 移動量 (z)
							),
							BULLETTYPE_ENEMY,	// 種類
							true				// 効果音
						);

						// 処理を抜ける
						break;
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	敵の攻撃範囲の当たり判定
//======================================================================================================================
bool CollisionEnemyAttack(Enemy *pEnemy, Player *pPlayer)
{
	// 変数を宣言
	float fLength  = 0.0f;	// プレイヤーと敵との間の距離

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// プレイヤーと敵との間の距離を求める
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fAttackRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fAttackRadius)))
		{ // 敵の攻撃範囲内の場合

			// 攻撃範囲内の場合 true を返す
			return true;
		}
	}

	// 攻撃範囲外の場合 false を返す
	return false;
}

//======================================================================================================================
//	敵の後退範囲の当たり判定
//======================================================================================================================
bool CollisionEnemyBackward(Enemy *pEnemy, Player *pPlayer)
{
	// 変数を宣言
	float fLength  = 0.0f;	// プレイヤーと敵との間の距離

	if (pPlayer->bUse == true)
	{ // プレイヤーが使用されている場合

		// プレイヤーと敵との間の距離を求める
		fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
				+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

		if (fLength < ((PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fBackwardRadius) * (PLAY_WIDTH + g_aEnemyStatus[pEnemy->nType].fBackwardRadius)))
		{ // 敵の後退範囲内の場合

			// 後退範囲内の場合 true を返す
			return true;
		}
	}

	// 後退範囲外の場合 false を返す
	return false;
}

//======================================================================================================================
//	プレイヤーの方向に向く処理
//======================================================================================================================
void LookPlayer(Enemy *pEnemy, Player *pPlayer)
{
	// 変数を宣言
	float fDestRot = 0.0f;	// 目標の向き代入用
	float fDiffRot = 0.0f;	// 向きの計算代入用

	// プレイヤーの向きを代入
	fDestRot = atan2f(pEnemy->pos.x - pPlayer->pos.x, pEnemy->pos.z - pPlayer->pos.z);	// 目標の向き

	// 目標向きまでの差分を計算
	fDiffRot = fDestRot - pEnemy->rot.y;

	// 差分向きの正規化
	if      (fDiffRot >  D3DX_PI) { fDiffRot -= D3DX_PI * 2; }
	else if (fDiffRot < -D3DX_PI) { fDiffRot += D3DX_PI * 2; }

	// 向きの更新
	pEnemy->rot.y += fDiffRot * g_aEnemyStatus[pEnemy->nType].fLookRevision;

	// 向きの正規化
	if      (pEnemy->rot.y >  D3DX_PI) { pEnemy->rot.y -= D3DX_PI * 2; }
	else if (pEnemy->rot.y < -D3DX_PI) { pEnemy->rot.y += D3DX_PI * 2; }
}

//======================================================================================================================
//	敵の位置の補正処理
//======================================================================================================================
void RevPosEnemy(Enemy *pEnemy)
{
	if (pEnemy->pos.z > GetLimitStage().fNear - pEnemy->pModelData->fRadius)
	{ // 範囲外の場合 (手前)

		// 手前に位置を補正
		pEnemy->pos.z = GetLimitStage().fNear - pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.z < GetLimitStage().fFar + pEnemy->pModelData->fRadius)
	{ // 範囲外の場合 (奥)

		// 奥に位置を補正
		pEnemy->pos.z = GetLimitStage().fFar + pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.x > GetLimitStage().fRight - pEnemy->pModelData->fRadius)
	{ // 範囲外の場合 (右)

		// 右に位置を補正
		pEnemy->pos.x = GetLimitStage().fRight - pEnemy->pModelData->fRadius;
	}
	if (pEnemy->pos.x < GetLimitStage().fLeft + pEnemy->pModelData->fRadius)
	{ // 範囲外の場合 (左)

		// 左に位置を補正
		pEnemy->pos.x = GetLimitStage().fLeft + pEnemy->pModelData->fRadius;
	}
}

//======================================================================================================================
//	敵のセットアップ処理
//======================================================================================================================
void TxtSetEnemy(void)
{
	// 変数を宣言
	int nType       = 0;		// 種類の代入用
	int nBackward   = 0;		// 後退の ON / OFF の変換用
	int nSpawnPoint = 0;		// 現在のスポーン中心位置の番号
	int nEnd        = 0;		// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;				// ファイルポインタ

	// ファイルを読み込み形式で開く
	pFile = fopen(ENEMY_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			//----------------------------------------------------------------------------------------------------------
			//	ステータスの設定
			//----------------------------------------------------------------------------------------------------------
			if (strcmp(&aString[0], "STATUS_ENEMYSET") == 0)
			{ // 読み込んだ文字列が STATUS_ENEMYSET の場合

				do
				{ // 読み込んだ文字列が END_STATUS_ENEMYSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "ENEMYSET") == 0)
					{ // 読み込んだ文字列が ENEMYSET の場合

						do
						{ // 読み込んだ文字列が END_ENEMYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							// 敵のステータス情報
							if (strcmp(&aString[0], "TYPE") == 0)
							{ // 読み込んだ文字列が TYPE の場合
								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nType);		// 種類を読み込む
							}
							else if (strcmp(&aString[0], "LIFE") == 0)
							{ // 読み込んだ文字列が LIFE の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nLife);				// 体力を読み込む
							}
							else if (strcmp(&aString[0], "SCORE") == 0)
							{ // 読み込んだ文字列が SCORE の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nScore);				// スコア加算量を読み込む
							}
							else if (strcmp(&aString[0], "ATTACK_CNT") == 0)
							{ // 読み込んだ文字列が ATTACK_CNT の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].nCounterAttack);		// 攻撃頻度を読み込む
							}
							else if (strcmp(&aString[0], "FORWARD_MOVE") == 0)
							{ // 読み込んだ文字列が FORWARD_MOVE の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fForwardMove);		// 前進の移動量を読み込む
							}
							else if (strcmp(&aString[0], "BACKWARD_MOVE") == 0)
							{ // 読み込んだ文字列が BACKWARD_MOVE の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fBackwardMove);		// 後退の移動量を読み込む
							}
							else if (strcmp(&aString[0], "LOOK_REV") == 0)
							{ // 読み込んだ文字列が LOOK_REV の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fLookRevision);		// 振り向き補正係数を読み込む
							}
							else if (strcmp(&aString[0], "FIND_RADIUS") == 0)
							{ // 読み込んだ文字列が FIND_RADIUS の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fFindRadius);		// 検知範囲を読み込む
							}
							else if (strcmp(&aString[0], "ATTACK_RADIUS") == 0)
							{ // 読み込んだ文字列が ATTACK_RADIUS の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fAttackRadius);		// 攻撃範囲を読み込む
							}
							else if (strcmp(&aString[0], "BACKWARD_RADIUS") == 0)
							{ // 読み込んだ文字列が BACKWARD_RADIUS の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &g_aEnemyStatus[nType].fBackwardRadius);	// 後退範囲を読み込む
							}
							else if (strcmp(&aString[0], "BACWARD_ENABLE") == 0)
							{ // 読み込んだ文字列が BACWARD_ENABLE の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &nBackward);								// 後退の ON / OFF を読み込む

								// 読み込んだ値が 0なら true、0以外なら falseを代入
								g_aEnemyStatus[nType].bBackward = (nBackward == 0) ? true : false;
							}

							// 雑草のスポーン情報
							else if (strcmp(&aString[0], "WEED_CNTSPAWN") == 0)
							{ // 読み込んだ文字列が WEED_CNTSPAWN の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nCounterSpawn);	// 出現頻度を読み込む
							}
							else if (strcmp(&aString[0], "WEED_MINSPAWN") == 0)
							{ // 読み込んだ文字列が WEED_MINSPAWN の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nMinSpawn);		// 最低出減数を読み込む
							}
							else if (strcmp(&aString[0], "WEED_MAXSPAWN") == 0)
							{ // 読み込んだ文字列が WEED_MAXSPAWN の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nMaxSpawn);		// 最高出現数を読み込む
							}
							else if (strcmp(&aString[0], "WEED_RANDSPAWN") == 0)
							{ // 読み込んだ文字列が WEED_RANDSPAWN の場合
								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &g_aEnemyStatus[nType].weed.nRandSpawnPos);	// 出現位置のブレの半径を読み込む
							}
						} while (strcmp(&aString[0], "END_ENEMYSET") != 0);	// 読み込んだ文字列が END_ENEMYSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUS_ENEMYSET") != 0);	// 読み込んだ文字列が END_STATUS_ENEMYSET ではない場合ループ
			}

			//----------------------------------------------------------------------------------------------------------
			//	スポーンの設定
			//----------------------------------------------------------------------------------------------------------
			else if (strcmp(&aString[0], "SPAWN_ENEMYSET") == 0)
			{ // 読み込んだ文字列が SPAWN_ENEMYSET の場合

				do
				{ // 読み込んだ文字列が END_SPAWN_ENEMYSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "MINSPAWN") == 0)
					{ // 読み込んだ文字列が MINSPAWN の場合

						fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
						fscanf(pFile, "%d", &g_enemySpawn.nMinSpawn);		// 最低出減数を読み込む
					}
					else if (strcmp(&aString[0], "MAXSPAWN") == 0)
					{ // 読み込んだ文字列が MAXSPAWN の場合

						fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
						fscanf(pFile, "%d", &g_enemySpawn.nMaxSpawn);		// 最高出現数を読み込む
					}
					else if (strcmp(&aString[0], "RANDSPAWN") == 0)
					{ // 読み込んだ文字列が RANDSPAWN の場合

						fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
						fscanf(pFile, "%d", &g_enemySpawn.nRandSpawnPos);	// 出現位置のブレの半径を読み込む
					}
					else if (strcmp(&aString[0], "POS") == 0)
					{ // 読み込んだ文字列が POS の場合
						fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].x);	// 出現の中心位置 (x) を読み込む
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].y);	// 出現の中心位置 (y) を読み込む
						fscanf(pFile, "%f", &g_enemySpawn.aPosSpawn[nSpawnPoint].z);	// 出現の中心位置 (z) を読み込む

						// 現在のスポーン中心位置の番号を加算
						nSpawnPoint++;
					}
				} while (strcmp(&aString[0], "END_SPAWN_ENEMYSET") != 0);	// 読み込んだ文字列が END_SPAWN_ENEMYSET ではない場合ループ
			}
		} while (nEnd != EOF);												// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ステージファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	敵の討伐数の取得処理
//======================================================================================================================
int GetKillEnemy(void)
{
	// 敵の討伐数を返す
	return g_nKillEnemy;
}

//======================================================================================================================
//	敵の取得処理
//======================================================================================================================
Enemy *GetEnemy(void)
{
	// 敵の情報の先頭アドレスを返す
	return &g_aEnemy[0];
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	敵の総数取得処理
//**********************************************************************************************************************
int GetNumEnemy(void)
{
	// 変数を宣言
	int nNumEnemy = 0;	// 敵の総数の確認用

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{ // 敵の最大表示数分繰り返す

		if (g_aEnemy[nCntEnemy].bUse == true)
		{ // 敵が使用されている場合

			// カウンターを加算
			nNumEnemy++;
		}
	}

	// 変数の値を返す
	return nNumEnemy;	// 敵の総数
}
#endif