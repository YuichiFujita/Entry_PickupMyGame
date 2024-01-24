//============================================================
//
//	マルチモデル処理 [multiModel.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "multiModel.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	親クラス [CMultiModel] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMultiModel::CMultiModel()
{
	// メンバ変数をクリア
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));		// ワールドマトリックス
	m_pParent = NULL;		// 親モデルへのポインタ
	m_pMat	= NULL;			// マテリアルへのポインタ
	m_pos	= VEC3_ZERO;	// 位置
	m_rot	= VEC3_ZERO;	// 向き
	m_scale	= VEC3_ZERO;	// 大きさ
}

//============================================================
//	デストラクタ
//============================================================
CMultiModel::~CMultiModel()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMultiModel::Init(void)
{
	// メンバ変数を初期化
	memset(&m_modelData, 0, sizeof(m_modelData));	// モデル情報
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));		// ワールドマトリックス
	m_pParent = NULL;		// 親モデルへのポインタ
	m_pMat	= NULL;			// マテリアルへのポインタ
	m_pos	= VEC3_ZERO;	// 位置
	m_rot	= VEC3_ZERO;	// 向き
	m_scale	= VEC3_ONE;		// 大きさ

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CMultiModel::Uninit(void)
{
	// マテリアルへのポインタを破棄
	if (USED(m_pMat))
	{ // ポインタが使用されていた場合

		// メモリ開放
		delete[] m_pMat;
		m_pMat = NULL;
	}
}

//============================================================
//	更新処理
//============================================================
void CMultiModel::Update(void)
{

}

//============================================================
//	描画処理
//============================================================
void CMultiModel::Draw(void)
{
	// 変数を宣言
	D3DXMATRIX   mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX   mtxParent;	// 親のマトリックス
	D3DMATERIAL9 matDef;	// 現在のマテリアル保存用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

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

	// 親マトリックスを設定
	if (UNUSED(m_pParent))
	{ // 親が存在しない場合

		// 現在のマトリックスを取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);	// 設定された最新のマトリックス (実体のマトリックス)
	}
	else
	{ // 親が存在する場合

		// 親のマトリックスを設定
		mtxParent = m_pParent->m_mtxWorld;
	}

	// ワールドマトリックスと親マトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルの設定
		pDevice->SetMaterial(&m_pMat[nCntMat].MatD3D);

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
CMultiModel *CMultiModel::Create(const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot, const D3DXVECTOR3& rScale)
{
	// ポインタを宣言
	CMultiModel *pMultiModel = NULL;	// マルチモデル生成用

	if (UNUSED(pMultiModel))
	{ // 使用されていない場合

		// メモリ確保
		pMultiModel = new CMultiModel;	// マルチモデル
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pMultiModel))
	{ // 確保に成功している場合

		// マルチモデルの初期化
		if (FAILED(pMultiModel->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pMultiModel;
			pMultiModel = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pMultiModel->SetPosition(rPos);

		// 向きを設定
		pMultiModel->SetRotation(rRot);

		// 大きさを設定
		pMultiModel->SetScaling(rScale);

		// 確保したアドレスを返す
		return pMultiModel;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CMultiModel::Release(CMultiModel *&prMultiModel)
{
	if (USED(prMultiModel))
	{ // 使用中の場合

		// マルチモデルの終了
		prMultiModel->Uninit();

		// メモリ開放
		delete prMultiModel;
		prMultiModel = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	モデル割当処理
//============================================================
void CMultiModel::BindModel(CModel::Model *pModel)
{
	if (USED(pModel))
	{ // 割り当てるモデルが使用中の場合

		// モデルを割り当て
		m_modelData = *pModel;

		// マテリアルの設定
		SetMaterial(m_modelData.pBuffMat, (int)m_modelData.dwNumMat);
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	親モデルの設定処理
//============================================================
void CMultiModel::SetParent(CMultiModel *pModel)
{
	// 引数のモデルポインタを親に設定
	m_pParent = pModel;
}

//============================================================
//	マトリックスの設定処理
//============================================================
void CMultiModel::SetMtxWorld(const D3DXMATRIX& rMtxWorld)
{
	// 引数のマトリックスを設定
	m_mtxWorld = rMtxWorld;
}

//============================================================
//	位置の設定処理
//============================================================
void CMultiModel::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CMultiModel::SetRotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_rot.x);
	useful::NormalizeRot(m_rot.y);
	useful::NormalizeRot(m_rot.z);
}

//============================================================
//	大きさの設定処理
//============================================================
void CMultiModel::SetScaling(const D3DXVECTOR3& rScale)
{
	// 引数の大きさを代入
	m_scale = rScale;
}

//============================================================
//	マテリアルの設定処理
//============================================================
void CMultiModel::SetMaterial(const D3DXMATERIAL& rMat)
{
	// 引数のマテリアルを全マテリアルに設定
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		m_pMat[nCntMat] = rMat;
	}
}

//============================================================
//	マテリアルの再設定処理
//============================================================
void CMultiModel::ResetMaterial(void)
{
	// ポインタを宣言
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ

	// マテリアルデータへのポインタを取得
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	// 全マテリアルに初期マテリアルを再設定
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		m_pMat[nCntMat] = pOriginMat[nCntMat];
	}
}

//============================================================
//	透明度の設定処理
//============================================================
void CMultiModel::SetAlpha(const float fAlpha)
{
	// ポインタを宣言
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ

	// マテリアルデータへのポインタを取得
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// 変数を宣言
		float fSetAlpha = fAlpha;	// 設定する透明度

		// 透明度の補正
		useful::LimitNum(fSetAlpha, 0.0f, pOriginMat[nCntMat].MatD3D.Diffuse.a);

		// 透明度を設定
		m_pMat[nCntMat].MatD3D.Diffuse.a = fSetAlpha;
	}
}

//============================================================
//	モデル情報の設定処理
//============================================================
void CMultiModel::SetModelData(const CModel::Model& rModel)
{
	// 引数のモデル情報を代入
	m_modelData = rModel;
}

//============================================================
//	マテリアルの設定処理
//============================================================
HRESULT CMultiModel::SetMaterial(const LPD3DXBUFFER pBuffMat, const int nNumMat)
{
	// ポインタを宣言
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ

	//--------------------------------------------------------
	//	メモリ開放・確保
	//--------------------------------------------------------
	if (USED(m_pMat))
	{ // ポインタが使用されていた場合

		// メモリ開放
		delete[] m_pMat;
		m_pMat = NULL;
	}

	if (UNUSED(m_pMat))
	{ // ポインタが使用されていない場合

		// マテリアル数分のメモリ確保
		m_pMat = new D3DXMATERIAL[nNumMat];

		if (USED(m_pMat))
		{ // 確保に成功した場合

			// メモリクリア
			memset(m_pMat, 0, sizeof(D3DXMATERIAL) * nNumMat);
		}
		else { assert(false); return E_FAIL; }	// 確保失敗
	}
	else { assert(false); return E_FAIL; }	// 使用中

	//--------------------------------------------------------
	//	マテリアル情報を設定
	//--------------------------------------------------------
	// マテリアルデータへのポインタを取得
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < nNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		// マテリアルデータをコピー
		m_pMat[nCntMat] = pOriginMat[nCntMat];
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	マトリックスポインタ取得処理
//============================================================
D3DXMATRIX *CMultiModel::GetPtrMtxWorld(void)
{
	// ワールドマトリックスのポインタを返す
	return &m_mtxWorld;
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CMultiModel::GetMtxWorld(void) const
{
	// ワールドマトリックスを返す
	return m_mtxWorld;
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CMultiModel::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CMultiModel::GetRotation(void) const
{
	// 向きを返す
	return m_rot;
}

//============================================================
//	大きさ取得処理
//============================================================
D3DXVECTOR3 CMultiModel::GetScaling(void) const
{
	// 大きさを返す
	return m_scale;
}

//============================================================
//	マテリアル取得処理
//============================================================
D3DXMATERIAL CMultiModel::GetMaterial(const int nID) const
{
	// 変数を宣言
	D3DXMATERIAL mat;	// 例外時のマテリアル

	// マテリアルのメモリをクリア
	ZeroMemory(&mat, sizeof(mat));

	if (nID < (int)m_modelData.dwNumMat)
	{ // 引数インデックスがマテリアルの最大数を超えていない場合

		// 引数インデックスのマテリアルを返す
		return m_pMat[nID];
	}
	else
	{ // 引数インデックスがマテリアルの最大数を超えている場合

		// 例外処理
		assert(false);

		// 例外時のマテリアルを返す
		return mat;
	}
}

//============================================================
//	透明度取得処理
//============================================================
float CMultiModel::GetAlpha(void) const
{
	// 変数を宣言
	float fAlpha = 0.0f;	// 最も不透明なマテリアルの透明度

	// 最も不透明な透明度を探す
	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		if (m_pMat[nCntMat].MatD3D.Diffuse.a > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のマテリアルの透明度を保存
			fAlpha = m_pMat[nCntMat].MatD3D.Diffuse.a;
		}
	}

	// 全パーツ内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	最大透明度取得処理
//============================================================
float CMultiModel::GetMaxAlpha(void) const
{
	// 変数を宣言
	float fAlpha = 0.0f;		// 最も不透明なマテリアルの透明度

	// ポインタを宣言
	D3DXMATERIAL *pOriginMat;	// マテリアルデータへのポインタ

	// マテリアルデータへのポインタを取得
	pOriginMat = (D3DXMATERIAL*)m_modelData.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_modelData.dwNumMat; nCntMat++)
	{ // マテリアルの数分繰り返す

		if (pOriginMat[nCntMat].MatD3D.Diffuse.a > fAlpha)
		{ // マテリアルの透明度がより不透明だった場合

			// 現在のマテリアルの透明度を保存
			fAlpha = pOriginMat[nCntMat].MatD3D.Diffuse.a;
		}
	}

	// 全パーツ内で最も不透明だったマテリアルの透明度を返す
	return fAlpha;
}

//============================================================
//	親取得処理
//============================================================
CMultiModel CMultiModel::GetParent(void) const
{
	// 親を返す
	return *m_pParent;
}

//============================================================
//	モデル情報取得処理
//============================================================
CModel::Model CMultiModel::GetModelData(void) const
{
	// モデル情報を返す
	return m_modelData;
}
