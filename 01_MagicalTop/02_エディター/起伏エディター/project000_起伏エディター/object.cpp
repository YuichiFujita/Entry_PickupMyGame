//============================================================
//
//	オブジェクト処理 [object.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "manager.h"
#include "debugproc.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CObject *CObject::m_apObject[MAX_PRIO][MAX_OBJECT] = {};	// オブジェクトの情報
int CObject::m_nNumAll = 0;	// オブジェクトの総数

//************************************************************
//	親クラス [CObject] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CObject::CObject()
{
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{ // オブジェクトの最大数分繰り返す

		if (UNUSED(m_apObject[DEFAULT_PRIO][nCntObject]))
		{ // 使用されていない場合

			// 自身の情報アドレスを設定
			m_apObject[DEFAULT_PRIO][nCntObject] = this;

			// 自身のインデックスを設定
			m_nID = nCntObject;

			// 自身の情報をクリア
			m_label		= LABEL_NONE;	// オブジェクトラベル
			m_nPriority	= DEFAULT_PRIO;	// 優先順位

			// 自身の判定を設定
			m_hrResult = S_OK;	// 成功にする

			// オブジェクトの総数を加算
			m_nNumAll++;

			// 関数を抜ける
			return;
		}
	}

	// 自身の判定を設定
	m_hrResult = E_FAIL;	// 失敗にする
}

//============================================================
//	オーバーロードコンストラクタ
//============================================================
CObject::CObject(const LABEL label, const int nPriority)
{
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{ // オブジェクトの最大数分繰り返す

		if (UNUSED(m_apObject[nPriority][nCntObject]))
		{ // 使用されていない場合

			// 自身の情報アドレスを設定
			m_apObject[nPriority][nCntObject] = this;

			// 自身の情報を設定
			m_label		= label;		// オブジェクトラベル
			m_nID		= nCntObject;	// インデックス
			m_nPriority	= nPriority;	// 優先順位

			// 自身の判定を設定
			m_hrResult = S_OK;	// 成功にする

			// オブジェクトの総数を加算
			m_nNumAll++;

			// 関数を抜ける
			return;
		}
	}

	// 自身の判定を設定
	m_hrResult = E_FAIL;	// 失敗にする
}

//============================================================
//	デストラクタ
//============================================================
CObject::~CObject()
{

}

//============================================================
//	ヒット処理
//============================================================
void CObject::Hit(const int nDmg)
{
	// 例外処理
	assert(false);
}

//============================================================
//	位置の設定処理
//============================================================
void CObject::SetPosition(const D3DXVECTOR3& rPos)
{
	// 例外処理
	assert(false);
}

//============================================================
//	向きの設定処理
//============================================================
void CObject::SetRotation(const D3DXVECTOR3& rRot)
{
	// 例外処理
	assert(false);
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CObject::GetMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX mtxNone;	// 返り値用

	// マトリックスの初期化
	D3DXMatrixIdentity(&mtxNone);

	// 例外処理
	assert(false);

	// 単位マトリックスを返す
	return mtxNone;
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CObject::GetPosition(void) const
{
	// 例外処理
	assert(false);

	// 初期値を返す
	return VEC3_ZERO;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CObject::GetRotation(void) const
{
	// 例外処理
	assert(false);

	// 初期値を返す
	return VEC3_ZERO;
}

//============================================================
//	大きさ取得処理
//============================================================
D3DXVECTOR3 CObject::GetScaling(void) const
{
	// 例外処理
	assert(false);

	// 初期値を返す
	return VEC3_ZERO;
}

//============================================================
//	半径取得処理
//============================================================
float CObject::GetRadius(void) const
{
	// 例外処理
	assert(false);

	// 初期値を返す
	return 0.0f;
}

//============================================================
//	全破棄処理
//============================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの最大数分繰り返す

			if (USED(m_apObject[nCntPri][nCntObject]))
			{ // 使用されている場合

				if (m_apObject[nCntPri][nCntObject]->GetLabel() != LABEL_NONE)
				{ // オブジェクトラベルが設定されている場合

					// オブジェクトの終了
					m_apObject[nCntPri][nCntObject]->Uninit();
				}
			}
		}
	}

	// 例外処理
	if (m_nNumAll != 0) { assert(false); }	// 破棄の失敗
}

//============================================================
//	全更新処理
//============================================================
void CObject::UpdateAll(void)
{
	// デバッグ表示
	CManager::GetDebugProc()->Print("オブジェクト数：%d\n", m_nNumAll);

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの最大数分繰り返す

			if (USED(m_apObject[nCntPri][nCntObject]))
			{ // 使用されている場合

				if (m_apObject[nCntPri][nCntObject]->GetLabel() != LABEL_NONE)
				{ // オブジェクトラベルが設定されている場合

					// オブジェクトの更新
					m_apObject[nCntPri][nCntObject]->Update();
				}
			}
		}
	}
}

//============================================================
//	全描画処理
//============================================================
void CObject::DrawAll(void)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの最大数分繰り返す

			if (USED(m_apObject[nCntPri][nCntObject]))
			{ // 使用されている場合

				if (m_apObject[nCntPri][nCntObject]->GetLabel() != LABEL_NONE)
				{ // オブジェクトラベルが設定されている場合

					// オブジェクトの描画
					m_apObject[nCntPri][nCntObject]->Draw();
				}
			}
		}
	}
}

//============================================================
//	オブジェクト取得処理
//============================================================
CObject *CObject::GetObject(const int nPriority, const int nID)
{
	if (nPriority < MAX_PRIO && nID < MAX_OBJECT)
	{ // 引数の優先順位・インデックスが使用可能な場合

		// 引数の優先順位・インデックスのオブジェクトを返す
		return m_apObject[nPriority][nID];
	}
	else { assert(false); return NULL; }	// 範囲外
}

//============================================================
//	使用確認処理
//============================================================
bool CObject::CheckUse(const CObject *pObject)
{
	if (USED(pObject))
	{ // ポインタが使用中の場合

		for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
		{ // 優先順位の総数分繰り返す

			for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
			{ // オブジェクトの総数分繰り返す

				if (CObject::GetObject(nCntPri, nCntObject) == pObject)
				{ // 同アドレスの場合

					// 真を返す
					return true;	// TODO：同じアドレスが確保されているだけの可能性がある
				}
			}
		}

		// 偽を返す
		return false;
	}
	else
	{ // ポインタが非使用中の場合

		// 偽を返す
		return false;
	}
}

//============================================================
//	失敗判定の取得処理
//============================================================
HRESULT CObject::GetResult(void) const
{
	// 失敗判定を返す
	return m_hrResult;
}

//============================================================
//	インデックス取得処理
//============================================================
int CObject::GetID(void)
{
	// インデックスを返す
	return m_nID;
}

//============================================================
//	優先順位取得処理
//============================================================
int CObject::GetPriority(void)
{
	// 優先順位を返す
	return m_nPriority;
}

//============================================================
//	ラベルの設定処理
//============================================================
void CObject::SetLabel(const LABEL label)
{
	if (label < LABEL_MAX)
	{ // 引数のラベルが使用可能な場合

		// 引数のラベルを設定
		m_label = label;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	ラベルの取得処理
//============================================================
CObject::LABEL CObject::GetLabel(void) const
{
	// ラベルを返す
	return m_label;
}

//============================================================
//	優先順位の設定処理
//============================================================
void CObject::SetPriority(const int nPriority)
{
	if (nPriority < MAX_PRIO)
	{ // 引数の優先順位が使用可能な場合

		// 引数の優先順位を設定
		m_nPriority = nPriority;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	優先順位の取得処理
//============================================================
int CObject::GetPriority(void) const
{
	// 優先順位を返す
	return m_nPriority;
}

//============================================================
//	破棄処理
//============================================================
void CObject::Release(void)
{
	// 変数を宣言
	int nPriority = m_nPriority;	// 自身の優先順位
	int nID = m_nID;				// 自身のインデックス

	if (USED(m_apObject[nPriority][nID]))
	{ // 使用されている場合

		// メモリ開放
		delete m_apObject[nPriority][nID];
		m_apObject[nPriority][nID] = NULL;

		// オブジェクトの総数を減算
		m_nNumAll--;
	}
}
