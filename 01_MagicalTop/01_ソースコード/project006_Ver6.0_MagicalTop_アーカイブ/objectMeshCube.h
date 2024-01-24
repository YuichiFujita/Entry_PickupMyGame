//============================================================
//
//	オブジェクトメッシュキューブヘッダー [objectMeshCube.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_MESHCUBE_H_
#define _OBJECT_MESHCUBE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトメッシュキューブクラス
class CObjectMeshCube : public CObject
{
public:
	// コンストラクタ
	CObjectMeshCube();
	CObjectMeshCube(const CObject::LABEL label, const int nPriority = DEFAULT_PRIO);

	// デストラクタ
	~CObjectMeshCube();

	// 縁取り列挙
	typedef enum
	{
		BORDERSTATE_OFF = 0,	// 縁取りなし
		BORDERSTATE_ON,			// 縁取りあり
		BORDERSTATE_MAX,		// この列挙型の総数
	}BORDERSTATE;

	// テクスチャ使用列挙
	typedef enum
	{
		TEXSTATE_ONE = 0,	// 同一テクスチャ
		TEXSTATE_SELECT,	// 全選択テクスチャ
		TEXSTATE_MAX,		// この列挙型の総数
	}TEXSTATE;

	// キューブ色列挙
	typedef enum
	{
		CUBECOL_CUBE = 0,	// キューブ色
		CUBECOL_BORDER,		// 縁取り色
		CUBECOL_MAX			// この列挙型の総数
	}CUBECOL;

	// キューブ分割数列挙
	typedef enum
	{
		CUBEPART_X = 0,	// テクスチャ分割数 x
		CUBEPART_Y,		// テクスチャ分割数 y
		CUBEPART_Z,		// テクスチャ分割数 z
		CUBEPART_MAX	// この列挙型の総数
	}CUBEPART;

	// テクスチャ構造体
	struct FACETEX
	{
	public:
		// コンストラクタ
		FACETEX() {}

		FACETEX(const int nAll)
		{ All = nAll; Left = 0; Right = 0; Bottom = 0; Top = 0; Near = 0; Far = 0; }

		FACETEX(const int nLeft, const int nRight, const int nBottom, const int nTop, const int nNear, const int nFar)
		{ All = 0; Left = nLeft; Right = nRight; Bottom = nBottom; Top = nTop; Near = nNear; Far = nFar; }

		FACETEX(const int nAll, const int nLeft, const int nRight, const int nBottom, const int nTop, const int nNear, const int nFar)
		{ All = nAll; Left = nLeft; Right = nRight; Bottom = nBottom; Top = nTop; Near = nNear; Far = nFar; }

		// デストラクタ
		~FACETEX() {}

		// メンバ変数
		int All;	// 全面のテクスチャインデックス
		int Left;	// 左のテクスチャインデックス
		int Right;	// 右のテクスチャインデックス
		int Bottom;	// 下のテクスチャインデックス
		int Top;	// 上のテクスチャインデックス
		int Near;	// 前のテクスチャインデックス
		int Far;	// 後のテクスチャインデックス
	};

	// メッシュキューブ構造体
	typedef struct
	{
		D3DXVECTOR3	pos;			// 位置
		D3DXVECTOR3	rot;			// 向き
		D3DXVECTOR3	size;			// 大きさ
		D3DXCOLOR	aCol[2];		// 色
		D3DXMATRIX	mtxWorld;		// ワールドマトリックス
		BORDERSTATE	bordState;		// 縁取り使用状態
		float		fBordThick;		// 縁取り太さ
		TEXSTATE	texState;		// テクスチャ使用状態
		FACETEX		texID;			// テクスチャ種類
		D3DXVECTOR2	aTexPart[3];	// テクスチャ分割数
		D3DCULL		cull;			// カリング状況
		bool		bLight;			// ライティング状況
	}MeshCube;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CObjectMeshCube *Create		// 生成
	( // 引数
		const D3DXVECTOR3& rPos,		// 位置
		const D3DXVECTOR3& rRot,		// 向き
		const D3DXVECTOR3& rSize,		// 大きさ
		const D3DXCOLOR& rCubeCol,		// キューブ色
		const D3DXCOLOR& rBorderCol,	// 縁取り色
		const BORDERSTATE bordState,	// 縁取り状態
		const float fBordThick,			// 縁取り太さ
		const TEXSTATE texState,		// テクスチャ状態
		const FACETEX& rTexID,			// テクスチャ種類
		const D3DXVECTOR2& rTexPartX = VEC2_ONE,	// テクスチャ分割数 x
		const D3DXVECTOR2& rTexPartY = VEC2_ONE,	// テクスチャ分割数 y
		const D3DXVECTOR2& rTexPartZ = VEC2_ONE,	// テクスチャ分割数 z
		const D3DCULL cull = D3DCULL_CCW,			// カリング状況
		const bool bLight = true					// ライティング状況
	);

	// メンバ関数
	void BindTexture(const FACETEX textureID);				// テクスチャ割当
	void SetPosition(const D3DXVECTOR3& rPos);				// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);				// 向き設定
	void SetScaling(const D3DXVECTOR3& rSize);				// 大きさ設定
	void SetCubeColor(const D3DXCOLOR& rCol);				// キューブ色設定
	void SetBorderColor(const D3DXCOLOR& rCol);				// 縁取り色設定
	HRESULT SetBorderState(const BORDERSTATE bordState);	// 縁取り状態設定
	void SetBorderThick(const float fBordThick);			// 縁取り太さ設定
	void SetTextureState(const TEXSTATE texState);			// テクスチャ状態設定
	void SetTexturePatternX(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数 x設定
	void SetTexturePatternY(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数 y設定
	void SetTexturePatternZ(const D3DXVECTOR2& rTexPart);	// テクスチャ分割数 z設定
	void SetCulling(const D3DCULL cull);					// カリング設定
	void SetLighting(const bool bLight);					// ライティング設定

	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	D3DXVECTOR3 GetScaling(void) const;			// 大きさ取得
	D3DXCOLOR GetCubeColor(void) const;			// キューブ色取得
	D3DXCOLOR GetBorderColor(void) const;		// 縁取り色取得
	BORDERSTATE GetBorderState(void) const;		// 縁取り状態取得
	float GetBorderThick(void) const;			// 縁取り太さ取得
	TEXSTATE GetTextureState(void) const;		// テクスチャ状態取得
	D3DXVECTOR2 GetTexturePatternX(void) const;	// テクスチャ分割数 x取得
	D3DXVECTOR2 GetTexturePatternY(void) const;	// テクスチャ分割数 y取得
	D3DXVECTOR2 GetTexturePatternZ(void) const;	// テクスチャ分割数 z取得
	D3DCULL GetCulling(void) const;				// カリング取得
	bool GetLighting(void) const;				// ライティング取得

protected:
	// メンバ関数
	void SetVtx(void);	// 頂点情報の設定
	void SetIdx(void);	// インデックス情報の設定

private:
	// 静的メンバ変数
	static const D3DXVECTOR3 aPosMeshCube[];	// 頂点座標の設定用定数
	static const D3DXVECTOR3 aNorMeshCube[];	// 法線ベクトルの設定用定数
	static const D3DXCOLOR aColMeshCube[];		// 頂点カラーの設定用定数
	static const D3DXVECTOR2 aTexMeshCube[];	// テクスチャ座標の設定用定数
	static const int aIdxMeshCube[];			// インデックスの設定用定数

	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9  m_pIdxBuff;	// インデックスバッファへのポインタ

	MeshCube m_meshCube;	// メッシュキューブの情報
	int m_nNumVtx;			// 必要頂点数
	int m_nNumIdx;			// 必要インデックス数
};

#endif	// _OBJECT_MESHCUBE_H_
