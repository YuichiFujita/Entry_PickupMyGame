//============================================================
//
//	海処理 [sea.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sea.h"
#include "manager.h"
#include "renderer.h"
#include "scrollMeshField.h"
#include "texture.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SEA_PRIO	(0)		// 海の優先順位
#define ADD_ROT		(50)	// 頂点ごとのカーブ加算量
#define ADD_UP		(40.0f)	// 波の最高点

#define ADD_SINROT	(D3DXToRadian(0.5f))	// 毎フレームの波打ち向き加算量

#define SEA_DOWN	(D3DXVECTOR2(-0.0f, 0.002f))	// 海(下)の移動量
#define SEA_UP		(D3DXVECTOR2(-0.0f, 0.0001f))	// 海(上)の移動量

#define SEA_ROT		(D3DXVECTOR3(0.0f, D3DXToRadian(45), 0.0f))	// 向き
#define SEA_SIZE	(D3DXVECTOR2(24000.0f, 24000.0f))			// 大きさ

#define SEA_COL		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))	// 色
#define SEA_PART	(POSGRID2(128, 128))				// 分割数

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CSea::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\sea000.jpg",	// 海(下)
	"data\\TEXTURE\\sea000.png",	// 海(上)
};

//************************************************************
//	子クラス [CSea] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSea::CSea() : CObject(CObject::LABEL_SEA, SEA_PRIO)
{
	// メンバ変数をクリア
	memset(&m_apScrollMeshField[0], 0, sizeof(m_apScrollMeshField));	// スクロールメッシュフィールドの情報
	m_fSinRot = 0.0f;	// 波打ち向き
}

//============================================================
//	デストラクタ
//============================================================
CSea::~CSea()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSea::Init(void)
{
	// 変数を宣言
	int nTextureID;	// テクスチャインデックス

	// 変数配列を宣言
	D3DXVECTOR2 aMoveTexU[TEXTURE_MAX] =	// テクスチャの横移動量の設定用
	{ // 初期値
		SEA_UP,		// 海(下)の移動量
		SEA_DOWN,	// 海(上)の移動量
	};

	D3DXCOLOR aCol[TEXTURE_MAX] =	// 色の設定用
	{ // 初期値
		XCOL_WHITE,	// 海(下)の色
		SEA_COL,	// 海(上)の色
	};

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apScrollMeshField[0], 0, sizeof(m_apScrollMeshField));	// スクロールメッシュフィールドの情報
	m_fSinRot = 0.0f;	// 波打ち向き

	for (int nCntSea = 0; nCntSea < TEXTURE_MAX; nCntSea++)
	{ // テクスチャの最大数分繰り返す

		// スクロールメッシュフィールドの生成
		m_apScrollMeshField[nCntSea] = CScrollMeshField::Create
		( // 引数
			aMoveTexU[nCntSea].x,	// 横座標の移動量
			aMoveTexU[nCntSea].y,	// 縦座標の移動量
			VEC3_ZERO,		// 位置
			SEA_ROT,		// 向き
			SEA_SIZE,		// 大きさ
			aCol[nCntSea],	// 色
			SEA_PART,		// 分割数
			D3DCULL_CCW,	// カリング状況
			false			// ライティング状況
		);
		if (UNUSED(m_apScrollMeshField[nCntSea]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録
		nTextureID = pTexture->Regist(mc_apTextureFile[nCntSea]);

		// テクスチャを割当
		m_apScrollMeshField[nCntSea]->BindTexture(nTextureID);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSea::Uninit(void)
{
	// スクロールメッシュフィールドを破棄
	for (int nCntSea = 0; nCntSea < TEXTURE_MAX; nCntSea++)
	{ // テクスチャの最大数分繰り返す

		// スクロールメッシュフィールドの終了
		m_apScrollMeshField[nCntSea]->Uninit();
	}

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CSea::Update(void)
{
	// 変数を宣言
	POSGRID2 part = m_apScrollMeshField[0]->GetPattern();	// 分割数
	D3DXVECTOR3 pos;	// 頂点座標取得用
	int nLine;			// 頂点の行
	float fSinRot;		// サインカーブに使用する角度

	// メッシュの頂点座標の更新
	for (int nCntVtx = 0; nCntVtx < (part.x + 1) * (part.y + 1); nCntVtx++)
	{ // 使用頂点数分繰り返す

		for (int nCntSea = 0; nCntSea < TEXTURE_MAX; nCntSea++)
		{ // テクスチャの最大数分繰り返す

			// 頂点座標を取得
			pos = m_apScrollMeshField[nCntSea]->GetMeshVertexPosition(nCntVtx);

			// 波打ち用の角度を求める
			nLine = nCntVtx / (part.x + 1);							// 頂点の行
			fSinRot = m_fSinRot + D3DXToRadian(nLine * ADD_ROT);	// サインカーブに使用する角度

			// サインカーブ用の向きを補正
			useful::NormalizeRot(fSinRot);

			// 頂点座標を設定
			m_apScrollMeshField[nCntSea]->SetMeshVertexPosition(nCntVtx, D3DXVECTOR3(pos.x, (sinf(fSinRot) * ADD_UP) + ADD_UP, pos.z));
		}
	}

	// 波打ち向きを加算
	m_fSinRot += ADD_SINROT;

	// 向きを補正
	useful::NormalizeRot(m_fSinRot);

	// スクロールの更新
	for (int nCntSea = 0; nCntSea < TEXTURE_MAX; nCntSea++)
	{ // テクスチャの最大数分繰り返す

		// スクロールメッシュフィールドの更新
		m_apScrollMeshField[nCntSea]->Update();
	}
}

//============================================================
//	描画処理
//============================================================
void CSea::Draw(void)
{

}

//============================================================
//	生成処理
//============================================================
CSea *CSea::Create(void)
{
	// ポインタを宣言
	CSea *pSea = NULL;	// 海生成用

	if (UNUSED(pSea))
	{ // 使用されていない場合

		// メモリ確保
		pSea = new CSea;	// 海
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pSea))
	{ // 確保に成功している場合

		// 海の初期化
		if (FAILED(pSea->Init()))
		{ // 初期化に失敗した場合

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pSea;
	}
	else { assert(false); return NULL; }	// 確保失敗
}
