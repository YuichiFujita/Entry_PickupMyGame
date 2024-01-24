//============================================================
//
//	窓ヘッダー [window.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WINDOW_H_
#define _WINDOW_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CObject3D;	// オブジェクト3Dクラス

//************************************************************
//	クラス定義
//************************************************************
// 窓クラス
class CWindow : public CObject
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_NORMAL = 0,	// 通常ビル窓テクスチャ
		TYPE_MAX			// この列挙型の総数
	};

	// 窓列挙
	enum EWindow
	{
		WINDOW_NEAR_FRONT = 0,	// 手前・表の壁
		WINDOW_NEAR_BACK,		// 手前・裏の壁
		WINDOW_FAR_FRONT,		// 奥・表の壁
		WINDOW_FAR_BACK,		// 奥・裏の壁
		WINDOW_MAX				// この列挙型の総数
	};

	// コンストラクタ
	CWindow();

	// デストラクタ
	~CWindow();

	// ステータス情報構造体
	struct SStatusInfo
	{
		D3DXVECTOR3 size;	// 大きさ
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画

	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	D3DXVECTOR3 GetVec3Rotation(void) const override;		// 向き取得
	void SetVec3Sizing(const D3DXVECTOR3& rSize) override;	// 大きさ設定
	D3DXVECTOR3 GetVec3Sizing(void) const override;			// 大きさ取得
	void SetColor(const D3DXCOLOR& rCol) override;			// 色設定
	void SetEnableUpdate(const bool bUpdate) override;		// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;			// 描画状況設定

	void SetType(const int nType) override;		// 種類設定
	int GetType(void) const override;			// 種類取得
	void SetScale(const float fScale) override;	// 拡大率設定
	float GetScale(void) const override;		// 拡大率取得

	// 静的メンバ関数
	static CWindow *Create	// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const float fScale = 1.0f	// 拡大率
	);

private:
	// メンバ関数
	void SetPositionRelative(void);	// 相対位置設定

	// 静的メンバ変数
	static const char *mc_apTextureFile[][WINDOW_MAX];	// テクスチャ定数
	static SStatusInfo m_aStatusInfo[];	// ステータス情報

	// メンバ変数
	CObject3D *m_pWall[WINDOW_MAX];	// 窓の情報
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_rot;		// 向き
	D3DXVECTOR3 m_size;		// 大きさ
	SStatusInfo m_status;	// ステータス定数
	EType m_type;			// 種類定数
	float m_fScale;			// 拡大率
};

#endif	// _WINDOW_H_
