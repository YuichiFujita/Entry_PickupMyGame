//============================================================
//
//	マルチモデルヘッダー [multiModel.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MULTIMODEL_H_
#define _MULTIMODEL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "model.h"

//************************************************************
//	クラス定義
//************************************************************
// マルチモデルクラス
class CMultiModel
{
public:
	// コンストラクタ
	CMultiModel();

	// デストラクタ
	~CMultiModel();

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void BindModel(CModel::Model *pModel);	// モデル割当
	void SetParent(CMultiModel *pModel);	// 親モデル設定

	void SetMtxWorld(const D3DXMATRIX& rMtxWorld);	// マトリックス設定
	void SetPosition(const D3DXVECTOR3& rPos);		// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);		// 向き設定
	void SetScaling(const D3DXVECTOR3& rScale);		// 大きさ設定
	void SetMaterial(const D3DXMATERIAL& rMat);		// マテリアル設定
	void ResetMaterial(void);						// マテリアル再設定
	void SetAlpha(const float fAlpha);				// 透明度設定
	void SetModelData(const CModel::Model& rModel);	// モデル情報設定
	HRESULT SetMaterial(const LPD3DXBUFFER pBuffMat, const int nNumMat);	// マテリアル設定

	D3DXMATRIX *GetPtrMtxWorld(void);		// マトリックスポインタ取得
	D3DXMATRIX GetMtxWorld(void) const;		// マトリックス取得
	D3DXVECTOR3 GetPosition(void) const;	// 位置取得
	D3DXVECTOR3 GetRotation(void) const;	// 向き取得
	D3DXVECTOR3 GetScaling(void) const;		// 大きさ取得

	D3DXMATERIAL GetMaterial(const int nID) const;	// マテリアル取得
	float GetAlpha(void) const;						// 透明度取得
	float GetMaxAlpha(void) const;					// 最大透明度取得
	CMultiModel GetParent(void) const;				// 親取得
	CModel::Model GetModelData(void) const;			// モデル情報取得

	// 静的メンバ関数
	static CMultiModel *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 大きさ
	);
	static HRESULT Release(CMultiModel *&prMultiModel);	// 破棄

private:
	// メンバ変数
	CModel::Model	m_modelData;	// モデル情報
	CMultiModel		*m_pParent;		// 親モデルへのポインタ
	D3DXMATERIAL	*m_pMat;		// マテリアルへのポインタ
	D3DXMATRIX		m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3		m_pos;			// 位置
	D3DXVECTOR3		m_rot;			// 向き
	D3DXVECTOR3		m_scale;		// 大きさ
};

#endif	// _MULTIMODEL_H_
