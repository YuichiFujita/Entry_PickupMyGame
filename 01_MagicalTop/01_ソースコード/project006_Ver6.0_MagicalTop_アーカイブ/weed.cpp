//============================================================
//
//	草処理 [weed.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "weed.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "texture.h"

#include "collision.h"
#include "player.h"
#include "target.h"
#include "shadow.h"
#include "field.h"
#include "stage.h"

//************************************************************
//	マクロ定義
//************************************************************
#define WEED_SETUP_TXT	"data\\TXT\\weed.txt"	// セットアップテキスト相対パス
#define WEED_PRIO	(2)	// 草の優先順位

#define SHADOW_SIZE	(D3DXVECTOR3(80.0f, 0.0f, 80.0f))	// 影の大きさ
#define SHADOW_ALPHA	(0.2f)	// 影のα値

#define ADD_ROT		(0.2f)	// なびき向きの加算量
#define RANGE		(15.0f)	// なびき量の範囲
#define SUB_RANGE	(25.0f)	// なびき向きの減算量

#define PREC_PLUS_RADIUS	(80.0f)		// 生成制限の半径加算量
#define PREC_IN_RADIUS		(3000.0f)	// 生成制限の最大半径

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CWeed::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\weed000.png",	// 春草テクスチャ
	"data\\TEXTURE\\weed001.png",	// 夏草テクスチャ
	"data\\TEXTURE\\weed002.png",	// 秋草テクスチャ
	"data\\TEXTURE\\weed003.png",	// 冬草テクスチャ
};
CWeed::StatusInfo CWeed::m_aStatusInfo[TYPE_MAX] = {};	// ステータス情報

//************************************************************
//	子クラス [CWeed] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWeed::CWeed(void) : CObject3D(CObject::LABEL_WEED, WEED_PRIO)
{
	// メンバ変数をクリア
	m_pShadow = NULL;		// 影の情報
	m_type = TYPE_SPRING;	// 種類
	m_fSinRot = 0.0f;		// なびき向き
}

//============================================================
//	デストラクタ
//============================================================
CWeed::~CWeed()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWeed::Init(void)
{
	// メンバ変数を初期化
	m_pShadow = NULL;		// 影の情報
	m_type = TYPE_SPRING;	// 種類
	m_fSinRot = (float)(rand() % 629 - 314) * 0.01f;	// なびき向き

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
void CWeed::Uninit(void)
{
	// 影を破棄
	m_pShadow->Uninit();

	// オブジェクト3Dの終了
	CObject3D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWeed::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// 位置

	// なびき向きを加算
	m_fSinRot += ADD_ROT;
	useful::NormalizeRot(m_fSinRot);	// 向き正規化

	// 頂点のずれ量を設定
	SetGapPosition(0, D3DXVECTOR3(0.0f, 0.0f, sinf(m_fSinRot) * RANGE - SUB_RANGE));
	SetGapPosition(1, D3DXVECTOR3(0.0f, 0.0f, sinf(m_fSinRot) * RANGE - SUB_RANGE));

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
void CWeed::Draw(void)
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
CWeed *CWeed::Create
(
	const TYPE type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rSize	// 大きさ
)
{
	// 変数を宣言
	D3DXVECTOR3 pos = rPos;	// 座標設定用

	// ポインタを宣言
	CWeed *pWeed = NULL;	// 草生成用

	if (UNUSED(pWeed))
	{ // 使用されていない場合

		// メモリ確保
		pWeed = new CWeed;	// 草
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pWeed))
	{ // 確保に成功している場合

		// 草の初期化
		if (FAILED(pWeed->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pWeed;
			pWeed = NULL;

			// 失敗を返す
			return NULL;
		}

		// 原点を設定
		pWeed->SetOrigin(ORIGIN_DOWN);

		// 位置を設定
		pos.y = CScene::GetField()->GetPositionHeight(pos);	// 高さを地面に設定
		pWeed->SetPosition(pos);

		// 向きを設定
		pWeed->SetRotation(rRot);

		// 大きさを設定
		pWeed->SetScaling(rSize);

		// 種類を設定
		pWeed->SetType((int)type);

		// カリングを設定
		pWeed->SetCulling(D3DCULL_NONE);

		// ライティングを設定
		pWeed->SetLighting(false);

		// Zテストを設定
		pWeed->SetFunc(D3DCMP_LESSEQUAL);

		// Zバッファの使用状況を設定
		pWeed->SetZEnable(true);

		// 確保したアドレスを返す
		return pWeed;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ランダム生成処理
//============================================================
void CWeed::RandomSpawn
(
	const int nNum,	// 生成数
	const TYPE type	// 種類
)
{
	// 変数を宣言
	D3DXVECTOR3 posSet;	// 位置設定用
	D3DXVECTOR3 rotSet;	// 向き設定用
	D3DXVECTOR3 posCenter = CScene::GetStage()->GetStageLimit().center;	// 中心座標

	for (int nCntGrow = 0; nCntGrow < nNum; nCntGrow++)
	{ // 生成数分繰り返す

		// 生成位置を設定
		posSet.x = (float)(rand() % ((int)PREC_IN_RADIUS * 2) - (int)PREC_IN_RADIUS + 1);
		posSet.y = 0.0f;
		posSet.z = (float)(rand() % ((int)PREC_IN_RADIUS * 2) - (int)PREC_IN_RADIUS + 1);

		// 生成位置を補正
		collision::CirclePillar(posSet, posCenter, m_aStatusInfo[type].size.x, CScene::GetStage()->GetStageBarrier().fRadius + PREC_PLUS_RADIUS);	// ターゲット内部の生成防止
		collision::InCirclePillar(posSet, posCenter, m_aStatusInfo[type].size.x, PREC_IN_RADIUS);	// 範囲外の生成防止

		// 生成向きを設定
		rotSet = D3DXVECTOR3(0.0f, atan2f(posSet.x - posCenter.x, posSet.z - posCenter.z), 0.0f);

		// 草オブジェクトの生成
		CWeed::Create(type, posSet, rotSet, m_aStatusInfo[type].size);
	}
}

//============================================================
//	季節の設定処理
//============================================================
void CWeed::SetSeason(const CWaveManager::SEASON season)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_WEED)
				{ // オブジェクトラベルが草ではない場合

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
//	種類の設定処理
//============================================================
void CWeed::SetType(const int nType)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// 引数の種類を設定
	m_type = (TYPE)nType;
	
	// テクスチャを登録・割当
	BindTexture(pTexture->Regist(mc_apTextureFile[m_type]));

	// 大きさを設定
	SetScaling(m_aStatusInfo[m_type].size);
}

//============================================================
//	セットアップ処理
//============================================================
void CWeed::LoadSetup(void)
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
	pFile = fopen(WEED_SETUP_TXT, "r");

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

					if (strcmp(&aString[0], "WEEDSET") == 0)
					{ // 読み込んだ文字列が WEEDSET の場合

						do
						{ // 読み込んだ文字列が END_WEEDSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "TYPE") == 0)
							{ // 読み込んだ文字列が TYPE の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nType);		// 種類を読み込む
							}
							else if (strcmp(&aString[0], "SIZE") == 0)
							{ // 読み込んだ文字列が SIZE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.x);	// 大きさXを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.y);	// 大きさYを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].size.z);	// 大きさZを読み込む
							}
						} while (strcmp(&aString[0], "END_WEEDSET") != 0);	// 読み込んだ文字列が END_WEEDSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);		// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "草セットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
