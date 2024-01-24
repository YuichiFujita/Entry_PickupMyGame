//============================================================
//
//	経験値マネージャーヘッダー [expManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EXPMANAGER_H_
#define _EXPMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectGauge2D;	// オブジェクトゲージ2Dクラス
class CMultiValue;		// マルチ数字クラス

//************************************************************
//	クラス定義
//************************************************************
// 経験値マネージャークラス
class CExpManager
{
public:
	// コンストラクタ
	CExpManager();

	// デストラクタ
	~CExpManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void AddLevel(const int nAdd);	// レベル加算
	void AddExp(const int nAdd);	// 経験値加算
	int  GetLevel(void) const;		// レベル取得
	void SetEnableExpDraw(const bool bDraw);	// 経験値描画状況の設定

	// 静的メンバ関数
	static CExpManager *Create(void);	// 生成
	static HRESULT Release(CExpManager *&prExpManager);	// 破棄

private:
	// メンバ変数
	CObjectGauge2D *m_pExp;	// 経験値の情報
	CMultiValue *m_pLevel;	// レベルの情報
};

#endif	// _EXPMANAGER_H_
