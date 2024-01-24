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
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)	// 頂点フォーマット [3D]

#define MAX_OBJECT		(1024)	// オブジェクトの最大数
#define MAX_PRIO		(8)		// 優先順位の総数
#define DEFAULT_PRIO	(3)		// デフォルトの優先順位

//************************************************************
//	構造体定義
//************************************************************
// 頂点情報 [3D]
struct VERTEX_3D
{
	D3DXVECTOR3	pos;	// 頂点座標
	D3DXVECTOR3	nor;	// 法線ベクトル
	D3DCOLOR	col;	// 頂点カラー
	D3DXVECTOR2	tex;	// テクスチャ座標
};

//************************************************************
//	前方宣言
//************************************************************
class CObject2D;	// オブジェクト2Dクラス

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
		LABEL_PLAYER,		// プレイヤー
		LABEL_ENEMY,		// 敵
		LABEL_TARGET,		// ターゲット
		LABEL_MAGIC,		// 魔法
		LABEL_LOCK,			// ロックオン
		LABEL_BULLET,		// 弾
		LABEL_SHADOW,		// 影
		LABEL_FIELD,		// 地面
		LABEL_WALL,			// 壁
		LABEL_SCENERY,		// 景色
		LABEL_SEA,			// 海
		LABEL_SCORE,		// スコア
		LABEL_TIMER,		// タイマー
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
	virtual void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	virtual void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	virtual D3DXMATRIX GetMtxWorld(void) const;			// マトリックス取得
	virtual D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	virtual D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	virtual D3DXVECTOR3 GetScaling(void) const;			// 大きさ取得
	virtual float GetRadius(void) const;				// 半径取得

	// 静的メンバ関数
	static void ReleaseAll(void);	// 全破棄
	static void UpdateAll(void);	// 全更新
	static void DrawAll(void);		// 全描画
	static CObject *GetObject	// オブジェクト取得
	( // 引数
		const int nPriority,	// 優先順位
		const int nID			// インデックス
	);

	static bool CheckUse(const CObject *pObject);	// 使用確認

	// メンバ関数
	HRESULT GetResult(void) const;			// 失敗判定取得
	int   GetID(void);						// インデックス取得
	int   GetPriority(void);				// 優先順位取得
	void  SetLabel(const LABEL label);		// ラベル設定
	LABEL GetLabel(void) const;				// ラベル取得
	void  SetPriority(const int nPriority);	// 優先順位設定
	int   GetPriority(void) const;			// 優先順位取得

protected:
	// メンバ関数
	void Release(void);	// 破棄

private:
	// 静的メンバ変数
	static CObject *m_apObject[MAX_PRIO][MAX_OBJECT];	// オブジェクトの情報
	static int m_nNumAll;	// オブジェクトの総数

	// メンバ変数
	HRESULT m_hrResult;	// 自身の失敗判定
	LABEL m_label;		// 自身のオブジェクトラベル
	int   m_nPriority;	// 自身の優先順位
	int   m_nID;		// 自身のインデックス
};

#endif	// _OBJECT_H_
