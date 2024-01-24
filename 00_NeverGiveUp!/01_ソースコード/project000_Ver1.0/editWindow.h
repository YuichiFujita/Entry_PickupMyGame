//============================================================
//
//	エディット窓ヘッダー [editWindow.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_WINDOW_H_
#define _EDIT_WINDOW_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "window.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス

//************************************************************
//	クラス定義
//************************************************************
// エディット窓クラス
class CEditWindow
{
public:
	// コンストラクタ
	CEditWindow();

	// デストラクタ
	~CEditWindow();

	// 窓情報構造体
	struct SInfo
	{
		CWindow *pWindow;		// 窓情報
		CWindow::EType type;	// 窓種類
	};

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetDisp(const bool bDisp);	// 表示設定
	void DrawDebugControl(void);	// 操作表示描画
	void DrawDebugInfo(void);		// 情報表示描画
	void Save(FILE *pFile);			// 保存

	// 静的メンバ関数
	static CEditWindow *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditWindow *&prEditWindow);		// 破棄

private:
	// メンバ関数
	void UpdateChangeType(void);	// 種類変更の更新
	void UpdateChangeScale(void);	// 拡大率変更の更新
	void MyCreateWindow(void);		// 窓生成
	void ReleaseWindow(void);		// 窓破棄

	void DeleteCollisionWindow(const bool bRelase);	// 窓の削除判定
	void InitAllColorWindow(void);					// 窓の色全初期化

	// メンバ変数
	CEditStageManager *m_pEdit;	// エディットステージの情報
	SInfo m_window;	// 窓配置情報
};

#endif	// _EDIT_WINDOW_H_
