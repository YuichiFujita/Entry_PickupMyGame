//============================================================
//
//	ハリケーン処理 [hurricane.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "hurricane.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "collision.h"
#include "sceneGame.h"
#include "target.h"

//************************************************************
//	マクロ定義
//************************************************************
#define HURRICANE_COL	(D3DXCOLOR(0.6f, 0.9f, 1.0f, 0.6f))	// ハリケーンのポリゴン色
#define HURRICANE_PRIO	(5)	// ハリケーンの優先順位

#define HURRICANE_NUMAROUND		(4)			// ハリケーンの渦の周回数
#define HURRICANE_PATTERN		(32)		// ハリケーンの渦の分割数
#define HURRICANE_MOVEROT		(-0.4f)		// ハリケーンの向きの変更量
#define HURRICANE_THICKNESS		(22.5f)		// ハリケーンのポリゴンの太さ
#define HURRICANE_OUTERPLUS		(3.5f)		// ハリケーンのポリゴン外周のY座標加算量
#define HURRICANE_SETWIDTH		(100.0f)	// ハリケーンの生成時の横ずれ量
#define HURRICANE_SETALPHA		(0.8f)		// ハリケーンの生成時の透明度
#define HURRICANE_ADDWIDTH		(3.0f)		// ハリケーンの横ずれの加算量
#define HURRICANE_ADDHEIGHT		(3.8f)		// ハリケーンの縦ずれの加算量
#define HURRICANE_SUBALPHA		(0.01f)		// ハリケーンの透明度の減算量
#define HURRICANE_GROWWIDTH		(0.0f)		// ハリケーンの横ずれの成長量
#define HURRICANE_GROWHEIGHT	(0.0f)		// ハリケーンの縦ずれの成長量
#define HURRICANE_GROWALPHA		(0.0f)		// ハリケーンの透明度の成長量

//************************************************************
//	子クラス [CHurricane] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CHurricane::CHurricane() : CObjectTornado(CObject::LABEL_NONE, HURRICANE_PRIO)
{
	// メンバ変数をクリア
	m_state = STATE_NORMAL;	// 状態
}

//============================================================
//	デストラクタ
//============================================================
CHurricane::~CHurricane()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CHurricane::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_NORMAL;	// 状態

	// 竜巻の初期化
	if (FAILED(CObjectTornado::Init()))
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
void CHurricane::Uninit(void)
{
	// 竜巻の終了
	CObjectTornado::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CHurricane::Update(void)
{
	// 竜巻の更新
	CObjectTornado::Update();
}

//============================================================
//	描画処理
//============================================================
void CHurricane::Draw(void)
{
	// 竜巻の描画
	CObjectTornado::Draw();
}

//============================================================
//	生成処理
//============================================================
CHurricane *CHurricane::Create(const D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	CHurricane *pHurricane = NULL;	// ハリケーン生成用

	if (UNUSED(pHurricane))
	{ // 使用されていない場合

		// メモリ確保
		pHurricane = new CHurricane;	// ハリケーン
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pHurricane))
	{ // 確保に成功している場合

		// ハリケーンの初期化
		if (FAILED(pHurricane->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pHurricane;
			pHurricane = NULL;

			// 失敗を返す
			return NULL;
		}

		// テクスチャを割当
		pHurricane->BindTexture(NONE_IDX);

		// 位置を設定
		pHurricane->SetPosition(rPos);

		// 向きを設定
		pHurricane->SetDirectionRotation(VEC3_ZERO);

		// 色を設定
		pHurricane->SetColor(HURRICANE_COL);

		// 親のマトリックスを設定
		pHurricane->SetMatrixParent(NULL);

		// ポリゴンの太さを設定
		pHurricane->SetThickness(HURRICANE_THICKNESS);

		// ポリゴン外周のy座標加算量を設定
		pHurricane->SetOuterPlus(HURRICANE_OUTERPLUS);

		// 生成時の横ずれ量を設定
		pHurricane->SetWidth(HURRICANE_SETWIDTH);

		// 生成時の透明度を設定
		pHurricane->SetAlpha(HURRICANE_SETALPHA);

		// 横ずれの加算量を設定
		pHurricane->SetAddWidth(HURRICANE_ADDWIDTH);

		// 縦ずれの加算量を設定
		pHurricane->SetAddHeight(HURRICANE_ADDHEIGHT);

		// 透明度の減算量を設定
		pHurricane->SetSubAlpha(HURRICANE_SUBALPHA);

		// カリングを設定
		pHurricane->SetCulling(D3DCULL_NONE);

		// ライティングを設定
		pHurricane->SetLighting(false);

		// 成長性を設定
		pHurricane->SetGrow(HURRICANE_MOVEROT, HURRICANE_GROWWIDTH, HURRICANE_GROWHEIGHT, HURRICANE_GROWALPHA);

		// 渦を設定
		if (FAILED(pHurricane->SetVortex(HURRICANE_NUMAROUND, HURRICANE_PATTERN)))
		{ // 渦の設定に失敗した場合

			// メモリ開放
			delete pHurricane;
			pHurricane = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pHurricane;
	}
	else { assert(false); return NULL; }	// 確保失敗
}
