//============================================================
//
//	魔法処理 [magic.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "magic.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "sound.h"
#include "texture.h"
#include "collision.h"
#include "effect3D.h"
#include "target.h"
#include "enemy.h"
#include "field.h"
#include "stage.h"
#include "bubble.h"
#include "shadow.h"
#include "score.h"

//************************************************************
//	マクロ定義
//************************************************************
#define MAGIC_SETUP_TXT	"data\\TXT\\magic.txt"	// セットアップテキスト相対パス

#define SHADOW_SIZE		(D3DXVECTOR3(10.0f, 0.0f, 10.0f))	// 影の大きさ
#define SHADOW_ALPHA	(0.2f)	// 影のα値

#define BUBBLE_POSY_UP	(20.0f)		// バブルの縦位置上昇量
#define BUBBLE_INIT_LEVEL	(10)	// レベルの初期値

#define MAGIC_PRIO	(3)	// 魔法の優先順位

#define MAGIC_NORMAL_DMG	(1)	// 通常ショット時の攻撃力
#define MAGIC_LONG_DMG		(2)	// ロングショット時の攻撃力
#define MAGIC_TRICK_DMG		(3)	// トリックショット時の攻撃力

#define BONUS_LONG_SCORE	(4000)	// ロングショット時のスコア加算量
#define BONUS_TRICK_SCORE	(9000)	// トリックショット時のスコア加算量

#define MOVE_NORMAL			(0.05f)	// 通常時の魔法の移動量
#define MOVE_INHALE_INSIDE	(8.0f)	// 吸い込まれ時の魔法の内側への移動量
#define MOVE_INHALE_LEFT	(14.5f)	// 吸い込まれ時の魔法の左側への移動量
#define MOVE_DELETE			(3.0f)	// 消失時の魔法の移動量

#define MAGIC_EFF_CNT			(2)		// エフェクトの生成フレーム
#define MAGIC_EFF_MUL_POS		(2.0f)	// エフェクトの生成位置の乗算量
#define MAGIC_EFF_MUL_MOVE		(1.0f)	// エフェクトの移動量の乗算量
#define MAGIC_EFF_ALPHA			(0.6f)	// エフェクトの透明度
#define MAGIC_EFF_LIFE			(45)	// エフェクトの寿命
#define MAGIC_EFF_MUL_RADIUS	(1.5f)	// エフェクトの半径の乗算量
#define MAGIC_EFF_SUB_RADIUS	(0.2f)	// エフェクトの半径の減算量
#define MAGIC_EFF_PRIORITY		(3)		// エフェクトの優先順位

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CMagic::StatusInfo CMagic::m_statusInfo = {};	// ステータス情報
CMagic::LevelInfo CMagic::m_level = {};			// レベル情報

//************************************************************
//	子クラス [CMagic] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMagic::CMagic() : CObject(CObject::LABEL_MAGIC)
{
	// メンバ変数をクリア
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	m_pBubble	= NULL;			// バブル情報
	m_pShadow	= NULL;			// 影情報
	m_pos		= VEC3_ZERO;	// 現在位置
	m_movePos	= VEC3_ZERO;	// 位置移動量
	m_rot		= VEC3_ZERO;	// 向き
	m_moveRot	= VEC3_ZERO;	// 向き変更量
	m_state		= STATE_NORMAL;	// 状態
	m_nCounterEffect = 0;		// エフェクト管理カウンター
}

//============================================================
//	デストラクタ
//============================================================
CMagic::~CMagic()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMagic::Init(void)
{
	// メンバ変数を初期化
	memset(&m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	m_pBubble	= NULL;			// バブル情報
	m_pShadow	= NULL;			// 影情報
	m_pos		= VEC3_ZERO;	// 現在位置
	m_movePos	= VEC3_ZERO;	// 位置移動量
	m_rot		= VEC3_ZERO;	// 向き
	m_moveRot	= VEC3_ZERO;	// 向き変更量
	m_state		= STATE_NORMAL;	// 状態
	m_nCounterEffect = 0;		// エフェクト管理カウンター

	// バブル情報の生成
	m_pBubble = CBubble::Create(this, m_statusInfo.nLife + BUBBLE_INIT_LEVEL, VEC3_ALL(m_statusInfo.fBubbleRadius), VEC3_ZERO, 0.0f);
	if (UNUSED(m_pBubble))
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pBubble->SetPriority(MAGIC_PRIO);

	// 初期レベルを設定
	m_pBubble->SetLevel(BUBBLE_INIT_LEVEL);

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, SHADOW_SIZE, this, SHADOW_ALPHA, SHADOW_ALPHA);
	if (UNUSED(m_pShadow))
	{ // 非使用中の場合

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
void CMagic::Uninit(void)
{
	// バブルの終了
	m_pBubble->Uninit();

	// 影の終了
	m_pShadow->Uninit();

	// 魔法を破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CMagic::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 vecTarg, vecSide;	// 吸い込み方向ベクトル

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:	// 通常状態

		// 移動量を加算
		m_pos += m_movePos;

		// バブルレベルを加算
		m_pBubble->AddLevel(1);

		if (CScene::GetStage()->CollisionBarrier(m_pos, m_pBubble->GetRadius())
		||  m_pBubble->GetLevel() >= m_statusInfo.nLife)
		{ // 消失範囲に当たっているまたは寿命が来た場合

			// 状態を設定
			m_state = STATE_DELETE;	// 消失状態
		}

		break;

	case STATE_DELETE:	// 消失状態
		
		// ターゲット方向のベクトルを計算
		vecTarg = CScene::GetTarget()->GetPosition() - m_pos;
		vecTarg.y = 0.0f;						// ベクトルの縦方向を無視
		D3DXVec3Normalize(&vecTarg, &vecTarg);	// ベクトル正規化

		// ターゲット横方向ベクトルを計算
		vecSide = D3DXVECTOR3(vecTarg.z, 0.0f, -vecTarg.x);

		if (CScene::GetStage()->CollisionBarrier(m_pos, m_pBubble->GetRadius()))
		{ // バリアの判定内の場合

			// 移動量を設定
			m_movePos = vecTarg * MOVE_DELETE;
			m_movePos += vecSide * MOVE_DELETE;
		}
		else
		{ // バリアの判定外の場合

			// 移動量を設定
			m_movePos = vecTarg * MOVE_INHALE_INSIDE;
			m_movePos += vecSide * MOVE_INHALE_LEFT;
		}

		// 移動量を加算
		m_pos += m_movePos;

		// バブルレベルを減算
		m_pBubble->AddLevel(-1);

		if (m_pBubble->GetLevel() <= 0)
		{ // レベルが下がり切った場合

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	if (CScene::GetField()->IsPositionRange(m_pos))
	{ // 地面の範囲内の場合

		// 縦座標を地形に添わせる
		m_pos.y = CScene::GetField()->GetPositionHeight(m_pos);
		m_pos.y += m_pBubble->GetRadius() + BUBBLE_POSY_UP;
	}

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
			m_pos + (vecRandom * MAGIC_EFF_MUL_POS),		// 位置
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

	// バブルの更新
	m_pBubble->Update();

	// 影の大きさを設定
	m_pShadow->SetScalingOrigin(SHADOW_SIZE + (D3DXVECTOR3(1.0f, 0.0f, 1.0f) * m_pBubble->GetRadius()));

	// 影の更新
	m_pShadow->Update();

	if (CollisionEnemy())
	{ // 敵に当たっていた場合

		// オブジェクトの終了
		Uninit();

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_STICK);	// 泡着弾音

		// 関数を抜ける
		return;
	}
}

//============================================================
//	描画処理
//============================================================
void CMagic::Draw(void)
{
	// 変数を宣言
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//============================================================
//	生成処理
//============================================================
CMagic *CMagic::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const D3DXVECTOR3& rVec		// 移動方向
)
{
	// ポインタを宣言
	CMagic *pMagic = NULL;		// 魔法生成用

	if (UNUSED(pMagic))
	{ // 使用されていない場合

		// メモリ確保
		pMagic = new CMagic;	// 魔法
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pMagic))
	{ // 使用されている場合
		
		// 魔法の初期化
		if (FAILED(pMagic->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pMagic;
			pMagic = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pMagic->SetPosition(rPos);

		// 向きを設定
		pMagic->SetRotation(rRot);

		// 移動量を設定
		pMagic->SetMove(rVec, pMagic->GetStatusInfo().aLevel[pMagic->m_level.nMove].fMove);

		// 確保したアドレスを返す
		return pMagic;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ステータス情報取得処理
//============================================================
CMagic::StatusInfo CMagic::GetStatusInfo(void)
{
	// ステータス情報を返す
	return m_statusInfo;
}

//============================================================
//	ステータスレベルの加算処理
//============================================================
void CMagic::AddLevelStatus(const LEVELINFO level)
{
	// 引数のレベルを加算
	switch (level)
	{ // レベル情報ごとの処理
	case LEVELINFO_NUM:		// 発射数

		// 発射数のレベルを加算
		m_level.nNumBunnle++;

		// 発射数オーバー
		assert(m_level.nNumBunnle < LEVEL_MAX);

		break;

	case LEVELINFO_SPEED:	// 弾速

		// 弾速のレベルを加算
		m_level.nMove++;

		// 弾速オーバー
		assert(m_level.nMove < LEVEL_MAX);

		break;

	case LEVELINFO_RAPID:	// 連射速度

		// 連射速度のレベルを加算
		m_level.nCoolTime++;

		// 連射速度オーバー
		assert(m_level.nCoolTime < LEVEL_MAX);

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	ステータスレベル取得処理
//============================================================
int CMagic::GetLevelStatus(const LEVELINFO level)
{
	// 引数のレベルを返す
	switch (level)
	{ // レベル情報ごとの処理
	case LEVELINFO_NUM:		// 発射数

		// 発射数のレベルを返す
		return m_level.nNumBunnle;

	case LEVELINFO_SPEED:	// 弾速

		// 弾速のレベルを返す
		return m_level.nMove;

	case LEVELINFO_RAPID:	// 連射速度

		// 連射速度のレベルを返す
		return m_level.nCoolTime;

	default:	// 例外処理
		assert(false);
		return NONE_IDX;
	}
}

//============================================================
//	移動量の設定処理
//============================================================
void CMagic::SetMove(D3DXVECTOR3 vec, const float fMove)
{
	// ベクトルの正規化
	D3DXVec3Normalize(&vec, &vec);

	// 移動量を設定
	m_movePos = vec * fMove;
}

//============================================================
//	位置の設定処理
//============================================================
void CMagic::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;
}

//============================================================
//	向きの設定処理
//============================================================
void CMagic::SetRotation(const D3DXVECTOR3& rRot)
{
	// 引数の向きを設定
	m_rot = rRot;

	// 向きの正規化
	useful::NormalizeRot(m_rot.x);
	useful::NormalizeRot(m_rot.y);
	useful::NormalizeRot(m_rot.z);
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CMagic::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CMagic::GetRotation(void) const
{
	// 向きを返す
	return m_rot;
}

//============================================================
//	敵との当たり判定
//============================================================
bool CMagic::CollisionEnemy(void)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_ENEMY)
				{ // オブジェクトラベルが敵ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() == CEnemy::STATE_SPAWN
				||  pObjCheck->GetState() == CEnemy::STATE_DEATH)
				{ // 敵の状態がスポーンまたは死亡状態の場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 魔法判定
				if (collision::Circle3D
				( // 引数
					m_pos,						// 判定位置
					pObjCheck->GetPosition(),	// 判定目標位置
					m_pBubble->GetRadius(),		// 判定半径
					pObjCheck->GetRadius()		// 判定目標半径
				))
				{ // 魔法に当たっていた場合

					if (m_state == STATE_NORMAL)
					{ // 状態が通常の場合

						if (m_pBubble->GetLevel() / (m_pBubble->GetMaxLevel() / 2) < 1)
						{ // バブルサイズが半分以上成長していない場合

							// 敵のヒット処理
							pObjCheck->HitKnockBack(MAGIC_NORMAL_DMG, m_movePos);
						}
						else
						{ // バブルサイズが半分以上成長している場合

							if (CManager::GetScene()->GetMode() == CScene::MODE_GAME)
							{ // モードがゲームの場合

								// スコアを加算
								CSceneGame::GetScore()->Add(BONUS_LONG_SCORE);
							}

							// 敵のヒット処理
							pObjCheck->HitKnockBack(MAGIC_LONG_DMG, m_movePos);
						}
					}
					else
					{ // 状態が通常ではない場合

						if (CManager::GetScene()->GetMode() == CScene::MODE_GAME)
						{ // モードがゲームの場合

							// スコアを加算
							CSceneGame::GetScore()->Add(BONUS_TRICK_SCORE);
						}

						// 敵のヒット処理
						pObjCheck->HitKnockBack(MAGIC_TRICK_DMG, m_movePos);
					}

					// 当たった判定を返す
					return true;
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 当たっていない判定を返す
	return false;
}

//============================================================
//	セットアップ処理
//============================================================
void CMagic::LoadSetup(void)
{
	// 変数を宣言
	int nLevel = 0;	// レベルの代入用
	int nEnd = 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 静的メンバ変数の情報をクリア
	memset(&m_statusInfo, 0, sizeof(m_statusInfo));	// ステータス情報
	memset(&m_level, 0, sizeof(m_level));			// レベル情報

	// ファイルを読み込み形式で開く
	pFile = fopen(MAGIC_SETUP_TXT, "r");

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

					// 基本情報
					if (strcmp(&aString[0], "MAGICSET") == 0)
					{ // 読み込んだ文字列が MAGICSET の場合

						do
						{ // 読み込んだ文字列が END_MAGICSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "LIFE") == 0)
							{ // 読み込んだ文字列が LIFE の場合

								fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
								fscanf(pFile, "%d", &m_statusInfo.nLife);	// 寿命を読み込む
							}
							else if (strcmp(&aString[0], "SHOT_PARTS") == 0)
							{ // 読み込んだ文字列が SHOT_PARTS の場合

								fscanf(pFile, "%s", &aString[0]);				// = を読み込む (不要)
								fscanf(pFile, "%d", &m_statusInfo.nShotParts);	// 発射パーツを読み込む
							}
							else if (strcmp(&aString[0], "SHOT_POS") == 0)
							{ // 読み込んだ文字列が SHOT_POS の場合

								fscanf(pFile, "%s", &aString[0]);				// = を読み込む (不要)
								fscanf(pFile, "%f", &m_statusInfo.shotPos.x);	// 発射位置Xを読み込む
								fscanf(pFile, "%f", &m_statusInfo.shotPos.y);	// 発射位置Yを読み込む
								fscanf(pFile, "%f", &m_statusInfo.shotPos.z);	// 発射位置Zを読み込む
							}
							else if (strcmp(&aString[0], "BUBBLE_RADIUS") == 0)
							{ // 読み込んだ文字列が BUBBLE_RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_statusInfo.fBubbleRadius);	// バブル半径を読み込む
							}
							else if (strcmp(&aString[0], "SHOT_ANGLE") == 0)
							{ // 読み込んだ文字列が SHOT_ANGLE の場合

								fscanf(pFile, "%s", &aString[0]);					// = を読み込む (不要)
								fscanf(pFile, "%f", &m_statusInfo.fShotAngle);		// 射撃角度を読み込む

								// 読み込んだ角度を変換
								m_statusInfo.fShotAngle = D3DXToRadian(m_statusInfo.fShotAngle);
							}
						} while (strcmp(&aString[0], "END_MAGICSET") != 0);	// 読み込んだ文字列が END_MAGICSET ではない場合ループ
					}

					// レベル情報
					else if (strcmp(&aString[0], "LEVELSET") == 0)
					{ // 読み込んだ文字列が LEVELSET の場合

						do
						{ // 読み込んだ文字列が END_LEVELSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "LEVEL") == 0)
							{ // 読み込んだ文字列が LEVEL の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nLevel);		// レベルを読み込む
							}
							else if (strcmp(&aString[0], "NUMBUBBLE") == 0)
							{ // 読み込んだ文字列が NUMBUBBLE の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &m_statusInfo.aLevel[nLevel].nNumBunnle);	// バブル数を読み込む
							}
							else if (strcmp(&aString[0], "MOVE") == 0)
							{ // 読み込んだ文字列が MOVE の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_statusInfo.aLevel[nLevel].fMove);		// 移動量を読み込む
							}
							else if (strcmp(&aString[0], "COOLTIME") == 0)
							{ // 読み込んだ文字列が COOLTIME の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%d", &m_statusInfo.aLevel[nLevel].nCoolTime);	// クールタイムを読み込む
							}
						} while (strcmp(&aString[0], "END_LEVELSET") != 0);	// 読み込んだ文字列が END_LEVELSET ではない場合ループ
					}
				} while (strcmp(&aString[0], "END_STATUSSET") != 0);		// 読み込んだ文字列が END_STATUSSET ではない場合ループ
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "魔法セットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
