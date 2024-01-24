//============================================================
//
//	オブジェクトモデルヘッダー [objectModel.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECTMODEL_H_
#define _OBJECTMODEL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"
#include "model.h"

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトモデルクラス
class CObjectModel : public CObject
{
public:
	// コンストラクタ
	CObjectModel();
	CObjectModel(const CObject::LABEL label, const int nPriority = DEFAULT_PRIO);

	// デストラクタ
	~CObjectModel();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CObjectModel *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 大きさ
	);

	// メンバ関数
	void BindModel(CModel::Model *pModel);			// モデル割当
	void SetMtxWorld(const D3DXMATRIX& rMtxWorld);	// マトリックス設定
	void SetPosition(const D3DXVECTOR3& rPos);		// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);		// 向き設定
	void SetScaling(const D3DXVECTOR3& rScale);		// 大きさ設定
	void SetModelData(const CModel::Model& rModel);	// モデル情報設定
	D3DXMATRIX GetMtxWorld(void) const;				// マトリックス取得
	D3DXVECTOR3 GetPosition(void) const;			// 位置取得
	D3DXVECTOR3 GetRotation(void) const;			// 向き取得
	D3DXVECTOR3 GetScaling(void) const;				// 大きさ取得
	CModel::Model GetModelData(void) const;			// モデル情報取得

private:
	// メンバ変数
	CModel::Model	m_modelData;	// モデル情報
	D3DXMATRIX		m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3		m_pos;			// 位置
	D3DXVECTOR3		m_rot;			// 向き
	D3DXVECTOR3		m_scale;		// 大きさ
};

#endif	// _OBJECTMODEL_H_
