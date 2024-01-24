//============================================================
//
//	マナフラワー処理 [flower.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "flower.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "sound.h"
#include "texture.h"

#include "collision.h"
#include "player.h"
#include "target.h"
#include "shadow.h"
#include "field.h"
#include "stage.h"
#include "particle3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define FLOWER_SETUP_TXT	"data\\TXT\\flower.txt"	// セットアップテキスト相対パス

#define FLOWER_PRIO	(2)	// マナフラワーの優先順位

#define SHADOW_SIZE	(D3DXVECTOR3(80.0f, 0.0f, 80.0f))	// 影の大きさ
#define SHADOW_ALPHA	(0.2f)	// 影のα値

#define NORMAL_CNT	(20)	// ダメージ状態から通常状態に戻るまでのフレーム数
#define MIN_COL		(0.5f)	// 色の最小値

#define REV_MIN_SIZE		(0.6f)	// 最低限の大きさの補正係数
#define PREC_PLUS_RADIUS	(80.0f)	// 生成制限の半径加算量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CFlower::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\flower000.png",	// 春マナフラワーテクスチャ
	"data\\TEXTURE\\flower001.png",	// 夏マナフラワーテクスチャ
	"data\\TEXTURE\\flower002.png",	// 秋マナフラワーテクスチャ
	"data\\TEXTURE\\flower003.png",	// 冬マナフラワーテクスチャ
};
CFlower::StatusInfo CFlower::m_aStatusInfo[TYPE_MAX] = {};	// ステータス情報
int CFlower::m_nNumAll = 0;	// マナフラワーの総数

//************************************************************
//	子クラス [CFlower] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFlower::CFlower() : CObject3D(CObject::LABEL_FLOWER, FLOWER_PRIO)
{
	// メンバ変数をクリア
	m_pShadow = NULL;		// 影の情報
	m_state = STATE_NORMAL;	// 状態
	m_type = TYPE_SPRING;	// 種類
	m_nLife = 0;			// 体力
	m_nCounterState = 0;	// 状態管理カウンター

	// マナフラワーの総数を加算
	m_nNumAll++;
}

//============================================================
//	デストラクタ
//============================================================
CFlower::~CFlower()
{
	// マナフラワーの総数を減算
	m_nNumAll--;
}

//============================================================
//	初期化処理
//============================================================
HRESULT CFlower::Init(void)
{
	// メンバ変数を初期化
	m_pShadow = NULL;		// 影の情報
	m_state = STATE_NORMAL;	// 状態
	m_type = TYPE_SPRING;	// 種類
	m_nLife = 0;			// 体力
	m_nCounterState = 0;	// 状態管理カウンター

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, SHADOW_SIZE, this, SHADOW_ALPHA, SHADOW_ALPHA);
	if (UNUSED(m_pShadow))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// オブジェクト3Dの初期化
	if (FAILED(CObject3D::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CFlower::Uninit(void)
{
	// 影を破棄
	m_pShadow->Uninit();

	// オブジェクト3Dの終了
	CObject3D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CFlower::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// 位置

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:

		// プレイヤーとの当たり判定
		if (CollisionPlayer(pos))
		{ // 体力がなくなった場合

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}

		break;

	case STATE_DAMAGE:

		if (m_nCounterState < NORMAL_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_nCounterState++;

			// 変数を宣言
			D3DXVECTOR3 size = GetScaling();	// マナフラワー大きさ
			float fSizeY = ((m_aStatusInfo[m_type].size.y - (m_aStatusInfo[m_type].size.y * REV_MIN_SIZE)) / NORMAL_CNT) * m_nCounterState + (m_aStatusInfo[m_type].size.y * REV_MIN_SIZE);	// 縦の大きさ

			// 大きさを設定
			SetScaling(D3DXVECTOR3(size.x, fSizeY, size.z));
		}
		else
		{ // カウンターが一定値以上の場合

			// カウンターを初期化
			m_nCounterState = 0;

			// 大きさを初期化
			SetScaling(m_aStatusInfo[m_type].size);

			// 状態を設定
			m_state = STATE_NORMAL;	// 通常状態
		}

		break;

	default:
		assert(false);
		break;
	}

	// 位置を求める
	pos.y = CScene::GetField()->GetPositionHeight(pos);	// 高さを地面に設定

	// 位置を更新
	SetPosition(pos);

	// 影の更新
	m_pShadow->Update();

	// オブジェクト3Dの更新
	CObject3D::Update();
}

//============================================================
//	描画処理
//============================================================
void CFlower::Draw(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// αテストの参照値設定

	// オブジェクト3Dの描画
	CObject3D::Draw();

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//============================================================
//	生成処理
//============================================================
CFlower *CFlower::Create
(
	const TYPE type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rSize,	// 大きさ
	const int nLife				// 体力
)
{
	// 変数を宣言
	D3DXVECTOR3 pos = rPos;		// 座標設定用

	// ポインタを宣言
	CFlower *pFlower = NULL;	// マナフラワー生成用

	if (UNUSED(pFlower))
	{ // 使用されていない場合

		// メモリ確保
		pFlower = new CFlower;	// マナフラワー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pFlower))
	{ // 確保に成功している場合

		// マナフラワーの初期化
		if (FAILED(pFlower->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pFlower;
			pFlower = NULL;

			// 失敗を返す
			return NULL;
		}

		// 原点を設定
		pFlower->SetOrigin(ORIGIN_DOWN);

		// 位置を設定
		pos.y = CScene::GetField()->GetPositionHeight(pos);	// 高さを地面に設定
		pFlower->SetPosition(pos);

		// 向きを設定
		pFlower->SetRotation(rRot);

		// 大きさを設定
		pFlower->SetScaling(rSize);

		// 種類を設定
		pFlower->SetType((int)type);

		// 体力を設定
		pFlower->SetLife(nLife);

		// カリングを設定
		pFlower->SetCulling(D3DCULL_NONE);

		// ライティングを設定
		pFlower->SetLighting(false);

		// Zテストを設定
		pFlower->SetFunc(D3DCMP_LESSEQUAL);

		// Zバッファの使用状況を設定
		pFlower->SetZEnable(true);

		// 確保したアドレスを返す
		return pFlower;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ランダム生成処理
//============================================================
void CFlower::RandomSpawn
(
	const int nNum,	// 生成数
	const TYPE type	// 種類
)
{
	// 変数を宣言
	D3DXVECTOR3 posSet;	// 位置設定用
	D3DXVECTOR3 rotSet;	// 向き設定用
	D3DXVECTOR3 posTarget = CScene::GetStage()->GetStageLimit().center;	// 中心座標
	int nLimit = (int)CScene::GetStage()->GetStageLimit().fRadius;		// ステージ範囲

	for (int nCntGrow = 0; nCntGrow < nNum; nCntGrow++)
	{ // 生成数分繰り返す

		// 生成位置を設定
		posSet.x = (float)(rand() % (nLimit * 2) - nLimit + 1);
		posSet.y = 0.0f;
		posSet.z = (float)(rand() % (nLimit * 2) - nLimit + 1);

		// 生成位置を補正
		collision::CirclePillar(posSet, posTarget, m_aStatusInfo[type].size.x, CScene::GetStage()->GetStageBarrier().fRadius + PREC_PLUS_RADIUS);	// ターゲット内部の生成防止
		CScene::GetStage()->LimitPosition(posSet, m_aStatusInfo[type].size.x);	// ステージ範囲外の生成防止

		// 生成向きを設定
		rotSet = D3DXVECTOR3(0.0f, atan2f(posSet.x - posTarget.x, posSet.z - posTarget.z), 0.0f);

		// マナフラワーオブジェクトの生成
		CFlower::Create(type, posSet, rotSet, m_aStatusInfo[type].size, m_aStatusInfo[type].nLife);
	}
}

//============================================================
//	季節の設定処理
//============================================================
void CFlower::SetSeason(const CWaveManager::SEASON season)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (USED(pObjectTop))
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (USED(pObjCheck))
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_FLOWER)
				{ // オブジェクトラベルがマナフラワーではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 引数の季節を設定
				pObjCheck->SetType((int)season);

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	総数取得処理
//============================================================
int CFlower::GetNumAll(void)
{
	// 現在のマナフラワーの総数を返す
	return m_nNumAll;
}

//============================================================
//	種類の設定処理
//============================================================
void CFlower::SetType(const int nType)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// 引数の種類を設定
	m_type = (TYPE)nType;
	
	// テクスチャを登録・割当
	BindTexture(pTexture->Regist(mc_apTextureFile[m_type]));

	// 大きさを設定
	SetScaling(m_aStatusInfo[m_type].size);

	// 体力を設定
	SetLife(m_aStatusInfo[m_type].nLife);
}

//============================================================
//	体力の設定処理
//============================================================
void CFlower::SetLife(const int nLife)
{
	// 引数の体力を設定
	m_nLife = nLife;
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
bool CFlower::CollisionPlayer(const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bDeath = false;	// 死亡状況

	if (USED(CScene::GetPlayer()))
	{ // プレイヤーが使用されている場合

		// ポインタを宣言
		CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報

		// 変数を宣言
		D3DXVECTOR3 posPlayer = pPlayer->GetPosition();	// プレイヤー位置
		D3DXVECTOR3 sizeFlower = GetScaling();	// マナフラワー大きさ

		if (pPlayer->GetState() == CPlayer::STATE_NORMAL)
		{ // プレイヤーが通常状態の場合

			if (collision::Circle2D(rPos, posPlayer, sizeFlower.x * 0.5f, pPlayer->GetRadius()))
			{ // プレイヤーに当たっていた場合

				// 変数を宣言
				float fCol = 0.0f;	// マナフラワー色

				if (pPlayer->GetMotionType() != CPlayer::MOTION_ACCELE)
				{ // プレイヤーが加速中ではない場合

					// 体力を減算
					m_nLife--;
				}
				else
				{ // プレイヤーが加速中の場合

					// 体力を0にする
					m_nLife = 0;
				}

				// マナフラワーの色を設定
				fCol = (MIN_COL / (float)m_aStatusInfo[m_type].nLife) * m_nLife + fabsf(MIN_COL - 1.0f);

				if (m_nLife > 0)
				{ // 体力が残っていた場合

					// マナフラワーの色を設定
					SetColor(D3DXCOLOR(fCol, fCol, fCol, 1.0f));

					// 状態を設定
					m_state = STATE_DAMAGE;	// ダメージ状態
				}
				else
				{ // 体力がなくなった場合

					// 死亡状態にする
					bDeath = true;
				}

				// パーティクル3Dオブジェクトを生成
				CParticle3D::Create(CParticle3D::TYPE_STOMP_PLANT, GetPosition(), D3DXCOLOR(fCol, fCol, fCol, 1.0f));

				// サウンドの再生
				CManager::GetSound()->Play(CSound::LABEL_SE_STOMP);	// 植物踏みつけ音
			}
		}
	}

	// 死亡状況を返す
	return bDeath;
}

//============================================================
//	セットアップ処理
//============================================================
void CFlower::LoadSetup(void)
{
	// 変数を宣言
	int nType	= 0;	// 種類の代入用
	int nEnd	= 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 静的メンバ変数の情報をクリア
	memset(&m_aStatusInfo[0], 0, sizeof(m_aStatusInfo));	// ステータス情報

	// ファイルを読み込み形式で開く
	pFile = fopen(FLOWER_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// ステータスの設定
			if (strcmp(&aString[0], "STATUSSET") == 0)
			{ // 読み込んだ文字列が STATUSSET の場合

				do
				{ // 読み込んだ文字列が END_STATUSSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "FLOWERSET") == 0)
					{ // 読み込んだ文字列が FLOWERSET の場合

						do
						{ // 読み込んだ文字列が END_FLOWERSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "TYPE") == 0)
							{ // 読み込んだ文字列が TYPE の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nType);		// 種類を読み込む
							}
							else if (strcmp(&aString[0], "LIFE") == 0)
							{ // 読み込んだ文字列が LIFE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nLife);	// 体力を読み込む
							}
							else if (strcmp(&aString[0], "SIZE") == 0)
							{ // 読み込んだ文字列が SIZE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.x);	// 大きさXを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.y);	// 大きさYを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.z);	// 大きさZを読み込む
							}
						} while (strcmp(&aString[0], "END_FLOWERSET") != 0);	// 読み込んだ文字列が END_FLOWERSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);			// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "マナフラワーセットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
