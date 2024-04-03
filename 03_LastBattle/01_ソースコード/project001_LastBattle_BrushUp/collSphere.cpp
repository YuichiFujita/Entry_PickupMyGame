//============================================================
//
//	円判定処理 [collSphere.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "collSphere.h"
#include "debugCollSphere.h"
#include "object.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
bool CCollSphere::m_bVisualColl = false;	// 判定見た目表示フラグ

//************************************************************
//	親クラス [CCollSphere] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CCollSphere::CCollSphere()
{
	// メンバ変数をクリア
	m_coll.clear();	// 要素をクリア
}

//============================================================
//	デストラクタ
//============================================================
CCollSphere::~CCollSphere()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CCollSphere::Init(void)
{
	// メンバ変数を初期化
	m_coll.clear();	// 要素をクリア

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CCollSphere::Uninit(void)
{
	// 要素をクリア
	m_coll.clear();
}

//============================================================
//	更新処理
//============================================================
void CCollSphere::Update(void)
{
#if _DEBUG

	for (auto info : m_coll)
	{ // 全要素分繰り返す

		// オフセットマトリックスを求める
		D3DXMATRIX mtxParent = m_pParent->GetMtxWorld();	// 親マトリックス
		D3DXMATRIX mtxOffset;	// オフセット計算マトリックス
		D3DXMatrixTranslation(&mtxOffset, info.offset.x, info.offset.y, info.offset.z);
		D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxParent);

		// 情報を見た目に反映
		D3DXVECTOR3 posColl = useful::GetMatrixPosition(mtxOffset);
		info.pVisual->SetVec3Position(posColl);	// 位置
		info.pVisual->SetRadius(info.fRadius);	// 半径

		// 見た目の表示状況を設定
		info.pVisual->SetEnableDraw(m_bVisualColl);
	}

#endif	// _DEBUG
}

//============================================================
//	判定位置の計算処理
//============================================================
D3DXVECTOR3 CCollSphere::CalcWorldPosition(const int nID) const
{
	auto info = m_coll.begin();	// 配列の先頭イテレーター
	info += nID;	// イテレーターをインデックス分動かす

	// オフセットマトリックスを求める
	D3DXMATRIX mtxParent = m_pParent->GetMtxWorld();	// 親マトリックス
	D3DXMATRIX mtxOffset;	// オフセット計算マトリックス
	D3DXMatrixTranslation(&mtxOffset, info->offset.x, info->offset.y, info->offset.z);
	D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxParent);

	// 判定位置を返す
	return useful::GetMatrixPosition(mtxOffset);
}

//============================================================
//	判定情報の設定処理
//============================================================
void CCollSphere::SetInfo(const SInfo& rInfo, const int nID)
{
	auto info = m_coll.begin();	// 配列の先頭イテレーター
	info += nID;	// イテレーターをインデックス分動かす

	// イテレーターの中身を設定
	*info = rInfo;
}

//============================================================
//	判定情報の取得処理
//============================================================
CCollSphere::SInfo CCollSphere::GetInfo(const int nID) const
{
	auto info = m_coll.begin();	// 配列の先頭イテレーター
	info += nID;	// イテレーターをインデックス分動かす

	// イテレーターの中身を返す
	return *info;
}

//============================================================
//	配列の設定処理
//============================================================
void CCollSphere::SetVector(const std::vector<SInfo>& rVector)
{
	// 配列を設定
	m_coll = rVector;
}

//============================================================
//	配列の取得処理
//============================================================
std::vector<CCollSphere::SInfo> CCollSphere::GetVector(void) const
{
	// 配列を返す
	return m_coll;
}

//============================================================
//	判定の追加処理
//============================================================
void CCollSphere::AddColl
(
	const D3DXVECTOR3& rOffset,	// 判定位置オフセット
	const float fRadius			// 判定半径
)
{
	// 情報を設定
	SInfo tempInfo;	// 設定用情報
	tempInfo.offset  = rOffset;	// オフセット
	tempInfo.fRadius = fRadius;	// 半径

#if _DEBUG

	// オフセットマトリックスを求める
	D3DXMATRIX mtxParent = m_pParent->GetMtxWorld();	// 親マトリックス
	D3DXMATRIX mtxOffset;	// オフセット計算マトリックス
	D3DXMatrixTranslation(&mtxOffset, rOffset.x, rOffset.y, rOffset.z);
	D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxParent);

	// オフセットワールド座標を見た目に設定
	D3DXVECTOR3 posColl = useful::GetMatrixPosition(mtxOffset);
	tempInfo.pVisual = CDebugCollSphere::Create(posColl, fRadius);

#endif	// _DEBUG

	// 配列最後尾に追加情報を設定
	m_coll.push_back(tempInfo);
}

//============================================================
//	判定の削除処理
//============================================================
void CCollSphere::SubColl(const int nID)
{
	auto info = m_coll.begin();	// 配列の先頭イテレーター
	info += nID;	// イテレーターをインデックス分動かす

#if _DEBUG

	// デバッグ用の見た目を終了
	SAFE_UNINIT(info->pVisual);

#endif	// _DEBUG

	// 配列の要素から削除
	m_coll.erase(info);
}

//============================================================
//	生成処理
//============================================================
CCollSphere *CCollSphere::Create(CObject *pParent)
{
	// 円判定の生成
	CCollSphere *pCollSphere = new CCollSphere;
	if (pCollSphere == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 円判定の初期化
		if (FAILED(pCollSphere->Init()))
		{ // 初期化に失敗した場合

			// 円判定の破棄
			SAFE_DELETE(pCollSphere);
			return nullptr;
		}

		// 親オブジェクトを設定
		pCollSphere->m_pParent = pParent;

		// 確保したアドレスを返す
		return pCollSphere;
	}
}

//============================================================
//	破棄処理
//============================================================
void CCollSphere::Release(CCollSphere *&prCollSphere)
{
	// 円判定の終了
	assert(prCollSphere != nullptr);
	prCollSphere->Uninit();

	// メモリ開放
	SAFE_DELETE(prCollSphere);
}
