//============================================================
//
//	ステージ処理 [stage.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "stage.h"
#include "manager.h"
#include "texture.h"
#include "scene.h"
#include "sceneGame.h"
#include "player.h"
#include "target.h"
#include "field.h"
#include "hurricane.h"
#include "object3D.h"
#include "collision.h"
#include "wind.h"

//************************************************************
//	マクロ定義
//************************************************************
#define STAGE_SETUP_TXT	"data\\TXT\\stage.txt"	// セットアップテキスト相対パス

#define AREA_PRIO	(2)			// エリア表示の優先順位
#define AREA_ROT	(0.025f)	// エリアの回転量
#define AREA_SUBROT	(0.003f)	// エリアの回転減算量

#define WIND_POS	(3000.0f)	// 風生成位置
#define WIND_CNT	(60)		// 風生成カウント
#define WIND_SPAWN	(4)			// 風生成数

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CStage::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\area002.png",	// バリア表示テクスチャ
	"data\\TEXTURE\\area000.png",	// エリア表示テクスチャ
};

//************************************************************
//	親クラス [CStage] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CStage::CStage()
{
	// メンバ変数をクリア
	m_pHurricane = NULL;	// ハリケーンの情報
	m_pStageBarrier = NULL;	// バリア表示の情報
	m_pStageArea = NULL;	// ステージエリア表示の情報
	m_area = AREA_NONE;		// プレイヤーの現在エリア
	m_bWind = false;		// 風生成状況
	memset(&m_stageWind, 0, sizeof(m_stageWind));		// 風速
	memset(&m_stageBarrier, 0, sizeof(m_stageBarrier));	// バリア
	memset(&m_aStageArea[0], 0, sizeof(m_aStageArea));	// エリア
	memset(&m_stageLimit, 0, sizeof(m_stageLimit));		// 範囲
}

//============================================================
//	デストラクタ
//============================================================
CStage::~CStage()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CStage::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_pHurricane = NULL;	// ハリケーンの情報
	m_pStageBarrier = NULL;	// バリア表示の情報
	m_pStageArea = NULL;	// ステージエリア表示の情報
	m_area = AREA_NONE;		// プレイヤーの現在エリア
	m_bWind = true;			// 風生成状況
	memset(&m_stageWind, 0, sizeof(m_stageWind));		// 風速
	memset(&m_stageBarrier, 0, sizeof(m_stageBarrier));	// バリア
	memset(&m_aStageArea[0], 0, sizeof(m_aStageArea));	// エリア
	memset(&m_stageLimit, 0, sizeof(m_stageLimit));		// 範囲

	// ハリケーンの生成
	m_pHurricane = CHurricane::Create(VEC3_ZERO);
	if (UNUSED(m_pHurricane))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// ステージエリア表示の生成
	m_pStageArea = CObject3D::Create
	( // 引数
		VEC3_ZERO,					// 位置
		VEC3_ZERO,					// 大きさ
		VEC3_ZERO,					// 向き
		XCOL_WHITE,					// 色
		CObject3D::ORIGIN_CENTER,	// 原点
		D3DCULL_CCW,				// カリング状況
		false,						// ライティング状況
		D3DCMP_ALWAYS,				// Zテスト設定
		false						// Zバッファの使用状況
	);
	if (UNUSED(m_pStageArea))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pStageArea->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_AREA]));

	// 優先順位を設定
	m_pStageArea->SetPriority(AREA_PRIO);

	// ステージバリア表示の生成
	m_pStageBarrier = CObject3D::Create
	( // 引数
		VEC3_ZERO,					// 位置
		VEC3_ZERO,					// 大きさ
		VEC3_ZERO,					// 向き
		XCOL_WHITE,					// 色
		CObject3D::ORIGIN_CENTER,	// 原点
		D3DCULL_CCW,				// カリング状況
		false,						// ライティング状況
		D3DCMP_ALWAYS,				// Zテスト設定
		false						// Zバッファの使用状況
	);
	if (UNUSED(m_pStageBarrier))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pStageBarrier->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_BARRIER]));

	// 優先順位を設定
	m_pStageBarrier->SetPriority(AREA_PRIO);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CStage::Uninit(void)
{
	// ハリケーンの終了
	m_pHurricane->Uninit();

	// ステージバリア表示の終了
	m_pStageBarrier->Uninit();

	// ステージエリア表示の終了
	m_pStageArea->Uninit();
}

//============================================================
//	更新処理
//============================================================
void CStage::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 posCenter;	// 世界の中心位置
	D3DXVECTOR3 rotBarrier = m_pStageBarrier->GetRotation();	// バリア表示向き

	// 世界の中心位置を設定
	posCenter = m_stageLimit.center;	// 中心座標を設定
	posCenter.y = CScene::GetField()->GetPosition().y;	// 地面の縦位置を設定

	//--------------------------------------------------------
	//	ハリケーンの更新
	//--------------------------------------------------------
	// ハリケーンの位置を設定
	m_pHurricane->SetPosition(posCenter);

	// ハリケーンの更新
	m_pHurricane->Update();

	//--------------------------------------------------------
	//	バリア表示の更新
	//--------------------------------------------------------
	// バリア表示の位置を設定
	m_pStageBarrier->SetPosition(posCenter);

	// バリア表示の向きを設定
	rotBarrier.y -= AREA_ROT;
	m_pStageBarrier->SetRotation(rotBarrier);

	//--------------------------------------------------------
	//	風の生成
	//--------------------------------------------------------
	if (m_bWind)
	{ // 風を生成する場合

		if (m_stageWind.nCounter < WIND_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_stageWind.nCounter++;
		}
		else
		{ // カウンターが一定値以上の場合

			// 変数を宣言
			D3DXVECTOR3 spawnPos;	// 生成位置
			float fRot = (float)(rand() % 629 - 314) * 0.01f;	// 向き

			// カウンターを初期化
			m_stageWind.nCounter = 0;

			for (int nCntWind = 0; nCntWind < WIND_SPAWN; nCntWind++)
			{ // 風の生成数分繰り返す

				// 生成位置を設定
				spawnPos.x = sinf(fRot + ((D3DX_PI * 0.5f) * nCntWind)) * WIND_POS;
				spawnPos.y = 0.0f;
				spawnPos.z = cosf(fRot + ((D3DX_PI * 0.5f) * nCntWind)) * WIND_POS;

				// 風の生成
				CWind::Create(spawnPos);
			}
		}
	}

	//--------------------------------------------------------
	//	エリア表示の更新
	//--------------------------------------------------------
	if (USED(CScene::GetPlayer()) && USED(CScene::GetTarget()))
	{ // プレイヤー・ターゲットが使用されている場合

		// 変数を宣言
		D3DXVECTOR3 posPlayer = CScene::GetPlayer()->GetPosition();	// プレイヤー位置
		D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
		float fRadiusPlayer = CScene::GetPlayer()->GetRadius();		// プレイヤー半径

		// 現在のエリアを初期化
		m_area = AREA_NONE;

		for (int nCntArea = 0; nCntArea < AREA_MAX; nCntArea++)
		{ // エリアの最大数分繰り返す

			if (collision::Circle2D(posPlayer, posTarget, fRadiusPlayer, m_aStageArea[nCntArea].fRadius))
			{ // エリア内の場合

				// エリア表示の大きさを設定
				m_pStageArea->SetScaling(D3DXVECTOR3(m_aStageArea[nCntArea].fRadius * 2.0f, 0.0f, m_aStageArea[nCntArea].fRadius * 2.0f));

				// エリア表示の色を設定
				m_pStageArea->SetColor(m_aStageArea[nCntArea].col);

				// 現在のエリアを設定
				m_area = (AREA)nCntArea;

				// 処理を抜ける
				break;
			}
		}

		// エリア表示の位置を設定
		m_pStageArea->SetPosition(posCenter);

		// エリア表示の向きを設定
		m_pStageArea->SetRotation(D3DXVECTOR3(0.0f, atan2f(posPlayer.x - posTarget.x, posPlayer.z - posTarget.z), 0.0f));

		// 例外処理
		assert(m_area != AREA_NONE);	// エリア外
	}
}

//============================================================
//	バリアとの当たり判定
//============================================================
bool CStage::CollisionBarrier(D3DXVECTOR3& rPos, float fRadius)
{
	// 変数を宣言
	bool bHit = false;	// 判定確認用

	// 円の当たり判定
	bHit = collision::Circle2D
	( // 引数
		rPos,							// 判定位置
		m_pStageBarrier->GetPosition(),	// 判定目標位置
		fRadius,						// 判定半径
		m_stageBarrier.fRadius			// 判定目標半径
	);

	// 判定状況を返す
	return bHit;
}

//============================================================
//	位置の補正処理
//============================================================
void CStage::LimitPosition(D3DXVECTOR3& rPos, const float fRadius)
{
	switch (m_stageLimit.mode)
	{ // 制限モードごとの処理
	case LIMIT_BOX:		// 矩形範囲

		// 角柱の内側制限
		collision::InBoxPillar
		( // 引数
			rPos,				// 判定位置
			VEC3_ZERO,			// 判定原点位置
			VEC3_ALL(fRadius),	// 判定サイズ(右・上・後)
			VEC3_ALL(fRadius),	// 判定サイズ(左・下・前)
			D3DXVECTOR3(fabsf(m_stageLimit.fRight), 0.0f, fabsf(m_stageLimit.fFar)),	// 判定原点サイズ(右・上・後)
			D3DXVECTOR3(fabsf(m_stageLimit.fLeft),  0.0f, fabsf(m_stageLimit.fNear))	// 判定原点サイズ(左・下・前)
		);

		break;

	case LIMIT_CIRCLE:	// 円範囲

		// 円柱の内側制限
		collision::InCirclePillar
		( // 引数
			rPos,					// 判定位置
			VEC3_ZERO,				// 判定原点位置
			fRadius,				// 判定半径
			m_stageLimit.fRadius	// 判定原点半径
		);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	範囲外の着地処理
//============================================================
bool CStage::LandPosition(D3DXVECTOR3& rPos, D3DXVECTOR3& rMove, const float fHeight)
{
	if (rPos.y - fHeight < m_stageLimit.fField)
	{ // 位置が地面より下の場合

		// 位置を補正
		rPos.y = m_stageLimit.fField + fHeight;

		// 移動量を初期化
		rMove.y = 0.0f;

		// 着地している状況を返す
		return true;
	}

	// 着地していない状況を返す
	return false;
}

//============================================================
//	ステージ範囲の設定処理
//============================================================
void CStage::SetStageLimit(const StageLimit& rLimit)
{
	// 引数のステージ範囲を設定
	m_stageLimit = rLimit;
}

//============================================================
//	ステージ範囲取得処理
//============================================================
CStage::StageLimit CStage::GetStageLimit(void) const
{
	// ステージ範囲を返す
	return m_stageLimit;
}

//============================================================
//	ステージエリアの設定処理
//============================================================
void CStage::SetStageArea(const int nID, const StageArea& rArea)
{
	// 引数インデックスのステージエリアを設定
	m_aStageArea[nID] = rArea;
}

//============================================================
//	ステージエリア取得処理
//============================================================
CStage::StageArea CStage::GetStageArea(const int nID) const
{
	// 引数インデックスのステージエリアを返す
	return m_aStageArea[nID];
}

//============================================================
//	ステージエリアの描画設定処理
//============================================================
void CStage::SetEnableDrawArea(const bool bDraw)
{
	// 引数の描画状況をステージエリアに設定
	m_pStageArea->SetEnableDraw(bDraw);
}

//============================================================
//	プレイヤーの現在エリア取得処理
//============================================================
CStage::AREA CStage::GetAreaPlayer(void) const
{
	// プレイヤーの現在エリアを返す
	return m_area;
}

//============================================================
//	ステージバリアの設定処理
//============================================================
void CStage::SetStageBarrier(const StageArea& rBarrier)
{
	// 引数のステージバリアを設定
	m_stageBarrier = rBarrier;

	// エリア表示の大きさを設定
	m_pStageBarrier->SetScaling(D3DXVECTOR3(m_stageBarrier.fRadius * 2.0f, 0.0f, m_stageBarrier.fRadius * 2.0f));

	// エリア表示の色を設定
	m_pStageBarrier->SetColor(m_stageBarrier.col);
}

//============================================================
//	ステージバリア取得処理
//============================================================
CStage::StageArea CStage::GetStageBarrier(void) const
{
	// ステージバリアを返す
	return m_stageBarrier;
}

//============================================================
//	ステージバリアの描画設定処理
//============================================================
void CStage::SetEnableDrawBarrier(const bool bDraw)
{
	// 引数の描画状況をステージバリアに設定
	m_pStageBarrier->SetEnableDraw(bDraw);
}

//============================================================
//	ステージバリアの位置取得処理
//============================================================
D3DXVECTOR3 CStage::GetStageBarrierPosition(void) const
{
	// バリアの位置を返す
	return m_pStageBarrier->GetPosition();
}

//============================================================
//	ハリケーンの描画設定処理
//============================================================
void CStage::SetEnableDrawHurricane(const bool bDraw)
{
	// 引数の描画状況をハリケーンに設定
	m_pHurricane->SetEnableDraw(bDraw);
}

//============================================================
//	風の生成設定処理
//============================================================
void CStage::SetEnebleCreateWind(const bool bCreate)
{
	// 引数の生成状況を設定
	m_bWind = bCreate;
}

//============================================================
//	生成処理
//============================================================
CStage *CStage::Create(void)
{
	// ポインタを宣言
	CStage *pStage = NULL;		// ステージ生成用

	if (UNUSED(pStage))
	{ // 使用されていない場合

		// メモリ確保
		pStage = new CStage;	// ステージ
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pStage))
	{ // 確保に成功している場合

		// ステージの初期化
		if (FAILED(pStage->Init()))
		{ // 初期化に失敗した場合

			// 失敗を返す
			return NULL;
		}

		// セットアップの読み込み
		LoadSetup(pStage);

		// 確保したアドレスを返す
		return pStage;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CStage::Release(CStage *&prStage)
{
	if (USED(prStage))
	{ // 使用中の場合

		// ステージの終了
		prStage->Uninit();

		// メモリ開放
		delete prStage;
		prStage = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	セットアップ処理
//============================================================
void CStage::LoadSetup(CStage *pStage)
{
	// 変数を宣言
	StageArea stageBarrier;	// ステージバリアの代入用
	StageArea stageArea;	// ステージエリアの代入用
	StageLimit stageLimit;	// ステージ範囲の代入用
	int nArea = 0;			// エリアの読み込み数
	int nEnd = 0;			// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// ファイルを読み込み形式で開く
	pFile = fopen(STAGE_SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// ステージ範囲の設定
			if (strcmp(&aString[0], "LIMITSET") == 0)
			{ // 読み込んだ文字列が LIMITSET の場合

				do
				{ // 読み込んだ文字列が END_LIMITSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "CENTER") == 0)
					{ // 読み込んだ文字列が CENTER の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.center.x);	// 中心座標Xを読み込む
						fscanf(pFile, "%f", &stageLimit.center.y);	// 中心座標Yを読み込む
						fscanf(pFile, "%f", &stageLimit.center.z);	// 中心座標Zを読み込む
					}
					else if (strcmp(&aString[0], "NEAR") == 0)
					{ // 読み込んだ文字列が NEAR の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fNear);		// 前位置を読み込む

						// 制限モードを矩形範囲に設定
						stageLimit.mode = LIMIT_BOX;
					}
					else if (strcmp(&aString[0], "FAR") == 0)
					{ // 読み込んだ文字列が FAR の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fFar);		// 後位置を読み込む

						// 制限モードを矩形範囲に設定
						stageLimit.mode = LIMIT_BOX;
					}
					else if (strcmp(&aString[0], "RIGHT") == 0)
					{ // 読み込んだ文字列が RIGHT の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fRight);	// 右位置を読み込む

						// 制限モードを矩形範囲に設定
						stageLimit.mode = LIMIT_BOX;
					}
					else if (strcmp(&aString[0], "LEFT") == 0)
					{ // 読み込んだ文字列が LEFT の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fLeft);		// 左位置を読み込む

						// 制限モードを矩形範囲に設定
						stageLimit.mode = LIMIT_BOX;
					}
					else if (strcmp(&aString[0], "RADIUS") == 0)
					{ // 読み込んだ文字列が RADIUS の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fRadius);	// 半径を読み込む

						// 制限モードを円範囲に設定
						stageLimit.mode = LIMIT_CIRCLE;
					}
					else if (strcmp(&aString[0], "FIELD") == 0)
					{ // 読み込んだ文字列が FIELD の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageLimit.fField);	// 地面位置を読み込む
					}
				} while (strcmp(&aString[0], "END_LIMITSET") != 0);	// 読み込んだ文字列が END_LIMITSET ではない場合ループ

				// ステージ範囲の設定
				pStage->SetStageLimit(stageLimit);

				// 例外処理
				assert(stageLimit.mode == CStage::LIMIT_CIRCLE);	// 範囲制限エラー
			}

			// ステージバリアの設定
			else if (strcmp(&aString[0], "BARRIERSET") == 0)
			{ // 読み込んだ文字列が BARRIERSET の場合

				do
				{ // 読み込んだ文字列が END_BARRIERSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "COL") == 0)
					{ // 読み込んだ文字列が COL の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageBarrier.col.r);	// 赤色を読み込む
						fscanf(pFile, "%f", &stageBarrier.col.g);	// 緑色を読み込む
						fscanf(pFile, "%f", &stageBarrier.col.b);	// 青色を読み込む
						fscanf(pFile, "%f", &stageBarrier.col.a);	// α値を読み込む
					}
					else if (strcmp(&aString[0], "RADIUS") == 0)
					{ // 読み込んだ文字列が RADIUS の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%f", &stageBarrier.fRadius);	// 半径を読み込む
					}
				} while (strcmp(&aString[0], "END_BARRIERSET") != 0);	// 読み込んだ文字列が END_BARRIERSET ではない場合ループ

				// ステージバリアの設定
				pStage->SetStageBarrier(stageBarrier);
			}

			// ステージエリアの設定
			else if (strcmp(&aString[0], "AREASET") == 0)
			{ // 読み込んだ文字列が AREASET の場合

				do
				{ // 読み込んだ文字列が END_AREASET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "AREA") == 0)
					{ // 読み込んだ文字列が AREA の場合

						do
						{ // 読み込んだ文字列が END_AREA ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "COL") == 0)
							{ // 読み込んだ文字列が COL の場合

								fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
								fscanf(pFile, "%f", &stageArea.col.r);		// 赤色を読み込む
								fscanf(pFile, "%f", &stageArea.col.g);		// 緑色を読み込む
								fscanf(pFile, "%f", &stageArea.col.b);		// 青色を読み込む
								fscanf(pFile, "%f", &stageArea.col.a);		// α値を読み込む
							}
							else if (strcmp(&aString[0], "RADIUS") == 0)
							{ // 読み込んだ文字列が RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
								fscanf(pFile, "%f", &stageArea.fRadius);	// 半径を読み込む
							}
						} while (strcmp(&aString[0], "END_AREA") != 0);	// 読み込んだ文字列が END_AREA ではない場合ループ

						// 例外処理
						assert(nArea < AREA_MAX);	// エリア数オーバー

						// ステージエリアの設定
						pStage->SetStageArea(nArea, stageArea);

						// エリア数を加算
						nArea++;
					}
				} while (strcmp(&aString[0], "END_AREASET") != 0);	// 読み込んだ文字列が END_AREASET ではない場合ループ

				// 例外処理
				assert(nArea == AREA_MAX);	// エリア未設定
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ステージセットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
