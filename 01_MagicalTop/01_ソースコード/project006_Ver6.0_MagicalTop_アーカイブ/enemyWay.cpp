//============================================================
//
//	敵方向表示処理 [enemyWay.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemyWay.h"
#include "manager.h"
#include "scene.h"
#include "renderer.h"
#include "texture.h"
#include "target.h"

//************************************************************
//	マクロ定義
//************************************************************
#define WAY_PRIO	(2)		// 方向表示の優先順位
#define FADE_LEVEL	(0.04f)	// フェードのα値の加減量

#define RADIUS_MIN_WAY	(420.0f)	// 方向表示の大きさの最小値
#define RADIUS_MAX_WAY	(750.0f)	// 方向表示の大きさの最大値

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CEnemyWay::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\enemyWay000.png",	// 方向表示テクスチャ
};

//************************************************************
//	子クラス [CEnemyWay] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyWay::CEnemyWay(const float fMaxLimitRadius) : CObject3D(CObject::LABEL_WAY, WAY_PRIO), m_fMaxLimitRadius(fMaxLimitRadius)
{
	// メンバ変数をクリア
	m_pObject = NULL;		// 方向表示オブジェクト
	m_state = STATE_SPAWN;	// 状態
}

//============================================================
//	デストラクタ
//============================================================
CEnemyWay::~CEnemyWay()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyWay::Init(void)
{
	// メンバ変数を初期化
	m_pObject = NULL;		// 方向表示オブジェクト
	m_state = STATE_SPAWN;	// 状態

	// オブジェクト3Dの初期化
	if (FAILED(CObject3D::Init()))
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
void CEnemyWay::Uninit(void)
{
	// オブジェクト3Dの終了
	CObject3D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemyWay::Update(void)
{
	// 変数を宣言
	D3DXCOLOR colWay = GetColor();	// 方向表示の色

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_SPAWN:

		// 透明度を上げる
		colWay.a += FADE_LEVEL;

		if (colWay.a >= 1.0f)
		{ // 透明度が上がり切った場合

			// 透明度を補正
			colWay.a = 1.0f;

			// 状態を設定
			m_state = STATE_NORMAL;	// 通常状態
		}

		break;

	case STATE_NORMAL:

		// 無し

		break;

	case STATE_DELETE:

		// 透明度を下げる
		colWay.a -= FADE_LEVEL;

		if (colWay.a <= 0.0f)
		{ // 透明度が下がり切った場合

			// 透明度を補正
			colWay.a = 0.0f;

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}

		break;

	default:
		assert(false);
		break;
	}

	if (m_state != STATE_DELETE)
	{ // 破棄状態ではない場合

		// TODO：ポインタの確認もっときれいに
		if (CObject::CheckUse(m_pObject) && USED(CScene::GetTarget()))
		{ // 方向表示対象が使用されている且つ、ターゲットが使用されている場合

			// 変数を宣言
			D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
			D3DXVECTOR3 posWay = m_pObject->GetPosition();				// 方向表示位置
			float fDisTarget = sqrtf((posWay.x - posTarget.x) * (posWay.x - posTarget.x) + (posWay.z - posTarget.z) * (posWay.z - posTarget.z)) * 0.5f;	// ターゲットとの距離
			float fSizeWay = ((RADIUS_MAX_WAY - RADIUS_MIN_WAY) / m_fMaxLimitRadius) * fDisTarget + RADIUS_MIN_WAY;										// 方向表示の大きさ

			// 位置を設定
			SetPosition(CScene::GetTarget()->GetPosition());

			// 向きを設定
			SetRotation(D3DXVECTOR3(0.0f, atan2f(posWay.x - posTarget.x, posWay.z - posTarget.z), 0.0f));

			// 大きさを設定
			SetScaling(D3DXVECTOR3(fSizeWay, 0.0, fSizeWay));
		}
	}

	// 色を反映
	SetColor(colWay);

	// オブジェクト3Dの更新
	CObject3D::Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemyWay::Draw(void)
{
	// オブジェクト3Dの描画
	CObject3D::Draw();
}

//============================================================
//	生成処理
//============================================================
CEnemyWay *CEnemyWay::Create(CObject *pObject, const float fMaxLimitRadius)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ
	CEnemyWay *pEnemyWay = NULL;	// 敵方向表示生成用

	if (UNUSED(pEnemyWay))
	{ // 使用されていない場合

		// メモリ確保
		pEnemyWay = new CEnemyWay(fMaxLimitRadius);	// 敵方向表示
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pEnemyWay))
	{ // 確保に成功している場合

		// 敵方向表示の初期化
		if (FAILED(pEnemyWay->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEnemyWay;
			pEnemyWay = NULL;

			// 失敗を返す
			return NULL;
		}

		// テクスチャを登録・割当
		pEnemyWay->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_WAY]));

		// 原点を設定
		pEnemyWay->SetOrigin(ORIGIN_CENTER);

		// 位置を設定
		pEnemyWay->SetPosition(VEC3_ZERO);

		// 向きを設定
		pEnemyWay->SetRotation(VEC3_ZERO);

		// 大きさを設定
		pEnemyWay->SetScaling(VEC3_ZERO);

		// 色を設定
		pEnemyWay->SetColor(XCOL_AWHITE);

		// オブジェクトを設定
		pEnemyWay->SetObject(pObject);

		// カリングを設定
		pEnemyWay->SetCulling(D3DCULL_CCW);

		// ライティングを設定
		pEnemyWay->SetLighting(false);

		// Zテストを設定
		pEnemyWay->SetFunc(D3DCMP_ALWAYS);

		// Zバッファの使用状況を設定
		pEnemyWay->SetZEnable(false);

		// 確保したアドレスを返す
		return pEnemyWay;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	削除処理
//============================================================
void CEnemyWay::Delete(void)
{
	// 削除状態にする
	m_state = STATE_DELETE;
}

//============================================================
//	オブジェクトの設定処理
//============================================================
void CEnemyWay::SetObject(CObject *pObject)
{
	// 引数のオブジェクトを設定
	m_pObject = pObject;
}
