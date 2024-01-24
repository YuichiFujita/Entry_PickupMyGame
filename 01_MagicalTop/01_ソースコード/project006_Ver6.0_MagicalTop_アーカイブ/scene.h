//============================================================
//
//	シーンヘッダー [scene.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SCENE_H_
#define _SCENE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CStage;	// ステージクラス
class CPlayer;	// プレイヤークラス
class CTarget;	// ターゲットクラス
class CField;	// 地面クラス

//************************************************************
//	クラス定義
//************************************************************
// シーンクラス
class CScene
{
public:
	// モード列挙
	enum MODE
	{
		MODE_TITLE = 0,	// タイトル画面
		MODE_TUTORIAL,	// チュートリアル画面
		MODE_GAME,		// ゲーム画面
		MODE_RESULT,	// リザルト画面
		MODE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CScene(const MODE mode);

	// デストラクタ
	~CScene();

	// 純粋仮想関数
	virtual void Draw(void)	= 0;	// 描画

	// 仮想関数
	virtual HRESULT Init(void);		// 初期化
	virtual HRESULT Uninit(void);	// 終了
	virtual void Update(void);		// 更新

	// 静的メンバ関数
	static CScene *Create(MODE mode);			// 生成
	static HRESULT Release(CScene *&prScene);	// 破棄

	static CStage *GetStage(void);			// ステージ取得
	static CPlayer *GetPlayer(void);		// プレイヤー取得
	static CTarget *GetTarget(void);		// ターゲット取得
	static HRESULT CreateField(MODE mode);	// 地面生成
	static CField *GetField(void);			// 地面取得

	// メンバ関数
	void SetMode(const MODE mode);	// モード設定
	MODE GetMode(void) const;		// モード取得

private:
	// 静的メンバ変数
	static CStage *m_pStage;	// ステージ
	static CPlayer *m_pPlayer;	// プレイヤーオブジェクト
	static CTarget *m_pTarget;	// ターゲットオブジェクト
	static CField *m_pField;	// 地面オブジェクト

	// メンバ変数
	MODE m_mode;	// モード
};

#endif	// _SCENE_H_
