//============================================================
//
//	モデル文字処理 [modelFont.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "modelFont.h"
#include "manager.h"
#include "model.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *MODEL_FILE[] =	// モデルファイル
	{
		"data\\MODEL\\FONT\\name_boss000.x",	// ボスの名前
	};
	const int PRIORITY = 5;	// モデル文字の優先順位
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(MODEL_FILE) == CModelFont::TYPE_MAX, "ERROR : Type Count Mismatch");

//************************************************************
//	子クラス [CModelFont] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CModelFont::CModelFont() : CObjectModel(CObject::LABEL_UI, CObject::DIM_3D, PRIORITY)
{

}

//============================================================
//	デストラクタ
//============================================================
CModelFont::~CModelFont()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CModelFont::Init(void)
{
	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CModelFont::Uninit(void)
{
	// オブジェクトモデルの終了
	CObjectModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CModelFont::Update(void)
{
	// オブジェクトモデルの更新
	CObjectModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CModelFont::Draw(CShader *pShader)
{
	// オブジェクトモデルの描画
	CObjectModel::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CModelFont *CModelFont::Create
( // 引数
	const EType type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rScale	// 大きさ
)
{
	// モデル文字の生成
	CModelFont *pModelFont = new CModelFont;
	if (pModelFont == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// モデル文字の初期化
		if (FAILED(pModelFont->Init()))
		{ // 初期化に失敗した場合

			// モデル文字の破棄
			SAFE_DELETE(pModelFont);
			return nullptr;
		}

		// モデルを登録・割当
		pModelFont->BindModel(GET_MANAGER->GetModel()->Regist(MODEL_FILE[type]));

		// 位置を設定
		pModelFont->SetVec3Position(rPos);

		// 向きを設定
		pModelFont->SetVec3Rotation(rRot);

		// 拡大率を設定
		pModelFont->SetVec3Scaling(rScale);

		// 確保したアドレスを返す
		return pModelFont;
	}
}
