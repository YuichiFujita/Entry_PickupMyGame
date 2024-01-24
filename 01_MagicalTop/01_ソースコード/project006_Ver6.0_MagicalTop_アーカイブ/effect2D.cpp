//============================================================
//
//	エフェクト2D処理 [effect2D.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "effect2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	マクロ定義
//************************************************************
#define EFF_PRIO	(2)	// エフェクト2Dの優先順位
#define PART_PRIO	(4)	// パーティクル2Dの優先順位

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CEffect2D::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\effect000.jpg",	// 通常テクスチャ
};

//************************************************************
//	子クラス [CEffect2D] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEffect2D::CEffect2D() : CObject2D(CObject::LABEL_EFFECT2D), m_type(TYPE_NORMAL)
{
	// メンバ変数をクリア
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
	m_nLife		= 0;	// 寿命
	m_fSubSize	= 0.0f;	// 大きさの減算量
	m_fSubAlpha	= 0.0f;	// 透明度の減算量
}

//============================================================
//	オーバーロードコンストラクタ
//============================================================
CEffect2D::CEffect2D(const TYPE type, const LABEL label) : CObject2D(CObject::LABEL_EFFECT2D, (label == LABEL_EFFECT2D) ? EFF_PRIO : PART_PRIO), m_type(type)
{
	// メンバ変数をクリア
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
	m_nLife		= 0;	// 寿命
	m_fSubSize	= 0.0f;	// 大きさの減算量
	m_fSubAlpha	= 0.0f;	// 透明度の減算量
}

//============================================================
//	デストラクタ
//============================================================
CEffect2D::~CEffect2D()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEffect2D::Init(void)
{
	// メンバ変数を初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
	m_nLife		= 0;	// 寿命
	m_fSubSize	= 0.0f;	// 大きさの減算量
	m_fSubAlpha	= 0.0f;	// 透明度の減算量

	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
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
void CEffect2D::Uninit(void)
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEffect2D::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos  = GetPosition();	// 位置
	D3DXVECTOR3 rot  = GetRotation();	// 向き
	D3DXVECTOR3 size = GetScaling();	// 大きさ
	D3DXCOLOR   col  = GetColor();		// 色

	if (m_nLife <= 0		// 寿命を迎えた
	||  size.x  <= 0.0f		// サイズが0.0f以下
	||  size.y  <= 0.0f		// サイズが0.0f以下
	||  col.a   <= 0.0f)	// 完全に透明
	{ // 上記のどれかになった場合

		// オブジェクトを破棄
		Uninit();

		// 関数を抜ける
		return;
	}

	// 移動量を加算
	pos += m_move;

	// 寿命を減算
	m_nLife--;

	// 半径を減算
	size.x -= m_fSubSize;
	size.y -= m_fSubSize;

	// α値を減算
	col.a -= m_fSubAlpha;

	// 位置を設定
	CObject2D::SetPosition(pos);

	// 向きを設定
	CObject2D::SetRotation(rot);

	// 大きさを設定
	CObject2D::SetScaling(size);

	// 色を設定
	CObject2D::SetColor(col);

	// オブジェクト2Dの更新
	CObject2D::Update();
}

//============================================================
//	描画処理
//============================================================
void CEffect2D::Draw(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// オブジェクト2Dの描画
	CObject2D::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//============================================================
//	生成処理
//============================================================
CEffect2D *CEffect2D::Create
(
	const TYPE type,			// テクスチャ
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rMove,	// 移動量
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXCOLOR& rCol,		// 色
	const int nLife,			// 寿命
	const float fRadius,		// 半径
	const float fSubSize,		// 半径の減算量
	const float fSubAlpha,		// 透明度の減算量
	const LABEL label			// オブジェクトラベル
)
{
	// 変数を宣言
	int nTextureID;	// テクスチャインデックス

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ
	CEffect2D *pEffect2D = NULL;	// エフェクト2D生成用

	if (UNUSED(pEffect2D))
	{ // 使用されていない場合

		// メモリ確保
		pEffect2D = new CEffect2D(type, label);	// エフェクト2D
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pEffect2D))
	{ // 確保に成功している場合

		// エフェクト2Dの初期化
		if (FAILED(pEffect2D->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEffect2D;
			pEffect2D = NULL;

			// 失敗を返す
			return NULL;
		}

		// テクスチャを登録
		nTextureID = pTexture->Regist(mc_apTextureFile[type]);

		// テクスチャを割当
		pEffect2D->BindTexture(nTextureID);

		// 位置を設定
		pEffect2D->SetPosition(rPos);

		// 向きを設定
		pEffect2D->SetRotation(rRot);

		// 大きさを設定
		pEffect2D->SetScaling(D3DXVECTOR3(fRadius, fRadius, 0.0f));

		// 色を設定
		pEffect2D->SetColor(rCol);

		// 引数の情報を設定
		pEffect2D->m_move		= rMove;		// 移動量
		pEffect2D->m_nLife		= nLife;		// 寿命
		pEffect2D->m_fSubSize	= fSubSize;		// 大きさの減算量
		pEffect2D->m_fSubAlpha	= fSubAlpha;	// 透明度の減算量

		// 確保したアドレスを返す
		return pEffect2D;
	}
	else { assert(false); return NULL; }	// 確保失敗
}
