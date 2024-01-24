//============================================================
//
//	オブジェクト2Dヘッダー [object2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクト2Dクラス
class CObject2D : public CObject
{
public:
	// コンストラクタ
	CObject2D();
	CObject2D(const CObject::LABEL label, const int nPriority = DEFAULT_PRIO);

	// デストラクタ
	~CObject2D();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CObject2D *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rSize = VEC3_ONE,	// 大きさ
		const D3DXVECTOR3& rRot = VEC3_ZERO,	// 向き
		const D3DXCOLOR& rCol = XCOL_WHITE		// 色
	);

	// メンバ関数
	void BindTexture(const int nTextureID);		// テクスチャ割当
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	void SetScaling(const D3DXVECTOR3& rSize);	// 大きさ設定
	void SetColor(const D3DXCOLOR& rCol);		// 色設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	D3DXVECTOR3 GetScaling(void) const;			// 大きさ取得
	D3DXCOLOR GetColor(void) const;				// 色取得

protected:
	// メンバ関数
	void SetVtx(void);	// 頂点情報の設定

	void SetAnimTex		// アニメーションのテクスチャ座標の設定
	( // 引数
		const int nPattern,		// アニメーションパターン
		const int nWidthPtrn,	// テクスチャの横の分割数
		const int nHeightPtrn	// テクスチャの縦の分割数
	);

	void SetScrollTex	// スクロールのテクスチャ座標の設定
	( // 引数
		const float fTexU,	// テクスチャの横座標の開始位置
		const float fTexV	// テクスチャの縦座標の開始位置
	);

private:
	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	D3DXVECTOR3	m_pos;	// 位置
	D3DXVECTOR3	m_rot;	// 向き
	D3DXVECTOR3	m_size;	// 大きさ
	D3DXCOLOR	m_col;	// 色
	float m_fAngle;		// 対角線の角度
	float m_fLength;	// 対角線の長さ
	int m_nTextureID;	// テクスチャインデックス
};

#endif	// _OBJECT2D_H_
