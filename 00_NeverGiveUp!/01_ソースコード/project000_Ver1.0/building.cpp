//============================================================
//
//	ビル処理 [building.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "building.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	マクロ定義
//************************************************************
#define BUILDING_PRIO	(1)	// ビルの優先順位

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CBuilding::mc_apTextureFile[][6] =	// テクスチャ定数
{
	{ // ビル00テクスチャ
		"data\\TEXTURE\\buildingSide000.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide000.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof000.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof000.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide000.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide000.png",	// 後テクスチャ
	},

	{ // ビル01テクスチャ
		"data\\TEXTURE\\buildingSide001.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide001.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof001.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof001.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide001.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide001.png",	// 後テクスチャ
	},

	{ // ビル02テクスチャ
		"data\\TEXTURE\\buildingSide002.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide002.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof002.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof002.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide002.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide002.png",	// 後テクスチャ
	},

	{ // ビル下テクスチャ
		"data\\TEXTURE\\buildingSide003.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide003.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide003.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide003.png",	// 後テクスチャ
	},

	{ // ビル中テクスチャ
		"data\\TEXTURE\\buildingSide004.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide004.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide004.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide004.png",	// 後テクスチャ
	},

	{ // ビル上テクスチャ
		"data\\TEXTURE\\buildingSide005.png",	// 左テクスチャ
		"data\\TEXTURE\\buildingSide005.png",	// 右テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 下テクスチャ
		"data\\TEXTURE\\buildingRoof003.png",	// 上テクスチャ
		"data\\TEXTURE\\buildingSide005.png",	// 前テクスチャ
		"data\\TEXTURE\\buildingSide005.png",	// 後テクスチャ
	},
};

CBuilding::SStatusInfo CBuilding::m_aStatusInfo[] =	// ステータス情報
{
	{ // ビル00ステータス
		D3DXVECTOR3(280.0f, 560.0f, 280.0f),	// 大きさ
	},

	{ // ビル01ステータス
		D3DXVECTOR3(280.0f, 560.0f, 280.0f),	// 大きさ
	},

	{ // ビル02ステータス
		D3DXVECTOR3(280.0f, 560.0f, 280.0f),	// 大きさ
	},

	{ // ビル下ステータス
		D3DXVECTOR3(280.0f, 280.0f, 280.0f),	// 大きさ
	},

	{ // ビル中間ステータス
		D3DXVECTOR3(280.0f, 280.0f, 280.0f),	// 大きさ
	},

	{ // ビル上ステータス
		D3DXVECTOR3(280.0f, 280.0f, 280.0f),	// 大きさ
	},
};

//************************************************************
//	子クラス [CBuilding] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CBuilding::CBuilding() : CObjectMeshCube(CObject::LABEL_BUILDING, BUILDING_PRIO)
{
	// メンバ変数をクリア
	memset(&m_status, 0, sizeof(m_status));	// ステータス定数
	m_collision = COLLISION_NONE;	// 当たり判定
	m_type = TYPE_00;	// 種類定数
	m_fScale = 0.0f;	// 拡大率
}

//============================================================
//	デストラクタ
//============================================================
CBuilding::~CBuilding()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CBuilding::Init(void)
{
	// メンバ変数を初期化
	memset(&m_status, 0, sizeof(m_status));	// ステータス定数
	m_collision = COLLISION_NONE;	// 当たり判定
	m_type = TYPE_00;	// 種類定数
	m_fScale = 1.0f;	// 拡大率

	// オブジェクトメッシュキューブの初期化
	if (FAILED(CObjectMeshCube::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 原点を設定
	SetOrigin(CObjectMeshCube::ORIGIN_DOWN);

	// キューブ色を設定
	SetColor(XCOL_WHITE);

	// 縁取り色を設定
	SetBorderColor(XCOL_WHITE);

	// 縁取りの状態を設定
	if (FAILED(SetBorderState(CObjectMeshCube::BORDER_OFF)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 縁取りの太さを設定
	SetBorderThick(0.0f);

	// テクスチャの状態を設定
	SetTextureState(CObjectMeshCube::TEXSTATE_SELECT);

	// テクスチャの分割数Xを設定
	SetTexturePatternX(VEC2_ONE);

	// テクスチャの分割数Yを設定
	SetTexturePatternY(VEC2_ONE);

	// テクスチャの分割数Zを設定
	SetTexturePatternZ(VEC2_ONE);

	// カリングを設定
	SetCulling(D3DCULL_CCW);

	// ライティングを設定
	SetLighting(true);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CBuilding::Uninit(void)
{
	// オブジェクトメッシュキューブの終了
	CObjectMeshCube::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CBuilding::Update(void)
{
	// オブジェクトメッシュキューブの更新
	CObjectMeshCube::Update();
}

//============================================================
//	描画処理
//============================================================
void CBuilding::Draw(void)
{
	// オブジェクトメッシュキューブの描画
	CObjectMeshCube::Draw();
}

//============================================================
//	種類の設定処理
//============================================================
void CBuilding::SetType(const int nType)
{
	// 変数を宣言
	SFaceTex faceTex;	// テクスチャインデックス設定用

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

		// 引数の種類のテクスチャを登録
		faceTex = SFaceTex
		( // 引数
			pTexture->Regist(mc_apTextureFile[nType][0]),	// 左
			pTexture->Regist(mc_apTextureFile[nType][1]),	// 右
			pTexture->Regist(mc_apTextureFile[nType][2]),	// 下
			pTexture->Regist(mc_apTextureFile[nType][3]),	// 上
			pTexture->Regist(mc_apTextureFile[nType][4]),	// 前
			pTexture->Regist(mc_apTextureFile[nType][5])	// 後
		);
		BindTexture(faceTex);	// テクスチャを割当
	}
	else { assert(false); }	// 種類オーバー
}

//============================================================
//	種類取得処理
//============================================================
int CBuilding::GetType(void) const
{
	// 種類を返す
	return m_type;
}

//============================================================
//	当たり判定の設定処理
//============================================================
void CBuilding::SetState(const int nState)
{
	if (nState > NONE_IDX && nState < COLLISION_MAX)
	{ // インデックスが範囲内の場合

		// 引数の当たり判定を設定
		m_collision = (ECollision)nState;
	}
	else { assert(false); }	// 範囲外
}

//============================================================
//	当たり判定取得処理
//============================================================
int CBuilding::GetState(void) const
{
	// 当たり判定を返す
	return m_collision;
}

//============================================================
//	拡大率の設定処理
//============================================================
void CBuilding::SetScale(const float fScale)
{
	// 引数の拡大率を設定
	m_fScale = fScale;

	// 大きさを設定
	SetVec3Sizing(m_status.size * m_fScale);
}

//============================================================
//	拡大率取得処理
//============================================================
float CBuilding::GetScale(void) const
{
	// 拡大率を返す
	return m_fScale;
}

//============================================================
//	生成処理
//============================================================
CBuilding *CBuilding::Create
(
	const EType type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const ECollision collision,	// 当たり判定
	const float fScale			// 拡大率
)
{
	// ポインタを宣言
	CBuilding *pBuilding = NULL;	// ビル生成用

	if (pBuilding == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pBuilding = new CBuilding;	// ビル
	}
	else { assert(false); return NULL; }	// 使用中

	if (pBuilding != NULL)
	{ // 確保に成功している場合

		// ビルの初期化
		if (FAILED(pBuilding->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pBuilding;
			pBuilding = NULL;

			// 失敗を返す
			return NULL;
		}

		// 種類を設定
		pBuilding->SetType(type);

		// 位置を設定
		pBuilding->SetVec3Position(rPos);

		// 向きを設定
		pBuilding->SetVec3Rotation(rRot);

		// 当たり判定を設定
		pBuilding->SetState(collision);

		// 拡大率を設定
		pBuilding->SetScale(fScale);

		// 確保したアドレスを返す
		return pBuilding;
	}
	else { assert(false); return NULL; }	// 確保失敗
}
