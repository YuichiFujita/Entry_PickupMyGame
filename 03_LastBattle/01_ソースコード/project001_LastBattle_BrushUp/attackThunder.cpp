//============================================================
//
//	雷攻撃処理 [attackThunder.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "attackThunder.h"
#include "manager.h"
#include "sound.h"
#include "thunder.h"
#include "object3D.h"
#include "scene.h"
#include "stage.h"
#include "collision.h"
#include "waveLand.h"
#include "player.h"
#include "camera.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_WARNING = "data\\TEXTURE\\warning000.png";	// 警告表示のテクスチャファイル

	const CCamera::SSwing LAND_SWING = CCamera::SSwing(8.0f, 1.5f, 0.25f);	// 雷着弾時のカメラ揺れ

	const int	WARNING_PRIO	= 1;		// 警告表示の優先順位
	const int	DMG_THUNDER		= 30;		// 雷のダメージ量
	const float COLL_RADIUS		= 120.0f;	// 判定の半径

	const int	DIV_DIRRAND	= 21;			// 方向の剰余算の値
	const int	SUB_DIRRAND	= 10;			// 方向の減算の値
	const float SHIFT_POS_LENGTH = 40.0f;	// 雷生成位置のずらす長さ

	// 衝撃波の情報
	namespace wave
	{
		const CWave::SGrow GROW		= CWave::SGrow(4.5f, 2.0f, 0.0f);	// 成長量
		const CWave::SGrow ADDGROW	= CWave::SGrow(0.0f, 0.0f, 0.0f);	// 成長加速量

		const D3DXCOLOR COL = XCOL_WHITE;	// 色
		const float	HOLE_RADIUS	= 80.0f;	// 穴の半径
		const float	THICKNESS	= 1.0f;		// 太さ
		const float	OUTER_PLUSY	= 20.0f;	// 外周のY座標加算量
		const float	MAX_RADIUS	= 0.0f;		// 半径の最大成長量
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CListManager<CAttackThunder> *CAttackThunder::m_pList = nullptr;	// オブジェクトリスト

//************************************************************
//	子クラス [CAttackThunder] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CAttackThunder::CAttackThunder() : CObject(CObject::LABEL_THUNDER, DIM_3D),
	m_pWarning		(nullptr),		// 警告表示の情報
	m_posOrigin		(VEC3_ZERO),	// 雷の原点位置
	m_state			(STATE_WARN),	// 状態
	m_nCounterState	(0),			// 状態管理カウンター
	m_nWarnFrame	(0),			// 警告表示フレーム数
	m_bSound		(false)			// 効果音再生フラグ
{
	// メンバ変数をクリア
	memset(&m_apThunder[0], 0, sizeof(m_apThunder));	// 雷の情報
}

//============================================================
//	デストラクタ
//============================================================
CAttackThunder::~CAttackThunder()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CAttackThunder::Init(void)
{
	// メンバ変数をクリア
	memset(&m_apThunder[0], 0, sizeof(m_apThunder));	// 雷の情報
	m_pWarning	= nullptr;		// 警告表示の情報
	m_posOrigin = VEC3_ZERO;	// 雷の原点位置
	m_state		= STATE_WARN;	// 状態
	m_nCounterState = 0;		// 状態管理カウンター
	m_nWarnFrame	= 0;		// 警告表示フレーム数
	m_bSound		= false;	// 効果音再生フラグ

	// 警告表示の生成
	m_pWarning = CObject3D::Create
	( // 引数
		VEC3_ZERO,	// 位置
		D3DXVECTOR3(COLL_RADIUS * 2.0f, 0.0f, COLL_RADIUS * 2.0f)	// 大きさ
	);
	if (m_pWarning == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pWarning->BindTexture(TEXTURE_WARNING);

	// 優先順位を設定
	m_pWarning->SetPriority(WARNING_PRIO);

	// レンダーステートの設定
	CRenderState *pRenderState = m_pWarning->GetRenderState();	// レンダーステートの情報
	pRenderState->SetZFunc(D3DCMP_ALWAYS);						// Zテストを絶対成功にする
	pRenderState->SetAlphaBlend(CRenderState::BLEND_ADD);		// 加算合成にする

	if (m_pList == nullptr)
	{ // リストマネージャーが存在しない場合

		// リストマネージャーの生成
		m_pList = CListManager<CAttackThunder>::Create();
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
void CAttackThunder::Uninit(void)
{
	// 警告表示の終了
	SAFE_UNINIT(m_pWarning);

	// リストから自身のオブジェクトを削除
	m_pList->DeleteList(m_iterator);

	if (m_pList->GetNumAll() == 0)
	{ // オブジェクトが一つもない場合

		// リストマネージャーの破棄
		m_pList->Release(m_pList);
	}

	// 雷攻撃オブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CAttackThunder::Update(void)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージの情報
	assert(pStage != nullptr);	// ステージ未使用

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_WARN:

		// カウンターを加算
		m_nCounterState++;
		if (m_nCounterState > m_nWarnFrame)
		{ // 警告表示が終了した場合

			// 攻撃開始の設定
			if (FAILED(SetAttack())) { assert(false); }
		}

		// 警告表示の更新
		m_pWarning->Update();

		break;

	case STATE_ATTACK:

		// プレイヤーとの当たり判定
		CollisionPlayer();

		for (int nCntThunder = 0; nCntThunder < attackThunder::NUM_THUNDER; nCntThunder++)
		{ // 雷の生成数分繰り返す

			if (m_apThunder[nCntThunder]->IsLanding())
			{ // 雷が地面に到達している場合

				// カメラ揺れを設定
				GET_MANAGER->GetCamera()->SetSwing(CCamera::TYPE_MAIN, LAND_SWING);

				// 衝撃波の生成
				D3DXVECTOR3 posImpact = D3DXVECTOR3(m_posOrigin.x, pStage->GetStageLimit().fField, m_posOrigin.z);
				CWaveLand::Create
				( // 引数
					CWave::TEXTURE_IMPACT,	// 種類
					posImpact,				// 位置
					wave::COL,				// 色
					wave::GROW,				// 成長量
					wave::ADDGROW,			// 成長加速量
					wave::HOLE_RADIUS,		// 穴の半径
					wave::THICKNESS,		// 太さ
					wave::OUTER_PLUSY,		// 外周のY座標加算量
					wave::MAX_RADIUS		// 半径の最大成長量
				);

				if (m_bSound)
				{
					// 雷音の再生
					PLAY_SOUND(CSound::LABEL_SE_THUNDER);
				}

				// 自身を終了
				Uninit();

				// 処理を抜ける
				return;
			}
		}

		break;

	default:
		assert(false);
		break;
	}
}

//============================================================
//	描画処理
//============================================================
void CAttackThunder::Draw(CShader * /*pShader*/)
{

}

//============================================================
//	生成処理
//============================================================
CAttackThunder *CAttackThunder::Create
(
	const D3DXVECTOR3 &rPos,	// 位置
	const bool bSound,			// 効果音再生フラグ
	const int nWarnFrame		// 警告表示フレーム数
)
{
	// 雷攻撃の生成
	CAttackThunder *pAttackThunder = new CAttackThunder;
	if (pAttackThunder == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 雷攻撃の初期化
		if (FAILED(pAttackThunder->Init()))
		{ // 初期化に失敗した場合

			// 雷攻撃の破棄
			SAFE_DELETE(pAttackThunder);
			return nullptr;
		}

		// 雷の原点位置を設定
		pAttackThunder->SetOriginPosition(rPos);

		// 効果音再生フラグを設定
		pAttackThunder->m_bSound = bSound;

		// 警告表示フレームを設定
		pAttackThunder->m_nWarnFrame = nWarnFrame;

		if (bSound)
		{
			// 雷警告音の再生
			PLAY_SOUND(CSound::LABEL_SE_THUNDER_WARN);
		}

		// 確保したアドレスを返す
		return pAttackThunder;
	}
}

//============================================================
//	リスト取得処理
//============================================================
CListManager<CAttackThunder> *CAttackThunder::GetList(void)
{
	// オブジェクトリストを返す
	return m_pList;
}

//============================================================
//	破棄処理
//============================================================
void CAttackThunder::Release(void)
{
	// オブジェクトの破棄
	CObject::Release();
}

//============================================================
//	原点位置の設定処理
//============================================================
void CAttackThunder::SetOriginPosition(const D3DXVECTOR3 &rPos)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージの情報
	assert(pStage != nullptr);	// ステージ未使用

	// 原点位置を設定
	m_posOrigin = rPos;

	// ステージ範囲外の補正
	pStage->LimitPosition(m_posOrigin, COLL_RADIUS);

	// 警告表示の位置を設定
	D3DXVECTOR3 posWarn = D3DXVECTOR3(m_posOrigin.x, pStage->GetStageLimit().fField, m_posOrigin.z);
	m_pWarning->SetVec3Position(posWarn);
}

//============================================================
//	雷位置の設定処理
//============================================================
void CAttackThunder::SetThunderPosition(const D3DXVECTOR3 &rPos)
{
	// 最初の雷の位置を真ん中に設定
	m_apThunder[0]->SetVec3Position(rPos);

	for (int nCntThunder = 0; nCntThunder < attackThunder::NUM_THUNDER - 1; nCntThunder++)
	{ // 雷の生成数分繰り返す

		// 雷の生成方向を設定
		float fRotRate = (D3DX_PI * 2.0f) / (float)(attackThunder::NUM_THUNDER - 1);	// 生成方向の分割割合
		float fRotDir  = (fRotRate * nCntThunder - D3DX_PI) + ((rand() % DIV_DIRRAND - SUB_DIRRAND) * 0.01f);	// 生成方向
		useful::NormalizeRot(fRotDir);	// 方向を正規化

		// 雷の位置を設定
		D3DXVECTOR3 posThunder = VEC3_ZERO;
		posThunder.x = rPos.x + sinf(fRotDir) * SHIFT_POS_LENGTH;
		posThunder.z = rPos.z + cosf(fRotDir) * SHIFT_POS_LENGTH;

		// 雷の位置を設定
		int nSetID = nCntThunder + 1;	// 位置を設定する雷のインデックス
		m_apThunder[nSetID]->SetVec3Position(posThunder);
	}
}

//============================================================
//	攻撃開始の設定処理
//============================================================
HRESULT CAttackThunder::SetAttack(void)
{
	// 攻撃開始状態にする
	m_state = STATE_ATTACK;

	// 状態管理カウンター
	m_nCounterState = 0;

	for (int nCntThunder = 0; nCntThunder < attackThunder::NUM_THUNDER; nCntThunder++)
	{ // 雷の生成数分繰り返す

		// 雷の生成
		m_apThunder[nCntThunder] = CThunder::Create(VEC3_ZERO);
		if (m_apThunder[nCntThunder] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}
	}

	// 雷位置を設定
	SetThunderPosition(m_posOrigin);

	// 成功を返す
	return S_OK;
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
void CAttackThunder::CollisionPlayer(void)
{
	CListManager<CPlayer> *pList = CPlayer::GetList();		// プレイヤーリスト
	if (pList == nullptr)		 { assert(false); return; }	// リスト未使用
	if (pList->GetNumAll() != 1) { assert(false); return; }	// プレイヤーが1人じゃない
	auto player = pList->GetList().front();					// プレイヤー情報

	D3DXVECTOR3 posPlayer	= player->GetVec3Position();	// プレイヤーの位置
	float fRadiusPlayer		= player->GetRadius();			// プレイヤーの半径
	float fHeightPlayer		= player->GetHeight();			// プレイヤーの縦幅

	if (collision::Circle2D(m_posOrigin, posPlayer, COLL_RADIUS, fRadiusPlayer))
	{ // 衝撃波に当たっている場合

		if (m_apThunder[0]->GetVec3Position().y < posPlayer.y + fHeightPlayer)
		{ // プレイヤーの頭が雷に当たっている場合

			// プレイヤーのヒット処理
			player->Hit(DMG_THUNDER);
		}
	}
}
