//============================================================
//
//	手紙マネージャーヘッダー [letterManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LETTER_MANAGER_H_
#define _LETTER_MANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 手紙マネージャークラス
class CLetterManager
{
public:
	// テクスチャ列挙
	enum TEXTURE
	{
		TEXTURE_LETTER = 0,	// 手紙のテクスチャ
		TEXTURE_CONTROL,	// 操作表示のテクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum STATE
	{
		STATE_NONE = 0,		// 何もしない状態
		STATE_LETTER,		// 手紙の表示状態
		STATE_FADEIN,		// フェードイン状態
		STATE_PAPER_TAKE,	// 便箋の取り出し状態
		STATE_WAIT,			// 待機状態
		STATE_PAPER_RETURN,	// 便箋のしまい状態
		STATE_END,			// 終了状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CLetterManager();

	// デストラクタ
	~CLetterManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetPaper(const char *pPassTex);	// 便箋設定
	void SetLook(const char *pPassTex);		// 手紙表示開始
	void SetState(const STATE state);		// 状態設定
	STATE GetState(void) const;				// 状態取得

	// 静的メンバ関数
	static CLetterManager *Create(void);	// 生成
	static HRESULT Release(CLetterManager *&prLetterManager);	// 破棄

private:
	// メンバ関数
	void UpdateLetter(void);		// 手紙表示
	void UpdateFade(void);			// フェードイン
	void UpdatePaperTake(void);		// 便箋取出
	void UpdateWait(void);			// 待機
	void UpdatePaperReturn(void);	// 便箋収納

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	CObject2D *m_pLetter;	// 手紙の情報
	CObject2D *m_pFade;		// フェードの情報
	CObject2D *m_pPaper;	// 便箋の情報
	CObject2D *m_pControl;	// 操作方法の情報
	STATE m_state;			// 状態
	int m_nCounterState;	// 状態管理カウンター
	float m_fMove;			// 便箋の移動量
	float m_fSinRot;		// 操作方法の点滅向き
};

#endif	// _LETTER_MANAGER_H_
