//============================================================
//
//	スマホマネージャーヘッダー [phoneManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PHONE_MANAGER_H_
#define _PHONE_MANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	マクロ定義
//************************************************************
#define NUM_MESSAGE	(3)	// 受信メッセージ数

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス
class CAnim2D;		// アニメーション2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// スマホマネージャークラス
class CPhoneManager
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_PHONE = 0,	// スマホのテクスチャ
		TEXTURE_STRESS,		// 強調のテクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// 種類列挙
	enum EType
	{
		TYPE_START = 0,	// 開始
		TYPE_END,		// 終了
		TYPE_MAX		// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,				// 何もしない状態
		STATE_FADEIN,				// フェードイン状態
		STATE_PHONE_TAKE,			// スマホの取り出し状態
		STATE_PHONE_SCALE_WAIT,		// スマホの拡大待機状態
		STATE_PHONE_SCALE,			// スマホの拡大状態
		STATE_MESSAGE,				// メッセージ受信状態
		STATE_FACE_WAIT,			// 表情の拡大待機状態
		STATE_FACE,					// 表情の拡大状態
		STATE_PHONE_RETURN_WAIT,	// スマホのしまい待機状態
		STATE_PHONE_RETURN,			// スマホのしまい状態
		STATE_STAGING,				// 演出の表示状態
		STATE_END_WAIT,				// 終了待機状態
		STATE_END,					// 終了状態
		STATE_MAX					// この列挙型の総数
	};

	// コンストラクタ
	CPhoneManager();

	// デストラクタ
	~CPhoneManager();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetLook(const EType type);			// スマホ表示開始
	void SetState(const EState state);		// 状態設定
	EState GetState(void) const;			// 状態取得
	void SetEnableDisp(const bool bDisp);	// 表示設定
	bool IsDisp(void) const;				// 表示取得

	// 静的メンバ関数
	static CPhoneManager *Create(void);	// 生成
	static HRESULT Release(CPhoneManager *&prPhoneManager);	// 破棄

private:
	// メンバ関数
	void UpdateFade(void);			// フェードイン
	void UpdatePhoneTake(void);		// スマホ取出
	void UpdatePhoneScale(void);	// スマホ拡大
	void UpdateMessage(void);		// メッセージ受信
	void UpdateFace(void);			// 表情拡大
	void UpdatePhoneReturn(void);	// スマホ収納
	void UpdateStaging(void);		// 演出表示
	void SkipPhoneStaging(void);	// スマホ演出スキップ
	void SetPositionRelative(void);	// メッセージ相対位置設定
	bool UpdateDispWait(const int nWait);	// 表示待機

	// 静的メンバ変数
	static const char *mc_apTextureFile[];			// テクスチャ定数
	static const char *mc_apMessageTextureFile[];	// メッセージテクスチャ定数
	static const char *mc_apFaceTextureFile[];		// 表情テクスチャ定数
	static const char *mc_apStagingTextureFile[];	// 演出テクスチャ定数

	// メンバ変数
	CAnim2D *m_apMessage[NUM_MESSAGE];	// メッセージの情報
	CObject2D *m_pStaging;	// 演出の情報
	CObject2D *m_pFace;		// 表情の情報
	CObject2D *m_pStress;	// 強調の情報
	CObject2D *m_pPhone;	// スマホの情報
	CObject2D *m_pFade;		// フェードの情報
	EState m_state;			// 状態
	int m_nCounterState;	// 状態管理カウンター
	int m_nCounterDisp;		// 表示管理カウンター
	float m_fMove;			// スマホの移動量
	float m_fScale;			// スマホの拡大率
	bool m_bDisp;			// 表示状況
};

#endif	// _PHONE_MANAGER_H_
