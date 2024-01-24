//============================================================
//
//	オブジェクトメッシュシリンダーヘッダー [objectMeshCylinder.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_MESHCYLINDER_H_
#define _OBJECT_MESHCYLINDER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトメッシュシリンダークラス
class CObjectMeshCylinder : public CObject
{
public:
	// コンストラクタ
	CObjectMeshCylinder();
	CObjectMeshCylinder(const CObject::LABEL label, const int nPriority = DEFAULT_PRIO);

	// デストラクタ
	~CObjectMeshCylinder();

	// メッシュシリンダー構造体
	typedef struct
	{
		D3DXVECTOR3	pos;		// 位置
		D3DXVECTOR3	rot;		// 向き
		D3DXCOLOR	col;		// 色
		D3DXMATRIX	mtxWorld;	// ワールドマトリックス
		float fRadius;	// 半径
		float fHeight;	// 縦幅
		D3DCULL cull;	// カリング状況
		bool bLight;	// ライティング状況
	}MeshCylinder;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CObjectMeshCylinder *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const float fRadius,		// 半径
		const float fHeight,		// 縦幅
		const D3DCULL cull = D3DCULL_CCW,	// カリング状況
		const bool bLight = true			// ライティング状況
	);

	// メンバ関数
	void BindTexture(const int nTextureID);		// テクスチャ割当
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	void SetColor(const D3DXCOLOR& rCol);		// 色設定
	void SetRadius(const float fRadius);		// 半径設定
	void SetHeight(const float fHeight);		// 縦幅設定
	void SetCulling(const D3DCULL cull);		// カリング設定
	void SetLighting(const bool bLight);		// ライティング設定
	HRESULT SetPattern(const POSGRID2& rPart);	// 分割数設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	D3DXCOLOR GetColor(void) const;				// 色取得
	float GetRadius(void) const;				// 半径取得
	float GetHeight(void) const;				// 縦幅取得
	D3DCULL GetCulling(void) const;				// カリング取得
	bool GetLighting(void) const;				// ライティング取得
	POSGRID2 GetPattern(void) const;			// 分割数取得

protected:
	// メンバ関数
	void SetVtx(void);	// 頂点情報の設定
	void SetIdx(void);	// インデックス情報の設定

	void SetScrollTex	// スクロールのテクスチャ座標の設定
	( // 引数
		const float fTexU,	// テクスチャの横座標の開始位置
		const float fTexV	// テクスチャの縦座標の開始位置
	);

private:
	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9  m_pIdxBuff;	// インデックスバッファへのポインタ

	MeshCylinder m_meshCylinder;	// メッシュシリンダーの情報
	POSGRID2 m_part;	// 分割数
	int m_nNumVtx;		// 必要頂点数
	int m_nNumIdx;		// 必要インデックス数
	int m_nTextureID;	// テクスチャインデックス
};

#endif	// _OBJECT_MESHCYLINDER_H_
