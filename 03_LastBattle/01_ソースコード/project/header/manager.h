//============================================================
//
//	マネージャーヘッダー [manager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "scene.h"

//************************************************************
//	前方宣言
//************************************************************
class CTime;				// タイムクラス
class CRenderer;			// レンダラークラス
class CInputKeyboard;		// キーボードクラス
class CInputMouse;			// マウスクラス
class CInputPad;			// パッドクラス
class CSound;				// サウンドクラス
class CCamera;				// カメラクラス
class CLight;				// ライトクラス
class CTexture;				// テクスチャクラス
class CModel;				// モデルクラス
class CFade;				// フェードクラス
class CRetentionManager;	// データ保存マネージャークラス
class CDebugProc;			// デバッグ表示クラス
class CDebug;				// デバッグクラス

//************************************************************
//	クラス定義
//************************************************************
// マネージャークラス
class CManager
{
public:
	// コンストラクタ
	CManager();

	// デストラクタ
	~CManager();

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CManager *Create(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);	// 生成
	static CManager *GetInstance(void);			// 取得
	static void Release(CManager *&prManager);	// 破棄

	// メンバ関数
	void SetScene(const CScene::EMode mode, const int nWait = 0);	// シーン設定
	HRESULT SetMode(const CScene::EMode mode);	// モード設定
	CScene::EMode GetMode(void);				// モード取得

	CTime				*GetTime(void);			// タイム取得
	CRenderer			*GetRenderer(void);		// レンダラー取得
	CInputKeyboard		*GetKeyboard(void);		// キーボード取得
	CInputMouse			*GetMouse(void);		// マウス取得
	CInputPad			*GetPad(void);			// パッド取得
	CSound				*GetSound(void);		// サウンド取得
	CCamera				*GetCamera(void);		// カメラ取得
	CLight				*GetLight(void);		// ライト取得
	CTexture			*GetTexture(void);		// テクスチャ取得
	CModel				*GetModel(void);		// モデル取得
	CFade				*GetFade(void);			// フェード取得
	CScene				*GetScene(void);		// シーン取得
	CRetentionManager	*GetRetention(void);	// データ保存マネージャー取得
	CDebugProc			*GetDebugProc(void);	// デバッグ表示取得
	CDebug				*GetDebug(void);		// デバッグ取得

private:
	// 静的メンバ変数
	static CManager *m_pManager;	// マネージャー

	// メンバ変数
	CTime				*m_pTime;		// タイムインスタンス
	CRenderer			*m_pRenderer;	// レンダラーインスタンス
	CInputKeyboard		*m_pKeyboard;	// キーボードインスタンス
	CInputMouse			*m_pMouse;		// マウスインスタンス
	CInputPad			*m_pPad;		// パッドインスタンス
	CSound				*m_pSound;		// サウンドインスタンス
	CCamera				*m_pCamera;		// カメラインスタンス
	CLight				*m_pLight;		// ライトインスタンス
	CTexture			*m_pTexture;	// テクスチャインスタンス
	CModel				*m_pModel;		// モデルインスタンス
	CFade				*m_pFade;		// フェードインスタンス
	CScene				*m_pScene;		// シーンインスタンス
	CRetentionManager	*m_pRetention;	// データ保存マネージャー
	CDebugProc			*m_pDebugProc;	// デバッグ表示
	CDebug				*m_pDebug;		// デバッグ
};

#endif	// _MANAGER_H_
