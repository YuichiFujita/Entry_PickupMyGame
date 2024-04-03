//============================================================
//
//	衝撃波処理 [impact.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "impact.h"
#include "player.h"
#include "collision.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2	PART			= POSGRID2(64, 1);	// 分割数
	const POSGRID2	TEX_PART		= POSGRID2(32, 1);	// テクスチャ分割数
	const float		SUB_HEIGHT		= 2.0f;				// 衝撃波の外周高さ減算量
	const float		MOVE_TEXU		= 0.05f;			// テクスチャ横移動量
	const int		DMG_IMPACT		= 15;				// 衝撃波のダメージ量
	const int		ALPHA_NUMREF	= 10;				// αテストの参照値
}

//************************************************************
//	子クラス [CImpact] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CImpact::CImpact() :
	m_fMaxGrowRadius	(0.0f),						// 半径の最大成長量
	m_addGrow			(SGrow(0.0f, 0.0f, 0.0f))	// 成長加速量
{

}

//============================================================
//	デストラクタ
//============================================================
CImpact::~CImpact()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CImpact::Init(void)
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
void CImpact::Uninit(void)
{
	// 波動の終了
	CWave::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CImpact::Update(void)
{
	// 波動の更新
	CWave::Update();

	// プレイヤーとの当たり判定
	CollisionPlayer();

	// 成長を加速させる
	SGrow curGrow = GetGrow();	// 現在の成長情報
	SetGrow(SGrow
	( // 引数
		curGrow.fAddHoleRadius	+ m_addGrow.fAddHoleRadius,	// 穴の半径の成長量
		curGrow.fAddThickness	+ m_addGrow.fAddThickness,	// 太さの成長量
		curGrow.fSubAlpha		+ m_addGrow.fSubAlpha		// 透明度の成長量
	));

	if (GetHoleRadius() + GetThickness() > m_fMaxGrowRadius)
	{ // 半径の最大値に到達した場合

		float fHeight = GetOuterPlusY();	// 外周の高さ

		// 外周の高さを下げる
		fHeight -= SUB_HEIGHT;
		if (fHeight <= 0.0f)
		{ // 見えなくなった場合

			// 自身の終了
			Uninit();
			return;
		}

		// 外周の高さを反映
		SetOuterPlusY(fHeight);
	}
}

//============================================================
//	描画処理
//============================================================
void CImpact::Draw(CShader *pShader)
{
	// 波動の描画
	CWave::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CImpact *CImpact::Create
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
	// 衝撃波の生成
	CImpact *pImpact = new CImpact;
	if (pImpact == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 衝撃波の初期化
		if (FAILED(pImpact->Init()))
		{ // 初期化に失敗した場合

			// 衝撃波の破棄
			SAFE_DELETE(pImpact);
			return nullptr;
		}

		// テクスチャを設定
		pImpact->SetTexture(texture);

		// 位置を設定
		pImpact->SetVec3Position(rPos);

		// 色を設定
		pImpact->SetColor(rCol);

		// 成長情報を設定
		pImpact->SetGrow(rGrow);

		// 成長加速情報を設定
		pImpact->SetAddGrow(rAddGrow);

		// 穴の半径を設定
		pImpact->SetHoleRadius(fHoleRadius);

		// 太さを設定
		pImpact->SetThickness(fThickness);

		// 外周のY座標加算量を設定
		pImpact->SetOuterPlusY(fOuterPlusY);

		// 半径の最大成長量を設定
		pImpact->SetMaxGrowRadius(fMaxGrowRadius);

		// 確保したアドレスを返す
		return pImpact;
	}
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
void CImpact::CollisionPlayer(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();		// プレイヤーリスト
	if (pList == nullptr)		 { assert(false); return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { assert(false); return; }	// プレイヤーが1人じゃない
	auto player = pList->GetList().front();					// プレイヤー情報

	D3DXVECTOR3 posPlayer	= player->GetVec3Position();	// プレイヤーの位置
	D3DXVECTOR3 posImpact	= GetVec3Position();			// 衝撃波の位置
	float fRadiusPlayer		= player->GetRadius();			// プレイヤーの半径
	float fRadiusInImpact	= GetHoleRadius() - fRadiusPlayer;					// 衝撃波の内側半径
	float fRadiusOutImpact	= GetHoleRadius() + GetThickness() + fRadiusPlayer;	// 衝撃波の外側半径

	bool bHitIn  = !collision::Circle2D(posImpact, posPlayer, fRadiusInImpact,  0.0f);	// 衝撃波の内円の外側にいる
	bool bHitOut =  collision::Circle2D(posImpact, posPlayer, fRadiusOutImpact, 0.0f);	// 衝撃波の外円の内側にいる
	if (bHitIn && bHitOut)
	{ // 衝撃波に当たっている場合

		float fPlayerFoot = posPlayer.y;						// プレイヤー足位置
		float fPlayerHead = posPlayer.y + player->GetHeight();	// プレイヤー頭位置
		float fImpactDown = posImpact.y;						// 衝撃波下位置
		float fImpactUp   = posImpact.y + GetOuterPlusY();		// 衝撃波上位置
		if (fPlayerFoot <= fImpactUp
		&&  fPlayerHead >= fImpactDown)
		{ // 上下の範囲内の場合

			// プレイヤーのヒット処理
			D3DXVECTOR3 vecKnock = posPlayer - posImpact;
			player->HitKnockBack(DMG_IMPACT, vecKnock);
		}
	}
}
