//============================================================
//
//	炎処理 [fire.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "fire.h"
#include "collision.h"
#include "player.h"
#include "particle3D.h"
#include "shadow.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXVECTOR3 SHADOW_SIZE = D3DXVECTOR3(70.0f, 0.0f, 70.0f);	// 影の大きさ
	const float SHADOW_MAX_ALPHA  = 0.35f;	// 影の最大透明度

	const int	DMG_FIRE	= 30;		// 炎のダメージ量
	const float COLL_RADIUS	= 44.0f;	// 判定の半径
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CListManager<CFire> *CFire::m_pList = nullptr;	// オブジェクトリスト

//************************************************************
//	子クラス [CFire] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFire::CFire() : CObject(CObject::LABEL_FIRE, DIM_3D),
	m_pShadow	(nullptr),		// 影の情報
	m_pos		(VEC3_ZERO),	// 位置
	m_move		(VEC3_ZERO),	// 移動量
	m_nLife		(0)				// 寿命

{

}

//============================================================
//	デストラクタ
//============================================================
CFire::~CFire()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CFire::Init(void)
{
	// メンバ変数をクリア
	m_pShadow	= nullptr;		// 影の情報
	m_pos		= VEC3_ZERO;	// 位置
	m_move		= VEC3_ZERO;	// 移動量
	m_nLife		= 0;			// 寿命

	// 影の生成
	m_pShadow = CShadow::Create
	( // 引数
		CShadow::TEXTURE_NORMAL,	// 種類
		SHADOW_SIZE,				// 大きさ
		this,						// 親オブジェクト
		shadow::MIN_ALPHA,			// 透明度の最小値
		SHADOW_MAX_ALPHA			// 透明度の最大値
	);
	if (m_pShadow == nullptr)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	if (m_pList == nullptr)
	{ // リストマネージャーが存在しない場合

		// リストマネージャーの生成
		m_pList = CListManager<CFire>::Create();
		if (m_pList == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// リストに自身のオブジェクトを追加・イテレーターを取得
	m_iterator = m_pList->AddList(this);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CFire::Uninit(void)
{
	// 影の終了
	m_pShadow->DeleteObjectParent();	// 親オブジェクトを削除
	SAFE_UNINIT(m_pShadow);

	// リストから自身のオブジェクトを削除
	m_pList->DeleteList(m_iterator);

	if (m_pList->GetNumAll() == 0)
	{ // オブジェクトが一つもない場合

		// リストマネージャーの破棄
		m_pList->Release(m_pList);
	}

	// 炎オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CFire::Update(void)
{
	// 位置を移動
	m_pos += m_move;

	// 炎パーティクルを生成
	CParticle3D::Create(CParticle3D::TYPE_FIRE, m_pos);

	// プレイヤーとの当たり判定
	CollisionPlayer();

	if (m_nLife > NONE_IDX)
	{ // 寿命管理がされている場合

		// 寿命を減らす
		m_nLife--;
		if (m_nLife <= 0)
		{ // 寿命がなくなった場合

			// 自身の終了
			Uninit();
			return;
		}
	}

	// 影の更新
	m_pShadow->Update();
}

//============================================================
//	描画処理
//============================================================
void CFire::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	位置の設定処理
//============================================================
void CFire::SetVec3Position(const D3DXVECTOR3 &rPos)
{
	// 位置を設定
	m_pos = rPos;

	// 描画情報を設定
	assert(m_pShadow != nullptr);
	m_pShadow->SetDrawInfo();
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CFire::GetVec3Position(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	生成処理
//============================================================
CFire *CFire::Create
(
	const D3DXVECTOR3& rPos,	// 生成位置
	const int nLife				// 寿命
)
{
	// 炎の生成
	CFire *pFire = new CFire;
	if (pFire == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 炎の初期化
		if (FAILED(pFire->Init()))
		{ // 初期化に失敗した場合

			// 炎の破棄
			SAFE_DELETE(pFire);
			return nullptr;
		}

		// 位置を設定
		pFire->SetVec3Position(rPos);

		// 寿命を設定
		pFire->SetLife(nLife);

		// 確保したアドレスを返す
		return pFire;
	}
}

//============================================================
//	リスト取得処理
//============================================================
CListManager<CFire> *CFire::GetList(void)
{
	// オブジェクトリストを返す
	return m_pList;
}

//============================================================
//	破棄処理
//============================================================
void CFire::Release(void)
{
	// オブジェクトの破棄
	CObject::Release();
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
void CFire::CollisionPlayer(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();		// プレイヤーリスト
	if (pList == nullptr)		 { assert(false); return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { assert(false); return; }	// プレイヤーが1人じゃない
	auto player = pList->GetList().front();					// プレイヤー情報

	D3DXVECTOR3 posPlayer	 = player->GetVec3Position();	// プレイヤーの中心位置
				posPlayer.y += player->GetHeight() * 0.5f;	// 高さの半分上にあげる
	float fRadiusPlayer		 = player->GetRadius();			// プレイヤーの半径

	if (collision::Circle3D(m_pos, posPlayer, COLL_RADIUS, fRadiusPlayer))
	{ // 衝撃波に当たっている場合

		// プレイヤーのヒット処理
		player->Hit(DMG_FIRE);
	}
}
