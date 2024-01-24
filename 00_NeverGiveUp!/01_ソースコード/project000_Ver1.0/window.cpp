//============================================================
//
//	窓処理 [window.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "window.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object3D.h"

#include "effect3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define WINDOW_FAR_PRIO		(4)	// 奥窓の優先順位
#define WINDOW_NEAR_PRIO	(5)	// 手前窓の優先順位

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CWindow::mc_apTextureFile[][WINDOW_MAX] =	// テクスチャ定数
{
	{ // 通常ビル窓テクスチャ
		"data\\TEXTURE\\wall000.png",			// 奥・裏テクスチャ
		"data\\TEXTURE\\buildingSide004.png",	// 奥・表テクスチャ
		"data\\TEXTURE\\window000.png",			// 手前・裏テクスチャ
		"data\\TEXTURE\\wall000.png",			// 手前・表テクスチャ
	},
};

CWindow::SStatusInfo CWindow::m_aStatusInfo[] =	// ステータス情報
{
	{ // 通常ビル窓ステータス
		D3DXVECTOR3(280.0f, 280.0f, 280.0f),	// 大きさ
	},
};

//************************************************************
//	子クラス [CWindow] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWindow::CWindow() : CObject(CObject::LABEL_WINDOW)
{
	// メンバ変数をクリア
	memset(&m_pWall[0], 0, sizeof(m_pWall));	// 窓の情報
	memset(&m_status, 0, sizeof(m_status));		// ステータス
	m_pos = VEC3_ZERO;		// 位置
	m_rot = VEC3_ZERO;		// 向き
	m_size = VEC3_ZERO;		// 大きさ
	m_type = TYPE_NORMAL;	// 種類
	m_fScale = 0.0f;		// 拡大率
}

//============================================================
//	デストラクタ
//============================================================
CWindow::~CWindow()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWindow::Init(void)
{
	// 変数配列を宣言
	static int aPrio[] = { WINDOW_FAR_PRIO, WINDOW_FAR_PRIO, WINDOW_NEAR_PRIO, WINDOW_NEAR_PRIO, };	// 優先順位

	// メンバ変数を初期化
	memset(&m_pWall[0], 0, sizeof(m_pWall));	// 窓の情報
	memset(&m_status, 0, sizeof(m_status));		// ステータス定数
	m_pos = VEC3_ZERO;		// 位置
	m_rot = VEC3_ZERO;		// 向き
	m_size = VEC3_ZERO;		// 大きさ
	m_type = TYPE_NORMAL;	// 種類定数
	m_fScale = 1.0f;		// 拡大率

	// 窓の生成
	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		// 壁の生成
		m_pWall[nCntWindow] = CObject3D::Create
		( // 引数
			VEC3_ZERO,	// 位置
			VEC3_ZERO,	// 大きさ
			VEC3_ZERO,	// 向き
			XCOL_WHITE,	// 色
			CObject3D::ORIGIN_CENTER	// 原点
		);
		if (m_pWall[nCntWindow] == NULL)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_pWall[nCntWindow]->SetPriority(aPrio[nCntWindow]);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CWindow::Uninit(void)
{
	// 窓の終了
	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の終了
			m_pWall[nCntWindow]->Uninit();
		}
		else { assert(false); }	// 非使用中
	}

	// 窓を破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CWindow::Update(void)
{
	// 窓の更新
	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の更新
			m_pWall[nCntWindow]->Update();
		}
		else { assert(false); }	// 非使用中
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	描画処理
//============================================================
void CWindow::Draw(void)
{

}

//============================================================
//	位置の設定処理
//============================================================
void CWindow::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CWindow::GetVec3Position(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	向きの設定処理
//============================================================
void CWindow::SetVec3Rotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;

	// 向きの正規化
	useful::Vec3NormalizeRot(m_rot);

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CWindow::GetVec3Rotation(void) const
{
	// 向きを返す
	return m_rot;
}

//============================================================
//	大きさの設定処理
//============================================================
void CWindow::SetVec3Sizing(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを代入
	m_size = rSize;

	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の大きさを設定
			m_pWall[nCntWindow]->SetVec3Sizing(D3DXVECTOR3(rSize.x, rSize.y, 0.0f) * 2.0f);
		}
		else { assert(false); }	// 非使用中
	}

	// 相対位置の設定
	SetPositionRelative();
}

//============================================================
//	大きさ取得処理
//============================================================
D3DXVECTOR3 CWindow::GetVec3Sizing(void) const
{
	// 大きさを返す
	return m_size;
}

//============================================================
//	色の設定処理
//============================================================
void CWindow::SetColor(const D3DXCOLOR& rCol)
{
	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の色を設定
			m_pWall[nCntWindow]->SetColor(rCol);
		}
		else { assert(false); }	// 非使用中
	}
}

//============================================================
//	更新状況の設定処理
//============================================================
void CWindow::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);	// 自身

	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の更新状況を設定
			m_pWall[nCntWindow]->SetEnableUpdate(bUpdate);
		}
		else { assert(false); }	// 非使用中
	}
}

//============================================================
//	描画状況の設定処理
//============================================================
void CWindow::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);	// 自身

	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の描画状況を設定
			m_pWall[nCntWindow]->SetEnableDraw(bDraw);
		}
		else { assert(false); }	// 非使用中
	}
}

//============================================================
//	種類の設定処理
//============================================================
void CWindow::SetType(const int nType)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	if (pTexture == NULL)
	{ // 使用されていない場合

		// 処理を抜ける
		assert(false);
		return;
	}

	if (nType > NONE_IDX && nType < TYPE_MAX)
	{ // 種類が範囲内の場合

		// 引数の種類を設定
		m_type = (EType)nType;

		// 引数の種類のステータスを設定
		m_status = m_aStatusInfo[m_type];	// ステータス情報
		SetVec3Sizing(m_status.size * m_fScale);	// 大きさ

		// 引数の種類のテクスチャを設定
		for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
		{ // 窓に使用する壁の総数分繰り返す

			if (m_pWall[nCntWindow] != NULL)
			{ // 使用中の場合

				// 壁のテクスチャを割り当て
				m_pWall[nCntWindow]->BindTexture(mc_apTextureFile[nType][nCntWindow]);
			}
			else { assert(false); }	// 非使用中
		}
	}
	else { assert(false); }	// 種類オーバー
}

//============================================================
//	種類取得処理
//============================================================
int CWindow::GetType(void) const
{
	// 種類を返す
	return m_type;
}

//============================================================
//	拡大率の設定処理
//============================================================
void CWindow::SetScale(const float fScale)
{
	// 引数の拡大率を設定
	m_fScale = fScale;

	// 大きさを設定
	SetVec3Sizing(m_status.size * m_fScale);
}

//============================================================
//	拡大率取得処理
//============================================================
float CWindow::GetScale(void) const
{
	// 拡大率を返す
	return m_fScale;
}

//============================================================
//	生成処理
//============================================================
CWindow *CWindow::Create
(
	const EType type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const float fScale			// 拡大率
)
{
	// ポインタを宣言
	CWindow *pWindow = NULL;	// 窓生成用

	if (pWindow == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pWindow = new CWindow;	// 窓
	}
	else { assert(false); return NULL; }	// 使用中

	if (pWindow != NULL)
	{ // 確保に成功している場合

		// 窓の初期化
		if (FAILED(pWindow->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pWindow;
			pWindow = NULL;

			// 失敗を返す
			return NULL;
		}

		// 種類を設定
		pWindow->SetType(type);

		// 位置を設定
		pWindow->SetVec3Position(rPos);

		// 向きを設定
		pWindow->SetVec3Rotation(rRot);

		// 拡大率を設定
		pWindow->SetScale(fScale);

		// 確保したアドレスを返す
		return pWindow;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	相対位置の設定処理
//============================================================
void CWindow::SetPositionRelative(void)
{
	// 変数を宣言
	D3DXVECTOR3 posNear = VEC3_ZERO;	// 手前の位置
	D3DXVECTOR3 posFar = VEC3_ZERO;		// 奥の位置

	// 奥の位置を求める
	posFar.x = m_pos.x + sinf(m_rot.y) * m_size.x;
	posFar.y = m_pos.y + m_size.y;
	posFar.z = m_pos.z + cosf(m_rot.y) * m_size.z;

	// 手前の位置を求める
	posNear.x = m_pos.x + sinf(m_rot.y + D3DX_PI) * m_size.x;
	posNear.y = m_pos.y + m_size.y;
	posNear.z = m_pos.z + cosf(m_rot.y + D3DX_PI) * m_size.z;

	// 変数配列を宣言
	D3DXVECTOR3 aPos[] =	// 位置
	{
		posFar,		// 手前
		posFar,		// 手前
		posNear,	// 奥
		posNear		// 奥
	};

	for (int nCntWindow = 0; nCntWindow < WINDOW_MAX; nCntWindow++)
	{ // 窓に使用する壁の総数分繰り返す

		// 変数を宣言
		D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, m_rot.y + (D3DX_PI * (float)(nCntWindow % 2)), 0.0f);	// 向き
		useful::Vec3NormalizeRot(rot);	// 向き正規化

		if (m_pWall[nCntWindow] != NULL)
		{ // 使用中の場合

			// 壁の位置を設定
			m_pWall[nCntWindow]->SetVec3Position(aPos[nCntWindow]);

			// 壁の向きを設定
			m_pWall[nCntWindow]->SetVec3Rotation(rot);
		}
		else { assert(false); }	// 非使用中
	}
}
