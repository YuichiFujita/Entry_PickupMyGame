//============================================================
//
//	オブジェクトモデル処理 [objectModel.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "objectModel.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	子クラス [CObjectModel] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObjectModel::CObjectModel()
{
	// メンバ変数をクリア
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));		// ワールドマトリックス
	m_pos	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_rot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
	m_scale	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 大きさ
}

//============================================================
//	オーバーロードコンストラクタ
//============================================================
CObjectModel::CObjectModel(const CObject::LABEL label, const int nPriority) : CObject(label, nPriority)
{
	// メンバ変数をクリア
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));		// ワールドマトリックス
	m_pos	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_rot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
	m_scale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 大きさ
}

//============================================================
//	デストラクタ
//============================================================
CObjectModel::~CObjectModel()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CObjectModel::Init(void)
{
	// メンバ変数を初期化
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));		// ワールドマトリックス
	m_pos	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_rot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);		// 大きさ

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CObjectModel::Uninit(void)
{
	// オブジェクトモデルを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CObjectModel::Update(void)
{

}

//============================================================
//	描画処理
//============================================================
void CObjectModel::Draw(void)
{
	// 変数を宣言
	D3DXMATRIX   mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;	// 現在のマテリアル保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ
	D3DXMATERIAL *pMat;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 拡大率を反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetTexture(m_modelData.pTextureID[nCntMat]));

		if (m_scale != VEC3_ONE)
		{ // 拡大率が変更されている場合

			// 頂点法線の自動正規化を有効にする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// モデルの描画
		m_modelData.pMesh->DrawSubset(nCntMat);

		// 頂点法線の自動正規化を無効にする
		pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//============================================================
//	生成処理
//============================================================
CObjectModel *CObjectModel::Create(const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot, const D3DXVECTOR3& rScale)
{
	// ポインタを宣言
	CObjectModel *pObjectModel = NULL;		// オブジェクトモデル生成用

	if (UNUSED(pObjectModel))
	{ // 使用されていない場合

		// メモリ確保
		pObjectModel = new CObjectModel;	// オブジェクトモデル
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pObjectModel))
	{ // 確保に成功している場合

		// オブジェクトモデルの初期化
		if (FAILED(pObjectModel->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pObjectModel;
			pObjectModel = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pObjectModel->SetPosition(rPos);

		// 向きを設定
		pObjectModel->SetRotation(rRot);

		// 大きさを設定
		pObjectModel->SetScaling(rScale);

		// 確保したアドレスを返す
		return pObjectModel;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	モデル割当処理
//============================================================
void CObjectModel::BindModel(CModel::Model *pModel)
{
	if (USED(pModel))
	{ // 割り当てるモデルが使用中の場合

		// モデルを割り当て
		m_modelData = *pModel;
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	マトリックスの設定処理
//============================================================
void CObjectModel::SetMtxWorld(const D3DXMATRIX& rMtxWorld)
{
	// 引数のマトリックスを設定
	m_mtxWorld = rMtxWorld;
}

//============================================================
//	位置の設定処理
//============================================================
void CObjectModel::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CObjectModel::SetRotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;
}

//============================================================
//	大きさの設定処理
//============================================================
void CObjectModel::SetScaling(const D3DXVECTOR3& rScale)
{
	// 引数の大きさを代入
	m_scale = rScale;
}

//============================================================
//	モデル情報の設定処理
//============================================================
void CObjectModel::SetModelData(const CModel::Model& rModel)
{
	// 引数のモデル情報を代入
	m_modelData = rModel;
}

//============================================================
//	マトリックスポインタ取得処理
//============================================================
D3DXMATRIX *CObjectModel::GetPtrMtxWorld(void)
{
	// ワールドマトリックスのポインタを返す
	return &m_mtxWorld;
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CObjectModel::GetMtxWorld(void) const
{
	// ワールドマトリックスを返す
	return m_mtxWorld;
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CObjectModel::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CObjectModel::GetRotation(void) const
{
	// 向きを返す
	return m_rot;
}

//============================================================
//	大きさ取得処理
//============================================================
D3DXVECTOR3 CObjectModel::GetScaling(void) const
{
	// 大きさを返す
	return m_scale;
}

//============================================================
//	モデル情報取得処理
//============================================================
CModel::Model CObjectModel::GetModelData(void) const
{
	// モデル情報を返す
	return m_modelData;
}
