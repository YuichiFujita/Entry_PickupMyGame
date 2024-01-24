//======================================================================================================================
//
//	プレイヤー処理 [player.cpp]
//	Author：藤田勇一
//
//======================================================================================================================
//**********************************************************************************************************************
//	インクルードファイル
//**********************************************************************************************************************
#include "main.h"
#include "input.h"
#include "tutorial.h"
#include "game.h"

#include "bullet.h"
#include "camera.h"
#include "enemy.h"
#include "explosion.h"
#include "life.h"
#include "object.h"
#include "particle.h"
#include "player.h"
#include "shadow.h"
#include "slash.h"
#include "sound.h"

#include "orbit.h"
#include "tornado.h"

//**********************************************************************************************************************
//	マクロ定義
//**********************************************************************************************************************
#define PLAYER_SETUP_TXT	"02_data\\00_TXT\\player.txt"	// プレイヤーセットアップ用のテキストファイルの相対パス

#define MOVE_PLAYER			(1.2f)		// プレイヤーの移動量
#define DEL_MOVE_PLAYER		(0.01f)		// プレイヤーの移動量が残る最低値
#define SUB_MOVE_PLAYER		(0.25f)		// プレイヤーの移動量の減速係数
#define REV_ROT_PLAYER		(0.15f)		// プレイヤーの回転量の補正係数

#define PLAY_GRAVITY		(0.75f)		// プレイヤーにかかる重力
#define PLAY_JUMP			(14.5f)		// プレイヤーのジャンプ量

#define KICK_DMG_PLAY		(75)		// キックの攻撃力 (プレイヤー)
#define PLAY_KICK_RADIUS	(28.0f)		// プレイヤーのキックの半径
#define PLAY_KICK_HEIGHT	(4.0f)		// プレイヤーのキックの縦幅 / 2
#define PLAY_KICK_JUMP		(8.0f)		// プレイヤーのキック成功時の跳ね返り量

#define POS_BULL_PLAYER_X	(30.0f)		// プレイヤーの弾発射位置の加算量 (x)
#define POS_BULL_PLAYER_Y	(30.0f)		// プレイヤーの弾発射位置の加算量 (y)
#define POS_BULL_PLAYER_Z	(30.0f)		// プレイヤーの弾発射位置の加算量 (z)

#define PLAY_SLASH_RADIUS	(85.0f)		// プレイヤーの斬撃の半径
#define PLAY_SLASH_POS_Y	(40.0f)		// プレイヤーの斬撃位置の加算量 (y)

#define SHADOW_PLAYER		(35.0f)		// プレイヤーの影の半径
#define SUB_MOTION_STOP		(2)			// ループしないモーションの停止カウントの減算用

//**********************************************************************************************************************
//	コンスト定義
//**********************************************************************************************************************
const char *apModelPlayer[] =			// モデルの相対パス
{
	"02_data\\03_MODEL\\02_PLAYER\\body.x",		// 体のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\head.x",		// 頭のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\sleeve_l.x",	// 左腕のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\sleeve_r.x",	// 右腕のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\wrist_l.x",	// 左手首のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\wrist_r.x",	// 右手首のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\hand_l.x",	// 左手のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\hand_r.x",	// 右手のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\pants_l.x",	// 左足のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\pants_r.x",	// 右足のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\leg_l.x",	// 左腿のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\leg_r.x",	// 右腿のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\shoes_l.x",	// 左靴のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\shoes_r.x",	// 右靴のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\knife.x",	// 武器 (ナイフ) のモデル相対パス
	"02_data\\03_MODEL\\02_PLAYER\\gun.x",		// 武器 (銃) のモデル相対パス
};

//**********************************************************************************************************************
//	プロトタイプ宣言
//**********************************************************************************************************************
void TutorialPlayer(void);						// チュートリアルのプレイヤーの更新処理
void GamePlayer(void);							// ゲームのプレイヤーの更新処理

MOTIONTYPE MovePlayer(void);					// プレイヤーの移動量の更新処理
MOTIONTYPE JumpPlayer(void);					// プレイヤーのジャンプの更新処理
void PosPlayer(void);							// プレイヤーの位置の更新処理
void RotPlayer(void);							// プレイヤーの向きの更新処理
void RevPlayer(void);							// プレイヤーの補正の更新処理

void CollisionKickEnemy(Player *pPlayer);		// プレイヤーのキックの当たり判定
void UpdateMotion(void);						// モーションの更新処理
void SetMotion(MOTIONTYPE type);				// モーションの設定処理
void TxtSetPlayer(void);						// プレイヤーのセットアップ処理

//**********************************************************************************************************************
//	グローバル変数
//**********************************************************************************************************************
Model  g_aModelPlayer[MODEL_PLAYER_MAX];		// プレイヤーのモデル情報
Player g_player;								// プレイヤーの本体情報

//======================================================================================================================
//	プレイヤーの初期化処理
//======================================================================================================================
void InitPlayer(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATERIAL      *pMat;					// マテリアルへのポインタ

	// プレイヤーのモデル情報の初期化
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		g_aModelPlayer[nCntPlayer].apTexture[MAX_MODEL_TEXTURE] = {};						// テクスチャへのポインタ
		g_aModelPlayer[nCntPlayer].pMesh    = NULL;											// メッシュ (頂点情報) へのポインタ
		g_aModelPlayer[nCntPlayer].pBuffMat = NULL;											// マテリアルへのポインタ
		g_aModelPlayer[nCntPlayer].dwNumMat = 0;											// マテリアルの数
		g_aModelPlayer[nCntPlayer].vtxMin   = D3DXVECTOR3( 9999.0f,  9999.0f,  9999.0f);	// 最小の頂点座標
		g_aModelPlayer[nCntPlayer].vtxMax   = D3DXVECTOR3(-9999.0f, -9999.0f, -9999.0f);	// 最大の頂点座標
		g_aModelPlayer[nCntPlayer].fHeight  = 0.0f;											// 立幅
		g_aModelPlayer[nCntPlayer].fRadius  = 0.0f;											// 半径
	}

	// プレイヤーの本体情報の初期化
	g_player.pos           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の位置
	g_player.oldPos        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 前回の位置
	g_player.move          = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
	g_player.rot           = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の向き
	g_player.destRot       = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の向き
	g_player.state         = ACTIONSTATE_NORMAL;			// プレイヤーの状態
	g_player.nLife         = PLAY_LIFE;						// 体力
	g_player.nCounterState = 0;								// 状態管理カウンター
	g_player.nShadowID     = NONE_SHADOW;					// 影のインデックス
	g_player.nSlashID      = NONE_SLASH;					// 斬撃のインデックス
	g_player.nTornadoID    = NONE_IDX;						// 竜巻のインデックス
	g_player.bJump         = false;							// ジャンプ状況
	g_player.bKick         = false;							// キック状況
	g_player.bUse          = false;							// 使用状況

	// モーション情報の初期化
	g_player.motion.type     = MOTIONTYPE_NONE;	// モーションの種類
	g_player.motion.nPose    = 0;				// モーションのポーズ番号
	g_player.motion.nCounter = 0;				// モーションのカウンター

	// パーツ情報の初期化
	for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
	{ // プレイヤーのパーツの最大数分繰り返す

		g_player.aParts[nCntParts].initPos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 初期の位置
		g_player.aParts[nCntParts].pos        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の位置
		g_player.aParts[nCntParts].initRot    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 初期の向き
		g_player.aParts[nCntParts].rot        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 現在の向き
		g_player.aParts[nCntParts].pModelData = &g_aModelPlayer[nCntParts];		// モデル情報
		g_player.aParts[nCntParts].nParentID  = NONE_PLAYER_PARENT;				// プレイヤーの親のインデックス
	}

	// プレイヤーのセットアップ
	TxtSetPlayer();

	// モーションの設定
	SetMotion(MOTIONTYPE_NONE);

	// xファイルの読み込み
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		D3DXLoadMeshFromX
		( // 引数
			apModelPlayer[nCntPlayer],				// モデル (パーツ) の相対パス
			D3DXMESH_SYSTEMMEM,
			pDevice,								// デバイスへのポインタ
			NULL,
			&g_aModelPlayer[nCntPlayer].pBuffMat,	// マテリアルへのポインタ
			NULL,
			&g_aModelPlayer[nCntPlayer].dwNumMat,	// マテリアルの数
			&g_aModelPlayer[nCntPlayer].pMesh		// メッシュ (頂点情報) へのポインタ
		);
	}

	// テクスチャの読み込み
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		// マテリアル情報に対するポインタを取得
		pMat = (D3DXMATERIAL*)g_aModelPlayer[nCntPlayer].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aModelPlayer[nCntPlayer].dwNumMat; nCntMat++)
		{ // マテリアルの数分繰り返す

			if (pMat[nCntMat].pTextureFilename != NULL)
			{ // テクスチャファイルが存在する場合

				// テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aModelPlayer[nCntPlayer].apTexture[nCntMat]);
			}
		}
	}
}

//======================================================================================================================
//	プレイヤーの終了処理
//======================================================================================================================
void UninitPlayer(void)
{
	// テクスチャの破棄
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		for (int nCntMat = 0; nCntMat < TEXTURE_PLAYER_MAX; nCntMat++)
		{ // 使用するテクスチャ数分繰り返す

			if (g_aModelPlayer[nCntPlayer].apTexture[nCntMat] != NULL)
			{ // 変数 (g_aModelPlayer[nCntPlayer].apTexture) がNULLではない場合

				g_aModelPlayer[nCntPlayer].apTexture[nCntMat]->Release();
				g_aModelPlayer[nCntPlayer].apTexture[nCntMat] = NULL;
			}
		}
	}

	// メッシュの破棄
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		if (g_aModelPlayer[nCntPlayer].pMesh != NULL)
		{ // 変数 (g_aModelPlayer[nCntPlayer].pMesh) がNULLではない場合

			g_aModelPlayer[nCntPlayer].pMesh->Release();
			g_aModelPlayer[nCntPlayer].pMesh = NULL;
		}
	}

	// マテリアルの破棄
	for (int nCntPlayer = 0; nCntPlayer < MODEL_PLAYER_MAX; nCntPlayer++)
	{ // プレイヤーに使用するモデルの最大数分繰り返す

		if (g_aModelPlayer[nCntPlayer].pBuffMat != NULL)
		{ // 変数 (g_aModelPlayer[nCntPlayer].pBuffMat) がNULLではない場合

			g_aModelPlayer[nCntPlayer].pBuffMat->Release();
			g_aModelPlayer[nCntPlayer].pBuffMat = NULL;
		}
	}
}

//======================================================================================================================
//	プレイヤーの更新処理
//======================================================================================================================
void UpdatePlayer(void)
{
	switch (GetMode())
	{ // モードごとの更新
	case MODE_TUTORIAL:	// チュートリアル画面

		// チュートリアルのプレイヤーの更新
		TutorialPlayer();

		// 処理を抜ける
		break;

	case MODE_GAME:		// ゲーム画面

		// ゲームのプレイヤーの更新
		GamePlayer();

		// 処理を抜ける
		break;
	}
}

//======================================================================================================================
//	プレイヤーの描画処理
//======================================================================================================================
void DrawPlayer(void)
{
	// 変数を宣言
	D3DXMATRIX   mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATRIX   mtxRotParts, mtxTransParts;	// 階層構造の計算用マトリックス
	D3DXMATRIX   mtxParent;						// 親のマトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATERIAL     *pMat;						// マテリアルデータへのポインタ
	D3DXMATERIAL      redMat;					// マテリアルデータ (赤)

	if (g_player.bUse == true)
	{ // プレイヤーが使用されている場合

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_player.mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_player.rot.y, g_player.rot.x, g_player.rot.z);
		D3DXMatrixMultiply(&g_player.mtxWorld, &g_player.mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_player.pos.x, g_player.pos.y, g_player.pos.z);
		D3DXMatrixMultiply(&g_player.mtxWorld, &g_player.mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_player.mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
		{ // プレイヤーのパーツの最大数分繰り返す

			// パーツのワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_player.aParts[nCntParts].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRotParts, g_player.aParts[nCntParts].rot.y, g_player.aParts[nCntParts].rot.x, g_player.aParts[nCntParts].rot.z);
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxRotParts);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTransParts, g_player.aParts[nCntParts].pos.x, g_player.aParts[nCntParts].pos.y, g_player.aParts[nCntParts].pos.z);
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxTransParts);

			// パーツの親マトリックスを設定
			if (g_player.aParts[nCntParts].nParentID == NONE_PLAYER_PARENT)
			{ // 現在のパーツに親が存在しない場合

				// プレイヤーのマトリックスを設定
				mtxParent = g_player.mtxWorld;
			}
			else
			{ // 現在のパーツに親が存在する場合

				// 親のマトリックスを設定
				mtxParent = g_player.aParts[g_player.aParts[nCntParts].nParentID].mtxWorld;
			}

			// パーツのワールドマトリックスと親マトリックスを掛け合わせる
			D3DXMatrixMultiply(&g_player.aParts[nCntParts].mtxWorld, &g_player.aParts[nCntParts].mtxWorld, &mtxParent);

			// パーツのワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_player.aParts[nCntParts].mtxWorld);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_player.aParts[nCntParts].pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_player.aParts[nCntParts].pModelData->dwNumMat; nCntMat++)
			{ // マテリアルの数分繰り返す

				switch (g_player.state)
				{ // 状態ごとの処理
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
				pDevice->SetTexture(0, g_player.aParts[nCntParts].pModelData->apTexture[nCntMat]);

				// パーツの描画
				g_player.aParts[nCntParts].pModelData->pMesh->DrawSubset(nCntMat);
			}
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//======================================================================================================================
//	プレイヤーの位置・向きの設定処理
//======================================================================================================================
void SetPositionPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 引数を代入
	g_player.pos     = pos;		// 現在の位置
	g_player.oldPos  = pos;		// 前回の位置
	g_player.rot     = rot;		// 現在の向き
	g_player.destRot = rot;		// 目標の向き

	// 影のインデックスを設定
	g_player.nShadowID = SetShadow(0.5f, SHADOW_PLAYER, &g_player.nShadowID, &g_player.bUse);

	// 影の位置設定
	SetPositionShadow(g_player.nShadowID, g_player.pos);

	// 体力を設定
	switch (GetMode())
	{ // モードごとの設定
	case MODE_TUTORIAL:	// チュートリアル画面

		// 最大体力の半分を代入
		g_player.nLife = (int)(PLAY_LIFE * 0.5f);

		// 体力バーの設定
		SetLife(g_player.nLife, 0);

		// 処理を抜ける
		break;

	default:			// それ以外の画面

		// 最大体力を代入
		g_player.nLife = PLAY_LIFE;

		// 体力バーの設定
		SetLife(g_player.nLife, 0);

		// 処理を抜ける
		break;
	}

	// 使用している状態にする
	g_player.bUse = true;
}

//======================================================================================================================
//	プレイヤーの回復判定
//======================================================================================================================
void HealPlayer(Player *pPlayer, int nHeal)
{
	if (pPlayer->state == ACTIONSTATE_NORMAL)
	{ // プレイヤーが通常状態の場合

		// 体力バーの設定
		SetLife(pPlayer->nLife, nHeal);

		// 引数の回復分を体力に加算
		pPlayer->nLife += nHeal;

		if (pPlayer->nLife > PLAY_LIFE)
		{ // 体力が最大体力より多くなった場合

			// 体力を補正
			pPlayer->nLife = PLAY_LIFE;
		}

		// サウンドの再生
		PlaySound(SOUND_LABEL_SE_HEAL);		// SE (回復)
	}
}

//======================================================================================================================
//	プレイヤーのダメージ判定
//======================================================================================================================
void HitPlayer(Player *pPlayer, int nDamage)
{
	if (pPlayer->state == ACTIONSTATE_NORMAL)
	{ // プレイヤーが通常状態の場合

		// 体力バーの設定
		SetLife(pPlayer->nLife, -nDamage);

		// 引数のダメージ分を体力から減算
		pPlayer->nLife -= nDamage;

		if (pPlayer->nLife > 0)
		{ // 体力が残っている場合

			// ダメージ状態にする
			pPlayer->state = ACTIONSTATE_DAMAGE;

			// パーティクルの設定
			SetParticle
			( // 引数
				pPlayer->pos,						// 位置
				D3DXCOLOR(0.8f, 0.0f, 0.3f, 1.0f),	// 色
				PARTICLETYPE_DAMAGE,				// 種類
				SPAWN_PARTICLE_DAMAGE,				// エフェクト数
				2									// 寿命
			);

			// カウンターを設定
			pPlayer->nCounterState = DAMAGE_TIME_PLAY;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_DMG);			// SE (ダメージ)
		}
		else
		{ // 体力が尽きた場合

			// 爆発の設定
			SetExplosion(pPlayer->pos, SOUNDTYPE_EXPLOSION);

			// パーティクルの設定
			SetParticle
			( // 引数
				pPlayer->pos,						// 位置
				D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),	// 色
				PARTICLETYPE_EXPLOSION,				// 種類
				SPAWN_PARTICLE_EXPLOSION,			// エフェクト数
				2									// 寿命
			);

			// 使用していない状態にする
			pPlayer->bUse = false;
		}
	}
}

//======================================================================================================================
//	チュートリアルのプレイヤーの更新処理
//======================================================================================================================
void TutorialPlayer(void)
{
	// 変数を宣言
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// 現在のモーション
	MOTIONTYPE lessonMotion  = MOTIONTYPE_NONE;	// チュートリアルのレッスン加算用

	if (g_player.bUse == true)
	{ // プレイヤーが使用されている場合

		// 前回位置の更新
		g_player.oldPos = g_player.pos;

		// 状態の更新
		if (g_player.nCounterState > 0)
		{ // カウンターが 0より大きい場合

			// カウンターを減算
			g_player.nCounterState--;

			if (g_player.nCounterState == UNR_TIME_PLAY)
			{ // カウンターが一定値の場合

				// 無敵状態にする
				g_player.state = ACTIONSTATE_UNRIVALED;
			}
			else if (g_player.nCounterState <= 0)
			{ // カウンターが 0以下の場合

				// 通常状態にする
				g_player.state = ACTIONSTATE_NORMAL;
			}
		}

		// プレイヤーの移動量の更新
		currentMotion = MovePlayer();

		if (GetLessonState() >= LESSON_01)
		{ // レッスン1に挑戦中、またはクリアしている場合

			// プレイヤーのジャンプの更新
			lessonMotion = JumpPlayer();
		}

		// プレイヤーの位置の更新
		PosPlayer();

		// プレイヤーの向きの更新
		RotPlayer();

		// プレイヤーの補正の更新処理
		RevPlayer();

		// ジャンプ判定
		if (g_player.pos.y < GetLimitStage().fField)
		{ // 地面に当たっている場合

			// ジャンプしていない状態にする
			g_player.bJump = false;

			// キックしていない状態にする
			g_player.bKick = false;

			// 竜巻の成長設定
			SetGrowTornado
			( // 引数
				g_player.nTornadoID,	// 変更インデックス
				0.6f,	// 向きの変更量
				0.012f,	// 横ずれの成長量
				0.012f,	// 縦ずれの成長量
				0.032f	// 透明度の成長量
			);

			// 地面に位置を補正
			g_player.pos.y = GetLimitStage().fField;

			// 移動量を初期化
			g_player.move.y = 0.0f;
		}
		else
		{ // 地面に当たっていない場合

			// ジャンプしている状態にする
			g_player.bJump = true;
		}

		// オブジェクトとの当たり判定
		CollisionObject
		( // 引数
			&g_player.pos,		// 現在の位置
			&g_player.oldPos,	// 前回の位置
			PLAY_WIDTH,			// 横幅
			PLAY_DEPTH			// 奥行
		);

		// プレイヤーのキックの当たり判定
		CollisionKickEnemy(&g_player);

		if (GetLessonState() >= LESSON_02)
		{ // レッスン2に挑戦中、またはクリアしている場合

			// 斬撃の更新
			if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true)
			{ // 攻撃の操作が押された場合

				if (g_player.nSlashID == NONE_SLASH && g_player.motion.type != MOTIONTYPE_ATTACK)
				{ // 斬撃のインデックスが初期化されている且つ、現在のモーションが攻撃モーションではない場合

					// 現在のモーションに攻撃状態を代入
					currentMotion = MOTIONTYPE_ATTACK;

					// パーティクルの設定
					SetParticle
					( // 引数
						g_player.pos,						// 位置
						D3DXCOLOR(0.8f, 0.3f, 0.35f, 1.0f),	// 色
						PARTICLETYPE_SLASH,					// 種類
						SPAWN_PARTICLE_SLASH,				// エフェクト数
						4									// 寿命
					);

					// 斬撃の設定
					g_player.nSlashID = SetSlash
					( // 引数
						SLASHTYPE_PLAYER,	// 種類
						PLAY_SLASH_LIFE,	// 寿命
						PLAY_SLASH_RADIUS,	// 半径
						&g_player.nSlashID,	// 斬撃の親の斬撃インデックス
						&g_player.bUse		// 斬撃の親の使用状況
					);
				}
			}

			// 斬撃の位置・向きの設定
			SetPositionSlash
			( // 引数
				g_player.nSlashID,	// 斬撃のインデックス
				D3DXVECTOR3
				( // 引数
					g_player.pos.x,						// 位置 (x)
					g_player.pos.y + PLAY_SLASH_POS_Y,	// 位置 (y)
					g_player.pos.z						// 位置 (z)
				),
				g_player.rot		// 向き
			);
		}

		//--------------------------------------------------------------------------------------------------------------
		//	影の更新
		//--------------------------------------------------------------------------------------------------------------
		// 影の位置設定
		SetPositionShadow
		( // 引数
			g_player.nShadowID,	// 影のインデックス
			g_player.pos		// 位置
		);

		//--------------------------------------------------------------------------------------------------------------
		//	モーションの更新
		//--------------------------------------------------------------------------------------------------------------
		if (currentMotion != g_player.motion.type && g_player.motion.type != MOTIONTYPE_ATTACK)
		{ // 現在のモーションと前回のモーションが違う且つ、現在のモーションが攻撃モーションではない場合

			// モーションの設定
			SetMotion(currentMotion);
		}

		// モーションの更新
		UpdateMotion();

		//--------------------------------------------------------------------------------------------------------------
		//	レッスンの更新
		//--------------------------------------------------------------------------------------------------------------
		switch (GetLessonState())
		{ // レッスンごとの処理
		case LESSON_00:	// レッスン0 (移動)

			if (currentMotion == MOTIONTYPE_MOVE)
			{ // 現在のモーションが移動状態の場合

				// レッスンの状態の加算
				AddLessonState();
			}

			// 処理を抜ける
			break;

		case LESSON_01:	// レッスン1 (ジャンプ)

			if (lessonMotion == MOTIONTYPE_JUMP)
			{ // 現在のモーションがジャンプ状態の場合

				// レッスンの状態の加算
				AddLessonState();
			}

			// 処理を抜ける
			break;

		case LESSON_02:	// レッスン2 (攻撃)

			if (currentMotion == MOTIONTYPE_ATTACK)
			{ // 現在のモーションが攻撃状態の場合

				// レッスンの状態の加算
				AddLessonState();
			}

			// 処理を抜ける
			break;
		}
	}
}

//======================================================================================================================
//	ゲームのプレイヤーの更新処理
//======================================================================================================================
void GamePlayer(void)
{
	// 変数を宣言
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// 現在のモーション

	if (g_player.bUse == true)
	{ // プレイヤーが使用されている場合

		// 前回位置の更新
		g_player.oldPos = g_player.pos;
		
		// 状態の更新
		if (g_player.nCounterState > 0)
		{ // カウンターが 0より大きい場合

			// カウンターを減算
			g_player.nCounterState--;

			if (g_player.nCounterState == UNR_TIME_PLAY)
			{ // カウンターが一定値の場合

				// 無敵状態にする
				g_player.state = ACTIONSTATE_UNRIVALED;
			}
			else if (g_player.nCounterState <= 0)
			{ // カウンターが 0以下の場合

				// 通常状態にする
				g_player.state = ACTIONSTATE_NORMAL;
			}
		}

		// プレイヤーの移動量の更新
		currentMotion = MovePlayer();

		// プレイヤーのジャンプの更新
		JumpPlayer();

		// プレイヤーの位置の更新
		PosPlayer();

		// プレイヤーの向きの更新
		RotPlayer();

		// プレイヤーの補正の更新処理
		RevPlayer();

		//--------------------------------------------------------------------------------------------------------------
		//	ジャンプ判定
		//--------------------------------------------------------------------------------------------------------------
		if (g_player.pos.y < GetLimitStage().fField)
		{ // 地面に当たっている場合

			// ジャンプしていない状態にする
			g_player.bJump = false;

			// キックしていない状態にする
			g_player.bKick = false;

			// 竜巻の成長設定
			SetGrowTornado
			( // 引数
				g_player.nTornadoID,	// 変更インデックス
				0.6f,	// 向きの変更量
				0.012f,	// 横ずれの成長量
				0.012f,	// 縦ずれの成長量
				0.032f	// 透明度の成長量
			);

			// 地面に位置を補正
			g_player.pos.y  = GetLimitStage().fField;

			// 移動量を初期化
			g_player.move.y = 0.0f;
		}
		else
		{ // 地面に当たっていない場合

			// ジャンプしている状態にする
			g_player.bJump = true;
		}

		//--------------------------------------------------------------------------------------------------------------
		//	当たり判定
		//--------------------------------------------------------------------------------------------------------------
		// オブジェクトとの当たり判定
		CollisionObject
		( // 引数
			&g_player.pos,		// 現在の位置
			&g_player.oldPos,	// 前回の位置
			PLAY_WIDTH,			// 横幅
			PLAY_DEPTH			// 奥行
		);

		// プレイヤーのキックの当たり判定
		CollisionKickEnemy(&g_player);

		//--------------------------------------------------------------------------------------------------------------
		//	斬撃の更新
		//--------------------------------------------------------------------------------------------------------------
		if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true)
		{ // 攻撃の操作が押された場合

			if (g_player.nSlashID == NONE_SLASH && g_player.motion.type != MOTIONTYPE_ATTACK)
			{ // 斬撃のインデックスが初期化されている且つ、現在のモーションが攻撃モーションではない場合

				// 現在のモーションに攻撃状態を代入
				currentMotion = MOTIONTYPE_ATTACK;

				// パーティクルの設定
				SetParticle
				( // 引数
					g_player.pos,						// 位置
					D3DXCOLOR(0.8f, 0.3f, 0.35f, 1.0f),	// 色
					PARTICLETYPE_SLASH,					// 種類
					SPAWN_PARTICLE_SLASH,				// エフェクト数
					4									// 寿命
				);

				// 斬撃の設定
				g_player.nSlashID = SetSlash
				( // 引数
					SLASHTYPE_PLAYER,	// 種類
					PLAY_SLASH_LIFE,	// 寿命
					PLAY_SLASH_RADIUS,	// 半径
					&g_player.nSlashID,	// 斬撃の親の斬撃インデックス
					&g_player.bUse		// 斬撃の親の使用状況
				);
			}
		}

		// 斬撃の位置・向きの設定
		SetPositionSlash
		( // 引数
			g_player.nSlashID,	// 斬撃のインデックス
			D3DXVECTOR3
			( // 引数
				g_player.pos.x,						// 位置 (x)
				g_player.pos.y + PLAY_SLASH_POS_Y,	// 位置 (y)
				g_player.pos.z						// 位置 (z)
			),
			g_player.rot		// 向き
		);

		//--------------------------------------------------------------------------------------------------------------
		//	影の更新
		//--------------------------------------------------------------------------------------------------------------
		// 影の位置設定
		SetPositionShadow
		( // 引数
			g_player.nShadowID,	// 影のインデックス
			g_player.pos		// 位置
		);

		//--------------------------------------------------------------------------------------------------------------
		//	モーションの更新
		//--------------------------------------------------------------------------------------------------------------
		if (currentMotion != g_player.motion.type && g_player.motion.type != MOTIONTYPE_ATTACK)
		{ // 現在のモーションと前回のモーションが違う且つ、現在のモーションが攻撃モーションではない場合

			// モーションの設定
			SetMotion(currentMotion);
		}

		// モーションの更新
		UpdateMotion();
	}
}

//======================================================================================================================
//	プレイヤーの移動量の更新処理
//======================================================================================================================
MOTIONTYPE MovePlayer(void)
{
	// 変数を宣言
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// 現在のモーション

	// ポインタを宣言
	Camera *pCamera = GetCamera();	// カメラの情報

	if (GetKeyboardPress(DIK_W) == true || GetJoyKeyPress(JOYKEY_UP, 0) == true || GetJoyStickPressLY(0) > 0)
	{ // 奥移動の操作が行われた場合

		// 現在のモーションに移動状態を代入
		currentMotion = MOTIONTYPE_MOVE;

		if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
		{ // 左移動の操作も行われた場合 (左奥移動)

			// 移動量を更新
			g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(135) + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
		{ // 右移動の操作も行われた場合 (右奥移動)

			// 移動量を更新
			g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(225) + pCamera->rot.y;
		}
		else
		{ // 奥移動の操作だけが行われた場合 (奥移動)

			// 移動量を更新
			g_player.move.x += sinf(pCamera->rot.y) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(180) + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_S) == true || GetJoyKeyPress(JOYKEY_DOWN, 0) == true || GetJoyStickPressLY(0) < 0)
	{ // 手前移動の操作が行われた場合

		// 現在のモーションに移動状態を代入
		currentMotion = MOTIONTYPE_MOVE;

		if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
		{ // 左移動の操作も行われた場合 (左手前移動)

			// 移動量を更新
			g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;
			g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.75f)) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(45) + pCamera->rot.y;
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
		{ // 右移動の操作も行われた場合 (右手前移動)

			// 移動量を更新
			g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.25f)) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(315) + pCamera->rot.y;
		}
		else
		{ // 手前移動の操作だけが行われた場合 (手前移動)

			// 移動量を更新
			g_player.move.x -= sinf(pCamera->rot.y) * MOVE_PLAYER;
			g_player.move.z -= cosf(pCamera->rot.y) * MOVE_PLAYER;

			// 目標向きを更新
			g_player.destRot.y = D3DXToRadian(0) + pCamera->rot.y;
		}
	}
	else if (GetKeyboardPress(DIK_A) == true || GetJoyKeyPress(JOYKEY_LEFT, 0) == true || GetJoyStickPressLX(0) < 0)
	{ // 左移動の操作が行われた場合

		// 現在のモーションに移動状態を代入
		currentMotion = MOTIONTYPE_MOVE;

		// 移動量を更新
		g_player.move.x += sinf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;
		g_player.move.z += cosf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;

		// 目標向きを更新
		g_player.destRot.y = D3DXToRadian(90) + pCamera->rot.y;
	}
	else if (GetKeyboardPress(DIK_D) == true || GetJoyKeyPress(JOYKEY_RIGHT, 0) == true || GetJoyStickPressLX(0) > 0)
	{ // 右移動の操作が行われた場合

		// 現在のモーションに移動状態を代入
		currentMotion = MOTIONTYPE_MOVE;

		// 移動量を更新
		g_player.move.x -= sinf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;
		g_player.move.z -= cosf(pCamera->rot.y - (D3DX_PI * 0.5f)) * MOVE_PLAYER;

		// 目標向きを更新
		g_player.destRot.y = D3DXToRadian(270) + pCamera->rot.y;
	}

	// 現在のモーションを返す
	return currentMotion;
}

//======================================================================================================================
//	プレイヤーのジャンプの更新処理
//======================================================================================================================
MOTIONTYPE JumpPlayer(void)
{
	// 変数を宣言
	MOTIONTYPE currentMotion = MOTIONTYPE_NONE;	// 現在のモーション

	if (GetKeyboardTrigger(DIK_SPACE) == true || GetJoyKeyTrigger(JOYKEY_A, 0) == true)
	{ // ジャンプの操作が行われた場合

		if (g_player.bJump == false)
		{ // ジャンプ中ではない場合

			// 現在のモーションにジャンプ状態を代入
			currentMotion = MOTIONTYPE_JUMP;

			// 移動量を更新
			g_player.move.y += PLAY_JUMP;

			// サウンドの再生
			PlaySound(SOUND_LABEL_SE_JUMP);	// SE (ジャンプ)
		}
	}

	// 現在のモーションを返す
	return currentMotion;
}

//======================================================================================================================
//	プレイヤーの位置の更新処理
//======================================================================================================================
void PosPlayer(void)
{
	//------------------------------------------------------------------------------------------------------------------
	//	重力の更新
	//------------------------------------------------------------------------------------------------------------------
	g_player.move.y -= PLAY_GRAVITY;

	//------------------------------------------------------------------------------------------------------------------
	//	位置の更新
	//------------------------------------------------------------------------------------------------------------------
	// 位置の更新
	g_player.pos += g_player.move;

	//------------------------------------------------------------------------------------------------------------------
	//	移動量の減衰・削除
	//------------------------------------------------------------------------------------------------------------------
	// 移動量の減衰
	g_player.move.x += (0.0f - g_player.move.x) * SUB_MOVE_PLAYER;
	g_player.move.z += (0.0f - g_player.move.z) * SUB_MOVE_PLAYER;

	// 移動量の削除
	if (g_player.move.x <  DEL_MOVE_PLAYER
	&&  g_player.move.x > -DEL_MOVE_PLAYER)
	{ // プレイヤーの移動量 (x) が削除する範囲内の場合

		// プレイヤーの移動量 (x) を削除する
		g_player.move.x = 0.0f;
	}

	if (g_player.move.z <  DEL_MOVE_PLAYER
	&&  g_player.move.z > -DEL_MOVE_PLAYER)
	{ // プレイヤーの移動量 (z) が削除する範囲内の場合

		// プレイヤーの移動量 (z) を削除する
		g_player.move.z = 0.0f;
	}
}

//======================================================================================================================
//	プレイヤーの向きの更新処理
//======================================================================================================================
void RotPlayer(void)
{
	// 変数を宣言
	float fDiffRot = 0.0f;	// プレイヤーの向き計算代入用

	// 目標向きの正規化
	if      (g_player.destRot.y >  D3DX_PI) { g_player.destRot.y -= D3DX_PI * 2; }
	else if (g_player.destRot.y < -D3DX_PI) { g_player.destRot.y += D3DX_PI * 2; }

	// 目標向きまでの差分を計算
	fDiffRot = g_player.destRot.y - g_player.rot.y;

	// 差分向きの正規化
	if      (fDiffRot >  D3DX_PI) { fDiffRot -= D3DX_PI * 2; }
	else if (fDiffRot < -D3DX_PI) { fDiffRot += D3DX_PI * 2; }

	// 向きの更新
	g_player.rot.y += fDiffRot * REV_ROT_PLAYER;

	// 向きの正規化
	if      (g_player.rot.y >  D3DX_PI) { g_player.rot.y -= D3DX_PI * 2; }
	else if (g_player.rot.y < -D3DX_PI) { g_player.rot.y += D3DX_PI * 2; }
}

//======================================================================================================================
//	プレイヤーの補正の更新処理
//======================================================================================================================
void RevPlayer(void)
{
	if (g_player.pos.z > GetLimitStage().fNear - (PLAY_DEPTH * 2))
	{ // 範囲外の場合 (手前)

		// 手前に位置を補正
		g_player.pos.z = GetLimitStage().fNear - (PLAY_DEPTH * 2);
	}
	if (g_player.pos.z < GetLimitStage().fFar + (PLAY_DEPTH * 2))
	{ // 範囲外の場合 (奥)

		// 奥に位置を補正
		g_player.pos.z = GetLimitStage().fFar + (PLAY_DEPTH * 2);
	}
	if (g_player.pos.x > GetLimitStage().fRight - (PLAY_WIDTH * 2))
	{ // 範囲外の場合 (右)

		// 右に位置を補正
		g_player.pos.x = GetLimitStage().fRight - (PLAY_WIDTH * 2);
	}
	if (g_player.pos.x < GetLimitStage().fLeft + (PLAY_WIDTH * 2))
	{ // 範囲外の場合 (左)

		// 左に位置を補正
		g_player.pos.x = GetLimitStage().fLeft + (PLAY_WIDTH * 2);
	}
}

//======================================================================================================================
//	プレイヤーのキックの当たり判定
//======================================================================================================================
void CollisionKickEnemy(Player *pPlayer)
{
	// 変数を宣言
	float fLength = 0.0f;			// キックと敵との間の距離

	// ポインタを宣言
	Enemy *pEnemy = GetEnemy();		// 敵の情報

	if (g_player.bKick == false)
	{ // キックしていない状態の場合

		if (g_player.move.y < 0.0f)
		{ // プレイヤーの y移動量が下方向の場合

			// キックしている状態にする
			g_player.bKick = true;

			// 竜巻の設定
			g_player.nTornadoID = SetTornado
			( // 引数
				D3DXVECTOR3(0.0f, -10.0f, -8.0f),	// 位置
				D3DXVECTOR3(0.15f, 0.0f, 0.0f),		// 成長向き
				TORNADOTYPE_NORMAL,	// 種類
				&pPlayer->mtxWorld,	// 親のマトリックス
				2,		// 渦の周回数
				64,		// 渦の分割数
				0.4f,	// 向きの変更量
				13.0f,	// ポリゴンの太さ
				8.0f,	// ポリゴン外周の y座標加算量
				5.0f,	// 生成時の横ずれ量
				0.6f,	// 生成時の透明度
				0.25f,	// 横ずれの加算量
				0.55f,	// 縦ずれの加算量
				0.004f,	// 透明度の減算量
				0.0f,	// 横ずれの成長量
				0.0f,	// 縦ずれの成長量
				0.0f	// 透明度の成長量
			);
		}
	}
	else
	{ // キックしている状態の場合

		for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++, pEnemy++)
		{ // 敵の最大表示数分繰り返す

			if (pEnemy->bUse == true)
			{ // 敵が使用されている場合

				// プレイヤーの足と敵との間の距離を求める
				fLength = (pPlayer->pos.x - pEnemy->pos.x) * (pPlayer->pos.x - pEnemy->pos.x)
						+ (pPlayer->pos.z - pEnemy->pos.z) * (pPlayer->pos.z - pEnemy->pos.z);

				if (fLength < ((PLAY_KICK_RADIUS + pEnemy->pModelData->fRadius) * (PLAY_KICK_RADIUS + pEnemy->pModelData->fRadius))	// 円の当たり判定
				&&  pPlayer->pos.y + PLAY_KICK_HEIGHT > pEnemy->pos.y																// 縦の判定 (下)
				&&  pPlayer->pos.y - PLAY_KICK_HEIGHT < pEnemy->pos.y + pEnemy->pModelData->fHeight)								// 縦の判定 (上)
				{ // キックが敵に当たっている場合

					// 移動量を更新
					g_player.move.y = PLAY_KICK_JUMP;

					// 敵のダメージ判定
					if (HitEnemy(pEnemy, KICK_DMG_PLAY, DAMAGETYPE_KICK) == true)
					{ // 倒した場合

						// 竜巻の設定
						SetTornado
						( // 引数
							pEnemy->pos,		// 位置
							pEnemy->rot,		// 成長向き
							TORNADOTYPE_NORMAL,	// 種類
							NULL,	// 親のマトリックス
							2,		// 渦の周回数
							64,		// 渦の分割数
							0.2f,	// 向きの変更量
							10.0f,	// ポリゴンの太さ
							6.0f,	// ポリゴン外周の y座標加算量
							5.0f,	// 生成時の横ずれ量
							0.8f,	// 生成時の透明度
							0.2f,	// 横ずれの加算量
							0.4f,	// 縦ずれの加算量
							0.004f,	// 透明度の減算量
							0.01f,	// 横ずれの成長量
							0.01f,	// 縦ずれの成長量
							0.04f	// 透明度の成長量
						);
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	モーションの更新処理
//======================================================================================================================
void UpdateMotion(void)
{
	// 変数を宣言
	D3DXVECTOR3 diffPos;		// 次のポーズまでの差分 (位置)
	D3DXVECTOR3 diffRot;		// 次のポーズまでの差分 (向き)
	D3DXVECTOR3 currentPos;		// 現在のフレームの位置
	D3DXVECTOR3 currentRot;		// 現在のフレームの向き

	MOTIONTYPE  type  = g_player.motion.type;	// モーションの種類
	int         nPose = g_player.motion.nPose;	// モーションのポーズ番号

	// パーツの位置を更新
	for (int nCntKey = 0; nCntKey < MAX_PARTS; nCntKey++)
	{ // プレイヤーのパーツの最大数分繰り返す

		// 位置の差分を求める
		diffPos = g_player.motion.aMotion[type].aKey[(nPose + 1) % g_player.motion.aMotion[type].nNumKey].aPos[nCntKey] - g_player.motion.aMotion[type].aKey[nPose].aPos[nCntKey];

		// 向きの差分を求める
		diffRot = g_player.motion.aMotion[type].aKey[(nPose + 1) % g_player.motion.aMotion[type].nNumKey].aRot[nCntKey] - g_player.motion.aMotion[type].aKey[nPose].aRot[nCntKey];

		// 差分向きの正規化
		if      (diffRot.x >  D3DX_PI) { diffRot.x -= D3DX_PI * 2; }
		else if (diffRot.x < -D3DX_PI) { diffRot.x += D3DX_PI * 2; }
		if      (diffRot.y >  D3DX_PI) { diffRot.y -= D3DX_PI * 2; }
		else if (diffRot.y < -D3DX_PI) { diffRot.y += D3DX_PI * 2; }
		if      (diffRot.z >  D3DX_PI) { diffRot.z -= D3DX_PI * 2; }
		else if (diffRot.z < -D3DX_PI) { diffRot.z += D3DX_PI * 2; }

		// 現在のフレームの位置を設定
		currentPos = g_player.motion.aMotion[type].aKey[nPose].aPos[nCntKey] + diffPos * ((float)g_player.motion.nCounter / (float)g_player.motion.aMotion[type].aKey[nPose].nFrame);

		// 現在のフレームの向きを設定
		currentRot = g_player.motion.aMotion[type].aKey[nPose].aRot[nCntKey] + diffRot * ((float)g_player.motion.nCounter / (float)g_player.motion.aMotion[type].aKey[nPose].nFrame);

		// 差分向きの正規化
		if      (currentRot.x >  D3DX_PI) { currentRot.x -= D3DX_PI * 2; }
		else if (currentRot.x < -D3DX_PI) { currentRot.x += D3DX_PI * 2; }
		if      (currentRot.y >  D3DX_PI) { currentRot.y -= D3DX_PI * 2; }
		else if (currentRot.y < -D3DX_PI) { currentRot.y += D3DX_PI * 2; }
		if      (currentRot.z >  D3DX_PI) { currentRot.z -= D3DX_PI * 2; }
		else if (currentRot.z < -D3DX_PI) { currentRot.z += D3DX_PI * 2; }

		// 現在のパーツの位置を更新
		g_player.aParts[nCntKey].pos = g_player.aParts[nCntKey].initPos + currentPos;

		// 現在のパーツの向きを更新
		g_player.aParts[nCntKey].rot = g_player.aParts[nCntKey].initRot + currentRot;

		// 向きの正規化
		if      (g_player.aParts[nCntKey].rot.x >  D3DX_PI) { g_player.aParts[nCntKey].rot.x -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.x < -D3DX_PI) { g_player.aParts[nCntKey].rot.x += D3DX_PI * 2; }
		if      (g_player.aParts[nCntKey].rot.y >  D3DX_PI) { g_player.aParts[nCntKey].rot.y -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.y < -D3DX_PI) { g_player.aParts[nCntKey].rot.y += D3DX_PI * 2; }
		if      (g_player.aParts[nCntKey].rot.z >  D3DX_PI) { g_player.aParts[nCntKey].rot.z -= D3DX_PI * 2; }
		else if (g_player.aParts[nCntKey].rot.z < -D3DX_PI) { g_player.aParts[nCntKey].rot.z += D3DX_PI * 2; }
	}

	// モーションの遷移の更新
	if (g_player.motion.nCounter >= g_player.motion.aMotion[type].aKey[nPose].nFrame)
	{ // 現在のモーションカウンターが現在のポーズの再生フレーム数を超えている場合

		// 次のポーズに移行
		if (g_player.motion.aMotion[type].bLoop == true)
		{ // モーションがループする設定の場合

			// モーションカウンターを初期化
			g_player.motion.nCounter = 0;

			// ポーズカウントを加算 (総数に達した場合 0に戻す)
			g_player.motion.nPose = (g_player.motion.nPose + 1) % g_player.motion.aMotion[type].nNumKey;
		}
		else
		{ // モーションがループしない設定の場合

			if (g_player.motion.nPose < g_player.motion.aMotion[type].nNumKey - SUB_MOTION_STOP)
			{ // 現在のポーズが最終のポーズではない場合

				// モーションカウンターを初期化
				g_player.motion.nCounter = 0;

				// ポーズカウントを加算
				g_player.motion.nPose++;
			}
			else
			{ // 現在のポーズが最終のポーズの場合

				switch (type)
				{ // モーションの種類ごとの処理
				case MOTIONTYPE_ATTACK:		// 攻撃状態

					// 待機モーションに移行
					SetMotion(MOTIONTYPE_NONE);

					// 処理を抜ける
					break;

				case MOTIONTYPE_JUMP:		// ジャンプ状態

					// 無し

					// 処理を抜ける
					break;

				case MOTIONTYPE_LANDING:	// 着地状態

					// 無し

					// 処理を抜ける
					break;
				}
			}
		}
	}
	else
	{ // 現在のモーションカウンターが現在のポーズの再生フレーム数を超えていない場合

		// モーションカウンターを加算
		g_player.motion.nCounter++;
	}
}

//======================================================================================================================
//	モーションの設定処理
//======================================================================================================================
void SetMotion(MOTIONTYPE type)
{
	// 引数を代入
	g_player.motion.type = type;	// モーションの種類

	// モーション情報を初期化
	g_player.motion.nPose    = 0;	// モーションのポーズ番号
	g_player.motion.nCounter = 0;	// モーションのカウンター
}

//======================================================================================================================
//	プレイヤーのセットアップ処理
//======================================================================================================================
void TxtSetPlayer(void)
{
	// 変数を宣言
	int nID        = 0;			// インデックスの代入用
	int nNowMotion = 0;			// 現在のモーション番号
	int nNowPose   = 0;			// 現在のポーズ番号
	int nNowKey    = 0;			// 現在のキー番号
	int nLoop      = 0;			// ループの ON / OFF の変換用
	int nEnd       = 0;			// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;				// ファイルポインタ

	// ファイルを読み込み形式で開く
	pFile = fopen(PLAYER_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			//----------------------------------------------------------------------------------------------------------
			//	キャラクターの設定
			//----------------------------------------------------------------------------------------------------------
			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{ // 読み込んだ文字列が CHARACTERSET の場合

				do
				{ // 読み込んだ文字列が END_CHARACTERSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "PARTSSET") == 0)
					{ // 読み込んだ文字列が PARTSSET の場合

						do
						{ // 読み込んだ文字列が END_PARTSSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "INDEX") == 0)
							{ // 読み込んだ文字列が INDEX の場合
								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nID);			// モデル (パーツ) のインデックスを読み込む
							}
							else if (strcmp(&aString[0], "PARENT") == 0)
							{ // 読み込んだ文字列が PARENT の場合
								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &g_player.aParts[nID].nParentID);	// モデル (パーツ) の親のインデックスを読み込む
							}
							else if (strcmp(&aString[0], "POS") == 0)
							{ // 読み込んだ文字列が POS の場合
								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.x);	// X座標を読み込む
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.y);	// Y座標を読み込む
								fscanf(pFile, "%f", &g_player.aParts[nID].initPos.z);	// Z座標を読み込む

								// 現在の位置に読み込んだ値を設定
								g_player.aParts[nID].pos = g_player.aParts[nID].initPos;
							}
							else if (strcmp(&aString[0], "ROT") == 0)
							{ // 読み込んだ文字列が ROT の場合
								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.x);	// X向きを読み込む
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.y);	// Y向きを読み込む
								fscanf(pFile, "%f", &g_player.aParts[nID].initRot.z);	// Z向きを読み込む

								// 現在の向きに読み込んだ値を設定
								g_player.aParts[nID].rot = g_player.aParts[nID].initRot;
							}

						} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// 読み込んだ文字列が END_PARTSSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// 読み込んだ文字列が END_CHARACTERSET ではない場合ループ
			}

			//----------------------------------------------------------------------------------------------------------
			//	モーションの設定
			//----------------------------------------------------------------------------------------------------------
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // 読み込んだ文字列が MOTIONSET の場合

				// 現在のポーズ番号を初期化
				nNowPose = 0;

				do
				{ // 読み込んだ文字列が END_MOTIONSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "LOOP") == 0)
					{ // 読み込んだ文字列が LOOP の場合
						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &nLoop);		// ループの ON / OFF を読み込む

						// 読み込んだ値が 0なら true、0以外なら falseを代入
						g_player.motion.aMotion[nNowMotion].bLoop = (nLoop == 0) ? false : true;
					}
					else if (strcmp(&aString[0], "NUM_KEY") == 0)
					{ // 読み込んだ文字列が NUM_KEY の場合
						fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
						fscanf(pFile, "%d", &g_player.motion.aMotion[nNowMotion].nNumKey);	// キーの総数を読み込む
					}
					else if (strcmp(&aString[0], "KEYSET") == 0)
					{ // 読み込んだ文字列が KEYSET の場合

						// 現在のキー番号を初期化
						nNowKey = 0;

						do
						{ // 読み込んだ文字列が END_KEYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "FRAME") == 0)
							{ // 読み込んだ文字列が FRAME の場合
								fscanf(pFile, "%s", &aString[0]);													// = を読み込む (不要)
								fscanf(pFile, "%d", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].nFrame);	// キーが切り替わるまでのフレーム数を読み込む
							}
							else if (strcmp(&aString[0], "KEY") == 0)
							{ // 読み込んだ文字列が KEY の場合

								do
								{ // 読み込んだ文字列が END_KEY ではない場合ループ

									// ファイルから文字列を読み込む
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "POS") == 0)
									{ // 読み込んだ文字列が POS の場合
										fscanf(pFile, "%s", &aString[0]);															// = を読み込む (不要)
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].x);	// 位置 (x) を読み込む
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].y);	// 位置 (y) を読み込む
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aPos[nNowKey].z);	// 位置 (z) を読み込む
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // 読み込んだ文字列が ROT の場合
										fscanf(pFile, "%s", &aString[0]);															// = を読み込む (不要)
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].x);	// 向き (x) を読み込む
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].y);	// 向き (y) を読み込む
										fscanf(pFile, "%f", &g_player.motion.aMotion[nNowMotion].aKey[nNowPose].aRot[nNowKey].z);	// 向き (z) を読み込む
									}

								} while (strcmp(&aString[0], "END_KEY") != 0);	// 読み込んだ文字列が END_KEY ではない場合ループ

								// 現在のキー番号を加算
								nNowKey++;
							}
						} while (strcmp(&aString[0], "END_KEYSET") != 0);		// 読み込んだ文字列が END_KEYSET ではない場合ループ

						// 現在のポーズ番号を加算
						nNowPose++;
					}
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);			// 読み込んだ文字列が END_MOTIONSET ではない場合ループ

				// 現在のモーション番号を加算
				nNowMotion++;
			}
		} while (nEnd != EOF);													// 読み込んだ文字列が EOF ではない場合ループ
		
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
//	プレイヤーの取得処理
//======================================================================================================================
Player *GetPlayer(void)
{
	// プレイヤー本体の情報アドレスを返す
	return &g_player;
}

#ifdef _DEBUG	// デバッグ処理
//======================================================================================================================
//	デバッグ処理一覧
//======================================================================================================================
//**********************************************************************************************************************
//	プレイヤーの座標の取得処理
//**********************************************************************************************************************
D3DXVECTOR3 GetPlayerPos(void)
{
	// 座標を返す
	return g_player.move;
}

//**********************************************************************************************************************
//	プレイヤーの向きの取得処理
//**********************************************************************************************************************
D3DXVECTOR3 GetPlayerRot(void)
{
	// 座標を返す
	return g_player.rot;
}

//**********************************************************************************************************************
//	プレイヤーの再設定処理
//**********************************************************************************************************************
void ResetPlayer(void)
{
	// グローバル変数を再設定
	g_player.pos     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 現在の位置
	g_player.oldPos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 前回の位置
	g_player.rot     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 現在の向き
	g_player.destRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標の向き
}
#endif