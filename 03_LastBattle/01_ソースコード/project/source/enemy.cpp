//============================================================
//
//	敵処理 [enemy.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "enemy.h"
#include "manager.h"
#include "renderer.h"
#include "stage.h"
#include "enemyBossDragon.h"
#include "enemyMiniDragon.h"
#include "multiModel.h"
#include "collSphere.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	// 基本情報
	const char *SETUP_TXT[] =	// セットアップテキスト相対パス
	{
		"data\\TXT\\enemyBossDragon.txt",	// ボスドラゴン
		"data\\TXT\\enemyMiniDragon.txt",	// ミニドラゴン
	};

	const int	PRIORITY	= 3;		// 敵の優先順位
	const float	GRAVITY		= 1.0f;		// 重力
	const float	REV_ROTA	= 0.15f;	// 向き回転の補正係数
	const float	MAX_ROTA	= 0.1f;		// 向き回転の最高速度
	const float	JUMP_REV	= 0.16f;	// 空中の移動量の減衰係数
	const float	LAND_REV	= 0.16f;	// 地上の移動量の減衰係数
	const float	ADD_ALPHA	= 0.03f;	// 透明度の加算量

	const int	DAMAGE_FRAME	= 4;		// ダメージ状態の維持フレーム
	const int	INVULN_FRAME	= 12;		// 無敵状態の維持フレーム
	const int	STAN_FRAME		= 640;		// スタン状態の維持フレーム
	const float	STAN_ALPHA		= 0.75f;	// 無敵状態の基礎透明度
	const float	ADD_SINROT		= 0.1f;		// 透明度をふわふわさせる際のサインカーブ向き加算量
	const float	MAX_ADD_ALPHA	= 0.15f;	// 透明度の最大加算量
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CListManager<CEnemy> *CEnemy::m_pList = nullptr;			// オブジェクトリスト
CMotion::SInfo		CEnemy::m_aMotionInfo[TYPE_MAX]	= {};	// モーション情報
CEnemy::SPartsInfo	CEnemy::m_aPartsInfo[TYPE_MAX]	= {};	// パーツ情報
CEnemy::SStatusInfo	CEnemy::m_aStatusInfo[TYPE_MAX]	= {};	// ステータス情報
CEnemy::SCollInfo	CEnemy::m_aCollInfo[TYPE_MAX]	= {};	// 当たり判定情報
CEnemy::AFuncUpdateState CEnemy::m_aFuncUpdateState[] =		// 状態更新関数
{
	&CEnemy::UpdateNone,		// なにもしない状態時の更新
	&CEnemy::UpdateSpawn,		// スポーン状態時の更新
	&CEnemy::UpdateNormal,		// 通常状態時の更新
	&CEnemy::UpdateDamage,		// ダメージ状態時の更新
	&CEnemy::UpdateInvuln,		// 無敵状態時の更新
	&CEnemy::UpdateKnock,		// ノックバック状態時の更新
	&CEnemy::UpdateStan,		// スタン状態時の更新
	&CEnemy::UpdateRideFlyUp,	// ライド飛び上がり状態時の更新
	&CEnemy::UpdateRideRotate,	// ライド旋回状態時の更新
	&CEnemy::UpdateRideEnd,		// ライド終了状態時の更新
	&CEnemy::UpdateDeath,		// 死亡状態時の更新
};

//************************************************************
//	子クラス [CEnemy] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemy::CEnemy(const EType type) : CObjectChara(CObject::LABEL_ENEMY, CObject::DIM_3D, PRIORITY),
	m_type			(type),					// 種類定数
	m_status		(m_aStatusInfo[type]),	// ステータス定数
	m_parts			(m_aPartsInfo[type]),	// パーツ定数
	m_motion		(m_aMotionInfo[type]),	// モーション定数
	m_oldPos		(VEC3_ZERO),			// 過去位置
	m_destRot		(VEC3_ZERO),			// 目標向き
	m_move			(VEC3_ZERO),			// 移動量
	m_state			(STATE_NONE),			// 状態
	m_prevState		(STATE_NONE),			// ダメージを受ける前の状態
	m_fSinAlpha		(0.0f),					// 透明向き
	m_bJump			(false),				// ジャンプ状況
	m_nCounterState	(0)						// 状態管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemy::~CEnemy()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemy::Init(void)
{
	// メンバ変数を初期化
	m_oldPos	= VEC3_ZERO;	// 過去位置
	m_destRot	= VEC3_ZERO;	// 目標向き
	m_move		= VEC3_ZERO;	// 移動量
	m_state		= STATE_NONE;	// 状態
	m_prevState	= STATE_NONE;	// ダメージを受ける前の状態
	m_fSinAlpha	= 0.0f;			// 透明向き
	m_bJump		= false;		// ジャンプ状況
	m_nCounterState	= 0;		// 状態管理カウンター

	// オブジェクトキャラクターの初期化
	if (FAILED(CObjectChara::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	for (int nCntEnemy = 0; nCntEnemy < m_motion.nNumType; nCntEnemy++)
	{ // 読み込んだモーション数分繰り返す

		// モーション情報の設定
		CObjectChara::SetMotionInfo(m_motion.aMotionInfo[nCntEnemy]);
	}

	for (int nCntEnemy = 0; nCntEnemy < m_parts.nNumParts; nCntEnemy++)
	{ // パーツ数分繰り返す

		// パーツ情報の設定
		CObjectChara::SetPartsInfo
		( // 引数
			nCntEnemy,							// パーツインデックス
			m_parts.aInfo[nCntEnemy].nParentID,	// 親インデックス
			m_parts.aInfo[nCntEnemy].pos,		// 位置
			m_parts.aInfo[nCntEnemy].rot,		// 向き
			GetModelFileName(nCntEnemy)			// ファイル名
		);

		// 判定情報の設定
		CCollSphere *pColl = GetCollision(nCntEnemy);	// 円判定情報
		std::vector<CCollSphere::SInfo> vector = m_aCollInfo[m_type].apInfo[nCntEnemy]->GetVector();	// 円判定配列
		for (auto info : vector)
		{ // 配列の要素数分繰り返す

			// 判定を追加
			pColl->AddColl(info.offset, info.fRadius);
		}
	}

	// モデル情報の設定
	SetModelInfo();

	if (m_pList == nullptr)
	{ // リストマネージャーが存在しない場合

		// リストマネージャーの生成
		m_pList = CListManager<CEnemy>::Create();
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
void CEnemy::Uninit(void)
{
	// リストから自身のオブジェクトを削除
	m_pList->DeleteList(m_iterator);

	if (m_pList->GetNumAll() == 0)
	{ // オブジェクトが一つもない場合

		// リストマネージャーの破棄
		m_pList->Release(m_pList);
	}

	// オブジェクトキャラクターの終了
	CObjectChara::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemy::Update(void)
{
	// オブジェクトキャラクターの更新
	CObjectChara::Update();

	// 各状態ごとの更新
	assert(m_state > NONE_IDX && m_state < STATE_MAX);
	(this->*(m_aFuncUpdateState[m_state]))();

	// モーションの更新
	UpdateMotion();
}

//============================================================
//	描画処理
//============================================================
void CEnemy::Draw(CShader *pShader)
{
	// オブジェクトキャラクターの描画
	CObjectChara::Draw(pShader);
}

//============================================================
//	状態の設定処理
//============================================================
void CEnemy::SetState(const int nState)
{
	// 引数の状態を設定
	m_state = (EState)nState;

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// なにもしない状態
	case STATE_NORMAL:	// 通常状態
	case STATE_DAMAGE:	// ダメージ状態
		break;

	case STATE_SPAWN:	// スポーン状態

		// スポーン状態の設定
		SetSpawn();
		break;

	case STATE_INVULN:	// 無敵状態

		// 無敵状態の設定
		SetInvuln();
		break;

	case STATE_KNOCK:	// ノックバック状態

		// ノックバック状態の設定
		SetKnock();
		break;

	case STATE_STAN:	// スタン状態

		// スタン状態の設定
		SetStan();
		break;

	case STATE_RIDE_FLYUP:	// ライド飛び上がり状態

		// ライド飛び上がり状態の設定
		SetRideFlyUp();
		break;

	case STATE_RIDE_ROTATE:	// ライド旋回状態

		// ライド旋回状態の設定
		SetRideRotate();
		break;

	case STATE_RIDE_END:	// ライド終了状態

		// ライド終了状態の設定
		SetRideEnd();
		break;

	case STATE_DEATH:	// 死亡状態

		// 死亡状態の設定
		SetDeath();
		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	状態取得処理
//============================================================
int CEnemy::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	種類取得処理
//============================================================
int CEnemy::GetType(void) const
{
	// 種類を返す
	return m_type;
}

//============================================================
//	半径取得処理
//============================================================
float CEnemy::GetRadius(void) const
{
	// 半径を返す
	return m_status.fRadius;
}

//============================================================
//	縦幅取得処理
//============================================================
float CEnemy::GetHeight(void) const
{
	// 縦幅を返す
	return m_status.fHeight;
}

//============================================================
//	通常状態の初期化処理
//============================================================
void CEnemy::InitNormal(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 移動量を初期化
	m_move = VEC3_ZERO;
}

//============================================================
//	切り抜き用の描画処理
//============================================================
void CEnemy::DrawCrop(void)
{
	assert(false);
}

//============================================================
//	ヒット処理
//============================================================
bool CEnemy::Hit(const int /*nDamage*/)
{
	assert(false);
	return false;
}

//============================================================
//	UI描画の設定処理
//============================================================
void CEnemy::SetEnableDrawUI(const bool /*bDraw*/)
{

}

//============================================================
//	体力ゲージの優先順位の設定処理
//============================================================
void CEnemy::SetLifePriority(const int /*nPrio*/)
{
	assert(false);
}

//============================================================
//	ライド可能かの取得処理
//============================================================
bool CEnemy::IsRideOK(const D3DXVECTOR3& /*rPos*/) const
{
	assert(false);
	return false;
}

//============================================================
//	生成処理
//============================================================
CEnemy *CEnemy::Create(const EType type, const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot)
{
	// ポインタを宣言
	CEnemy *pEnemy = nullptr;	// 敵情報

	// 敵の生成
	switch (type)
	{ // 種類ごとの処理
	case TYPE_BOSS_DRAGON:

		// ボスドラゴンを生成
		pEnemy = new CEnemyBossDragon(type);

		break;

	case TYPE_MINI_DRAGON:

		// ミニドラゴンを生成
		pEnemy = new CEnemyMiniDragon(type);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (pEnemy == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 敵の初期化
		if (FAILED(pEnemy->Init()))
		{ // 初期化に失敗した場合

			// 敵の破棄
			SAFE_DELETE(pEnemy);
			return nullptr;
		}

		// 位置を設定
		pEnemy->SetVec3Position(rPos);

		// 向きを設定
		pEnemy->SetVec3Rotation(rRot);

		// 確保したアドレスを返す
		return pEnemy;
	}
}

//============================================================
//	リスト取得処理
//============================================================
CListManager<CEnemy> *CEnemy::GetList(void)
{
	// オブジェクトリストを返す
	return m_pList;
}

//============================================================
//	ステータス情報取得処理
//============================================================
CEnemy::SStatusInfo CEnemy::GetStatusInfo(const int nType)
{
	// 引数の種類のステータス情報を返す
	return m_aStatusInfo[nType];
}

//============================================================
//	セットアップ破棄処理
//============================================================
void CEnemy::ReleaseSetup(void)
{
	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{ // 種類の総数分繰り返す

		for (int nCntParts = 0; nCntParts < motion::MAX_PARTS; nCntParts++)
		{ // パーツの最大数分繰り返す

			// 判定情報を破棄
			SAFE_REF_RELEASE(m_aCollInfo[nCntType].apInfo[nCntParts]);
		}
	}
}

//============================================================
//	過去位置の更新処理
//============================================================
void CEnemy::UpdateOldPosition(void)
{
	// 過去位置を更新
	m_oldPos = GetVec3Position();
}

//============================================================
//	向きの更新処理
//============================================================
void CEnemy::UpdateRotation(D3DXVECTOR3 *pRot, D3DXVECTOR3 *pDestRot)
{
	// 変数を宣言
	float fDiffRot = 0.0f;	// 差分向き

	// 目標向きの正規化
	useful::NormalizeRot(pDestRot->y);

	// 目標向きまでの差分を計算
	fDiffRot = pDestRot->y - pRot->y;

	// 差分向きの正規化
	useful::NormalizeRot(fDiffRot);

	// 向き回転量を求める
	float fMoveRot = fDiffRot * REV_ROTA;
	useful::LimitNum(fMoveRot, -MAX_ROTA, MAX_ROTA);	// 回転量を補正

	// 向きの更新
	pRot->y += fMoveRot;

	// 向きの正規化
	useful::NormalizeRot(pRot->y);
}

//============================================================
//	過去位置取得処理
//============================================================
D3DXVECTOR3 CEnemy::GetOldPosition(void) const
{
	// 過去位置を返す
	return m_oldPos;
}

//============================================================
//	目標向きの設定処理
//============================================================
void CEnemy::SetDestRotation(const D3DXVECTOR3& rRot)
{
	// 目標向きを設定
	m_destRot = rRot;
}

//============================================================
//	目標向き取得処理
//============================================================
D3DXVECTOR3 CEnemy::GetDestRotation(void) const
{
	// 目標向きを返す
	return m_destRot;
}

//============================================================
//	移動量の設定処理
//============================================================
void CEnemy::SetMovePosition(const D3DXVECTOR3 &rMove)
{
	// 引数の移動量を設定
	m_move = rMove;
}

//============================================================
//	移動量取得処理
//============================================================
D3DXVECTOR3 CEnemy::GetMovePosition(void) const
{
	// 移動量を返す
	return m_move;
}

//============================================================
//	ステータス情報取得処理
//============================================================
CEnemy::SStatusInfo CEnemy::GetStatusInfo(void) const
{
	// ステータス情報を返す
	return m_status;
}

//============================================================
//	パーツ情報取得処理
//============================================================
CEnemy::SPartsInfo CEnemy::GetPartsInfo(void) const
{
	// パーツ情報を返す
	return m_parts;
}

//============================================================
//	モーション情報取得処理
//============================================================
CMotion::SInfo CEnemy::GetMotionInfo(void) const
{
	// モーション情報を返す
	return m_motion;
}

//============================================================
//	スポーン状態の設定処理
//============================================================
void CEnemy::SetSpawn(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 移動量を初期化
	m_move = VEC3_ZERO;

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を透明に再設定
	SetAlpha(0.0f);
}

//============================================================
//	無敵状態の設定処理
//============================================================
void CEnemy::SetInvuln(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を不透明に再設定
	SetAlpha(1.0f);
}

//============================================================
//	ノックバック状態の設定処理
//============================================================
void CEnemy::SetKnock(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を不透明に再設定
	SetAlpha(1.0f);
}

//============================================================
//	スタン状態の設定処理
//============================================================
void CEnemy::SetStan(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 透明向きを初期化
	m_fSinAlpha = 0.0f;

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 無敵の基礎透明度を設定
	SetAlpha(STAN_ALPHA);
}

//============================================================
//	ライド飛び上がり状態の設定処理
//============================================================
void CEnemy::SetRideFlyUp(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を再設定
	SetAlpha(1.0f);
}

//============================================================
//	ライド旋回状態の設定処理
//============================================================
void CEnemy::SetRideRotate(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を再設定
	SetAlpha(1.0f);
}

//============================================================
//	ライド終了状態の設定処理
//============================================================
void CEnemy::SetRideEnd(void)
{
	// カウンターを初期化
	m_nCounterState = 0;	// 状態管理カウンター

	// 描画を再開
	SetEnableDraw(true);

	// マテリアルを再設定
	ResetMaterial();

	// 透明度を再設定
	SetAlpha(1.0f);
}

//============================================================
//	死亡状態の設定処理
//============================================================
void CEnemy::SetDeath(void)
{

}

//============================================================
//	なにもしない状態時の更新処理
//============================================================
void CEnemy::UpdateNone(void)
{

}

//============================================================
//	スポーン状態時の更新処理
//============================================================
void CEnemy::UpdateSpawn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();	// 敵向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	// 過去位置の更新
	UpdateOldPosition();

	// 重力の更新
	UpdateGravity();

	// 位置更新
	UpdatePosition(&posEnemy);

	// 着地判定
	UpdateLanding(&posEnemy);

	// ステージ範囲外の補正
	pStage->LimitPosition(posEnemy, m_status.fRadius);

	// 位置を反映
	SetVec3Position(posEnemy);

	// 向きを反映
	SetVec3Rotation(rotEnemy);

	// フェードアウト状態時の更新
	if (UpdateFadeOut(ADD_ALPHA))
	{ // 不透明になり切った場合

		// 通常状態にする
		SetState(STATE_NORMAL);
	}
}

//============================================================
//	通常状態時の更新処理
//============================================================
void CEnemy::UpdateNormal(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetVec3Position();	// 敵位置
	D3DXVECTOR3 rotEnemy = GetVec3Rotation();	// 敵向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	// 過去位置の更新
	UpdateOldPosition();

	// 重力の更新
	UpdateGravity();

	// 位置更新
	UpdatePosition(&posEnemy);

	// 着地判定
	UpdateLanding(&posEnemy);

	// ステージ範囲外の補正
	pStage->LimitPosition(posEnemy, m_status.fRadius);

	// 位置を反映
	SetVec3Position(posEnemy);

	// 向きを反映
	SetVec3Rotation(rotEnemy);
}

//============================================================
//	ダメージ状態時の更新処理
//============================================================
void CEnemy::UpdateDamage(void)
{
	// 身体の色を赤くする
	SetAllMaterial(material::DamageRed());

	// ダメージを受ける前の状態更新
	assert(m_prevState > NONE_IDX && m_prevState < STATE_MAX);
	(this->*(m_aFuncUpdateState[m_prevState]))();

	// カウンターを加算
	m_nCounterState++;
	if (m_nCounterState > DAMAGE_FRAME)
	{ // ダメージ状態の終了フレームになった場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 身体の色を元に戻す
		ResetMaterial();

		// 無敵状態にする
		SetState(STATE_INVULN);
	}
}

//============================================================
//	無敵状態時の更新処理
//============================================================
void CEnemy::UpdateInvuln(void)
{
	// ダメージを受ける前の状態更新
	assert(m_prevState > NONE_IDX && m_prevState < STATE_MAX);
	(this->*(m_aFuncUpdateState[m_prevState]))();

	// カウンターを加算
	m_nCounterState++;
	if (m_nCounterState > INVULN_FRAME)
	{ // 無敵状態の終了フレームになった場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 不透明にする
		SetAlpha(1.0f);

		// ダメージを受ける前の状態を設定
		assert(m_prevState != STATE_NONE);
		SetState(m_prevState);
	}
}

//============================================================
//	ノックバック状態時の更新処理
//============================================================
void CEnemy::UpdateKnock(void)
{

}

//============================================================
//	スタン状態時の更新処理
//============================================================
void CEnemy::UpdateStan(void)
{
	float fAddAlpha = 0.0f;	// 透明度の加算量

	// サインカーブ向きを回転
	m_fSinAlpha += ADD_SINROT;
	useful::NormalizeRot(m_fSinAlpha);	// 向き正規化

	// 透明度加算量を求める
	fAddAlpha = (MAX_ADD_ALPHA / 2.0f) * (sinf(m_fSinAlpha) - 1.0f);

	// 透明度を設定
	SetAlpha(STAN_ALPHA + fAddAlpha);

	// カウンターを加算
	m_nCounterState++;
	if (m_nCounterState > STAN_FRAME)
	{ // スタン状態の終了フレームになった場合

		// カウンターを初期化
		m_nCounterState = 0;

		// 不透明にする
		SetAlpha(1.0f);

		// 通常状態を設定
		SetState(STATE_NORMAL);
	}

	D3DXVECTOR3 posEnemy = GetVec3Position();			// 敵位置
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	// 過去位置の更新
	UpdateOldPosition();

	// 重力の更新
	UpdateGravity();

	// 位置更新
	UpdatePosition(&posEnemy);

	// 着地判定
	UpdateLanding(&posEnemy);

	// ステージ範囲外の補正
	pStage->LimitPosition(posEnemy, m_status.fRadius);

	// 位置を反映
	SetVec3Position(posEnemy);
}

//============================================================
//	ライド飛び上がり状態時の更新処理
//============================================================
void CEnemy::UpdateRideFlyUp(void)
{

}

//============================================================
//	ライド旋回状態時の更新処理
//============================================================
void CEnemy::UpdateRideRotate(void)
{

}

//============================================================
//	ライド終了状態時の更新処理
//============================================================
void CEnemy::UpdateRideEnd(void)
{

}

//============================================================
//	死亡状態時の更新処理
//============================================================
void CEnemy::UpdateDeath(void)
{
	// 自身の終了
	Uninit();
}

//============================================================
//	対象の視認処理
//============================================================
void CEnemy::UpdateLook(const D3DXVECTOR3& rPosLook, const D3DXVECTOR3& rPosEnemy, D3DXVECTOR3 *pRotEnemy)
{
	// 変数を宣言
	float fDestRot = 0.0f;	// 目標向き
	float fDiffRot = 0.0f;	// 向き

	// 視認対象の方向を代入
	fDestRot = atan2f(rPosEnemy.x - rPosLook.x, rPosEnemy.z - rPosLook.z);	// 目標向き

	// 差分向きを求める
	fDiffRot = fDestRot - pRotEnemy->y;
	useful::NormalizeRot(fDiffRot);		// 差分向きの正規化

	// 向きの更新
	pRotEnemy->y += fDiffRot * m_status.fLookRev;
	useful::NormalizeRot(pRotEnemy->y);	// 向きの正規化
}

//============================================================
//	重力の更新処理
//============================================================
void CEnemy::UpdateGravity(void)
{
	// 重力を加算
	m_move.y -= GRAVITY;
}

//============================================================
//	着地状況の更新処理
//============================================================
bool CEnemy::UpdateLanding(D3DXVECTOR3 *pPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// ジャンプしている状態にする
	m_bJump = true;

	// 地面・制限位置の着地判定
	if (CScene::GetStage()->LandFieldPosition(*pPos, m_move)
	||  CScene::GetStage()->LandLimitPosition(*pPos, m_move, 0.0f))
	{ // プレイヤーが着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	位置の更新処理
//============================================================
void CEnemy::UpdatePosition(D3DXVECTOR3 *pPos)
{
	// 位置を移動
	*pPos += m_move;

	// 移動量を減衰
	if (m_bJump)
	{ // 空中の場合

		m_move.x += (0.0f - m_move.x) * JUMP_REV;
		m_move.z += (0.0f - m_move.z) * JUMP_REV;
	}
	else
	{ // 地上の場合

		m_move.x += (0.0f - m_move.x) * LAND_REV;
		m_move.z += (0.0f - m_move.z) * LAND_REV;
	}
}

//============================================================
//	フェードアウト状態時の更新処理
//============================================================
bool CEnemy::UpdateFadeOut(const float fAdd)
{
	// 変数を宣言
	bool bAlpha = false;		// 透明状況
	float fAlpha = GetAlpha();	// 透明度

	// プレイヤー自身の描画を再開
	CObject::SetEnableDraw(true);

	// 透明度を上げる
	fAlpha += fAdd;

	if (fAlpha >= GetMaxAlpha())
	{ // 透明度が上がり切った場合

		// 透明度を補正
		fAlpha = GetMaxAlpha();

		// 不透明になり切った状態にする
		bAlpha = true;
	}

	// 透明度を設定
	SetAlpha(fAlpha);

	// 透明状況を返す
	return bAlpha;
}

//============================================================
//	フェードイン状態時の更新処理
//============================================================
bool CEnemy::UpdateFadeIn(const float fSub)
{
	// 変数を宣言
	bool bAlpha = false;		// 透明状況
	float fAlpha = GetAlpha();	// 透明度

	// 透明度を下げる
	fAlpha -= fSub;

	if (fAlpha <= 0.0f)
	{ // 透明度が下がり切った場合

		// 透明度を補正
		fAlpha = 0.0f;

		// 透明になり切った状態にする
		bAlpha = true;

		// プレイヤー自身の描画を停止
		CObject::SetEnableDraw(false);
	}

	// 透明度を設定
	SetAlpha(fAlpha);

	// 透明状況を返す
	return bAlpha;
}

//============================================================
//	全セットアップ処理
//============================================================
void CEnemy::LoadAllSetup(void)
{
	for (int nCntEnemy = 0; nCntEnemy < TYPE_MAX; nCntEnemy++)
	{ // 敵の種類分繰り返す

		// セットアップ
		CEnemy::LoadSetup((EType)nCntEnemy);
	}
}

//============================================================
//	セットアップ処理
//============================================================
void CEnemy::LoadSetup(const EType typeID)
{
	// 変数を宣言
	std::vector<CCollSphere::SInfo> vector;	// 円判定配列
	CMotion::SMotionInfo infoKey;	// キーの代入用
	CCollSphere::SInfo infoColl;	// 判定情報
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nID			= 0;			// インデックスの代入用
	int nParts		= 0;			// パーツ番号
	int nNowPose	= 0;			// 現在のポーズ番号
	int nNowKey		= 0;			// 現在のキー番号
	int nWeapon		= 0;			// 武器表示のON/OFFの変換用
	int nLoop		= 0;			// ループのON/OFFの変換用
	int nEnd		= 0;			// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 引数の静的メンバ変数の情報をクリア
	memset(&m_aMotionInfo[typeID],	0, sizeof(m_aMotionInfo[typeID]));	// モーション情報
	memset(&m_aPartsInfo[typeID],	0, sizeof(m_aPartsInfo[typeID]));	// パーツ情報
	memset(&m_aStatusInfo[typeID],	0, sizeof(m_aStatusInfo[typeID]));	// ステータス情報
	memset(&m_aCollInfo[typeID],	0, sizeof(m_aCollInfo[typeID]));	// 当たり判定情報

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT[typeID], "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// ステータスの設定
			if (strcmp(&aString[0], "STATUSSET") == 0)
			{ // 読み込んだ文字列が STATUSSET の場合

				do
				{ // 読み込んだ文字列が END_STATUSSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "LIFE") == 0)
					{ // 読み込んだ文字列が LIFE の場合

						fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
						fscanf(pFile, "%d", &m_aStatusInfo[typeID].nMaxLife);	// 最大体力を読み込む
					}
					else if (strcmp(&aString[0], "RADIUS") == 0)
					{ // 読み込んだ文字列が RADIUS の場合

						fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
						fscanf(pFile, "%f", &m_aStatusInfo[typeID].fRadius);	// 半径を読み込む
					}
					else if (strcmp(&aString[0], "HEIGHT") == 0)
					{ // 読み込んだ文字列が HEIGHT の場合

						fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
						fscanf(pFile, "%f", &m_aStatusInfo[typeID].fHeight);	// 縦幅を読み込む
					}
					else if (strcmp(&aString[0], "LOOK_REV") == 0)
					{ // 読み込んだ文字列が LOOK_REV の場合

						fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
						fscanf(pFile, "%f", &m_aStatusInfo[typeID].fLookRev);	// 視認の補正係数を読み込む
					}
					else if (strcmp(&aString[0], "COLL_RADIUS") == 0)
					{ // 読み込んだ文字列が COLL_RADIUS の場合

						fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
						fscanf(pFile, "%f", &m_aStatusInfo[typeID].fCollRadius);	// 当たり判定の半径を読み込む
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);	// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}

			// キャラクターの設定
			else if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{ // 読み込んだ文字列が CHARACTERSET の場合

				do
				{ // 読み込んだ文字列が END_CHARACTERSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "PARTSSET") == 0)
					{ // 読み込んだ文字列が PARTSSET の場合

						do
						{ // 読み込んだ文字列が END_PARTSSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "INDEX") == 0)
							{ // 読み込んだ文字列が INDEX の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nID);			// モデルのインデックスを読み込む

								// パーツ数を加算
								m_aPartsInfo[typeID].nNumParts++;
							}
							else if (strcmp(&aString[0], "PARENT") == 0)
							{ // 読み込んだ文字列が PARENT の場合

								fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aPartsInfo[typeID].aInfo[nID].nParentID);	// モデルの親のインデックスを読み込む
							}
							else if (strcmp(&aString[0], "POS") == 0)
							{ // 読み込んだ文字列が POS の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].pos.x);	// X座標を読み込む
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].pos.y);	// Y座標を読み込む
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].pos.z);	// Z座標を読み込む
							}
							else if (strcmp(&aString[0], "ROT") == 0)
							{ // 読み込んだ文字列が ROT の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].rot.x);	// X向きを読み込む
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].rot.y);	// Y向きを読み込む
								fscanf(pFile, "%f", &m_aPartsInfo[typeID].aInfo[nID].rot.z);	// Z向きを読み込む
							}
						} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// 読み込んだ文字列が END_PARTSSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// 読み込んだ文字列が END_CHARACTERSET ではない場合ループ
			}

			// 当たり判定の設定
			else if (strcmp(&aString[0], "COLLISIONSET") == 0)
			{ // 読み込んだ文字列が COLLISIONSET の場合

				do
				{ // 読み込んだ文字列が END_COLLISIONSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "COLLSET") == 0)
					{ // 読み込んだ文字列が COLLSET の場合

						do
						{ // 読み込んだ文字列が END_COLLSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "PARTS") == 0)
							{ // 読み込んだ文字列が PARTS の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nParts);		// パーツ番号を読み込む

								// 円判定情報を設定
								m_aCollInfo->apInfo[nParts] = CCollSphere::Create(nullptr);

								// 円判定配列を設定
								vector = m_aCollInfo->apInfo[nParts]->GetVector();
							}
							else if (strcmp(&aString[0], "COLL") == 0)
							{ // 読み込んだ文字列が COLL の場合

								do
								{ // 読み込んだ文字列が END_COLL ではない場合ループ

									// ファイルから文字列を読み込む
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "OFFSET") == 0)
									{ // 読み込んだ文字列が OFFSET の場合

										fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
										fscanf(pFile, "%f", &infoColl.offset.x);	// Xオフセットを読み込む
										fscanf(pFile, "%f", &infoColl.offset.y);	// Yオフセットを読み込む
										fscanf(pFile, "%f", &infoColl.offset.z);	// Zオフセットを読み込む
									}
									else if (strcmp(&aString[0], "RADIUS") == 0)
									{ // 読み込んだ文字列が RADIUS の場合

										fscanf(pFile, "%s", &aString[0]);		// = を読み込む (不要)
										fscanf(pFile, "%f", &infoColl.fRadius);	// 半径を読み込む
									}
								} while (strcmp(&aString[0], "END_COLL") != 0);	// 読み込んだ文字列が END_COLL ではない場合ループ

								// 判定情報を配列に追加
								vector.push_back(infoColl);
							}
						} while (strcmp(&aString[0], "END_COLLSET") != 0);	// 読み込んだ文字列が END_COLLSET ではない場合ループ

						// 判定情報の配列を設定
						m_aCollInfo->apInfo[nParts]->SetVector(vector);
					}
				} while (strcmp(&aString[0], "END_COLLISIONSET") != 0);		// 読み込んだ文字列が END_COLLISIONSET ではない場合ループ
			}

			// モーションの設定
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // 読み込んだ文字列が MOTIONSET の場合

				// 現在のポーズ番号を初期化
				nNowPose = 0;

				// ポーズ代入用の変数を初期化
				memset(&infoKey, 0, sizeof(infoKey));

				// キャンセル・コンボフレームをなしにする
				infoKey.nCancelFrame = NONE_IDX;
				infoKey.nComboFrame  = NONE_IDX;

				// 攻撃判定情報を初期化
				infoKey.collLeft.nMin  = NONE_IDX;
				infoKey.collLeft.nMax  = NONE_IDX;
				infoKey.collRight.nMin = NONE_IDX;
				infoKey.collRight.nMax = NONE_IDX;

				// 武器表示をOFFにする
				infoKey.bWeaponDisp = false;

				do
				{ // 読み込んだ文字列が END_MOTION ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "WEAPON") == 0)
					{ // 読み込んだ文字列が WEAPON の場合

						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &nWeapon);		// 武器表示のON/OFFを読み込む

						// 読み込んだ値をbool型に変換
						infoKey.bWeaponDisp = (nWeapon == 0) ? false : true;
					}
					else if (strcmp(&aString[0], "LOOP") == 0)
					{ // 読み込んだ文字列が LOOP の場合

						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &nLoop);		// ループのON/OFFを読み込む

						// 読み込んだ値をbool型に変換
						infoKey.bLoop = (nLoop == 0) ? false : true;
					}
					else if (strcmp(&aString[0], "NUM_KEY") == 0)
					{ // 読み込んだ文字列が NUM_KEY の場合

						fscanf(pFile, "%s", &aString[0]);		// = を読み込む (不要)
						fscanf(pFile, "%d", &infoKey.nNumKey);	// キーの総数を読み込む
					}
					else if (strcmp(&aString[0], "CANCEL") == 0)
					{ // 読み込んだ文字列が CANCEL の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%d", &infoKey.nCancelFrame);	// キャンセル可能フレームを読み込む
					}
					else if (strcmp(&aString[0], "COMBO") == 0)
					{ // 読み込んだ文字列が COMBO の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%d", &infoKey.nComboFrame);	// コンボ可能フレームを読み込む
					}
					else if (strcmp(&aString[0], "LEFT_COLL") == 0)
					{ // 読み込んだ文字列が LEFT_COLL の場合

						fscanf(pFile, "%s", &aString[0]);				// = を読み込む (不要)
						fscanf(pFile, "%d", &infoKey.collLeft.nMin);	// 判定を出す開始フレームを読み込む
						fscanf(pFile, "%d", &infoKey.collLeft.nMax);	// 判定を消す終了フレームを読み込む
					}
					else if (strcmp(&aString[0], "RIGHT_COLL") == 0)
					{ // 読み込んだ文字列が RIGHT_COLL の場合

						fscanf(pFile, "%s", &aString[0]);				// = を読み込む (不要)
						fscanf(pFile, "%d", &infoKey.collRight.nMin);	// 判定を出す開始フレームを読み込む
						fscanf(pFile, "%d", &infoKey.collRight.nMax);	// 判定を消す終了フレームを読み込む
					}
					else if (strcmp(&aString[0], "KEYSET") == 0)
					{ // 読み込んだ文字列が KEYSET の場合

						// 現在のキー番号を初期化
						nNowKey = 0;

						do
						{ // 読み込んだ文字列が END_KEYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "FRAME") == 0)
							{ // 読み込んだ文字列が FRAME の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%d", &infoKey.aKeyInfo[nNowPose].nFrame);	// キーが切り替わるまでのフレーム数を読み込む
							}
							else if (strcmp(&aString[0], "MOVE") == 0)
							{ // 読み込んだ文字列が MOVE の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].move.x);	// キーが切り替わるまでの移動量を読み込む
								fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].move.y);	// キーが切り替わるまでの移動量を読み込む
								fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].move.z);	// キーが切り替わるまでの移動量を読み込む
							}
							else if (strcmp(&aString[0], "KEY") == 0)
							{ // 読み込んだ文字列が KEY の場合

								do
								{ // 読み込んだ文字列が END_KEY ではない場合ループ

									// ファイルから文字列を読み込む
									fscanf(pFile, "%s", &aString[0]);

									if (strcmp(&aString[0], "POS") == 0)
									{ // 読み込んだ文字列が POS の場合

										fscanf(pFile, "%s", &aString[0]);										// = を読み込む (不要)
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].pos.x);	// X位置を読み込む
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].pos.y);	// Y位置を読み込む
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].pos.z);	// Z位置を読み込む

										// 読み込んだ位置にパーツの初期位置を加算
										infoKey.aKeyInfo[nNowPose].aKey[nNowKey].pos += m_aPartsInfo[typeID].aInfo[nNowKey].pos;
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // 読み込んだ文字列が ROT の場合

										fscanf(pFile, "%s", &aString[0]);										// = を読み込む (不要)
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);	// X向きを読み込む
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);	// Y向きを読み込む
										fscanf(pFile, "%f", &infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);	// Z向きを読み込む

										// 読み込んだ向きにパーツの初期向きを加算
										infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot += m_aPartsInfo[typeID].aInfo[nNowKey].rot;

										// 初期向きを正規化
										useful::NormalizeRot(infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);
										useful::NormalizeRot(infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);
										useful::NormalizeRot(infoKey.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);
									}

								} while (strcmp(&aString[0], "END_KEY") != 0);	// 読み込んだ文字列が END_KEY ではない場合ループ

								// 現在のキー番号を加算
								nNowKey++;
							}
						} while (strcmp(&aString[0], "END_KEYSET") != 0);	// 読み込んだ文字列が END_KEYSET ではない場合ループ

						// 現在のポーズ番号を加算
						nNowPose++;
					}
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);	// 読み込んだ文字列が END_MOTIONSET ではない場合ループ

				// モーション情報の設定
				m_aMotionInfo[typeID].aMotionInfo[m_aMotionInfo[typeID].nNumType] = infoKey;

				// モーション数を加算
				m_aMotionInfo[typeID].nNumType++;
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "敵セットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
