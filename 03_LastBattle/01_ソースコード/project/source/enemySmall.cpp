//============================================================
//
//	雑魚敵処理 [enemySmall.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemySmall.h"
#include "manager.h"
#include "renderer.h"
#include "gauge3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_LIFEFRAME = "data\\TEXTURE\\lifeframe000.png";	// 体力フレーム表示のテクスチャファイル

	// 体力の情報
	namespace lifeInfo
	{
		const D3DXCOLOR		FRONT_COL	= D3DXCOLOR(1.0f, 0.3f, 0.4f, 1.0f);			// 表ゲージ色
		const D3DXCOLOR		BACK_COL	= D3DXCOLOR(0.03f, 0.008f, 0.01f, 1.0f);		// 裏ゲージ色
		const D3DXVECTOR3	GAUGE_SIZE	= D3DXVECTOR3(65.0f, 9.0f, 0.0f);				// ゲージの大きさ
		const D3DXVECTOR3	FRAME_SIZE	= GAUGE_SIZE + D3DXVECTOR3(0.5f, 0.5f, 0.0f);	// フレームの大きさ
		const int	CHANGE_FRAME		= 10;		// 表示値変動フレーム
		const float	PLUS_HEIGHT_POSY	= 20.0f;	// 体力表示Y座標の身長からの加算量
	}
}

//************************************************************
//	子クラス [CEnemySmall] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemySmall::CEnemySmall(const EType type) : CEnemy(type), m_pLife(nullptr)
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemySmall::~CEnemySmall()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemySmall::Init(void)
{
	// メンバ変数を初期化
	m_pLife = nullptr;	// 体力の情報

	// 敵の初期化
	if (FAILED(CEnemy::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 体力の生成
	SStatusInfo status = GetStatusInfo();	// ステータス情報
	float fPosUp = status.fHeight + lifeInfo::PLUS_HEIGHT_POSY;	// 表示Y位置の加算量
	m_pLife = CGauge3D::Create
	( // 引数
		this,					// ゲージ表示オブジェクト
		status.nMaxLife,		// 最大表示値
		lifeInfo::CHANGE_FRAME,	// 表示値変動フレーム
		fPosUp,					// 表示Y位置の加算量
		lifeInfo::GAUGE_SIZE,	// ゲージ大きさ
		lifeInfo::FRONT_COL,	// 表ゲージ色
		lifeInfo::BACK_COL,		// 裏ゲージ色
		true,					// 枠描画状況
		TEXTURE_LIFEFRAME,		// フレームテクスチャパス
		lifeInfo::FRAME_SIZE	// 枠大きさ
	);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemySmall::Uninit(void)
{
	// 自身の終了を伝える
	m_pLife->DeleteGauge();

	// 敵の終了
	CEnemy::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemySmall::Update(void)
{
	// 敵の更新
	CEnemy::Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemySmall::Draw(CShader *pShader)
{
	// 敵の描画
	CEnemy::Draw(pShader);
}
