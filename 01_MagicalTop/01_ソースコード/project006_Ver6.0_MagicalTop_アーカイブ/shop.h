//============================================================
//
//	ショップヘッダー [shop.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHOP_H_
#define _SHOP_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CAnim2D;	// アニメーション2Dクラス
class CValueUI;	// 数字UIクラス

//************************************************************
//	クラス定義
//************************************************************
// ショップクラス
class CShop : public CObject
{
public:
	// テクスチャ列挙
	enum TEXTURE
	{
		TEXTURE_LV = 0,				// 必要レベルテクスチャ
		TEXTURE_ICON,				// アイコンテクスチャ
		TEXTURE_EXPLAIN,			// 説明テクスチャ
		TEXTURE_SOLDOUT_ICON,		// 売り切れアイコンテクスチャ
		TEXTURE_SOLDOUT_EXPLAIN,	// 売り切れ説明テクスチャ
		TEXTURE_MAX,				// この列挙型の総数
	};

	// 購入列挙
	enum BUY
	{
		BUY_HEAL = 0,			// 回復
		BUY_LVUP_LIFE,			// 体力レベルアップ
		BUY_LVUP_MANA,			// マナレベルアップ
		BUY_LVUP_DASH,			// ダッシュレベルアップ
		BUY_LVUP_DEFENSE,		// 防御力レベルアップ
		BUY_LVUP_SPEED,			// 素早さレベルアップ
		BUY_LVUP_MAGIC_NUM,		// 魔法発射数レベルアップ
		BUY_LVUP_MAGIC_SPEED,	// 魔法弾速レベルアップ
		BUY_LVUP_MAGIC_RAPID,	// 魔法連射速度レベルアップ
		BUY_EXP_UP,				// 獲得経験値増加
		BUY_MAX					// この列挙型の総数
	};

	// コンストラクタ
	CShop();

	// デストラクタ
	~CShop();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void SetEnableDraw(const bool bDraw);	// 描画状況設定
	void SetPriority(const int nPriority);	// 優先順位設定

	// 静的メンバ関数
	static CShop *Create	// 生成
	( // 引数
		const BUY buy,			// 購入品
		const D3DXVECTOR3& rPos	// 位置
	);
	static int GetNeedLevel(const BUY buy);	// 購入必要レベル取得

	// メンバ関数
	void SetBuy(const BUY buy);	// 設定
	BUY  GetBuy(void) const;	// 取得
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	void SetColor(const D3DXCOLOR& rCol);		// 色設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	D3DXCOLOR GetColor(void) const;				// 色取得

private:
	// メンバ関数
	void SetPositionRelative(void);	// 相対位置設定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
	static const int mc_aNeedLevel[];		// 購入必要レベル定数

	// メンバ変数
	CAnim2D *m_pIcon;		// 購入品アイコン
	CAnim2D *m_pExplain;	// 購入品説明
	CValueUI *m_pLv;		// 必要レベル
	D3DXVECTOR3 m_pos;		// 位置
	D3DXCOLOR m_col;		// 色
	BUY m_buy;				// 購入品
};

#endif	// _SHOP_H_
