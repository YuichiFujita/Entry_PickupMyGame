//============================================================
//
//	ショップ省き情報ヘッダー [omitShop.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OMITSHOP_H_
#define _OMITSHOP_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "shop.h"

//************************************************************
//	クラス定義
//************************************************************
// ショップ省き情報クラス
class COmitShop
{
public:
	// コンストラクタ
	COmitShop();

	// デストラクタ
	~COmitShop();

	// メンバ関数
	void Init(void);	// 初期化
	void Uninit(void);	// 終了
	int *CreateOmit(void);		// 省く購入品生成
	HRESULT ReleaseOmit(void);	// 省く購入品破棄
	int GetNumOmit(void) const;	// 省く購入品数取得
	bool IsOmit(const CShop::BUY buy) const;	// 省き状況取得
	void SetEnableOmit(const CShop::BUY buy, const bool bOmit);	// 省き状況設定

	// 静的メンバ関数
	static COmitShop *Create(void);	// 生成
	static HRESULT Release(COmitShop *&prOmitShop);	// 破棄

private:
	int *m_pOmit;		// 省く購入品情報
	int  m_nNumOmit;	// 省く購入品数
	bool m_aOmitShop[CShop::BUY_MAX];	// 購入品の省き状況
};

#endif	// _OMITSHOP_H_
