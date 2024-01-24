//============================================================
//
//	ショップマネージャーヘッダー [shopManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHOPMANAGER_H_
#define _SHOPMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "shop.h"

//************************************************************
//	前方宣言
//************************************************************
class COmitShop;	// ショップ省き情報クラス
class CObject2D;	// オブジェクト2Dクラス
class CValueUI;		// 数字UIクラス
class CShop;		// ショップクラス

//************************************************************
//	クラス定義
//************************************************************
// ショップマネージャークラス
class CShopManager
{
public:
	// テクスチャ列挙
	enum TEXTURE
	{
		TEXTURE_PLAYER = 0,	// プレイヤーアイコンテクスチャ
		TEXTURE_EXP,		// 経験値アイコンテクスチャ
		TEXTURE_LIFE,		// 体力テクスチャ
		TEXTURE_SLASH,		// 最大体力区切りテクスチャ
		TEXTURE_LV,			// レベルテクスチャ
		TEXTURE_MAX,		// この列挙型の総数
	};

	// 選択列挙
	enum SELECT
	{
		SELECT_LEFT = 0,	// 左選択
		SELECT_CENTER,		// 中央選択
		SELECT_RIGHT,		// 右選択
		SELECT_MAX,			// この列挙型の総数
	};

	// コンストラクタ
	CShopManager();

	// デストラクタ
	~CShopManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetEnableOpen(const bool bOpen);	// 開店状況設定

	// 静的メンバ関数
	static CShopManager *Create(void);	// 生成
	static HRESULT Release(CShopManager *&prShopManager);	// 破棄

private:
	// メンバ関数
	void UpdateSelect(void);	// 購入品選択
	void UpdateBuy(void);		// 購入
	void CheckHealOmit(void);	// 回復省き確認
	void SetEnableDraw(const bool bDraw);	// 描画状況設定

	int RandomShop	// ショップ単変更
	( // 引数
		const int nShop,		// 変更するショップインデックス
		int *pOmit = NULL,		// ランダムを省く購入品の先頭アドレス
		const int nNumOmit = 0	// ランダムを省く購入品数
	);
	void AllRandomShop(void);	// ショップ全変更

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	COmitShop *m_pOmitShop;			// ショップ省き情報
	CObject2D *m_pBg;				// 背景情報
	CObject2D *m_pIconPlayer;		// プレイヤーアイコン情報
	CObject2D *m_pIconExp;			// 経験値アイコン情報
	CValueUI *m_pLife;				// プレイヤー体力情報
	CValueUI *m_pMaxLife;			// プレイヤー最大体力情報
	CValueUI *m_pLv;				// プレイヤーレベル情報
	CShop *m_apShop[SELECT_MAX];	// ショップ情報
	int m_nSelect;					// 現在の選択番号
	int m_nOldSelect;				// 過去の選択番号
};

#endif	// _SHOPMANAGER_H_
