//============================================================
//
//	タイマーUIヘッダー [timerUI.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TIMER_UI_H_
#define _TIMER_UI_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "timerManager.h"

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// タイマーUIクラス
class CTimerUI : public CTimerManager
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_BG = 0,	// 背景テクスチャ
		TEXTURE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CTimerUI();

	// デストラクタ
	~CTimerUI() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void SetPosition(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetPriority(const int nPriority) override;		// 優先順位設定
	void SetEnableUpdate(const bool bUpdate) override;	// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;		// 描画状況設定

	// 静的メンバ関数
	static CTimerUI *Create	// 生成
	( // 引数
		const ETime time,				// 設定タイム
		const long nTime,				// 制限時間
		const D3DXVECTOR3& rPos,		// 位置
		const D3DXVECTOR3& rSizeValue,	// 数字の大きさ
		const D3DXVECTOR3& rSizePart,	// 区切りの大きさ
		const D3DXVECTOR3& rSpaceValue,	// 数字の空白
		const D3DXVECTOR3& rSpacePart,	// 区切りの空白
		const D3DXVECTOR3& rOffsetBG,	// 背景のオフセット
		const D3DXVECTOR3& rSizeBG		// 背景の大きさ
	);
	static void Release(CTimerUI *&prTimerUI);	// 破棄

	// メンバ関数
	void SetOffset(const D3DXVECTOR3& rOffset);	// オフセット設定
	void SetSizingBG(const D3DXVECTOR3& rSize);	// 背景大きさ設定

private:
	// メンバ関数
	void SetPositionRelative(void);	// 相対位置設定

	// メンバ変数
	CObject2D *m_pBG;		// 背景の情報
	D3DXVECTOR3 m_offsetBG;	// 背景のオフセット
};

#endif	// _TIMER_UI_H_
