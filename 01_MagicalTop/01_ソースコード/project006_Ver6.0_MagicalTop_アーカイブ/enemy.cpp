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
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "sound.h"
#include "texture.h"
#include "multiModel.h"
#include "shadow.h"
#include "objectBillboard.h"
#include "enemyWay.h"
#include "bubble.h"
#include "collision.h"
#include "player.h"
#include "target.h"
#include "expOrb.h"
#include "bullet.h"
#include "stage.h"
#include "score.h"
#include "effect3D.h"
#include "particle3D.h"
#include "field.h"

//************************************************************
//	マクロ定義
//************************************************************
// 敵全体マクロ
#define ENEMY_SETUP_TXT	"data\\TXT\\enemy.txt"	// セットアップテキスト相対パス

#define SIZE_WARNING	(D3DXVECTOR3(160.0f, 160.0f, 0.0f))	// 警告表示の大きさ
#define WARNING_PRIO	(4)	// 警告表示の優先順位

#define ENE_REV				(0.02f)		// 敵の移動量の減衰係数
#define ENE_GRAVITY			(1.0f)		// 敵の重力
#define ENE_DMG_FRAME		(20)		// 敵のダメージ状態フレーム
#define ENE_WAR_ADD_POSY	(100.0f)	// 警告表示位置 yの加算量
#define ENE_BULL_LIMIT_Y	(0.1f)		// 弾の移動方向の制限値
#define ENE_DEATH_ADD_POSY	(6.0f)		// 死亡時のY座標上昇量
#define ENE_DEATH_POSY		(1300.0f)	// 死亡するY座標
#define ENE_SUB_ALPHA		(0.02f)		// 消失時のα値減算量

#define DAMAGE_CNT		(10)		// ダメージ状態維持カウント
#define BUBBLE_SUB_CNT	(420)		// バブル消失カウント
#define LIMIT_RADIUS	(2400.0f)	// 敵のステージ移動半径

#define MAGIC_EFF_CNT			(2)		// エフェクトの生成フレーム
#define MAGIC_EFF_MUL_POS		(2.0f)	// エフェクトの生成位置の乗算量
#define MAGIC_EFF_MUL_MOVE		(1.0f)	// エフェクトの移動量の乗算量
#define MAGIC_EFF_ALPHA			(0.6f)	// エフェクトの透明度
#define MAGIC_EFF_LIFE			(45)	// エフェクトの寿命
#define MAGIC_EFF_MUL_RADIUS	(1.5f)	// エフェクトの半径の乗算量
#define MAGIC_EFF_SUB_RADIUS	(0.2f)	// エフェクトの半径の減算量
#define MAGIC_EFF_PRIORITY		(3)		// エフェクトの優先順位

// 歩兵マクロ
#define HUMAN_FALL	(5.5f)	// 歩兵生成時の落下速度

// 戦車マクロ
#define ENECAR_REV_MUL		(0.03f)	// 主砲の向き補正値
#define ENECAR_SHOTRANGE	(0.25f)	// 主砲射撃時の許容される向きの誤差量

#define SPAWN_WAIT_CNT	(40)	// 戦車スポーン上空待機カウント

//************************************************************
//	静的メンバ変数宣言
//************************************************************
// 敵全体メンバ変数
CEnemy::StatusInfo CEnemy::m_aStatusInfo[CEnemy::TYPE_MAX] = {};	// ステータス情報
CEnemy::PartsInfo CEnemy::m_aPartsInfo[CEnemy::TYPE_MAX] = {};		// パーツ情報
CMotion::Info CEnemy::m_aMotionInfo[CEnemy::TYPE_MAX] = {};			// モーション情報
int CEnemy::m_nNumAll = 0;											// 敵の総数

const char *CEnemy::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\warning000.png",	// 警告表示テクスチャ
};

// 歩兵メンバ変数
const char *CEnemyHuman::mc_apModelFile[] =	// 歩兵モデル定数
{
	"data\\MODEL\\ENEMY\\HUMAN\\00_waist.x",				// 腰
	"data\\MODEL\\ENEMY\\HUMAN\\01_body.x",					// 体
	"data\\MODEL\\ENEMY\\HUMAN\\02_head.x",					// 頭
	"data\\MODEL\\ENEMY\\HUMAN\\03_armUL.x",				// 左上腕
	"data\\MODEL\\ENEMY\\HUMAN\\04_armUR.x",				// 右上腕
	"data\\MODEL\\ENEMY\\HUMAN\\05_armDL.x",				// 左下腕
	"data\\MODEL\\ENEMY\\HUMAN\\06_armDR.x",				// 右下腕
	"data\\MODEL\\ENEMY\\HUMAN\\07_handL.x",				// 左手
	"data\\MODEL\\ENEMY\\HUMAN\\08_handR.x",				// 右手
	"data\\MODEL\\ENEMY\\HUMAN\\09_legUL.x",				// 左太もも
	"data\\MODEL\\ENEMY\\HUMAN\\10_legUR.x",				// 右太もも
	"data\\MODEL\\ENEMY\\HUMAN\\11_legDL.x",				// 左脛
	"data\\MODEL\\ENEMY\\HUMAN\\12_legDR.x",				// 右脛
	"data\\MODEL\\ENEMY\\HUMAN\\13_footL.x",				// 左足
	"data\\MODEL\\ENEMY\\HUMAN\\14_footR.x",				// 右足
	"data\\MODEL\\ENEMY\\HUMAN\\15_waistpouchBack.x",		// ウエストポーチ(後)
	"data\\MODEL\\ENEMY\\HUMAN\\16_waistpouchRight.x",		// ウエストポーチ(左)
	"data\\MODEL\\ENEMY\\HUMAN\\17_waistpouchBackright.x",	// ウエストポーチ(左後)
	"data\\MODEL\\ENEMY\\HUMAN\\18_legURpouch.x",			// 右太ももポーチ
	"data\\MODEL\\ENEMY\\HUMAN\\19_armor.x",				// アーマー
	"data\\MODEL\\ENEMY\\HUMAN\\20_knife.x",				// ナイフ
	"data\\MODEL\\ENEMY\\HUMAN\\21_assault.x",				// 銃
};

// 戦車メンバ変数
const char *CEnemyCar::mc_apModelFile[] =	// 戦車モデル定数
{
	"data\\MODEL\\ENEMY\\CAR\\caterpillar000.x",	// キャタピラ
	"data\\MODEL\\ENEMY\\CAR\\cannon000.x",			// キャノン
};

//************************************************************
//	子クラス [CEnemy] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemy::CEnemy(const TYPE type) : CObjectChara(CObject::LABEL_ENEMY), m_type(type), m_status(m_aStatusInfo[type]), m_parts(m_aPartsInfo[type]), m_motion(m_aMotionInfo[type])
{
	// メンバ変数をクリア
	m_pShadow	= NULL;			// 影の情報
	m_pWarning	= NULL;			// 警告の情報
	m_pWay		= NULL;			// 方向の情報
	m_pBubble	= NULL;			// バブルの情報
	m_oldPos	= VEC3_ZERO;	// 過去位置
	m_movePos	= VEC3_ZERO;	// 位置移動量
	m_moveKnock	= VEC3_ZERO;	// ノックバック移動量
	m_moveRot	= VEC3_ZERO;	// 向き変更量
	m_deathMoveRot		= VEC3_ZERO;	// 死亡時の向き変更量
	m_state				= STATE_SPAWN;	// 状態
	m_nCounterEffect	= 0;			// エフェクト管理カウンター
	m_nCounterBubble	= 0;			// バブル管理カウンター
	m_nCounterState		= 0;			// 状態管理カウンター
	m_nCounterAtk		= 0;			// 攻撃管理カウンター

	// 敵の総数を加算
	m_nNumAll++;
}

//============================================================
//	デストラクタ
//============================================================
CEnemy::~CEnemy()
{
	// 敵の総数を減算
	m_nNumAll--;
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemy::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_pShadow	= NULL;			// 影の情報
	m_pWarning	= NULL;			// 警告の情報
	m_pWay		= NULL;			// 方向の情報
	m_pBubble	= NULL;			// バブルの情報
	m_oldPos	= VEC3_ZERO;	// 過去位置
	m_movePos	= VEC3_ZERO;	// 位置移動量
	m_moveKnock	= VEC3_ZERO;	// ノックバック移動量
	m_moveRot	= VEC3_ZERO;	// 向き変更量
	m_state		= STATE_SPAWN;	// 状態
	m_nCounterEffect	= 0;	// エフェクト管理カウンター
	m_nCounterBubble	= 0;	// バブル管理カウンター
	m_nCounterState		= 0;	// 状態管理カウンター
	m_nCounterAtk		= 0;	// 攻撃管理カウンター

	// 死亡時の向き変更量
	m_deathMoveRot.x += (rand() % 31 - 15) * 0.001f;
	m_deathMoveRot.y += (rand() % 31 - 15) * 0.001f;
	m_deathMoveRot.z += (rand() % 31 - 15) * 0.001f;

	//--------------------------------------------------------
	//	影の生成
	//--------------------------------------------------------
	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, D3DXVECTOR3(m_status.fShadowRadius, 0.0f, m_status.fShadowRadius), this);
	if (UNUSED(m_pShadow))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	警告表示の生成・設定
	//--------------------------------------------------------
	// 警告表示の生成
	m_pWarning = CObjectBillboard::Create(VEC3_ZERO, SIZE_WARNING);
	if (UNUSED(m_pWarning))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pWarning->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_WARNING]));

	// 優先順位を設定
	m_pWarning->SetPriority(WARNING_PRIO);

	//--------------------------------------------------------
	//	方向表示の生成
	//--------------------------------------------------------
	// 方向表示の生成
	m_pWay = CEnemyWay::Create(this, LIMIT_RADIUS - m_aStatusInfo[m_type].fRadius);
	if (UNUSED(m_pWay))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	バブルの生成
	//--------------------------------------------------------
	// バブルを生成
	m_pBubble = CBubble::Create(this, m_status.nLife, m_status.bubbleSize, VEC3_ZERO, m_status.fHeight * 0.5f);
	if (UNUSED(m_pBubble))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// オブジェクトキャラクターの初期化
	if (FAILED(CObjectChara::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
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
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemy::Uninit(void)
{
	// 方向表示を削除
	m_pWay->Delete();

	// 影の終了
	m_pShadow->Uninit();

	// 警告表示の終了
	m_pWarning->Uninit();

	// バブルの終了
	m_pBubble->Uninit();

	// オブジェクトキャラクターの終了
	CObjectChara::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemy::Update(void)
{
	// 影の更新
	m_pShadow->Update();

	// 警告表示の更新
	m_pWarning->Update();

	// バブルの更新
	m_pBubble->Update();

	// オブジェクトキャラクターの更新
	CObjectChara::Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemy::Draw(void)
{
	// オブジェクトキャラクターの描画
	CObjectChara::Draw();
}

//============================================================
//	ヒット処理
//============================================================
void CEnemy::Hit(const int nDmg)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// 敵位置

	if (IsDeath() != true && m_state == STATE_NORMAL)
	{ // 死亡フラグが立っていない且つ、通常状態の場合

		// レベルを加算
		m_pBubble->AddLevel(nDmg);

		if (m_pBubble->GetLevel() < m_status.nLife)
		{ // 生きている場合

			// カウンターを初期化
			m_nCounterState = 0;
			m_nCounterBubble = 0;

			// 状態を設定
			m_state = STATE_DAMAGE;	// ダメージ状態
		}
		else
		{ // 死んでいる場合

			// モーションを更新しない状態にする
			SetEnableMotionUpdate(false);

			// 方向表示を削除
			m_pWay->Delete();

			// 状態を設定
			m_state = STATE_DEATH;	// 死亡状態

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_FLY);	// 浮遊音
		}

		// パーティクル3Dオブジェクトを生成
		CParticle3D::Create(CParticle3D::TYPE_BUBBLE_EXPLOSION, pos);
	}
}

//============================================================
//	ノックバックヒット処理
//============================================================
void CEnemy::HitKnockBack(const int nDmg, const D3DXVECTOR3& vec)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// 敵位置
	D3DXVECTOR3 vecKnock = vec;			// ノックバックベクトル

	if (IsDeath() != true && m_state == STATE_NORMAL)
	{ // 死亡フラグが立っていない且つ、通常状態の場合

		// レベルを加算
		m_pBubble->AddLevel(nDmg);

		if (m_pBubble->GetLevel() < m_status.nLife)
		{ // 生きている場合

			// ノックバック移動量を設定
			vecKnock.y = 0.0f;									// 縦ベクトルを初期化
			D3DXVec3Normalize(&vecKnock, &vecKnock);			// ベクトルを正規化
			m_moveKnock = vecKnock * m_status.fKnockBackSide;	// ノックバック横移動量を計算
			m_moveKnock.y = m_status.fKnockBackUp;				// ノックバック縦移動量を代入

			// カウンターを初期化
			m_nCounterState = 0;
			m_nCounterBubble = 0;

			// 状態を設定
			m_state = STATE_DAMAGE;	// ダメージ状態
		}
		else
		{ // 死んでいる場合

			// モーションを更新しない状態にする
			SetEnableMotionUpdate(false);

			// 方向表示を削除
			m_pWay->Delete();

			// 状態を設定
			m_state = STATE_DEATH;	// 死亡状態

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_FLY);	// 浮遊音
		}

		// パーティクル3Dオブジェクトを生成
		CParticle3D::Create(CParticle3D::TYPE_BUBBLE_EXPLOSION, pos);
	}
}

//============================================================
//	生成処理
//============================================================
CEnemy *CEnemy::Create(const TYPE type, const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot)
{
	// ポインタを宣言
	CEnemy *pEnemy = NULL;	// 敵生成用

	if (UNUSED(pEnemy))
	{ // 使用されていない場合

		// メモリ確保
		switch (type)
		{ // 種類ごとの処理
		case TYPE_HUMAN:	// 戦車

			// 歩兵を生成
			pEnemy = new CEnemyHuman(type);

			break;

		case TYPE_CAR:	// 戦車

			// 戦車を生成
			pEnemy = new CEnemyCar(type);

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pEnemy))
	{ // 使用されている場合
		
		// 敵の初期化
		if (FAILED(pEnemy->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEnemy;
			pEnemy = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pEnemy->SetPosition(rPos);

		// 向きを設定
		pEnemy->SetRotation(rRot);

		// 確保したアドレスを返す
		return pEnemy;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ランダム生成処理
//============================================================
void CEnemy::RandomSpawn
(
	const int nNum,	// 生成数
	const TYPE type	// 種類
)
{
	// 変数を宣言
	D3DXVECTOR3 pos, rot;	// 位置・向き設定用
	D3DXVECTOR3 posTarget;	// ターゲット位置

	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (USED(CScene::GetTarget()))
	{ // ターゲットが使用されている場合

		for (int nCntGrow = 0; nCntGrow < nNum; nCntGrow++)
		{ // 生成数分繰り返す

			// ターゲット位置を取得
			posTarget = pTarget->GetPosition();

			// 生成位置を設定
			pos.x = (float)(rand() % ((int)LIMIT_RADIUS * 2) - (int)LIMIT_RADIUS + 1);
			pos.y = m_aStatusInfo[type].fSpawnHeight;
			pos.z = (float)(rand() % ((int)LIMIT_RADIUS * 2) - (int)LIMIT_RADIUS + 1);

			// 生成向きを設定
			rot = VEC3_ZERO;	// 向きを初期化
			rot.y = atan2f(pos.x - posTarget.x, pos.z - posTarget.z);	// ターゲットの方向を向かせる

			// 位置を補正
			pos.x = sinf(rot.y) * (LIMIT_RADIUS - m_aStatusInfo[type].fRadius);
			pos.z = cosf(rot.y) * (LIMIT_RADIUS - m_aStatusInfo[type].fRadius);

			// 敵オブジェクトの生成
			CEnemy::Create(type, pos, rot);
		}
	}
}

//============================================================
//	ステータス情報取得処理
//============================================================
CEnemy::StatusInfo CEnemy::GetStatusInfo(const int nType)
{
	// 引数の種類のステータス情報を返す
	return m_aStatusInfo[nType];
}

//============================================================
//	全消失処理
//============================================================
void CEnemy::SetAllVanish(void)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (USED(pObjectTop))
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (USED(pObjCheck))
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() == CObject::LABEL_ENEMY)
				{ // オブジェクトラベルが敵だった場合

					// 状態を設定
					pObjCheck->SetState(STATE_VANISH);	// 消失状態
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	総数取得処理
//============================================================
int CEnemy::GetNumAll(void)
{
	// 現在の敵の総数を返す
	return m_nNumAll;
}

//============================================================
//	過去位置の更新処理
//============================================================
void CEnemy::UpdateOldPosition(void)
{
	// 過去位置を更新
	m_oldPos = GetPosition();
}

//============================================================
//	ノックバックの更新処理
//============================================================
void CEnemy::UpdateKnockBack(D3DXVECTOR3& rPos)
{
	// ノックバックを位置に反映
	rPos += m_moveKnock;

	// ノックバックに重力を加算
	m_moveKnock.y -= ENE_GRAVITY;

	// ノックバックを減衰
	m_moveKnock.x += (0.0f - m_moveKnock.x) * m_status.fKnockBackRevision;
	m_moveKnock.z += (0.0f - m_moveKnock.z) * m_status.fKnockBackRevision;
}

//============================================================
//	位置移動量の設定処理
//============================================================
void CEnemy::SetMovePosition(const D3DXVECTOR3& rMove)
{
	// 引数の位置移動量を設定
	m_movePos = rMove;
}

//============================================================
//	向き変更量の設定処理
//============================================================
void CEnemy::SetMoveRotation(const D3DXVECTOR3& rMove)
{
	// 引数の向きを設定
	m_moveRot = rMove;

	// 向きの正規化
	useful::NormalizeRot(m_moveRot.x);
	useful::NormalizeRot(m_moveRot.y);
	useful::NormalizeRot(m_moveRot.z);
}

//============================================================
//	状態の設定処理
//============================================================
void CEnemy::SetState(const int nState)
{
	// 引数の状態を設定
	m_state = (STATE)nState;
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CEnemy::GetMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxWorld;	// 計算用マトリックス
	D3DXVECTOR3 posEnemy = GetPosition();	// 敵位置
	D3DXVECTOR3 rotEnemy = GetRotation();	// 敵向き

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotEnemy.y, rotEnemy.x, rotEnemy.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, posEnemy.x, posEnemy.y, posEnemy.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスを返す
	return mtxWorld;
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
//	位置移動量取得処理
//============================================================
D3DXVECTOR3 CEnemy::GetMovePosition(void) const
{
	// 位置移動量を返す
	return m_movePos;
}

//============================================================
//	向き変更量取得処理
//============================================================
D3DXVECTOR3 CEnemy::GetMoveRotation(void) const
{
	// 向き変更量を返す
	return m_moveRot;
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
	// 変数を宣言
	float fRadius = 0.0f;	// 半径

	if (m_status.fCollRadius < m_pBubble->GetRadius())
	{ // 当たり判定用半径がバブルの半径より小さい場合

		// バブルの半径を設定
		fRadius = m_pBubble->GetRadius();
	}
	else
	{ // 当たり判定用半径がバブルの半径より大きい場合

		// 当たり判定用半径を設定
		fRadius = m_status.fCollRadius;
	}

	// 半径を返す
	return fRadius;
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
//	ステータス情報取得処理
//============================================================
CEnemy::StatusInfo CEnemy::GetStatusInfo(void) const
{
	// ステータス情報を返す
	return m_status;
}

//============================================================
//	パーツ情報取得処理
//============================================================
CEnemy::PartsInfo CEnemy::GetPartsInfo(void) const
{
	// パーツ情報を返す
	return m_parts;
}

//============================================================
//	モーション情報取得処理
//============================================================
CMotion::Info CEnemy::GetMotionInfo(void) const
{
	// モーション情報を返す
	return m_motion;
}

//============================================================
//	スポーン動作
//============================================================
void CEnemy::Spawn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetPosition();		// 敵位置
	D3DXVECTOR3 moveEnemy = GetMovePosition();	// 敵移動量
	D3DXVECTOR3 posWarning = posEnemy;			// 警告位置
	D3DXVECTOR3 posBarrier = CScene::GetStage()->GetStageBarrierPosition();	// バリア位置

	// 過去位置の更新
	UpdateOldPosition();

	// スポーン状態の敵との当たり判定
	CollisionSpawnEnemy(posEnemy);

	// ステージ範囲外の補正
	collision::InCirclePillar(posEnemy, posBarrier, m_status.fRadius, LIMIT_RADIUS);

	// 重力を加算
	moveEnemy.y -= ENE_GRAVITY;

	// 移動量を加算
	posEnemy += moveEnemy;

	// 着地判定
	if (CScene::GetField()->LandPosition(posEnemy, moveEnemy))
	{ // 着地した場合

		// 描画をしない設定にする
		m_pWarning->SetEnableDraw(false);

		// 状態を設定
		m_state = STATE_NORMAL;	// 通常状態
	}

	// 位置を反映
	SetPosition(posEnemy);

	// 位置移動量を反映
	SetMovePosition(moveEnemy);

	// 警告位置を地形に添わせる
	posWarning.y = CScene::GetField()->GetPositionHeight(posWarning);

	// 警告表示位置を設定
	m_pWarning->SetPosition(D3DXVECTOR3(posWarning.x, posWarning.y + ENE_WAR_ADD_POSY, posWarning.z));
}

//============================================================
//	ダメージ動作
//============================================================
void CEnemy::Damage(void)
{
	if (m_nCounterState < DAMAGE_CNT)
	{ // カウンターが一定値より小さい場合

		// カウンターを加算
		m_nCounterState++;

		// マテリアルを設定
		SetMaterial(material::Red());	// 赤
	}
	else
	{ // カウンターが一定値以上の場合

		// カウンターを初期化
		m_nCounterState = 0;

		// マテリアル再設定
		ResetMaterial();

		// 状態を設定
		m_state = STATE_NORMAL;	// 通常状態
	}
}

//============================================================
//	死亡動作
//============================================================
bool CEnemy::Death(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetPosition();	// 敵位置
	float fBonus = CScene::GetPlayer()->GetStatusInfo((CPlayer::LEVEL)CScene::GetPlayer()->GetLevelStatus(CPlayer::LEVELINFO_EXP_UP)).fMulExp;	// 経験値ボーナス量

	// 上に位置を動かす
	posEnemy.y += ENE_DEATH_ADD_POSY;

	// 位置を反映
	SetPosition(posEnemy);

	if (m_nCounterEffect < MAGIC_EFF_CNT)
	{ // カウンターが一定値より小さい場合

		// カウンターを加算
		m_nCounterEffect++;
	}
	else
	{ // カウンターが一定値以上の場合

		// 変数を宣言
		D3DXVECTOR3 vecRandom;	// ランダムベクトル

		// ポインタを宣言
		CEffect3D *pEffect3D;	// エフェクト情報

		// ランダムベクトルを設定
		vecRandom.x = sinf((float)(rand() % 629 - 314) / 100.0f) * 1.0f;
		vecRandom.y = cosf((float)(rand() % 629 - 314) / 100.0f) * 1.0f;
		vecRandom.z = cosf((float)(rand() % 629 - 314) / 100.0f) * 1.0f;

		// エフェクト3Dを生成
		pEffect3D = CEffect3D::Create
		( // 引数
			CEffect3D::TYPE_BUBBLE,							// テクスチャ
			posEnemy + (vecRandom * MAGIC_EFF_MUL_POS),		// 位置
			vecRandom * MAGIC_EFF_MUL_MOVE,					// 移動量
			VEC3_ZERO,										// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, MAGIC_EFF_ALPHA),	// 色
			MAGIC_EFF_LIFE,									// 寿命
			m_pBubble->GetRadius() * MAGIC_EFF_MUL_RADIUS,	// 半径
			MAGIC_EFF_SUB_RADIUS							// 半径の減算量
		);

		// 優先順位を設定
		pEffect3D->SetPriority(MAGIC_EFF_PRIORITY);

		// カウンターを初期化
		m_nCounterEffect = 0;
	}

	if (posEnemy.y > ENE_DEATH_POSY)
	{ // 座標が上に行き切った場合

		if (CManager::GetScene()->GetMode() == CScene::MODE_GAME)
		{ // モードがゲームの場合

			// スコアを加算
			CSceneGame::GetScore()->Add(m_status.nScore);
		}

		// 経験値のランダム生成
		CExpOrb::RandomSpawn((int)(m_status.nExp * fBonus), posEnemy);

		// 敵オブジェクトの終了
		Uninit();

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_SPAWN);	// 生成音

		// 死亡を返す
		return true;
	}

	// 未死亡を返す
	return false;
}

//============================================================
//	消失動作
//============================================================
bool CEnemy::Vanish(void)
{
	// 変数を宣言
	float fAlpha = GetAlpha();	// 透明度

	if (fAlpha <= 0.0f)
	{ // 透明度が下がり切った場合

		// 敵オブジェクトの終了
		Uninit();

		// 消失を返す
		return true;
	}

	// 透明度を下げる
	fAlpha -= ENE_SUB_ALPHA;

	// 透明度を設定
	SetAlpha(fAlpha);

	// 未消失を返す
	return false;
}

//============================================================
//	対象視認処理
//============================================================
void CEnemy::Look(const D3DXVECTOR3& rPosLook, const D3DXVECTOR3& rPosEnemy, D3DXVECTOR3& rRotEnemy)
{
	// 変数を宣言
	float fDestRot = 0.0f;	// 目標向き
	float fDiffRot = 0.0f;	// 向き

	// 視認対象の方向を代入
	fDestRot = atan2f(rPosEnemy.x - rPosLook.x, rPosEnemy.z - rPosLook.z);	// 目標向き

	// 差分向きを求める
	fDiffRot = fDestRot - rRotEnemy.y;
	useful::NormalizeRot(fDiffRot);		// 差分向きの正規化

	// 向きの更新
	rRotEnemy.y += fDiffRot * m_status.fLookRevision;
	useful::NormalizeRot(rRotEnemy.y);	// 向きの正規化
}

//============================================================
//	攻撃処理
//============================================================
void CEnemy::Attack(const D3DXVECTOR3& rLookPos, const D3DXVECTOR3& rThisPos, const float fLookRadius)
{
	// カウンターを加算
	m_nCounterAtk++;

	if (m_nCounterAtk >= m_status.nCounterAttack)
	{ // カウンターが一定値以上の場合

		// 変数を宣言
		D3DXVECTOR3 posBull;			// 発射位置
		D3DXVECTOR3 vecBull;			// 発射方向
		D3DXMATRIX  mtxTrans, mtxWorld;	// 計算用マトリックス

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxWorld);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_status.bullPos.x, m_status.bullPos.y, m_status.bullPos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

		// マトリックスを掛け合わせる
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &GetMultiModel(m_status.nBullParts)->GetMtxWorld());

		// マトリックスから位置を求める
		posBull = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

		// 弾の移動方向を求める
		vecBull = rLookPos - posBull;
		D3DXVec3Normalize(&vecBull, &vecBull);								// 移動方向を正規化
		useful::LimitNum(vecBull.y, -ENE_BULL_LIMIT_Y, ENE_BULL_LIMIT_Y);	// 移動方向Yを制限

		if (!collision::Circle2D(rThisPos, rLookPos, m_status.fRadius, fLookRadius))
		{ // 発射位置より内側に標的が入っていない場合

			// 弾オブジェクトの生成
			CBullet::Create
			( // 引数
				CBullet::TYPE_ENEMY,			// 種類
				posBull,						// 位置
				VEC3_ALL(m_status.fBullRadius),	// 大きさ
				XCOL_WHITE,						// 色
				vecBull,						// 移動方向
				m_status.fBullMove,				// 移動速度
				m_status.nBullLife,				// 寿命
				m_status.nBullDamage			// 攻撃力
			);

			// カウンターを初期化
			m_nCounterAtk = 0;

			switch (m_type)
			{ // 種類ごとの処理
			case TYPE_HUMAN:	// 歩兵

				// パーティクル3Dオブジェクトの生成
				CParticle3D::Create(CParticle3D::TYPE_MUZZLE_FLASH, posBull);

				// サウンドの再生
				CManager::GetSound()->Play(CSound::LABEL_SE_ENEMY_SHOT000);	// 敵射撃音 (銃)

				break;

			case TYPE_CAR:		// 戦車

				// パーティクル3Dオブジェクトの生成
				CParticle3D::Create(CParticle3D::TYPE_TANK_FIRE, posBull);

				// サウンドの再生
				CManager::GetSound()->Play(CSound::LABEL_SE_ENEMY_SHOT001);	// 敵射撃音 (大砲)

				break;

			default:	// 例外処理
				assert(false);
				break;
			}
		}
	}
}

//============================================================
//	ターゲットとの当たり判定
//============================================================
void CEnemy::CollisionTarget(D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (USED(pTarget))
	{ // ターゲットが使用されている場合

		// ターゲットとの衝突判定
		collision::CirclePillar
		( // 引数
			rPos,	// 判定位置
			pTarget->GetPosition(),	// 判定目標位置
			m_status.fRadius,		// 判定半径
			pTarget->GetRadius() + m_status.fRadius	// 判定目標半径
		);
	}
}

//============================================================
//	スポーン時の敵との当たり判定
//============================================================
void CEnemy::CollisionSpawnEnemy(D3DXVECTOR3& rPos)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (USED(pObjectTop))
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (USED(pObjCheck))
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_ENEMY || pObjCheck == GetObject())
				{ // オブジェクトラベルが敵ではない、またはオブジェクトが自分自身だった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() != CEnemy::STATE_SPAWN)
				{ // 敵の状態がスポーン状態ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ターゲットとの衝突判定
				collision::CirclePillar
				( // 引数
					rPos,						// 判定位置
					pObjCheck->GetPosition(),	// 判定目標位置
					m_status.fRadius,			// 判定半径
					CEnemy::GetStatusInfo(pObjCheck->GetType()).fRadius	// 判定目標半径
				);

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	通常時の敵との当たり判定
//============================================================
void CEnemy::CollisionNormalEnemy(D3DXVECTOR3& rPos)
{
	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (USED(pObjectTop))
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (USED(pObjCheck))
			{ // オブジェクトが使用されている場合繰り返す

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_ENEMY || pObjCheck == GetObject())
				{ // オブジェクトラベルが敵ではない、またはオブジェクトが自分自身だった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() != CEnemy::STATE_NORMAL
				&&  pObjCheck->GetState() != CEnemy::STATE_DAMAGE)
				{ // 敵の状態が通常・ダメージ状態ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ターゲットとの衝突判定
				collision::CirclePillar
				( // 引数
					rPos,						// 判定位置
					pObjCheck->GetPosition(),	// 判定目標位置
					m_status.fRadius,			// 判定半径
					CEnemy::GetStatusInfo(pObjCheck->GetType()).fRadius	// 判定目標半径
				);

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	警告の描画状況の設定処理
//============================================================
void CEnemy::SetDrawWarning(const bool bDraw)
{
	// 引数の描画状況を設定
	m_pWarning->SetEnableDraw(bDraw);
}

//============================================================
//	警告の位置の設定処理
//============================================================
void CEnemy::SetPositionWarning(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pWarning->SetPosition(rPos);
}

//============================================================
//	ランダム回転処理
//============================================================
void CEnemy::RandomRotation(void)
{
	// 変数を宣言
	D3DXVECTOR3 rotEnemy = GetPartsRotation(0);	// 敵の親パーツ向き

	// 向きを加算
	rotEnemy += m_deathMoveRot;

	// 敵の親パーツ向きを設定
	SetPartsRotation(0, rotEnemy);
}

//============================================================
//	バブル削除処理
//============================================================
void CEnemy::SubBubble(void)
{
	if (m_pBubble->GetLevel() > 0)
	{ // バブルレベルが 0より大きい場合

		if (m_nCounterBubble < BUBBLE_SUB_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_nCounterBubble++;
		}
		else
		{ // カウンターが一定値以上の場合

			// カウンターを初期化
			m_nCounterBubble = 0;

			// レベルを減算
			m_pBubble->AddLevel(-1);
		}
	}
}

//************************************************************
//	子クラス [CEnemyHuman] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyHuman::CEnemyHuman(const TYPE type) : CEnemy(type)
{

}

//============================================================
//	デストラクタ
//============================================================
CEnemyHuman::~CEnemyHuman()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyHuman::Init(void)
{
	// 変数を宣言
	CMotion::Info motion = GetMotionInfo();

	// 敵の初期化
	if (FAILED(CEnemy::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	for (int nCntEnemy = 0; nCntEnemy < motion.nNumMotion; nCntEnemy++)
	{ // 読み込んだモーション数分繰り返す

		// モーション情報の設定
		CObjectChara::SetMotionInfo(motion.aMotionInfo[nCntEnemy]);
	}

	// モデル情報の設定
	SetModelInfo();

	// モーションの設定
	SetMotion(MOTION_MOVE);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemyHuman::Uninit(void)
{
	// 敵の終了
	CEnemy::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemyHuman::Update(void)
{
	switch (GetState())
	{ // 状態ごとの処理
	case STATE_SPAWN:

		// スポーン動作の更新
		Spawn();

		break;

	case STATE_NORMAL:

		// バブル削除
		SubBubble();

		// 敵の動作の更新
		CollisionFind();

		break;

	case STATE_DAMAGE:

		// ダメージ動作の更新
		Damage();

		// 敵の動作の更新
		CollisionFind();

		break;

	case STATE_DEATH:

		// 死亡動作の更新
		if (Death())
		{ // 死亡した場合

			// 処理を抜ける
			return;
		}

		// ランダム回転
		RandomRotation();

		break;

	case STATE_VANISH:

		// 消失動作の更新
		if (Vanish())
		{ // 消失した場合

			// 処理を抜ける
			return;
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 敵の更新
	CEnemy::Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemyHuman::Draw(void)
{
	// 敵の描画
	CEnemy::Draw();
}

//============================================================
//	モデルファイル取得処理
//============================================================
const char* CEnemyHuman::GetModelFileName(const int nModel) const
{
	if (nModel < MODEL_MAX)
	{ // 使用できるインデックスの場合

		// 引数のインデックスのモデルを返す
		return mc_apModelFile[nModel];
	}
	else { assert(false); return NONE_STRING; }	// 範囲外
}

//============================================================
//	スポーン動作
//============================================================
void CEnemyHuman::Spawn(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEnemy = GetPosition();		// 敵位置
	D3DXVECTOR3 moveEnemy = GetMovePosition();	// 敵移動量
	D3DXVECTOR3 posWarning = posEnemy;			// 警告位置

	// 過去位置の更新
	UpdateOldPosition();

	// スポーン状態の敵との当たり判定
	CollisionSpawnEnemy(posEnemy);

	// 位置を加算
	posEnemy.y -= HUMAN_FALL;
	
	// 着地判定
	if (CScene::GetField()->LandPosition(posEnemy, moveEnemy))
	{ // 着地した場合

		// 警告の描画状況の設定
		SetDrawWarning(false);	// 描画しない
	
		// 状態の設定
		SetState(STATE_NORMAL);	// 通常状態
	}

	// 位置を反映
	SetPosition(posEnemy);

	// 位置移動量を反映
	SetMovePosition(moveEnemy);

	// 警告位置を地形に添わせる
	posWarning.y = CScene::GetField()->GetPositionHeight(posWarning);

	// 警告の位置の設定
	SetPositionWarning(D3DXVECTOR3(posWarning.x, posWarning.y + ENE_WAR_ADD_POSY, posWarning.z));
}

//============================================================
//	検知範囲の当たり判定
//============================================================
void CEnemyHuman::CollisionFind(void)
{
	// 変数を宣言
	StatusInfo  status		= GetStatusInfo();		// 敵ステータス
	D3DXVECTOR3 posEnemy	= GetPosition();		// 敵位置
	D3DXVECTOR3 moveEnemy	= GetMovePosition();	// 敵移動量
	D3DXVECTOR3 rotEnemy	= GetRotation();		// 敵向き
	D3DXVECTOR3 posLook		= VEC3_ZERO;			// 視認対象位置
	float fLookRadius		= 0.0f;					// 視認対象半径

	D3DXVECTOR3 posBarrier	= CScene::GetStage()->GetStageBarrierPosition();	// バリア位置
	float fPlayerRadius		= CScene::GetPlayer()->GetRadius();	// プレイヤー半径

	// 過去位置の更新
	UpdateOldPosition();

	if (CScene::GetTarget()->GetState() != CTarget::STATE_DESTROY)
	{ // ターゲットが壊されていないる場合

		// 視認対象位置を設定
		posLook = CScene::GetTarget()->GetPosition();	// ターゲット位置

		// 視認対象半径を設定
		fLookRadius = CScene::GetTarget()->GetRadius();	// ターゲット半径

		// 視認対象の攻撃判定
		if (collision::Circle2D(posLook, posEnemy, fPlayerRadius, status.fAttackRadius) == false)
		{ // 敵の攻撃範囲外の場合

			// 対象の方向を向かせる
			Look(posLook, posEnemy, rotEnemy);

			// 対象の方向に移動 (前進)
			moveEnemy.x -= sinf(rotEnemy.y) * status.fForwardMove;
			moveEnemy.z -= cosf(rotEnemy.y) * status.fForwardMove;

			// 重力を加算
			moveEnemy.y -= ENE_GRAVITY;

			// 移動量を加算
			posEnemy += moveEnemy;

			// ノックバックの更新
			UpdateKnockBack(posEnemy);

			// 移動量を減衰
			moveEnemy.x += (0.0f - moveEnemy.x) * ENE_REV;
			moveEnemy.z += (0.0f - moveEnemy.z) * ENE_REV;

			// ターゲットとの当たり判定
			CollisionTarget(posEnemy);

			// 通常状態の敵との当たり判定
			CollisionNormalEnemy(posEnemy);

			// 着地判定
			CScene::GetField()->LandPosition(posEnemy, moveEnemy);

			// ステージ範囲外の補正
			collision::InCirclePillar(posEnemy, posBarrier, status.fRadius, LIMIT_RADIUS);

			if (GetMotionType() != MOTION_MOVE)
			{ // 現在のモーションが歩行じゃない場合

				// モーションを設定
				SetMotion(MOTION_MOVE);	// 歩行モーション
			}
		}
		else
		{ // 敵の攻撃範囲内の場合

			// ノックバックの更新
			UpdateKnockBack(posEnemy);

			// 対象の方向を向かせる
			Look(posLook, posEnemy, rotEnemy);

			// ターゲットとの当たり判定
			CollisionTarget(posEnemy);

			// 通常状態の敵との当たり判定
			CollisionNormalEnemy(posEnemy);

			// 着地判定
			CScene::GetField()->LandPosition(posEnemy, moveEnemy);

			// ステージ範囲外の補正
			collision::InCirclePillar(posEnemy, posBarrier, status.fRadius, LIMIT_RADIUS);

			// 攻撃
			Attack(posLook, posEnemy, fLookRadius);

			if (GetMotionType() != MOTION_ACTION)
			{ // 現在のモーションが攻撃じゃない場合

				// モーションを設定
				SetMotion(MOTION_ACTION);	// 攻撃モーション
			}
		}
	}

	// 位置を反映
	SetPosition(posEnemy);

	// 位置移動量を反映
	SetMovePosition(moveEnemy);

	// 向きを反映
	SetRotation(rotEnemy);
}

//************************************************************
//	子クラス [CEnemyCar] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEnemyCar::CEnemyCar(const TYPE type) : CEnemy(type)
{
	// メンバ変数をクリア
	m_spawn = SPAWN_WAIT;	// スポーン状態
	m_nCounterSpawn = 0;	// スポーン管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CEnemyCar::~CEnemyCar()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEnemyCar::Init(void)
{
	// メンバ変数を初期化
	m_spawn = SPAWN_WAIT;	// スポーン状態
	m_nCounterSpawn = 0;	// スポーン管理カウンター

	// 敵の初期化
	if (FAILED(CEnemy::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEnemyCar::Uninit(void)
{
	// 敵の終了
	CEnemy::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CEnemyCar::Update(void)
{
	switch (GetState())
	{ // 状態ごとの処理
	case STATE_SPAWN:

		// スポーン動作の更新
		Spawn();

		break;

	case STATE_NORMAL:

		// バブル削除
		SubBubble();

		// 敵の動作の更新
		CollisionFind();

		break;

	case STATE_DAMAGE:

		// ダメージ動作の更新
		Damage();

		// 敵の動作の更新
		CollisionFind();

		break;

	case STATE_DEATH:

		// 死亡動作の更新
		if (Death())
		{ // 死亡した場合

			// 処理を抜ける
			return;
		}

		// ランダム回転
		RandomRotation();

		break;

	case STATE_VANISH:

		// 消失動作の更新
		if (Vanish())
		{ // 消失した場合

			// 処理を抜ける
			return;
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 敵の更新
	CEnemy::Update();
}

//============================================================
//	描画処理
//============================================================
void CEnemyCar::Draw(void)
{
	// 敵の描画
	CEnemy::Draw();
}

//============================================================
//	モデルファイル取得処理
//============================================================
const char* CEnemyCar::GetModelFileName(const int nModel) const
{
	if (nModel < MODEL_MAX)
	{ // 使用できるインデックスの場合

		// 引数のインデックスのモデルを返す
		return mc_apModelFile[nModel];
	}
	else { assert(false); return NONE_STRING; }	// 範囲外
}

//============================================================
//	スポーン動作
//============================================================
void CEnemyCar::Spawn(void)
{
	// 変数を宣言
	StatusInfo  status = GetStatusInfo();		// 敵ステータス
	D3DXVECTOR3 posEnemy = GetPosition();		// 敵位置
	D3DXVECTOR3 moveEnemy = GetMovePosition();	// 敵移動量
	D3DXVECTOR3 posWarning = posEnemy;			// 警告位置
	D3DXVECTOR3 posBarrier = CScene::GetStage()->GetStageBarrierPosition();	// バリア位置

	// 過去位置の更新
	UpdateOldPosition();

	// スポーン状態の敵との当たり判定
	CollisionSpawnEnemy(posEnemy);

	// ステージ範囲外の補正
	collision::InCirclePillar(posEnemy, posBarrier, status.fRadius, LIMIT_RADIUS);

	switch (m_spawn)
	{ // スポーン状態ごとの処理
	case STATE_SPAWN:

		if (m_nCounterSpawn < SPAWN_WAIT_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_nCounterSpawn++;
		}
		else
		{ // カウンターが一定値以上の場合

			// カウンターを初期化
			m_nCounterSpawn = 0;

			// 状態を変更
			m_spawn = SPAWN_FALL;	// 落下状態
		}

		break;

	case STATE_NORMAL:
	
		// 重力を加算
		moveEnemy.y -= ENE_GRAVITY;
	
		// 移動量を加算
		posEnemy += moveEnemy;
	
		// 着地判定
		if (CScene::GetField()->LandPosition(posEnemy, moveEnemy))
		{ // 着地した場合

			// 警告の描画状況の設定
			SetDrawWarning(false);	// 描画しない
	
			// 状態の設定
			SetState(STATE_NORMAL);	// 通常状態
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 位置を反映
	SetPosition(posEnemy);

	// 位置移動量を反映
	SetMovePosition(moveEnemy);

	// 警告位置を地形に添わせる
	posWarning.y = CScene::GetField()->GetPositionHeight(posWarning);

	// 警告の位置の設定
	SetPositionWarning(D3DXVECTOR3(posWarning.x, posWarning.y + ENE_WAR_ADD_POSY, posWarning.z));
}

//============================================================
//	検知範囲の当たり判定
//============================================================
void CEnemyCar::CollisionFind(void)
{
	// 変数を宣言
	StatusInfo  status		= GetStatusInfo();		// 敵ステータス
	D3DXVECTOR3 posEnemy	= GetPosition();		// 敵位置
	D3DXVECTOR3 moveEnemy	= GetMovePosition();	// 敵移動量
	D3DXVECTOR3 rotEnemy	= GetRotation();		// 敵向き
	D3DXVECTOR3 posLook		= VEC3_ZERO;			// 視認対象位置
	D3DXVECTOR3 rotCannon	= VEC3_ZERO;			// キャノン向き
	float fLookRadius		= 0.0f;					// 視認対象半径

	D3DXVECTOR3 posBarrier	= CScene::GetStage()->GetStageBarrierPosition();	// バリア位置
	float fPlayerRadius		= CScene::GetPlayer()->GetRadius();	// プレイヤー半径

	// 過去位置の更新
	UpdateOldPosition();

	if (CScene::GetTarget()->GetState() != CTarget::STATE_DESTROY)
	{ // ターゲットが壊されていないる場合

		// 視認対象位置を設定
		posLook = CScene::GetTarget()->GetPosition();	// ターゲット位置

		// 視認対象半径を設定
		fLookRadius = CScene::GetTarget()->GetRadius();	// ターゲット半径

		// 視認対象の攻撃判定
		if (collision::Circle2D(posLook, posEnemy, fPlayerRadius, status.fAttackRadius) == false)
		{ // 敵の攻撃範囲外の場合

			// 対象の方向を向かせる
			Look(posLook, posEnemy, rotEnemy);

			// 対象の方向に移動 (前進)
			moveEnemy.x -= sinf(rotEnemy.y) * status.fForwardMove;
			moveEnemy.z -= cosf(rotEnemy.y) * status.fForwardMove;

			// 重力を加算
			moveEnemy.y -= ENE_GRAVITY;

			// 移動量を加算
			posEnemy += moveEnemy;

			// ノックバックの更新
			UpdateKnockBack(posEnemy);

			// 移動量を減衰
			moveEnemy.x += (0.0f - moveEnemy.x) * ENE_REV;
			moveEnemy.z += (0.0f - moveEnemy.z) * ENE_REV;

			// ターゲットとの当たり判定
			CollisionTarget(posEnemy);

			// 通常状態の敵との当たり判定
			CollisionNormalEnemy(posEnemy);

			// 着地判定
			CScene::GetField()->LandPosition(posEnemy, moveEnemy);

			// ステージ範囲外の補正
			collision::InCirclePillar(posEnemy, posBarrier, status.fRadius, LIMIT_RADIUS);
		}
		else
		{ // 敵の攻撃範囲内の場合

			// ノックバックの更新
			UpdateKnockBack(posEnemy);

			// ターゲットとの当たり判定
			CollisionTarget(posEnemy);

			// 通常状態の敵との当たり判定
			CollisionNormalEnemy(posEnemy);

			// 着地判定
			CScene::GetField()->LandPosition(posEnemy, moveEnemy);

			// ステージ範囲外の補正
			collision::InCirclePillar(posEnemy, posBarrier, status.fRadius, LIMIT_RADIUS);

			// キャノン向きの設定
			if (SetRotationCannon(posLook, posEnemy, rotEnemy))
			{ // 発射可能状態の場合

				// 攻撃
				Attack(posLook, posEnemy, fLookRadius);
			}
		}
	}

	// 位置を反映
	SetPosition(posEnemy);

	// 位置移動量を反映
	SetMovePosition(moveEnemy);

	// 向きを反映
	SetRotation(rotEnemy);
}

//============================================================
//	キャノン向きの設定処理
//============================================================
bool CEnemyCar::SetRotationCannon(const D3DXVECTOR3& rLookPos, const D3DXVECTOR3& rEnemyPos, const D3DXVECTOR3& rEnemyRot)
{
	// 変数を宣言
	D3DXVECTOR3 cannonRot;	// 主砲向き
	float fDestRot = 0.0f;	// 目標向き
	float fDiffRot = 0.0f;	// 向き

	// 主砲向きを取得
	cannonRot = GetMultiModel(MODEL_CANNON)->GetRotation() + rEnemyRot;	// 本体の向きを加算
	useful::NormalizeRot(cannonRot.y);	// 主砲向きの正規化

	// 目標向きの計算
	fDestRot = atan2f(rEnemyPos.x - rLookPos.x, rEnemyPos.z - rLookPos.z);
	useful::NormalizeRot(fDestRot);		// 目標向きの正規化

	// 差分向きの計算
	fDiffRot = fDestRot - cannonRot.y;
	useful::NormalizeRot(fDiffRot);		// 差分向きの正規化

	// 向きの更新
	cannonRot.y += fDiffRot * ENECAR_REV_MUL;
	useful::NormalizeRot(cannonRot.y);	// キャノン向きの正規化

	// 向きを設定
	GetMultiModel(MODEL_CANNON)->SetRotation(cannonRot - rEnemyRot);	// 本体の向きを減算

	if (fDestRot + D3DX_PI <= cannonRot.y + D3DX_PI + ENECAR_SHOTRANGE
	&&  fDestRot + D3DX_PI >= cannonRot.y + D3DX_PI - ENECAR_SHOTRANGE)
	{ // 主砲向きと目標向きの誤差が少ない場合

		// 発射可能状態を返す
		return true;
	}
	else
	{ // 主砲向きと目標向きの誤差が多い場合

		// 発射不可能状態を返す
		return false;
	}
}

//************************************************************
//	子クラス [CEnemy] のセットアップ関数
//************************************************************
//============================================================
//	セットアップ処理
//============================================================
void CEnemy::LoadSetup(void)
{
	// 変数を宣言
	CMotion::MotionInfo info;		// ポーズの代入用
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nType		= 0;			// 種類の代入用
	int nID			= 0;			// インデックスの代入用
	int nNowPose	= 0;			// 現在のポーズ番号
	int nNowKey		= 0;			// 現在のキー番号
	int nLoop		= 0;			// ループのON/OFFの変換用
	int nBackward	= 0;			// 後退のON/OFFの変換用
	int nEnd		= 0;			// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 静的メンバ変数の情報をクリア
	memset(&m_aStatusInfo[0], 0, sizeof(m_aStatusInfo));	// ステータス情報
	memset(&m_aPartsInfo[0], 0, sizeof(m_aPartsInfo));		// パーツ情報
	memset(&m_aMotionInfo[0], 0, sizeof(m_aMotionInfo));	// モーション情報

	// ファイルを読み込み形式で開く
	pFile = fopen(ENEMY_SETUP_TXT, "r");

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

					if (strcmp(&aString[0], "ENEMYSET") == 0)
					{ // 読み込んだ文字列が ENEMYSET の場合

						do
						{ // 読み込んだ文字列が END_ENEMYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "TYPE") == 0)
							{ // 読み込んだ文字列が TYPE の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nType);		// 種類を読み込む
							}
							else if (strcmp(&aString[0], "LIFE") == 0)
							{ // 読み込んだ文字列が LIFE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nLife);	// 体力を読み込む
							}
							else if (strcmp(&aString[0], "SCORE") == 0)
							{ // 読み込んだ文字列が SCORE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nScore);	// スコア加算量を読み込む
							}
							else if (strcmp(&aString[0], "EXP") == 0)
							{ // 読み込んだ文字列が EXP の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nExp);	// 経験値生成量を読み込む
							}
							else if (strcmp(&aString[0], "COLL_RADIUS") == 0)
							{ // 読み込んだ文字列が COLL_RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fCollRadius);	// 当たり判定の半径を読み込む
							}
							else if (strcmp(&aString[0], "RADIUS") == 0)
							{ // 読み込んだ文字列が RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fRadius);	// 半径を読み込む
							}
							else if (strcmp(&aString[0], "HEIGHT") == 0)
							{ // 読み込んだ文字列が HEIGHT の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fHeight);	// 縦幅を読み込む
							}
							else if (strcmp(&aString[0], "KNOCKBACK_UP") == 0)
							{ // 読み込んだ文字列が KNOCKBACK_UP の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fKnockBackUp);	// ノックバック上移動量を読み込む
							}
							else if (strcmp(&aString[0], "KNOCKBACK_SIDE") == 0)
							{ // 読み込んだ文字列が KNOCKBACK_SIDE の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fKnockBackSide);	// ノックバック横移動量を読み込む
							}
							else if (strcmp(&aString[0], "KNOCKBACK_REV") == 0)
							{ // 読み込んだ文字列が KNOCKBACK_REV の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fKnockBackRevision);	// ノックバック減衰係数を読み込む
							}
							else if (strcmp(&aString[0], "SPAWN_HEIGHT") == 0)
							{ // 読み込んだ文字列が SPAWN_HEIGHT の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fSpawnHeight);	// 生成するY座標を読み込む
							}
							else if (strcmp(&aString[0], "BULLET_LIFE") == 0)
							{ // 読み込んだ文字列が BULLET_LIFE の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nBullLife);	// 弾の寿命を読み込む
							}
							else if (strcmp(&aString[0], "BULLET_DAMAGE") == 0)
							{ // 読み込んだ文字列が BULLET_DAMAGE の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nBullDamage);	// 弾の攻撃力を読み込む
							}
							else if (strcmp(&aString[0], "BULLET_MOVE") == 0)
							{ // 読み込んだ文字列が BULLET_MOVE の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fBullMove);	// 弾の移動量を読み込む
							}
							else if (strcmp(&aString[0], "BULLET_RADIUS") == 0)
							{ // 読み込んだ文字列が BULLET_RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fBullRadius);	// 弾の半径を読み込む
							}
							else if (strcmp(&aString[0], "BULLET_PARTS") == 0)
							{ // 読み込んだ文字列が BULLET_PARTS の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nBullParts);	// 弾の発射パーツを読み込む
							}
							else if (strcmp(&aString[0], "BULLET_POS") == 0)
							{ // 読み込んだ文字列が BULLET_POS の場合

								fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bullPos.x);	// 弾の発射位置Xを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bullPos.y);	// 弾の発射位置Yを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bullPos.z);	// 弾の発射位置Zを読み込む
							}
							else if (strcmp(&aString[0], "BUBBLE_SIZE") == 0)
							{ // 読み込んだ文字列が BUBBLE_SIZE の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bubbleSize.x);	// バブルの大きさXを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bubbleSize.y);	// バブルの大きさYを読み込む
								fscanf(pFile, "%f", &m_aStatusInfo[nType].bubbleSize.z);	// バブルの大きさZを読み込む
							}
							else if (strcmp(&aString[0], "SHADOW_RADIUS") == 0)
							{ // 読み込んだ文字列が SHADOW_RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fShadowRadius);	// 影の半径を読み込む
							}
							else if (strcmp(&aString[0], "ATTACK_CNT") == 0)
							{ // 読み込んだ文字列が ATTACK_CNT の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%d", &m_aStatusInfo[nType].nCounterAttack);	// 攻撃頻度を読み込む
							}
							else if (strcmp(&aString[0], "FORWARD_MOVE") == 0)
							{ // 読み込んだ文字列が FORWARD_MOVE の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fForwardMove);	// 前進の移動量を読み込む
							}
							else if (strcmp(&aString[0], "LOOK_REV") == 0)
							{ // 読み込んだ文字列が LOOK_REV の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fLookRevision);	// 振り向き補正係数を読み込む
							}
							else if (strcmp(&aString[0], "ATTACK_RADIUS") == 0)
							{ // 読み込んだ文字列が ATTACK_RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_aStatusInfo[nType].fAttackRadius);	// 攻撃範囲を読み込む
							}
						} while (strcmp(&aString[0], "END_ENEMYSET") != 0);	// 読み込んだ文字列が END_ENEMYSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);		// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}

			// キャラクターの設定
			else if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{ // 読み込んだ文字列が CHARACTERSET の場合

				do
				{ // 読み込んだ文字列が END_CHARACTERSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "ENEMYSET") == 0)
					{ // 読み込んだ文字列が ENEMYSET の場合

						do
						{ // 読み込んだ文字列が END_ENEMYSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "TYPE") == 0)
							{ // 読み込んだ文字列が TYPE の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nType);		// 種類を読み込む
							}
							else if (strcmp(&aString[0], "PARTSSET") == 0)
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
										m_aPartsInfo[nType].nNumParts++;
									}
									else if (strcmp(&aString[0], "PARENT") == 0)
									{ // 読み込んだ文字列が PARENT の場合

										fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
										fscanf(pFile, "%d", &m_aPartsInfo[nType].aInfo[nID].nParentID);	// モデルの親のインデックスを読み込む
									}
									else if (strcmp(&aString[0], "POS") == 0)
									{ // 読み込んだ文字列が POS の場合

										fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].pos.x);		// X座標を読み込む
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].pos.y);		// Y座標を読み込む
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].pos.z);		// Z座標を読み込む
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // 読み込んだ文字列が ROT の場合

										fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].rot.x);		// X向きを読み込む
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].rot.y);		// Y向きを読み込む
										fscanf(pFile, "%f", &m_aPartsInfo[nType].aInfo[nID].rot.z);		// Z向きを読み込む
									}
								} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// 読み込んだ文字列が END_PARTSSET ではない場合ループ
							}
						} while (strcmp(&aString[0], "END_ENEMYSET") != 0);	// 読み込んだ文字列が END_ENEMYSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// 読み込んだ文字列が END_CHARACTERSET ではない場合ループ
			}

			// モーションの設定
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // 読み込んだ文字列が MOTIONSET の場合

				// モーション数を初期化
				m_aMotionInfo[nType].nNumMotion = 0;

				do
				{ // 読み込んだ文字列が END_MOTIONSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "TYPE") == 0)
					{ // 読み込んだ文字列が TYPE の場合

						fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
						fscanf(pFile, "%d", &nType);		// 種類を読み込む
					}
					else if (strcmp(&aString[0], "MOTION") == 0)
					{ // 読み込んだ文字列が MOTION の場合

						// 現在のポーズ番号を初期化
						nNowPose = 0;

						do
						{ // 読み込んだ文字列が END_MOTION ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "LOOP") == 0)
							{ // 読み込んだ文字列が LOOP の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nLoop);		// ループのON/OFFを読み込む

								// 読み込んだ値をbool型に変換
								info.bLoop = (nLoop == 0) ? false : true;
							}
							else if (strcmp(&aString[0], "NUM_KEY") == 0)
							{ // 読み込んだ文字列が NUM_KEY の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &info.nNumKey);	// キーの総数を読み込む
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

										fscanf(pFile, "%s", &aString[0]);						// = を読み込む (不要)
										fscanf(pFile, "%d", &info.aKeyInfo[nNowPose].nFrame);	// キーが切り替わるまでのフレーム数を読み込む
									}
									else if (strcmp(&aString[0], "KEY") == 0)
									{ // 読み込んだ文字列が KEY の場合

										do
										{ // 読み込んだ文字列が END_KEY ではない場合ループ

											// ファイルから文字列を読み込む
											fscanf(pFile, "%s", &aString[0]);

											if (strcmp(&aString[0], "POS") == 0)
											{ // 読み込んだ文字列が POS の場合

												fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.x);	// X位置を読み込む
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.y);	// Y位置を読み込む
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].pos.z);	// Z位置を読み込む

												// 読み込んだ位置にパーツの初期位置を加算
												info.aKeyInfo[nNowPose].aKey[nNowKey].pos += m_aPartsInfo[nType].aInfo[nNowKey].pos;
											}
											else if (strcmp(&aString[0], "ROT") == 0)
											{ // 読み込んだ文字列が ROT の場合

												fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);	// X向きを読み込む
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);	// Y向きを読み込む
												fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);	// Z向きを読み込む

												// 読み込んだ向きにパーツの初期向きを加算
												info.aKeyInfo[nNowPose].aKey[nNowKey].rot += m_aPartsInfo[nType].aInfo[nNowKey].rot;

												// 初期向きを正規化
												useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);
												useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);
												useful::NormalizeRot(info.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);
											}

										} while (strcmp(&aString[0], "END_KEY") != 0);	// 読み込んだ文字列が END_KEY ではない場合ループ

										// 現在のキー番号を加算
										nNowKey++;
									}
								} while (strcmp(&aString[0], "END_KEYSET") != 0);	// 読み込んだ文字列が END_KEYSET ではない場合ループ

								// 現在のポーズ番号を加算
								nNowPose++;
							}
						} while (strcmp(&aString[0], "END_MOTION") != 0);	// 読み込んだ文字列が END_MOTION ではない場合ループ

						// モーション情報の設定
						m_aMotionInfo[nType].aMotionInfo[m_aMotionInfo[nType].nNumMotion] = info;

						// モーション数を加算
						m_aMotionInfo[nType].nNumMotion++;
					}
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);	// 読み込んだ文字列が END_MOTIONSET ではない場合ループ
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
