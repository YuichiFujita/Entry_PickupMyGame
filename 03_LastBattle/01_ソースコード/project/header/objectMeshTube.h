//============================================================
//
//	オブジェクトメッシュチューブヘッダー [objectMeshTube.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_MESHTUBE_H_
#define _OBJECT_MESHTUBE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectMeshCircle;	// オブジェクトメッシュサークルクラス
class CObjectMeshCylinder;	// オブジェクトメッシュシリンダークラス
class CRenderState;			// レンダーステートクラス

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトメッシュチューブクラス
class CObjectMeshTube : public CObject
{
public:
	// 蓋列挙
	enum ECover
	{
		COVER_BOTTOM = 0,	// 下蓋
		COVER_TOP,			// 上蓋
		COVER_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CObjectMeshTube(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CObjectMeshTube() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void BindTexture(const int nTextureID) override;		// テクスチャ割当 (インデックス)
	void BindTexture(const char *pTexturePass) override;	// テクスチャ割当 (パス)
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	D3DXVECTOR3 GetVec3Rotation(void) const override;		// 向き取得
	void SetColor(const D3DXCOLOR& rCol) override;	// 色設定
	D3DXCOLOR GetColor(void) const override;		// 色取得
	void SetRadius(const float fRadius) override;	// 半径設定
	float GetRadius(void) const override;			// 半径取得
	void SetHeight(const float fHeight) override;	// 縦幅設定
	float GetHeight(void) const override;			// 縦幅取得
	void SetPriority(const int nPrio) override;		// 優先順位設定

	// 静的メンバ関数
	static CObjectMeshTube *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const POSGRID2& rTexPart,	// テクスチャ分割数
		const float fRadius,		// 半径
		const float fHeight			// 縦幅
	);

	// メンバ関数
	HRESULT SetPattern(const POSGRID2& rPart);		// 分割数設定
	POSGRID2 GetPattern(void) const;				// 分割数取得
	void SetTexPattern(const POSGRID2& rTexPart);	// テクスチャ分割数設定
	POSGRID2 GetTexPattern(void) const;				// テクスチャ分割数取得
	void SetRenderState(CRenderState renderState);	// レンダーステート情報設定
	CRenderState GetRenderState(void);				// レンダーステート情報取得
	void SetPositionRelative(void);					// 相対位置設定

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ関数
	D3DXMATRIX CalcCylinderMtxWorld(void) const;	// シリンダーマトリックス計算結果取得

	// メンバ変数
	CObjectMeshCircle *m_apCover[COVER_MAX];	// 蓋の情報
	CObjectMeshCylinder *m_pCylinder;			// 筒の情報
	POSGRID2 m_part;	// 分割数
	POSGRID2 m_texPart;	// テクスチャ分割数
};

#endif	// _OBJECT_MESHTUBE_H_
