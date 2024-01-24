//============================================================
//
//	風処理 [wind.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "wind.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "collision.h"
#include "scene.h"
#include "sceneGame.h"
#include "stage.h"
#include "field.h"

//************************************************************
//	マクロ定義
//************************************************************
#define WIND_COL	(D3DXCOLOR(0.6f, 0.9f, 1.0f, 0.6f))	// 風のポリゴン色
#define WIND_PRIO	(5)	// 風の優先順位

#define MOVE_BACK	(16.0f)		// 風の奥移動量
#define MOVE_SIDE	(15.5f)		// 風の横移動量
#define PLUS_POSY	(20.0f)		// 風の縦座標加算量
#define COLL_RADIUS	(50.0f)		// 風の当たり判定の半径

#define WIND_TEXPART	(1)		// 風のテクスチャ分割数
#define WIND_PART		(40)	// 風の分割数

//************************************************************
//	子クラス [CWind] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWind::CWind() : CObjectOrbit(CObject::LABEL_WIND, WIND_PRIO)
{
	// メンバ変数をクリア
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	m_pos = VEC3_ZERO;		// 位置
	m_state = STATE_NORMAL;	// 状態
}

//============================================================
//	デストラクタ
//============================================================
CWind::~CWind()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWind::Init(void)
{
	// メンバ変数を初期化
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	m_pos = VEC3_ZERO;		// 位置
	m_state = STATE_NORMAL;	// 状態

	// 軌跡の初期化
	if (FAILED(CObjectOrbit::Init()))
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
void CWind::Uninit(void)
{
	// 軌跡の終了
	CObjectOrbit::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWind::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 vecTarg, vecSide;	// ターゲット方向ベクトル・横方向ベクトル

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:

		// ターゲット方向のベクトルを計算
		vecTarg = CScene::GetStage()->GetStageLimit().center - m_pos;
		vecTarg.y = 0.0f;						// ベクトルの縦方向を無視
		D3DXVec3Normalize(&vecTarg, &vecTarg);	// ベクトルの正規化

		// 横方向ベクトルを計算
		vecSide = D3DXVECTOR3(vecTarg.z, 0.0f, -vecTarg.x);

		// 奥移動量を加算
		m_pos += vecTarg * MOVE_BACK;

		// 横移動量を加算
		m_pos += vecSide * MOVE_SIDE;

		// 縦座標を地面に添わせる
		if (CScene::GetField()->IsPositionRange(m_pos))
		{ // 地面の範囲内の場合

			// 縦座標を地形に添わせる
			m_pos.y = CScene::GetField()->GetPositionHeight(m_pos);
			m_pos.y += PLUS_POSY;
		}

		// バリアとの当たり判定
		if (collision::Circle2D(CScene::GetStage()->GetStageBarrierPosition(), m_pos, COLL_RADIUS, 0.0f))
		{ // 当たっていた場合

			// 状態を設定
			m_state = STATE_DELETE;	// 消失状態

			// 軌跡を消失状態に設定
			SetState(CObjectOrbit::STATE_VANISH);
		}

		break;

	case STATE_DELETE:

		if (GetState() == CObjectOrbit::STATE_NONE)
		{ // 軌跡がなくなり切った場合

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}

		break;
	}

	// 軌跡の更新
	CObjectOrbit::Update();
}

//============================================================
//	描画処理
//============================================================
void CWind::Draw(void)
{
	// 変数を宣言
	D3DXMATRIX mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 軌跡の描画
	CObjectOrbit::Draw();
}

//============================================================
//	生成処理
//============================================================
CWind *CWind::Create(const D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	CWind *pWind = NULL;	// 風生成用

	if (UNUSED(pWind))
	{ // 使用されていない場合

		// メモリ確保
		pWind = new CWind;	// 風
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pWind))
	{ // 確保に成功している場合

		// 風の初期化
		if (FAILED(pWind->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pWind;
			pWind = NULL;

			// 失敗を返す
			return NULL;
		}

		// テクスチャを割当
		pWind->BindTexture(NONE_IDX);

		// 位置を設定
		pWind->SetPosition(rPos);

		// 親のマトリックスを設定
		pWind->SetMatrixParent(pWind->GetPtrMtxWorld());

		// 色を設定
		pWind->SetColor(WIND_COL);

		// オフセットを設定
		pWind->SetOffset(CObjectOrbit::OFFSET_WIND);

		// テクスチャ分割数を設定
		pWind->SetTexPart(WIND_TEXPART);

		// 透明化状況を設定
		pWind->SetEnableAlpha(true);

		// 長さを設定
		if (FAILED(pWind->SetLength(WIND_PART)))
		{ // 長さの設定に失敗した場合

			// メモリ開放
			delete pWind;
			pWind = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pWind;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	位置の設定処理
//============================================================
void CWind::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;
}

//============================================================
//	マトリックスポインタ取得処理
//============================================================
D3DXMATRIX *CWind::GetPtrMtxWorld(void)
{
	// ワールドマトリックスのポインタを返す
	return &m_mtxWorld;
}
