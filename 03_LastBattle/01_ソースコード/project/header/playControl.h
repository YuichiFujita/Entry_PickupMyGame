//============================================================
//
//	プレイ操作ヘッダー [playControl.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PLAY_CONTROL_H_
#define _PLAY_CONTROL_H_

//************************************************************
//	前方宣言
//************************************************************
class CAnim2D;	// アニメーション2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// プレイ操作クラス
class CPlayControl
{
public:
	// 操作列挙
	enum EControl
	{
		CONTROL_NONE = 0,	// 操作なし
		CONTROL_RIDE,		// 騎乗操作
		CONTROL_RUSH,		// 連続攻撃操作
		CONTROL_MAX			// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NONE = 0,		// 何もしない
		STATE_SCALEIN,		// 表示開始
		STATE_DISP,			// 表示
		STATE_FADEOUT,		// 表示終了
		STATE_MAX			// この列挙型の総数
	};

	// 表示列挙
	enum EDisp
	{
		DISP_NORMAL = 0,	// 固定表示
		DISP_BLINK,			// 点滅表示
		DISP_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CPlayControl();

	// デストラクタ
	~CPlayControl();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	void SetDisp	// 表示設定
	( // 引数
		EControl contType,	// 表示操作
		EDisp dispType		// 表示形式
	);
	void SetHide(const bool bFlash = true);	// 非表示設定
	bool IsDisp(void);	// 表示取得

	// 静的メンバ関数
	static CPlayControl *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rSize	// 大きさ
	);
	static void Release(CPlayControl *&prPlayControl);	// 破棄

private:
	// メンバ変数
	CAnim2D *m_pControl;		// 操作の情報
	D3DXVECTOR3 m_originSize;	// 操作の元の大きさ
	EControl m_contType;		// 表示操作
	EDisp m_dispType;			// 表示形式
	EState m_state;		// 状態
	int m_nCounter;		// 状態管理カウンター
	float m_fSinScale;	// 拡縮向き
	float m_fScale;		// 拡大率
};

#endif	// _PLAY_CONTROL_H_
