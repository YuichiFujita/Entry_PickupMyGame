//============================================================
//
//	着地波動処理 [waveLand.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "waveLand.h"
#include "player.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2	PART			= POSGRID2(64, 1);	// 分割数
	const POSGRID2	TEX_PART		= POSGRID2(32, 1);	// テクスチャ分割数
	const float		SUB_ALPHA		= 0.05f;			// 着地波動の透明度減算量
	const float		MOVE_TEXU		= -0.05f;			// テクスチャ横移動量
	const int		ALPHA_NUMREF	= 10;				// αテストの参照値
}

//************************************************************
//	子クラス [CWaveLand] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWaveLand::CWaveLand() :
	m_fMaxGrowRadius	(0.0f),						// 半径の最大成長量
	m_addGrow			(SGrow(0.0f, 0.0f, 0.0f))	// 成長加速量
{

}

//============================================================
//	デストラクタ
//============================================================
CWaveLand::~CWaveLand()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWaveLand::Init(void)
{
	// メンバ変数を初期化
	m_fMaxGrowRadius = 0.0f;				// 半径の最大成長量
	m_addGrow = SGrow(0.0f, 0.0f, 0.0f);	// 成長加速量

	// 波動の初期化
	if (FAILED(CWave::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 分割数の設定
	if (FAILED(SetPattern(PART)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャ分割数の設定
	SetTexPattern(TEX_PART);

	// 横座標の移動量を設定
	SetMoveU(MOVE_TEXU);

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// αブレンドの設定
	pRenderState->SetAlphaBlend(CRenderState::BLEND_ADD);

	// αテストの設定
	pRenderState->SetAlphaTest(true);			// αテストの有効 / 無効の設定
	pRenderState->SetAlphaFunc(D3DCMP_GREATER);	// αテストの設定
	pRenderState->SetAlphaNumRef(ALPHA_NUMREF);	// αテストの参照値設定

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CWaveLand::Uninit(void)
{
	// 波動の終了
	CWave::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWaveLand::Update(void)
{
	// 波動の更新
	CWave::Update();

	if (GetHoleRadius() + GetThickness() > m_fMaxGrowRadius)
	{ // 半径の最大値に到達した場合

		// 透明度を減算
		SGrow curGrow = GetGrow();		// 現在の成長情報
		D3DXCOLOR curCol = GetColor();	// 現在の色
		SetColor(D3DXCOLOR(curCol.r, curCol.g, curCol.b, curCol.a - curGrow.fSubAlpha - SUB_ALPHA));
	}
	else
	{ // 半径の最大値に到達していない場合

		// 成長を加速させる
		SGrow curGrow = GetGrow();	// 現在の成長情報
		SetGrow(SGrow
		( // 引数
			curGrow.fAddHoleRadius	+ m_addGrow.fAddHoleRadius,	// 穴の半径の成長量
			curGrow.fAddThickness	+ m_addGrow.fAddThickness,	// 太さの成長量
			curGrow.fSubAlpha		+ m_addGrow.fSubAlpha		// 透明度の成長量
		));
	}
}

//============================================================
//	描画処理
//============================================================
void CWaveLand::Draw(CShader *pShader)
{
	// 波動の描画
	CWave::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CWaveLand *CWaveLand::Create
(
	const ETexture texture,		// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXCOLOR& rCol,		// 色
	const SGrow& rGrow,			// 成長量
	const SGrow& rAddGrow,		// 成長加速量
	const float fHoleRadius,	// 穴の半径
	const float fThickness,		// 太さ
	const float fOuterPlusY,	// 外周のY座標加算量
	const float fMaxGrowRadius	// 半径の最大成長量
)
{
	// 着地波動の生成
	CWaveLand *pWaveLand = new CWaveLand;
	if (pWaveLand == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 着地波動の初期化
		if (FAILED(pWaveLand->Init()))
		{ // 初期化に失敗した場合

			// 着地波動の破棄
			SAFE_DELETE(pWaveLand);
			return nullptr;
		}

		// テクスチャを設定
		pWaveLand->SetTexture(texture);

		// 位置を設定
		pWaveLand->SetVec3Position(rPos);

		// 色を設定
		pWaveLand->SetColor(rCol);

		// 成長情報を設定
		pWaveLand->SetGrow(rGrow);

		// 成長加速情報を設定
		pWaveLand->SetAddGrow(rAddGrow);

		// 穴の半径を設定
		pWaveLand->SetHoleRadius(fHoleRadius);

		// 太さを設定
		pWaveLand->SetThickness(fThickness);

		// 外周のY座標加算量を設定
		pWaveLand->SetOuterPlusY(fOuterPlusY);

		// 半径の最大成長量を設定
		pWaveLand->SetMaxGrowRadius(fMaxGrowRadius);

		// 確保したアドレスを返す
		return pWaveLand;
	}
}
