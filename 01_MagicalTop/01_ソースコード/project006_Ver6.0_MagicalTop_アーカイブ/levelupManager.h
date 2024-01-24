//============================================================
//
//	強化マネージャーヘッダー [levelupManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LEVELUPMANAGER_H_
#define _LEVELUPMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CShopManager;	// ショップマネージャークラス
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 強化マネージャークラス
class CLevelupManager
{
public:
	// テクスチャ列挙
	enum TEXTURE
	{
		TEXTURE_CONTROL = 0,	// 操作テクスチャ
		TEXTURE_MAX,			// この列挙型の総数
	};

	// 状態列挙
	enum STATE
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_SELECT,		// 強化選択状態
		STATE_MAX,			// この列挙型の総数
	};

	// コンストラクタ
	CLevelupManager();

	// デストラクタ
	~CLevelupManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新

	// 静的メンバ関数
	static CLevelupManager *Create(void);	// 生成
	static HRESULT Release(CLevelupManager *&prLevelupManager);	// 破棄

private:
	// メンバ関数
	void UpdateBlink(void);	// 点滅更新

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	CShopManager *m_pShopManager;	// ショップマネージャーの情報
	CObject2D *m_pControl;	// 操作情報
	STATE m_state;			// 状態
	float m_fSinRot;		// ポリゴン点滅向き
};

#endif	// _LEVELUPMANAGER_H_
