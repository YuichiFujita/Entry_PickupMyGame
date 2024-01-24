//============================================================
//
//	セーブポイント処理 [savePoint.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "savePoint.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "texture.h"
#include "collision.h"
#include "scene.h"
#include "player.h"
#include "particle3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const int	PRIORITY		= 4;		// セーブポイントの優先順位
	const int	CHANGE_MAT_ID	= 4;		// 変更するマテリアルのインデックス
	const int	RESET_MAT_CNT	= 45;		// マテリアル変更の再設定までのフレーム
	const float	COL_RADIUS		= 80.0f;	// 当たり判定の半径
	const float	COL_HEIGHT		= 280.0f;	// 当たり判定の縦幅

	// パーティクル情報
	namespace particle
	{
		const D3DXCOLOR	COL_HEAL = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);	// 回復パーティクルの色
	}
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CSavePoint::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\SAVEPOINT\\savepoint000.x",	// セーブポイントモデル
};
CObject *CSavePoint::m_pCurrentSave = NULL;	// 現在のセーブポイントへのポインタ
int CSavePoint::m_nNumAll = 0;	// セーブポイントの総数

//************************************************************
//	子クラス [CSavePoint] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSavePoint::CSavePoint() : CObjectModel(CObject::LABEL_SAVEPOINT, PRIORITY), m_nThisSaveID(m_nNumAll)
{
	// メンバ変数をクリア
	m_state = STATE_NORMAL;	// 状態
	m_nCounterState = 0;	// 状態管理カウンター

	// セーブポイントの総数を加算
	m_nNumAll++;
}

//============================================================
//	デストラクタ
//============================================================
CSavePoint::~CSavePoint()
{
	// セーブポイントの総数を減算
	m_nNumAll--;
}

//============================================================
//	初期化処理
//============================================================
HRESULT CSavePoint::Init(void)
{
	// メンバ変数を初期化
	m_state = STATE_NORMAL;	// 状態
	m_nCounterState = 0;	// 状態管理カウンター

	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルを割り当て
	BindModel(mc_apModelFile[MODEL_SAVEPOINT]);

	if (m_nThisSaveID == 0)
	{ // 自身のセーブポイントが一つ目の場合

		// 自身のセーブポイントを保存
		m_pCurrentSave = (CObject*)this;
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSavePoint::Uninit(void)
{
	// オブジェクトモデルの終了
	CObjectModel::Uninit();

	if (m_pCurrentSave == (CObject*)this)
	{ // 現在のセーブポイントが自身だった場合

		// 見つかった生成済みのセーブポイントを設定
		m_pCurrentSave = GetSavePoint();
	}
}

//============================================================
//	更新処理
//============================================================
void CSavePoint::Update(void)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NORMAL:

		// 無し

		break;

	case STATE_SAVE:

		if (m_nCounterState < RESET_MAT_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_nCounterState++;
		}
		else
		{ // カウンターが一定値以上の場合

			// カウンターを初期化
			m_nCounterState = 0;

			// マテリアルを再設定
			ResetMaterial();
		}

		break;

	default:
		assert(false);
		break;
	}

	// プレイヤーとの当たり判定
	CollisionPlayer();

	// オブジェクトモデルの更新
	CObjectModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CSavePoint::Draw(void)
{
	// オブジェクトモデルの描画
	CObjectModel::Draw();
}

//============================================================
//	自身のセーブポイントインデックス取得処理
//============================================================
int CSavePoint::GetIndex(void) const
{
	// 自身のセーブポイントインデックスを返す
	return m_nThisSaveID;
}

//============================================================
//	半径取得処理
//============================================================
float CSavePoint::GetRadius(void) const
{
	// 半径を返す
	return COL_RADIUS;
}

//============================================================
//	生成処理
//============================================================
CSavePoint *CSavePoint::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot		// 向き
)
{
	// ポインタを宣言
	CSavePoint *pSavePoint = NULL;	// セーブポイント生成用

	if (pSavePoint == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pSavePoint = new CSavePoint;	// セーブポイント
	}
	else { assert(false); return NULL; }	// 使用中

	if (pSavePoint != NULL)
	{ // 確保に成功している場合

		// セーブポイントの初期化
		if (FAILED(pSavePoint->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pSavePoint;
			pSavePoint = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pSavePoint->SetVec3Position(rPos);

		// 向きを設定
		pSavePoint->SetVec3Rotation(rRot);

		// 確保したアドレスを返す
		return pSavePoint;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	総数取得処理
//============================================================
int CSavePoint::GetNumAll(void)
{
	// 総数を返す
	return m_nNumAll;
}

//============================================================
//	セーブポイントインデックス取得処理
//============================================================
int CSavePoint::GetSavePointID(void)
{
	// 自身のセーブポイントインデックスを返す
	return m_pCurrentSave->GetIndex();
}

//============================================================
//	セーブポイント情報取得処理
//============================================================
CSavePoint::SInfo CSavePoint::GetSavePointInfo(void)
{
	// 変数を宣言
	SInfo info = { VEC3_ZERO, VEC3_ZERO };	// セーブポイント情報

	if (m_pCurrentSave != NULL)
	{ // セーブポイントが存在する場合

		// セーブポイントの情報を設定
		info.pos = m_pCurrentSave->GetVec3Position();	// 位置
		info.rot = m_pCurrentSave->GetVec3Rotation();	// 向き
	}
	else { assert(false); }	// セーブポイント無し

	// セーブポイント情報を返す
	return info;
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
void CSavePoint::CollisionPlayer(void)
{
	if (CManager::GetInstance()->GetMode() == CScene::MODE_GAME
	||  CManager::GetInstance()->GetMode() == CScene::MODE_TUTORIAL)
	{ // チュートリアル・ゲームモードの場合

		// ポインタを宣言
		CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報
		if (pPlayer == NULL)
		{ // プレイヤーが存在しない場合

			// 処理を抜ける
			assert(false);
			return;
		}

		// 変数を宣言
		D3DXVECTOR3 posPlayer = pPlayer->GetVec3Position();	// プレイヤー位置
		D3DXVECTOR3 posSave = GetVec3Position();	// セーブ位置
		float fPlayerRadius = pPlayer->GetRadius();	// プレイヤー半径
		float fPlayerHeight = pPlayer->GetHeight();	// プレイヤー縦幅
		bool  bHit = false;	// 判定状況

		if ((CObject*)this != m_pCurrentSave)
		{ // 現在のセーブポイントと一致しない場合

			if (posPlayer.y + fPlayerHeight >= posSave.y
			&&  posPlayer.y <= posSave.y + COL_HEIGHT)
			{ // Y座標が範囲内の場合

				// プレイヤーとの判定
				bHit = collision::Circle2D
				( // 引数
					posPlayer,		// 判定位置
					posSave,		// 判定目標位置
					fPlayerRadius,	// 判定半径
					COL_RADIUS		// 判定目標半径
				);
				if (bHit)
				{ // プレイヤーが判定内の場合

					// セーブポイントを自身に変更
					m_pCurrentSave = (CObject*)this;

					// カウンターを初期化
					m_nCounterState = 0;

					// セーブ状態を設定
					m_state = STATE_SAVE;

					// マテリアルを発光緑に差し替え
					SetMaterial(material::GlowGreen(), CHANGE_MAT_ID);

					// 回復パーティクルを生成
					CParticle3D::Create(CParticle3D::TYPE_HEAL, posSave, particle::COL_HEAL);

					// サウンドの再生
					CManager::GetInstance()->GetSound()->Play(CSound::LABEL_SE_SAVE);	// セーブ音
				}
			}
		}
	}
}

//============================================================
//	生成済みセーブポイント取得処理
//============================================================
CObject *CSavePoint::GetSavePoint(void)
{
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

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_SAVEPOINT)
				{ // オブジェクトラベルがセーブポイントではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck->IsDeath() == true)
				{ // オブジェクトラベルがセーブポイントではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 見つかったセーブポイントを返す
				return pObjCheck;
			}
		}
	}

	// NULLを返す
	return NULL;
}
