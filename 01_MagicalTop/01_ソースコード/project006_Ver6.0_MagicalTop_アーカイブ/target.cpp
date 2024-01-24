//============================================================
//
//	ターゲット処理 [target.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "target.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "model.h"
#include "stage.h"
#include "field.h"
#include "scene.h"
#include "sceneGame.h"
#include "waveManager.h"
#include "objectMeshCube.h"
#include "objectGauge3D.h"
#include "shadow.h"
#include "warningAttack.h"
#include "flower.h"
#include "particle3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define TARG_SHADOW_SIZE	(D3DXVECTOR3(200.0f, 0.0f, 200.0f))	// 影の大きさ

#define CUBE_COL	(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f))	// キューブの色
#define CUBE_SIZE	(D3DXVECTOR3(25.0f, 25.0f, 25.0f))	// キューブの大きさ
#define CUBE_BORD	(2.5f)		// キューブの縁取りの太さ
#define TARG_RADIUS	(100.0f)	// ターゲット半径
#define ADD_POS_Y	(92.0f)		// 台座位置からのキューブ相対位置の加算量
#define MUL_SIN_POS	(10.0f)		// サインカーブの補正係数

#define ADD_SIN_ROT	(D3DXToRadian(1))	// 浮遊向きの加算量
#define ADD_CUBEROT	(D3DXVECTOR3(0.01f, 0.001f, 0.01f))	// キューブの回転量

#define TARG_LIFE		(2000)		// ターゲットの体力
#define GAUGE_PLUS_Y	(160.0f)	// ターゲットのY位置の加算量
#define GAUGE_GAUGESIZE	(D3DXVECTOR3(100.0f, 20.0f, 0.0f))	// ゲージサイズ
#define GAUGE_FRAMESIZE	(D3DXVECTOR3(102.5f, 22.5f, 0.0f))	// ゲージフレームサイズ
#define GAUGE_FRONTCOL	(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))	// 表ゲージ色
#define GAUGE_BACKCOL	(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f))	// 裏ゲージ色

#define WARNING_POS		(D3DXVECTOR3(250.0f, 610.0f, 0.0f))	// 警告表示の位置
#define WARNING_SIZE	(D3DXVECTOR3(400.0f, 200.0f, 0.0f))	// 警告表示の大きさ

#define STATE_HEAL_CNT	(480)	// 回復状態に移行するまでのカウンター
#define NORMAL_CNT		(60)	// 通常状態に移行するまでのカウンター
#define WAIT_HEAL_CNT	(180)	// 回復までのカウンター
#define HEAL_EFF_COL	(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))	// 回復時のエフェクト色

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CTarget::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\TARGET\\pedestal000.x",	// 台座モデル
};

//************************************************************
//	子クラス [CTarget] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTarget::CTarget() : CObjectModel(CObject::LABEL_TARGET)
{
	// メンバ変数をクリア
	m_pMeshCube		= NULL;			// メッシュキューブの情報
	m_pLife			= NULL;			// 体力の情報
	m_pShadow		= NULL;			// 影の情報
	m_pWarning		= NULL;			// 攻撃警告表示の情報
	m_state			= STATE_NORMAL;	// 状態
	m_fSinRot		= 0.0f;			// 浮遊向き
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nCounterHeal	= 0;			// 回復管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CTarget::~CTarget()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CTarget::Init(void)
{
	// メンバ変数を初期化
	m_pMeshCube		= NULL;			// メッシュキューブの情報
	m_pLife			= NULL;			// 体力の情報
	m_pShadow		= NULL;			// 影の情報
	m_pWarning		= NULL;			// 攻撃警告表示の情報
	m_state			= STATE_NORMAL;	// 状態
	m_fSinRot		= 0.0f;			// 浮遊向き
	m_nCounterState	= 0;			// 状態管理カウンター
	m_nCounterHeal	= 0;			// 回復管理カウンター

	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// メッシュキューブの生成
	m_pMeshCube = CObjectMeshCube::Create
	( // 引数
		VEC3_ZERO,	// 位置
		VEC3_ZERO,	// 向き
		CUBE_SIZE,	// 大きさ
		CUBE_COL,	// キューブ色
		XCOL_BLACK,	// 縁取り色
		CObjectMeshCube::BORDERSTATE_ON,	// 縁取り状態
		CUBE_BORD,							// 縁取り太さ
		CObjectMeshCube::TEXSTATE_ONE,		// テクスチャ状態
		CObjectMeshCube::FACETEX(NONE_IDX),	// テクスチャ種類
		VEC2_ONE,		// テクスチャ分割数 x
		VEC2_ONE,		// テクスチャ分割数 y
		VEC2_ONE,		// テクスチャ分割数 z
		D3DCULL_CCW,	// カリング状況
		false			// ライティング状況
	);
	if (UNUSED(m_pMeshCube))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 体力ゲージ3Dの生成
	m_pLife = CObjectGauge3D::Create
	( // 引数
		CObject::LABEL_GAUGE,			// オブジェクトラベル
		this,							// ゲージ表示オブジェクト
		TARG_LIFE,						// 最大表示値
		(int)(NORMAL_CNT * 0.5f),		// 表示値変動フレーム
		GAUGE_PLUS_Y,					// 表示Y位置の加算量
		GAUGE_GAUGESIZE,				// ゲージ大きさ
		GAUGE_FRONTCOL,					// 表ゲージ色
		GAUGE_BACKCOL,					// 裏ゲージ色
		true,							// 枠描画状況
		CObjectGauge3D::TYPE_TARGET,	// 枠種類
		GAUGE_FRAMESIZE					// 枠大きさ
	);
	if (UNUSED(m_pLife))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, TARG_SHADOW_SIZE, this);
	if (UNUSED(m_pShadow))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 攻撃警告表示の生成
	m_pWarning = CWarningAttack::Create(WARNING_POS, WARNING_SIZE);
	if (UNUSED(m_pWarning))
	{ // 生成に失敗した場合

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
void CTarget::Uninit(void)
{
	// オブジェクトメッシュキューブを破棄
	m_pMeshCube->Uninit();

	// 影を破棄
	m_pShadow->Uninit();

	// オブジェクトモデルの終了
	CObjectModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CTarget::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();				// 台座位置
	D3DXVECTOR3 rot = m_pMeshCube->GetRotation();	// キューブ向き
	int nNumFlower = CFlower::GetNumAll();			// マナフラワーの総数
	bool bUpdate = true;	// 更新状況

	if (CManager::GetMode() == CScene::MODE_GAME)
	{ // モードがゲームの場合

		if (CSceneGame::GetWaveManager()->GetState() != CWaveManager::STATE_PROGRESSION)
		{ // ウェーブ進行状態ではない場合

			// 更新できない状態にする
			bUpdate = false;
		}
	}

	if (bUpdate)
	{ // 更新できる状態の場合

		// 状態管理
		switch (m_state)
		{ // 状態ごとの処理
		case STATE_NORMAL:	// 通常状態

			if (m_pLife->GetNum() < TARG_LIFE && nNumFlower > 0)
			{ // 体力が減少している且つ、マナフラワーが一本でも生えている場合

				if (m_nCounterState < STATE_HEAL_CNT)
				{ // カウンターが一定値より小さい場合

					// カウンターを加算
					m_nCounterState++;
				}
				else
				{ // カウンターが一定値以上の場合

					// カウンターを初期化
					m_nCounterState = 0;

					// 状態を変更
					m_state = STATE_HEAL;	// 回復状態
				}
			}

			break;

		case STATE_HEAL:	// 回復状態

			if (m_pLife->GetNum() < TARG_LIFE)
			{ // 体力が最大値より少ない場合

				if (m_nCounterHeal < WAIT_HEAL_CNT)
				{ // カウンターが一定値より小さい場合

					// カウンターを加算
					m_nCounterHeal++;
				}
				else
				{ // カウンターが一定値以上の場合

					// カウンターを初期化
					m_nCounterHeal = 0;

					// 体力を回復
					m_pLife->AddNum(nNumFlower * 2);	// マナフラワー量に応じて回復量増加

					// パーティクル3Dオブジェクトを生成
					CParticle3D::Create(CParticle3D::TYPE_HEAL, pos, HEAL_EFF_COL);
				}
			}
			else
			{ // 体力が全回復した場合

				// 状態を変更
				m_state = STATE_NORMAL;	// 通常状態
			}

			break;

		case STATE_DESTROY:	// 破壊状態

			// 無し

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}

	// キューブの縦位置を変更
	pos.y += ADD_POS_Y + sinf(m_fSinRot) * MUL_SIN_POS;

	// キューブの向きを変更
	rot += ADD_CUBEROT;

	// キューブの位置・向きを反映
	m_pMeshCube->SetPosition(pos);
	m_pMeshCube->SetRotation(rot);

	// 浮遊向きを加算
	m_fSinRot += ADD_SIN_ROT;
	useful::NormalizeRot(m_fSinRot);	// 向き補正

	// メッシュキューブの更新
	m_pMeshCube->Update();

	// 影の更新
	m_pShadow->Update();

	// オブジェクトモデルの更新
	CObjectModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CTarget::Draw(void)
{
	// オブジェクトモデルの描画
	CObjectModel::Draw();
}

//============================================================
//	ヒット処理
//============================================================
void CTarget::Hit(const int nDmg)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// ターゲット位置

	if (IsDeath() != true)
	{ // 死亡フラグが立っていない場合

		if (m_state == STATE_NORMAL || m_state == STATE_HEAL)
		{ // 通常状態または回復状態の場合

			// 体力からダメージ分減算
			m_pLife->AddNum(-nDmg);

			if (m_pLife->GetNum() > 0)
			{ // 生きている場合

				// パーティクル3Dオブジェクトを生成
				CParticle3D::Create(CParticle3D::TYPE_SMALL_EXPLOSION, pos);

				// カウンターを初期化
				m_nCounterState = 0;

				// 攻撃警告を表示させる
				m_pWarning->SetDisp();
			}
			else
			{ // 死んでいる場合

				// パーティクル3Dオブジェクトを生成
				CParticle3D::Create(CParticle3D::TYPE_BIG_EXPLOSION, pos);

				// 描画を停止
				m_pMeshCube->SetEnableDraw(false);

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を変更
				m_state = STATE_DESTROY;	// 破壊状態

				// サウンドの再生
				CManager::GetSound()->Play(CSound::LABEL_SE_BREAK);	// 破壊音
			}
		}
	}
}

//============================================================
//	生成処理
//============================================================
CTarget *CTarget::Create
(
	const MODEL model,			// モデル
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rScale	// 大きさ
)
{
	// 変数を宣言
	D3DXVECTOR3 pos = rPos;	// 座標設定用
	int nModelID;	// モデルインデックス

	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ
	CTarget *pTarget = NULL;	// ターゲット生成用

	if (UNUSED(pTarget))
	{ // 使用されていない場合

		// メモリ確保
		pTarget = new CTarget;	// ターゲット
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pTarget))
	{ // 使用されている場合
		
		// ターゲットの初期化
		if (FAILED(pTarget->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pTarget;
			pTarget = NULL;

			// 失敗を返す
			return NULL;
		}

		// モデルを登録
		nModelID = pModel->Regist(mc_apModelFile[model]);

		// モデルを割当
		pTarget->BindModel(pModel->GetModel(nModelID));

		// 位置を設定
		CScene::GetStage()->LimitPosition(pos, TARG_RADIUS);	// ステージ範囲外補正
		pos.y = CScene::GetField()->GetPositionHeight(pos);		// 高さを地面に設定
		pTarget->SetPosition(pos);

		// 向きを設定
		pTarget->SetRotation(rRot);

		// 大きさを設定
		pTarget->SetScaling(rScale);

		// メッシュキューブの位置を設定
		pTarget->m_pMeshCube->SetPosition(D3DXVECTOR3(rPos.x, rPos.y + ADD_POS_Y, rPos.z));

		// 確保したアドレスを返す
		return pTarget;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	更新状況の設定処理
//============================================================
void CTarget::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);		// 自身
	m_pMeshCube->SetEnableUpdate(bUpdate);	// メッシュキューブ
	m_pLife->SetEnableUpdate(bUpdate);		// 体力
	m_pShadow->SetEnableUpdate(bUpdate);	// 影
}

//============================================================
//	描画状況の設定処理
//============================================================
void CTarget::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pMeshCube->SetEnableDraw(bDraw);	// メッシュキューブ
	m_pLife->SetEnableDraw(bDraw);		// 体力
	m_pShadow->SetEnableDraw(bDraw);	// 影
}

//============================================================
//	体力の描画状況の設定処理
//============================================================
void CTarget::SetEnableDrawLife(const bool bDraw)
{
	// 引数の描画状況を設定
	m_pLife->SetEnableDraw(bDraw);	// 体力
}

//============================================================
//	体力の加算処理
//============================================================
void CTarget::AddLife(const int nAdd)
{
	// 変数を宣言
	int nLife = m_pLife->GetNum();	// 現在の体力

	// 体力に引数の値を加算
	nLife += nAdd;

	if (nLife < 1)
	{ // 死んでしまった場合

		// 死ぬ寸前の体力に補正
		nLife = 1;
	}

	// 体力を設定
	m_pLife->SetNum(nLife);
}

//============================================================
//	体力設定の設定処理
//============================================================
void CTarget::SetLife(const int nLife)
{
	// 引数の体力を設定
	m_pLife->SetNum(nLife);
}

//============================================================
//	体力取得処理
//============================================================
int CTarget::GetLife(void) const
{
	// 体力を返す
	return m_pLife->GetNum();
}

//============================================================
//	最大体力取得処理
//============================================================
int CTarget::GetMaxLife(void) const
{
	// 最大体力を返す
	return m_pLife->GetMaxNum();
}

//============================================================
//	状態取得処理
//============================================================
int CTarget::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	半径取得処理
//============================================================
float CTarget::GetRadius(void) const
{
	// 半径を返す
	return TARG_RADIUS;
}
