//============================================================
//
//	ショップ処理 [shop.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "shop.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "anim2D.h"
#include "valueUI.h"
#include "multiValue.h"

//************************************************************
//	マクロ定義
//************************************************************
#define PTRN_WIDTH	(5)	// テクスチャの横分割数
#define PTRN_HEIGHT	(2)	// テクスチャの縦分割数

#define ICON_SIZE	(D3DXVECTOR3(120.0f, 120.0f, 0.0f))	// アイコンの大きさ
#define DEFAULT_COL	(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f))	// 非選択時の色

#define EXPLAIN_SIZE	(D3DXVECTOR3(320.0f, 270.0f, 0.0f))	// 説明の大きさ
#define ADD_POS_EXPLAIN	(D3DXVECTOR3(0.0f, 210.0f, 0.0f))	// 説明の表示位置の相対座標への加算量

#define LV_VALUE_DIGIT	(2)	// レベルの数字の桁数
#define LV_SPACE		(D3DXVECTOR3(114.0f, -3.0f, 0.0f))	// レベルの行間
#define LV_TITLE_SIZE	(D3DXVECTOR3(180.0f, 50.0f, 0.0f))	// レベルのタイトル大きさ
#define LV_VALUE_SIZE	(D3DXVECTOR3(50.0f, 50.0f, 0.0f))	// レベルの数字大きさ
#define LV_VALUE_SPACE	(D3DXVECTOR3(38.0f, 0.0f, 0.0f))	// レベルの数字の行間
#define ADD_POS_LEVEL	(D3DXVECTOR3(-40.0f, 130.0f, 0.0f))	// レベルの表示位置の相対座標への加算量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CShop::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\shop003.png",	// 必要レベルテクスチャ
	"data\\TEXTURE\\shop004.png",	// アイコンテクスチャ
	"data\\TEXTURE\\shop005.png",	// 説明テクスチャ
	"data\\TEXTURE\\shop006.png",	// 売り切れアイコンテクスチャ
	"data\\TEXTURE\\shop007.png",	// 売り切れ説明テクスチャ
};

const int CShop::mc_aNeedLevel[] =	// 購入必要レベル定数
{
	1,	// 回復
	1,	// 体力レベルアップ
	1,	// マナレベルアップ
	1,	// ダッシュレベルアップ
	2,	// 防御力レベルアップ
	2,	// 素早さレベルアップ
	6,	// 魔法発射数レベルアップ
	3,	// 魔法弾速レベルアップ
	3,	// 魔法連射速度レベルアップ
	2,	// 獲得経験値増加
};

//************************************************************
//	子クラス [CShop] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CShop::CShop() : CObject(CObject::LABEL_NONE, DEFAULT_PRIO)
{
	// メンバ変数をクリア
	m_pIcon = NULL;		// 購入品アイコン
	m_pExplain = NULL;	// 購入品説明
	m_pLv = NULL;		// 必要レベル
	m_pos = VEC3_ZERO;	// 位置
	m_col = XCOL_WHITE;	// 色
	m_buy = BUY_HEAL;	// 購入品
}

//============================================================
//	デストラクタ
//============================================================
CShop::~CShop()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CShop::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャ

	// メンバ変数を初期化
	m_pIcon = NULL;		// 購入品アイコン
	m_pLv = NULL;		// 必要レベル
	m_pos = VEC3_ZERO;	// 位置
	m_col = XCOL_WHITE;	// 色
	m_buy = BUY_HEAL;	// 購入品

	//--------------------------------------------------------
	//	購入品アイコンの生成・設定
	//--------------------------------------------------------
	// 購入品アイコンの生成
	m_pIcon = CAnim2D::Create
	( // 引数
		PTRN_WIDTH,		// 横分割数
		PTRN_HEIGHT,	// 縦分割数
		VEC3_ZERO,		// 位置
		ICON_SIZE,		// 大きさ
		VEC3_ZERO,		// 向き
		DEFAULT_COL		// 色
	);
	if (UNUSED(m_pIcon))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	購入品説明の生成・設定
	//--------------------------------------------------------
	// 購入品説明の生成
	m_pExplain = CAnim2D::Create
	( // 引数
		PTRN_WIDTH,		// 横分割数
		PTRN_HEIGHT,	// 縦分割数
		VEC3_ZERO,		// 位置
		EXPLAIN_SIZE,	// 大きさ
		VEC3_ZERO,		// 向き
		DEFAULT_COL		// 色
	);
	if (UNUSED(m_pExplain))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	必要レベルの生成・設定
	//--------------------------------------------------------
	// 必要レベルの生成
	m_pLv = CValueUI::Create
	( // 引数
		mc_apTextureFile[TEXTURE_LV],	// タイトルテクスチャパス
		CValue::TEXTURE_ANGULAR,		// 数字テクスチャ
		LV_VALUE_DIGIT,	// 桁数
		VEC3_ZERO,		// 位置
		LV_SPACE,		// 行間
		LV_VALUE_SPACE,	// 数字行間
		LV_TITLE_SIZE,	// タイトル大きさ
		LV_VALUE_SIZE,	// 数字大きさ
		VEC3_ZERO,		// タイトル向き
		VEC3_ZERO,		// 数字向き
		DEFAULT_COL,	// タイトル色
		DEFAULT_COL		// 数字色
	);
	if (UNUSED(m_pLv))
	{ // 非使用中の場合

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
void CShop::Uninit(void)
{
	// 購入品アイコンの終了
	m_pIcon->Uninit();

	// 購入品説明の終了
	m_pExplain->Uninit();

	// 必要レベルの終了
	m_pLv->Uninit();

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CShop::Update(void)
{
	// 購入品アイコンの更新
	m_pIcon->Update();

	// 購入品説明の更新
	m_pExplain->Update();

	// 必要レベルの更新
	m_pLv->Update();
}

//============================================================
//	描画処理
//============================================================
void CShop::Draw(void)
{

}

//============================================================
//	描画状況の設定処理
//============================================================
void CShop::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pIcon->SetEnableDraw(bDraw);		// 購入品アイコン
	m_pExplain->SetEnableDraw(bDraw);	// 購入品説明
	m_pLv->SetEnableDraw(bDraw);		// 必要レベル
}

//============================================================
//	優先順位の設定処理
//============================================================
void CShop::SetPriority(const int nPriority)
{
	// 引数の優先順位を設定
	CObject::SetPriority(nPriority);	// 自身
	m_pIcon->SetPriority(nPriority);	// 購入品アイコン
	m_pExplain->SetPriority(nPriority);	// 購入品説明
	m_pLv->SetPriority(nPriority);		// 必要レベル
}

//============================================================
//	生成処理
//============================================================
CShop *CShop::Create
(
	const BUY buy,			// 購入品
	const D3DXVECTOR3& rPos	// 位置
)
{
	// ポインタを宣言
	CShop *pShop = NULL;	// ショップ生成用

	if (UNUSED(pShop))
	{ // 使用されていない場合

		// メモリ確保
		pShop = new CShop;	// ショップ
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pShop))
	{ // 確保に成功している場合

		// ショップの初期化
		if (FAILED(pShop->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pShop;
			pShop = NULL;

			// 失敗を返す
			return NULL;
		}

		// 購入品を設定
		pShop->SetBuy(buy);

		// 位置を設定
		pShop->SetPosition(rPos);

		// 確保したアドレスを返す
		return pShop;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	購入必要レベル取得処理
//============================================================
int CShop::GetNeedLevel(const BUY buy)
{
	// 引数の購入品の購入必要レベルを返す
	return mc_aNeedLevel[buy];
}

//============================================================
//	設定処理
//============================================================
void CShop::SetBuy(const BUY buy)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャ

	// 引数の購入品を設定
	m_buy = buy;

	if (m_buy == NONE_IDX)
	{ // 売り切れだった場合

		// テクスチャを登録・割当
		m_pIcon->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_SOLDOUT_ICON]));			// 購入品アイコン
		m_pExplain->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_SOLDOUT_EXPLAIN]));	// 購入品説明

		// 購入品アイコンのテクスチャ分割数を設定
		m_pIcon->SetWidthPattern(1);
		m_pIcon->SetHeightPattern(1);

		// 購入品説明のテクスチャ分割数を設定
		m_pExplain->SetWidthPattern(1);
		m_pExplain->SetHeightPattern(1);

		// 購入品のパターンを設定
		m_pIcon->SetPattern(0);		// 購入品アイコン
		m_pExplain->SetPattern(0);	// 購入品説明

		// 購入品の必要レベルを設定
		m_pLv->GetMultiValue()->SetNum(0);
	}
	else
	{ // 売り切れではなかった場合

		// テクスチャを登録・割当
		m_pIcon->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_ICON]));			// 購入品アイコン
		m_pExplain->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_EXPLAIN]));	// 購入品説明

		// 購入品アイコンのテクスチャ分割数を設定
		m_pIcon->SetWidthPattern(PTRN_WIDTH);
		m_pIcon->SetHeightPattern(PTRN_HEIGHT);

		// 購入品説明のテクスチャ分割数を設定
		m_pExplain->SetWidthPattern(PTRN_WIDTH);
		m_pExplain->SetHeightPattern(PTRN_HEIGHT);

		// 購入品のパターンを設定
		m_pIcon->SetPattern(m_buy);		// 購入品アイコン
		m_pExplain->SetPattern(m_buy);	// 購入品説明

		// 購入品の必要レベルを設定
		m_pLv->GetMultiValue()->SetNum(mc_aNeedLevel[m_buy]);
	}
}

//============================================================
//	取得処理
//============================================================
CShop::BUY CShop::GetBuy(void) const
{
	// 購入品を返す
	return m_buy;
}

//============================================================
//	位置の設定処理
//============================================================
void CShop::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	相対位置の設定処理
//============================================================
void CShop::SetPositionRelative(void)
{
	// 購入品アイコンの位置を設定
	m_pIcon->SetPosition(m_pos);

	// 購入品説明の表示位置を設定
	m_pExplain->SetPosition(m_pos + ADD_POS_EXPLAIN);

	// 必要レベルの表示位置を設定
	m_pLv->SetPosition(m_pos + ADD_POS_LEVEL);
}

//============================================================
//	色の設定処理
//============================================================
void CShop::SetColor(const D3DXCOLOR& rCol)
{
	// 引数の色を代入
	m_col = rCol;

	// 購入品アイコンの色を設定
	m_pIcon->SetColor(rCol);

	// 購入品説明の色を設定
	m_pExplain->SetColor(rCol);

	// 必要レベルの表示位置を設定
	m_pLv->SetColorTitle(rCol);				// タイトル色
	m_pLv->GetMultiValue()->SetColor(rCol);	// 数字色
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CShop::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	色取得処理
//============================================================
D3DXCOLOR CShop::GetColor(void) const
{
	// 色を返す
	return m_col;
}
