//============================================================
//
//	友達処理 [friend.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "friend.h"
#include "manager.h"
#include "sceneGame.h"
#include "gameManager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "collision.h"
#include "fade.h"

#include "multiModel.h"
#include "objectBillboard.h"
#include "shadow.h"
#include "stage.h"
#include "player.h"
#include "field.h"
#include "building.h"
#include "obstacle.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* SETUP_TXT = "data\\TXT\\friend.txt";	// 友達セットアップテキスト

	const int PRIORITY = 3;		// 友達の優先順位

	// 友達基本情報
	namespace basic
	{
		const float	GRAVITY		= 1.0f;		// 重力
		const float	RADIUS		= 20.0f;	// 半径
		const float	HEIGHT		= 100.0f;	// 縦幅
		const float	REV_ROTA	= 0.03f;	// 向き変更の補正係数
		const float	MIN_VARY	= 0.2f;		// 向きと目標向きの違う量の許容値

		const float	NOR_JUMP_REV	= 0.16f;	// 通常状態時の空中の移動量の減衰係数
		const float	NOR_LAND_REV	= 0.16f;	// 通常状態時の地上の移動量の減衰係数

		
	}

	// 友達他クラス情報
	namespace other
	{
		const D3DXVECTOR3 EMOTION_PLUSPOS = D3DXVECTOR3(0.0f, 160.0f, 0.0f);	// 感情吹き出しの位置加算量
		const D3DXVECTOR3 EMOTION_SIZE = D3DXVECTOR3(120.0f, 120.0f, 0.0f);		// 感情吹き出しの大きさ
		const D3DXVECTOR3 SHADOW_SIZE = D3DXVECTOR3(80.0f, 0.0f, 80.0f);		// 影の大きさ
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CFriend::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\emotion000.png",	// 嬉しい感情テクスチャ
};
const char *CFriend::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\FRIEND\\00_waist.x",	// 腰
	"data\\MODEL\\FRIEND\\01_body.x",	// 体
	"data\\MODEL\\FRIEND\\02_head.x",	// 頭
	"data\\MODEL\\FRIEND\\03_armUL.x",	// 左上腕
	"data\\MODEL\\FRIEND\\04_armUR.x",	// 右上腕
	"data\\MODEL\\FRIEND\\05_armDL.x",	// 左下腕
	"data\\MODEL\\FRIEND\\06_armDR.x",	// 右下腕
	"data\\MODEL\\FRIEND\\07_handL.x",	// 左手
	"data\\MODEL\\FRIEND\\08_handR.x",	// 右手
	"data\\MODEL\\FRIEND\\09_legUL.x",	// 左太もも
	"data\\MODEL\\FRIEND\\10_legUR.x",	// 右太もも
	"data\\MODEL\\FRIEND\\11_legDL.x",	// 左脛
	"data\\MODEL\\FRIEND\\12_legDR.x",	// 右脛
	"data\\MODEL\\FRIEND\\13_footL.x",	// 左足
	"data\\MODEL\\FRIEND\\14_footR.x",	// 右足
};

//************************************************************
//	子クラス [CFriend] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CFriend::CFriend() : CObjectChara(CObject::LABEL_FRIEND, PRIORITY)
{
	// メンバ変数をクリア
	m_pEmotion		= NULL;			// 感情吹き出しの情報
	m_pShadow		= NULL;			// 影の情報
	m_oldPos		= VEC3_ZERO;	// 過去位置
	m_move			= VEC3_ZERO;	// 移動量
	m_destRot		= VEC3_ZERO;	// 目標向き
	m_state			= STATE_NONE;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター
	m_bJump			= false;		// ジャンプ状況
}

//============================================================
//	デストラクタ
//============================================================
CFriend::~CFriend()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CFriend::Init(void)
{
	// メンバ変数を初期化
	m_pEmotion		= NULL;			// 感情吹き出しの情報
	m_pShadow		= NULL;			// 影の情報
	m_oldPos		= VEC3_ZERO;	// 過去位置
	m_move			= VEC3_ZERO;	// 移動量
	m_destRot		= VEC3_ZERO;	// 目標向き
	m_state			= STATE_NORMAL;	// 状態
	m_nCounterState	= 0;			// 状態管理カウンター
	m_bJump			= false;		// ジャンプ状況

	// オブジェクトキャラクターの初期化
	if (FAILED(CObjectChara::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// セットアップの読み込み
	LoadSetup();

	// モデル情報の設定
	SetModelInfo();

	// 感情吹き出しの生成
	m_pEmotion = CObjectBillboard::Create(VEC3_ZERO, other::EMOTION_SIZE);
	if (m_pEmotion == NULL)
	{ // 非使用中の場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを読込・割当
	m_pEmotion->BindTexture(mc_apTextureFile[TEXTURE_HAPPY]);

	// 自動描画をOFFにする
	m_pEmotion->SetEnableDraw(false);

	// 影の生成
	m_pShadow = CShadow::Create(CShadow::TEXTURE_NORMAL, other::SHADOW_SIZE, this);
	if (m_pShadow == NULL)
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
void CFriend::Uninit(void)
{
	// 感情吹き出しの終了
	m_pEmotion->Uninit();

	// 影の終了
	m_pShadow->Uninit();

	// オブジェクトキャラクターの終了
	CObjectChara::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CFriend::Update(void)
{
	// 変数を宣言
	EMotion currentMotion = MOTION_IDOL;	// 現在のモーション

	// 過去位置の更新
	UpdateOldPosition();

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:
		break;

	case STATE_NORMAL:

		// 通常状態の更新
		currentMotion = UpdateNormal();

		break;

	case STATE_UNION:

		// 合流状態時の更新
		currentMotion = UpdateUnion();

		break;

	default:
		assert(false);
		break;
	}

	// 感情吹き出しの更新
	m_pEmotion->Update();

	// 影の更新
	m_pShadow->Update();

	// モーション・オブジェクトキャラクターの更新
	UpdateMotion(currentMotion);
}

//============================================================
//	描画処理
//============================================================
void CFriend::Draw(void)
{
	// オブジェクトキャラクターの描画
	CObjectChara::Draw();
}

//============================================================
//	状態の設定処理
//============================================================
void CFriend::SetState(const int nState)
{
	// 引数の状態を設定
	m_state = (EState)nState;
}

//============================================================
//	状態取得処理
//============================================================
int CFriend::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	半径取得処理
//============================================================
float CFriend::GetRadius(void) const
{
	// 半径を返す
	return basic::RADIUS;
}

//============================================================
//	縦幅取得処理
//============================================================
float CFriend::GetHeight(void) const
{
	// 縦幅を返す
	return basic::HEIGHT;
}

//============================================================
//	位置の設定処理
//============================================================
void CFriend::SetVec3Position(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	CObjectChara::SetVec3Position(rPos);	// 自身
	m_pEmotion->SetVec3Position(rPos + other::EMOTION_PLUSPOS);	// 感情吹き出し
}

//============================================================
//	更新状況の設定処理
//============================================================
void CFriend::SetEnableUpdate(const bool bUpdate)
{
	// 引数の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);		// 自身
	m_pShadow->SetEnableUpdate(bUpdate);	// 影
}

//============================================================
//	描画状況の設定処理
//============================================================
void CFriend::SetEnableDraw(const bool bDraw)
{
	// 引数の描画状況を設定
	CObject::SetEnableDraw(bDraw);		// 自身
	m_pShadow->SetEnableDraw(bDraw);	// 影
}

//============================================================
//	マトリックス取得処理
//============================================================
D3DXMATRIX CFriend::GetMtxWorld(void) const
{
	// 変数を宣言
	D3DXMATRIX  mtxRot, mtxTrans, mtxWorld;		// 計算用マトリックス
	D3DXVECTOR3 posFriend = GetVec3Position();	// 友達位置
	D3DXVECTOR3 rotFriend = GetVec3Rotation();	// 友達向き

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rotFriend.y, rotFriend.x, rotFriend.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, posFriend.x, posFriend.y, posFriend.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスを返す
	return mtxWorld;
}

//============================================================
//	生成処理
//============================================================
CFriend *CFriend::Create(void)
{
	// ポインタを宣言
	CFriend *pFriend = NULL;	// 友達生成用

	if (pFriend == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pFriend = new CFriend;	// 友達
	}
	else { assert(false); return NULL; }	// 使用中

	if (pFriend != NULL)
	{ // 使用されている場合

		// 友達の初期化
		if (FAILED(pFriend->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pFriend;
			pFriend = NULL;

			// 失敗を返す
			return NULL;
		}
	}

	// 確保したアドレスを返す
	return pFriend;
}

//============================================================
//	通常状態時の更新処理
//============================================================
CFriend::EMotion CFriend::UpdateNormal(void)
{
	// 変数を宣言
	D3DXVECTOR3 posFriend = GetVec3Position();	// 友達位置
	D3DXVECTOR3 rotFriend = GetVec3Rotation();	// 友達向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報
	if (pStage == NULL)
	{ // ステージが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_IDOL;
	}

	// 重力の更新
	UpdateGravity();

	// 着地判定
	UpdateLanding(posFriend);

	// 向き更新
	UpdateRotation(rotFriend);

	// ステージ範囲外の補正
	pStage->LimitPosition(posFriend, basic::RADIUS);

	// 位置を反映
	SetVec3Position(posFriend);

	// 向きを反映
	SetVec3Rotation(rotFriend);

	// 待機モーションを返す
	return MOTION_IDOL;
}

//============================================================
//	合流状態時の更新処理
//============================================================
CFriend::EMotion CFriend::UpdateUnion(void)
{
	// ポインタを宣言
	CStage *pStage = CScene::GetStage();	// ステージ情報
	if (pStage == NULL)
	{ // ステージが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_WAVEHAND;
	}

	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報
	if (pPlayer == NULL)
	{ // プレイヤーが使用されていない場合

		// 処理を抜ける
		assert(false);
		return MOTION_WAVEHAND;
	}

	// 変数を宣言
	D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 posFriend = GetVec3Position();			// 友達位置
	D3DXVECTOR3 rotFriend = GetVec3Rotation();			// 友達向き

	// 目標向きを設定
	m_destRot.y = atan2f(posFriend.x - posPlayer.x, posFriend.z - posPlayer.z);

	if (fabsf(rotFriend.y - m_destRot.y) <= basic::MIN_VARY)
	{ // 向きがある程度一致した場合

		// 自動描画をONにする
		m_pEmotion->SetEnableDraw(true);
	}

	// 重力の更新
	UpdateGravity();

	// 着地判定
	UpdateLanding(posFriend);

	// 向き更新
	UpdateRotation(rotFriend);

	// ステージ範囲外の補正
	pStage->LimitPosition(posFriend, basic::RADIUS);

	// 位置を反映
	SetVec3Position(posFriend);

	// 向きを反映
	SetVec3Rotation(rotFriend);

	// 手振りモーションを返す
	return MOTION_WAVEHAND;
}

//============================================================
//	過去位置の更新処理
//============================================================
void CFriend::UpdateOldPosition(void)
{
	// 過去位置を更新
	m_oldPos = GetVec3Position();
}

//============================================================
//	重力の更新処理
//============================================================
void CFriend::UpdateGravity(void)
{
	// 重力を加算
	m_move.y -= basic::GRAVITY;
}

//============================================================
//	着地状況の更新処理
//============================================================
bool CFriend::UpdateLanding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// ジャンプしている状態にする
	m_bJump = true;

	// 位置の更新
	// ビルの当たり判定・着地判定
	if (CollisionBuilding(rPos))
	{ // ビルに着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;
	}

	// 障害物の当たり判定・着地判定
	if (LandObstacle(rPos))
	{ // 障害物に着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;
	}

	// 地面・制限位置の着地判定
	if (CScene::GetStage()->LandFieldPosition(rPos, m_move)
	||  CScene::GetStage()->LandLimitPosition(rPos, m_move, 0.0f))
	{ // 友達が着地していた場合

		// 着地している状態にする
		bLand = true;

		// ジャンプしていない状態にする
		m_bJump = false;
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	向きの更新処理
//============================================================
void CFriend::UpdateRotation(D3DXVECTOR3& rRot)
{
	// 変数を宣言
	float fDiffRot = 0.0f;	// 差分向き

	// 目標向きの正規化
	useful::NormalizeRot(m_destRot.y);

	// 目標向きまでの差分を計算
	fDiffRot = m_destRot.y - rRot.y;

	// 差分向きの正規化
	useful::NormalizeRot(fDiffRot);

	// 向きの更新
	rRot.y += fDiffRot * basic::REV_ROTA;

	// 向きの正規化
	useful::NormalizeRot(rRot.y);
}

//============================================================
//	モーション・オブジェクトキャラクターの更新処理
//============================================================
void CFriend::UpdateMotion(int nMotion)
{
	// 変数を宣言
	int nAnimMotion = GetMotionType();	// 現在再生中のモーション

	if (nMotion != NONE_IDX)
	{ // モーションが設定されている場合

		if (IsMotionLoop(nAnimMotion))
		{ // ループするモーションだった場合

			if (nAnimMotion != nMotion)
			{ // 現在のモーションが再生中のモーションと一致しない場合

				// 現在のモーションの設定
				SetMotion(nMotion);
			}
		}
	}

	// オブジェクトキャラクターの更新
	CObjectChara::Update();
}

//============================================================
//	ビルとの一軸ごとの当たり判定
//============================================================
bool CFriend::ResponseSingleBuilding(const EAxis axis, D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinFriend = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// 友達最小大きさ
	D3DXVECTOR3 sizeMaxFriend = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// 友達最大大きさ
	bool bMin = false;	// 不の方向の判定状況
	bool bMax = false;	// 正の方向の判定状況
	bool bHit = false;	// 各軸の判定情報

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 posBuild = VEC3_ZERO;		// ビル位置
				D3DXVECTOR3 rotBuild = VEC3_ZERO;		// ビル向き
				D3DXVECTOR3 sizeMinBuild = VEC3_ZERO;	// ビル最小大きさ
				D3DXVECTOR3 sizeMaxBuild = VEC3_ZERO;	// ビル最大大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
				{ // オブジェクトラベルがビルではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ビルの位置を設定
				posBuild = pObjCheck->GetVec3Position();

				// ビルの向きを設定
				rotBuild = pObjCheck->GetVec3Rotation();

				// ビルの最小の大きさを設定
				sizeMinBuild = pObjCheck->GetVec3Sizing();
				sizeMinBuild.y = 0.0f;	// 縦の大きさを初期化

				// ビルの最大の大きさを設定
				sizeMaxBuild = pObjCheck->GetVec3Sizing();
				sizeMaxBuild.y *= 2.0f;	// 縦の大きさを倍にする

				switch (axis)
				{ // 判定軸ごとの処理
				case AXIS_X:	// X軸

					// X軸の衝突判定
					collision::ResponseSingleX
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxFriend,	// 判定サイズ(右・上・後)
						sizeMinFriend,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 左からの判定
						&bMax			// 右からの判定
					);

					if (bMin || bMax)
					{ // 横から当たっていた場合

						// ぶつかり状況にする
						bHit = true;
					}

					break;

				case AXIS_Y:	// Y軸

					// Y軸の衝突判定
					collision::ResponseSingleY
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxFriend,	// 判定サイズ(右・上・後)
						sizeMinFriend,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 下からの判定
						&bMax			// 上からの判定
					);

					if (bMax)
					{ // 上から当たっていた場合

						// 着地している状況にする
						bHit = true;
					}

					break;

				case AXIS_Z:	// Z軸

					// Z軸の衝突判定
					collision::ResponseSingleZ
					( // 引数
						rPos,			// 判定位置
						m_oldPos,		// 判定過去位置
						posBuild,		// 判定目標位置
						sizeMaxFriend,	// 判定サイズ(右・上・後)
						sizeMinFriend,	// 判定サイズ(左・下・前)
						sizeMaxBuild,	// 判定目標サイズ(右・上・後)
						sizeMinBuild,	// 判定目標サイズ(左・下・前)
						&m_move,		// 移動量
						&bMin,			// 前からの判定
						&bMax			// 後からの判定
					);

					if (bMin || bMax)
					{ // 横から当たっていた場合

						// ぶつかり状況にする
						bHit = true;
					}

					break;

				default:	// 例外処理
					assert(false);
					break;
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 各軸の判定情報を返す
	return bHit;
}

//============================================================
//	障害物との着地判定
//============================================================
bool CFriend::LandObstacle(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 sizeMinFriend = D3DXVECTOR3(basic::RADIUS, 0.0f, basic::RADIUS);			// 友達最小大きさ
	D3DXVECTOR3 sizeMaxFriend = D3DXVECTOR3(basic::RADIUS, basic::HEIGHT, basic::RADIUS);	// 友達最大大きさ
	bool bLand = false;	// 着地状況

	for (int nCntPri = 0; nCntPri < MAX_PRIO; nCntPri++)
	{ // 優先順位の総数分繰り返す

		// ポインタを宣言
		CObject *pObjectTop = CObject::GetTop(nCntPri);	// 先頭オブジェクト

		if (pObjectTop != NULL)
		{ // 先頭が存在する場合

			// ポインタを宣言
			CObject *pObjCheck = pObjectTop;	// オブジェクト確認用

			while (pObjCheck != NULL)
			{ // オブジェクトが使用されている場合繰り返す

				// 変数を宣言
				D3DXVECTOR3 pos0, pos1, pos2, pos3;	// 頂点座標代入用
				D3DXVECTOR3 posObs  = VEC3_ZERO;	// 障害物位置
				D3DXVECTOR3 rotObs  = VEC3_ZERO;	// 障害物向き
				D3DXVECTOR3 sizeObs = VEC3_ZERO;	// 障害物大きさ
				float fAngle  = 0.0f;	// 対角線の角度
				float fLength = 0.0f;	// 対角線の長さ
				bool  bHit = false;		// 判定情報

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
				{ // オブジェクトラベルが障害物ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->GetState() != CObstacle::STATE_LAND)
				{ // 着地できる特殊状態ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 障害物の情報を取得
				posObs  = pObjCheck->GetVec3Position();	// 位置
				rotObs  = pObjCheck->GetVec3Rotation();	// 向き
				sizeObs = pObjCheck->GetVec3Sizing();	// 大きさ
				fAngle  = pObjCheck->GetAngle();		// 対角線角度
				fLength = pObjCheck->GetLength();		// 対角線長さ

				// 頂点座標を計算
				pos0.x = posObs.x + sinf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos0.y = 0.0f;
				pos0.z = posObs.z + cosf(rotObs.y + (D3DX_PI + fAngle)) * fLength;
				pos1.x = posObs.x + sinf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos1.y = 0.0f;
				pos1.z = posObs.z + cosf(rotObs.y + (D3DX_PI - fAngle)) * fLength;
				pos3.x = posObs.x + sinf(rotObs.y - fAngle) * fLength;
				pos3.y = 0.0f;
				pos3.z = posObs.z + cosf(rotObs.y - fAngle) * fLength;
				pos2.x = posObs.x + sinf(rotObs.y + fAngle) * fLength;
				pos2.y = 0.0f;
				pos2.z = posObs.z + cosf(rotObs.y + fAngle) * fLength;

				// 障害物との判定
				if (rPos.y + basic::HEIGHT > posObs.y
				&&  rPos.y < posObs.y + sizeObs.y)
				{ // Y座標が範囲内の場合

					// 二軸の矩形の衝突判定
					bHit = collision::BoxOuterPillar
					( // 引数
						pos0,	// 四角の各頂点
						pos1,	// 四角の各頂点
						pos2,	// 四角の各頂点
						pos3,	// 四角の各頂点
						rPos	// 判定位置
					);
				}

				if (bHit)
				{ // 当たっていた場合

					if (m_oldPos.y >= posObs.y + sizeObs.y)
					{ // 前回の位置が着地点以上の場合

						// 友達位置を障害物の上にする
						rPos.y = posObs.y + sizeObs.y;

						// 着地している状況にする
						bLand = true;

						// 縦移動量を初期化
						m_move.y = 0.0f;
					}
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	ビルとの当たり判定
//============================================================
bool CFriend::CollisionBuilding(D3DXVECTOR3& rPos)
{
	// 変数を宣言
	bool bLand = false;	// 着地状況

	// 移動量を加算
	rPos.x += m_move.x;

	// X軸の当たり判定
	if (ResponseSingleBuilding(AXIS_X, rPos))
	{ // 横に当たっていた場合

		// ヒット
		Hit();
	}

	// 移動量を加算
	rPos.y += m_move.y;

	// Y軸の当たり判定
	if (ResponseSingleBuilding(AXIS_Y, rPos))
	{ // 上に当たっていた場合

		// 着地している状況にする
		bLand = true;
	}

	// 移動量を加算
	rPos.z += m_move.z;

	// Z軸の当たり判定
	if (ResponseSingleBuilding(AXIS_Z, rPos))
	{ // 横に当たっていた場合

		// ヒット
		Hit();
	}

	// 移動量を減衰
	if (m_bJump)
	{ // 空中の場合

		m_move.x += (0.0f - m_move.x) * basic::NOR_JUMP_REV;
		m_move.z += (0.0f - m_move.z) * basic::NOR_JUMP_REV;
	}
	else
	{ // 地上の場合

		m_move.x += (0.0f - m_move.x) * basic::NOR_LAND_REV;
		m_move.z += (0.0f - m_move.z) * basic::NOR_LAND_REV;
	}

	// 着地状況を返す
	return bLand;
}

//============================================================
//	セットアップ処理
//============================================================
void CFriend::LoadSetup(void)
{
	// 変数を宣言
	CMotion::SMotionInfo info;		// ポーズの代入用
	D3DXVECTOR3 pos = VEC3_ZERO;	// 位置の代入用
	D3DXVECTOR3 rot = VEC3_ZERO;	// 向きの代入用
	int nID			= 0;	// インデックスの代入用
	int nParentID	= 0;	// 親インデックスの代入用
	int nNowPose	= 0;	// 現在のポーズ番号
	int nNowKey		= 0;	// 現在のキー番号
	int nLoop		= 0;	// ループのON/OFFの変換用
	int nEnd		= 0;	// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// ポーズ代入用の変数を初期化
	memset(&info, 0, sizeof(info));

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// キャラクターの設定
			if (strcmp(&aString[0], "CHARACTERSET") == 0)
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
							}
							else if (strcmp(&aString[0], "PARENT") == 0)
							{ // 読み込んだ文字列が PARENT の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &nParentID);	// モデルの親のインデックスを読み込む
							}
							else if (strcmp(&aString[0], "POS") == 0)
							{ // 読み込んだ文字列が POS の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%f", &pos.x);		// X座標を読み込む
								fscanf(pFile, "%f", &pos.y);		// Y座標を読み込む
								fscanf(pFile, "%f", &pos.z);		// Z座標を読み込む
							}
							else if (strcmp(&aString[0], "ROT") == 0)
							{ // 読み込んだ文字列が ROT の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%f", &rot.x);		// X向きを読み込む
								fscanf(pFile, "%f", &rot.y);		// Y向きを読み込む
								fscanf(pFile, "%f", &rot.z);		// Z向きを読み込む
							}
						} while (strcmp(&aString[0], "END_PARTSSET") != 0);	// 読み込んだ文字列が END_PARTSSET ではない場合ループ

						// パーツ情報の設定
						CObjectChara::SetPartsInfo(nID, nParentID, pos, rot, mc_apModelFile[nID]);
					}
				} while (strcmp(&aString[0], "END_CHARACTERSET") != 0);		// 読み込んだ文字列が END_CHARACTERSET ではない場合ループ
			}

			// モーションの設定
			else if (strcmp(&aString[0], "MOTIONSET") == 0)
			{ // 読み込んだ文字列が MOTIONSET の場合

				// 現在のポーズ番号を初期化
				nNowPose = 0;

				do
				{ // 読み込んだ文字列が END_MOTIONSET ではない場合ループ

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
										info.aKeyInfo[nNowPose].aKey[nNowKey].pos += GetPartsPosition(nNowKey);
									}
									else if (strcmp(&aString[0], "ROT") == 0)
									{ // 読み込んだ文字列が ROT の場合

										fscanf(pFile, "%s", &aString[0]);									// = を読み込む (不要)
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.x);	// X向きを読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.y);	// Y向きを読み込む
										fscanf(pFile, "%f", &info.aKeyInfo[nNowPose].aKey[nNowKey].rot.z);	// Z向きを読み込む

										// 読み込んだ向きにパーツの初期向きを加算
										info.aKeyInfo[nNowPose].aKey[nNowKey].rot += GetPartsRotation(nNowKey);

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
				} while (strcmp(&aString[0], "END_MOTIONSET") != 0);	// 読み込んだ文字列が END_MOTIONSET ではない場合ループ

				// モーション情報の設定
				CObjectChara::SetMotionInfo(info);
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "友達セットアップファイルの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
