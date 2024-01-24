//============================================================
//
//	看板処理 [signboard.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "signboard.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "multiModel.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SIGNBOARD_PRIO	(1)	// 看板の優先順位

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXVECTOR3 DEFAULT_SIZE = D3DXVECTOR3(250.0f, 130.0f, 6.0f);	// 看板のデフォルトの大きさ
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CSignboard::mc_apTextureFile[][6] =	// テクスチャ定数
{
	{ // 右テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 左テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 右テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 下テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 上テクスチャ
		"data\\TEXTURE\\signboard000.png",		// 前テクスチャ
		"data\\TEXTURE\\signboardBack000.png",	// 後テクスチャ
	},

	{ // 左テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 左テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 右テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 下テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 上テクスチャ
		"data\\TEXTURE\\signboard000.png",		// 前テクスチャ
		"data\\TEXTURE\\signboardBack000.png",	// 後テクスチャ
	},

	{ // 上テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 左テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 右テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 下テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 上テクスチャ
		"data\\TEXTURE\\signboard000.png",		// 前テクスチャ
		"data\\TEXTURE\\signboardBack000.png",	// 後テクスチャ
	},

	{ // 下テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 左テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 右テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 下テクスチャ
		"data\\TEXTURE\\signboardSide000.png",	// 上テクスチャ
		"data\\TEXTURE\\signboard000.png",		// 前テクスチャ
		"data\\TEXTURE\\signboardBack000.png",	// 後テクスチャ
	},
};

const char *CSignboard::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\SIGNBOARD\\signboard000.x",	// スタンドモデル
};

//************************************************************
//	子クラス [CSignboard] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSignboard::CSignboard() : CObjectMeshCube(CObject::LABEL_SIGNBOARD, SIGNBOARD_PRIO)
{
	// メンバ変数をクリア
	m_pStand = NULL;		// 看板スタンドの情報
	m_type   = TYPE_LEFT;	// 種類
	m_fScale = 0.0f;		// 拡大率
}

//============================================================
//	デストラクタ
//============================================================
CSignboard::~CSignboard()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSignboard::Init(void)
{
	// メンバ変数を初期化
	m_pStand = NULL;		// 看板スタンドの情報
	m_type   = TYPE_LEFT;	// 種類
	m_fScale = 1.0f;		// 拡大率

	//--------------------------------------------------------
	//	オブジェクトメッシュキューブの初期化
	//--------------------------------------------------------
	// オブジェクトメッシュキューブの初期化
	if (FAILED(CObjectMeshCube::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 原点を設定
	SetOrigin(CObjectMeshCube::ORIGIN_CENTER);

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

	//--------------------------------------------------------
	//	看板スタンドの初期化
	//--------------------------------------------------------
	// 看板スタンドの生成
	m_pStand = CMultiModel::Create(VEC3_ZERO, VEC3_ZERO);
	if (m_pStand == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルを割当
	m_pStand->BindModel(mc_apModelFile[MODEL_STAND]);

	// 親オブジェクトの設定
	m_pStand->SetParentObject(this);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSignboard::Uninit(void)
{
	// 看板スタンドの終了
	m_pStand->Uninit();

	// オブジェクトメッシュキューブの終了
	CObjectMeshCube::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CSignboard::Update(void)
{
	// 看板スタンドの更新
	m_pStand->Update();

	// オブジェクトメッシュキューブの更新
	CObjectMeshCube::Update();
}

//============================================================
//	描画処理
//============================================================
void CSignboard::Draw(void)
{
	// 看板スタンドの描画
	m_pStand->Draw();

	// オブジェクトメッシュキューブの描画
	CObjectMeshCube::Draw();
}

//============================================================
//	種類の設定処理
//============================================================
void CSignboard::SetType(const int nType)
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

	if (nType < TYPE_MAX)
	{ // 種類がある場合

		// 引数の種類を設定
		m_type = (EType)nType;

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

		// テクスチャを割当
		BindTexture(faceTex);
	}
	else { assert(false); }	// 種類オーバー
}

//============================================================
//	種類取得処理
//============================================================
int CSignboard::GetType(void) const
{
	// 種類を返す
	return m_type;
}

//============================================================
//	拡大率の設定処理
//============================================================
void CSignboard::SetScale(const float fScale)
{
	// 引数の拡大率を設定
	m_fScale = fScale;

	// メッシュキューブの大きさを設定
	SetVec3Sizing(DEFAULT_SIZE * fScale);

	// モデルの拡大率を設定
	m_pStand->SetVec3Scaling(VEC3_ALL(fScale));
}

//============================================================
//	拡大率取得処理
//============================================================
float CSignboard::GetScale(void) const
{
	// 拡大率を返す
	return m_fScale;
}

//============================================================
//	生成処理
//============================================================
CSignboard *CSignboard::Create
(
	const EType type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const float fScale,			// 拡大率
	const D3DXCOLOR& rCol		// 色
)
{
	// ポインタを宣言
	CSignboard *pSignboard = NULL;	// 看板生成用

	if (pSignboard == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pSignboard = new CSignboard;	// 看板
	}
	else { assert(false); return NULL; }	// 使用中

	if (pSignboard != NULL)
	{ // 確保に成功している場合

		// 看板の初期化
		if (FAILED(pSignboard->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pSignboard;
			pSignboard = NULL;

			// 失敗を返す
			return NULL;
		}

		// 種類を設定
		pSignboard->SetType(type);

		// 位置を設定
		pSignboard->SetVec3Position(rPos);

		// 向きを設定
		pSignboard->SetVec3Rotation(rRot);

		// 拡大率を設定
		pSignboard->SetScale(fScale);

		// キューブ色を設定
		pSignboard->SetColor(rCol);

		// 確保したアドレスを返す
		return pSignboard;
	}
	else { assert(false); return NULL; }	// 確保失敗
}
