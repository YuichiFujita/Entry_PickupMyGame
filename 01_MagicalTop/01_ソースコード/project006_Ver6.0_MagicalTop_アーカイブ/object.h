//============================================================
//
//	オブジェクトヘッダー [object.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	マクロ定義
//************************************************************
#define FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)				// 頂点フォーマット [2D]
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)	// 頂点フォーマット [3D]

#define MAX_PRIO		(8)	// 優先順位の総数
#define DEFAULT_PRIO	(3)	// デフォルトの優先順位

//************************************************************
//	構造体定義
//************************************************************
// 頂点情報 [2D]
struct VERTEX_2D
{
	D3DXVECTOR3	pos;	// 頂点座標
	float		rhw;	// 座標変換用係数 (1.0fで固定)
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex;	// テクスチャ座標
};

// 頂点情報 [3D]
struct VERTEX_3D
{
	D3DXVECTOR3	pos;	// 頂点座標
	D3DXVECTOR3	nor;	// 法線ベクトル
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex;	// テクスチャ座標
};

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトクラス
class CObject
{
public:
	// オブジェクトラベル列挙
	typedef enum
	{
		LABEL_NONE = 0,		// なし
		LABEL_UI,			// UI
		LABEL_PLAYER,		// プレイヤー
		LABEL_ENEMY,		// 敵
		LABEL_TARGET,		// ターゲット
		LABEL_MAGIC,		// 魔法
		LABEL_LOCK,			// ロックオン
		LABEL_GAUGE,		// ゲージ
		LABEL_NUMBER,		// 数字
		LABEL_EXP,			// 経験値
		LABEL_BULLET,		// 弾
		LABEL_FLOWER,		// マナフラワー
		LABEL_WAY,			// 方向表示
		LABEL_WEED,			// 草
		LABEL_WIND,			// 風
		LABEL_FIELD,		// 地面
		LABEL_WALL,			// 壁
		LABEL_SCENERY,		// 景色
		LABEL_SKY,			// 空
		LABEL_SEA,			// 海
		LABEL_EFFECT2D,		// エフェクト
		LABEL_PARTICLE2D,	// パーティクル
		LABEL_EFFECT3D,		// エフェクト
		LABEL_PARTICLE3D,	// パーティクル
		LABEL_MAX			// この列挙型の総数
	}LABEL;

	// コンストラクタ
	CObject();
	CObject(const LABEL label, const int nPriority = DEFAULT_PRIO);

	// デストラクタ
	virtual ~CObject();

	// 純粋仮想関数
	virtual HRESULT Init(void)	= 0;	// 初期化
	virtual void Uninit(void)	= 0;	// 終了
	virtual void Update(void)	= 0;	// 更新
	virtual void Draw(void)		= 0;	// 描画

	// 仮想関数
	virtual void Hit(const int nDmg);	// ヒット
	virtual void HitKnockBack(const int nDmg, const D3DXVECTOR3& vec);	// ノックバックヒット

	virtual void BindTexture(const int nTextureID);		// テクスチャ割当
	virtual void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	virtual void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	virtual void SetState(const int nState);			// 状態設定
	virtual void SetType(const int nType);				// 種類設定
	virtual void SetEnableUpdate(const bool bUpdate);	// 更新状況設定
	virtual void SetEnableDraw(const bool bDraw);		// 描画状況設定
	virtual void SetPriority(const int nPriority);		// 優先順位設定
	virtual D3DXMATRIX *GetPtrMtxWorld(void);			// マトリックスポインタ取得
	virtual D3DXMATRIX GetMtxWorld(void) const;			// マトリックス取得
	virtual D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	virtual D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	virtual D3DXVECTOR3 GetScaling(void) const;			// 大きさ取得
	virtual int GetState(void) const;					// 状態取得
	virtual int GetType(void) const;					// 種類取得
	virtual float GetRadius(void) const;				// 半径取得
	virtual float GetHeight(void) const;				// 縦幅取得

	// 静的メンバ関数
	static void ReleaseAll(void);	// 全破棄
	static void UpdateAll(void);	// 全更新
	static void DrawAll(void);		// 全描画
	static int GetNumAll(void);		// 総数取得
	static bool CheckUse(const CObject *pObject);	// 使用確認
	static CObject *GetTop(const int nPriority);	// 先頭オブジェクト取得
	static CObject *GetCur(const int nPriority);	// 最後尾オブジェクト取得

	// メンバ関数
	void	SetLabel(const LABEL label);	// ラベル設定
	LABEL	GetLabel(void) const;			// ラベル取得
	int		GetPriority(void) const;		// 優先順位取得

	DWORD	GetID(void) const;		// ユニークID取得
	bool	IsUpdate(void) const;	// 更新状況取得
	bool	IsDraw(void) const;		// 描画状況取得
	bool	IsDeath(void) const;	// 死亡フラグ取得
	CObject	*GetObject(void);		// オブジェクト取得
	CObject	*GetPrev(void) const;	// 前オブジェクト取得
	CObject	*GetNext(void) const;	// 次オブジェクト取得

protected:
	// メンバ関数
	void Release(void);	// 破棄

private:
	// 静的メンバ関数
	static void DeathAll(void);	// 全死亡

	// 静的メンバ変数
	static CObject *m_apTop[MAX_PRIO];	// 先頭のオブジェクトへのポインタ
	static CObject *m_apCur[MAX_PRIO];	// 最後尾のオブジェクトへのポインタ
	static DWORD m_dwNextID;			// 次のユニークID
	static int m_nNumAll;				// オブジェクトの総数

	// メンバ変数
	LABEL m_label;		// 自身のオブジェクトラベル
	DWORD m_dwID;		// 自身のユニークID
	int m_nPriority;	// 自身の優先順位
	bool m_bUpdate;		// 自身の更新状況
	bool m_bDraw;		// 自身の描画状況
	bool m_bDeath;		// 自身の死亡フラグ
	CObject *m_pPrev;	// 前のオブジェクトへのポインタ
	CObject *m_pNext;	// 次のオブジェクトへのポインタ
};

#endif	// _OBJECT_H_
