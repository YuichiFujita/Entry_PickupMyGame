//============================================================
//
//	プレイヤー処理 [player.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "player.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "collision.h"
#include "debugproc.h"
#include "field.h"

//************************************************************
//	マクロ定義
//************************************************************
#define PLAYER_SETUP_TXT	"data\\TXT\\player.txt"	// セットアップテキスト相対パス

#define PLAY_MOVE		(2.0f)	// プレイヤー移動量
#define PLAY_REV		(0.2f)	// プレイヤー移動量の減衰係数
#define PLAY_REV_ROTA	(0.15f)	// プレイヤー向き変更の減衰係数
#define PLAY_CAM_ROTA	(0.04f)	// カメラ回転量
#define PLAY_JUMP		(20.0f)	// プレイヤージャンプ量
#define PLAY_GRAVITY	(1.0f)	// プレイヤー重力
#define PLAY_RADIUS		(20.0f)	// プレイヤー半径

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CPlayer::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\escapekun000.x",		// エスケープ君モデル
};

//************************************************************
//	子クラス [CPlayer] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayer::CPlayer() : CObjectModel(CObject::LABEL_PLAYER)
{
	// メンバ変数をクリア
	m_move		= VEC3_ZERO;	// 移動量
	m_destRot	= VEC3_ZERO;	// 目標向き
	m_bJump		= false;		// ジャンプ状況
}

//============================================================
//	デストラクタ
//============================================================
CPlayer::~CPlayer()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayer::Init(void)
{
	// メンバ変数を初期化
	m_move		= VEC3_ZERO;	// 移動量
	m_destRot	= VEC3_ZERO;	// 目標向き
	m_bJump		= false;		// ジャンプ状況

	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
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
void CPlayer::Uninit(void)
{
	// オブジェクトモデルの終了
	CObjectModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayer::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos = GetPosition();	// 位置

	// 移動操作
	Move();

	// 向き更新
	Rot();

	// ジャンプ操作
	Jump();

	// 重力を加算
	m_move.y -= PLAY_GRAVITY;

	// 移動量を加算
	pos += m_move;

	// 移動量を減衰
	m_move.x += (0.0f - m_move.x) * PLAY_REV;
	m_move.z += (0.0f - m_move.z) * PLAY_REV;

	// 着地判定
	Land(pos);

	// 位置を反映
	SetPosition(pos);

	// オブジェクトモデルの更新
	CObjectModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CPlayer::Draw(void)
{
	// オブジェクトモデルの描画
	CObjectModel::Draw();
}

//============================================================
//	生成処理
//============================================================
CPlayer *CPlayer::Create(const D3DXVECTOR3& rPos, const D3DXVECTOR3& rRot)
{
	// 変数を宣言
	int nModelID;	// モデルインデックス

	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ
	CPlayer *pPlayer = NULL;	// プレイヤー生成用

	if (UNUSED(pPlayer))
	{ // 使用されていない場合

		// メモリ確保
		pPlayer = new CPlayer;	// プレイヤー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pPlayer))
	{ // 使用されている場合
		
		if (SUCCEEDED(pPlayer->GetResult()))
		{ // オブジェクトの情報設定に成功した場合

			// プレイヤーの初期化
			if (FAILED(pPlayer->Init()))
			{ // 初期化に失敗した場合

				// メモリ開放
				delete pPlayer;
				pPlayer = NULL;

				// 失敗を返す
				return NULL;
			}

			// モデルを登録
			nModelID = pModel->Regist(mc_apModelFile[MODEL_ESCAPEKUN]);

			// モデルを割当
			pPlayer->BindModel(pModel->GetModel(nModelID));

			// 位置を設定
			pPlayer->SetPosition(rPos);

			// 向きを設定
			pPlayer->SetRotation(rRot);

			// 確保したアドレスを返す
			return pPlayer;
		}
		else
		{ // オブジェクトの情報設定に失敗した場合

			// メモリ開放
			delete pPlayer;
			pPlayer = NULL;

			// 失敗を返す
			return NULL;
		}
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	移動処理
//============================================================
void CPlayer::Move(void)
{
	// 変数を宣言
	D3DXVECTOR3 rot = CManager::GetCamera()->GetRotation();	// カメラの向き

	// ポインタを宣言
	CInputKeyboard	*pKeyboard = CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad = CManager::GetPad();				// パッド

	// 移動操作
	if (pKeyboard->GetPress(DIK_W))
	{ // 奥移動の操作が行われた場合

		if (pKeyboard->GetPress(DIK_A))
		{ // 左移動の操作も行われた場合 (左奥移動)

			// 移動量を更新
			m_move.x += sinf(rot.y - (D3DX_PI * 0.25f)) * PLAY_MOVE;
			m_move.z += cosf(rot.y - (D3DX_PI * 0.25f)) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(135) + rot.y;
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // 右移動の操作も行われた場合 (右奥移動)

			// 移動量を更新
			m_move.x -= sinf(rot.y - (D3DX_PI * 0.75f)) * PLAY_MOVE;
			m_move.z -= cosf(rot.y - (D3DX_PI * 0.75f)) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(225) + rot.y;
		}
		else
		{ // 奥移動の操作だけが行われた場合 (奥移動)

			// 移動量を更新
			m_move.x += sinf(rot.y) * PLAY_MOVE;
			m_move.z += cosf(rot.y) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(180) + rot.y;
		}
	}
	else if (pKeyboard->GetPress(DIK_S))
	{ // 手前移動の操作が行われた場合

		if (pKeyboard->GetPress(DIK_A))
		{ // 左移動の操作も行われた場合 (左手前移動)

			// 移動量を更新
			m_move.x += sinf(rot.y - (D3DX_PI * 0.75f)) * PLAY_MOVE;
			m_move.z += cosf(rot.y - (D3DX_PI * 0.75f)) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(45) + rot.y;
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // 右移動の操作も行われた場合 (右手前移動)

			// 移動量を更新
			m_move.x -= sinf(rot.y - (D3DX_PI * 0.25f)) * PLAY_MOVE;
			m_move.z -= cosf(rot.y - (D3DX_PI * 0.25f)) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(315) + rot.y;
		}
		else
		{ // 手前移動の操作だけが行われた場合 (手前移動)

			// 移動量を更新
			m_move.x -= sinf(rot.y) * PLAY_MOVE;
			m_move.z -= cosf(rot.y) * PLAY_MOVE;

			// 目標向きを更新
			m_destRot.y = D3DXToRadian(0) + rot.y;
		}
	}
	else if (pKeyboard->GetPress(DIK_A))
	{ // 左移動の操作が行われた場合

		// 移動量を更新
		m_move.x += sinf(rot.y - (D3DX_PI * 0.5f)) * PLAY_MOVE;
		m_move.z += cosf(rot.y - (D3DX_PI * 0.5f)) * PLAY_MOVE;

		// 目標向きを更新
		m_destRot.y = D3DXToRadian(90) + rot.y;
	}
	else if (pKeyboard->GetPress(DIK_D))
	{ // 右移動の操作が行われた場合

		// 移動量を更新
		m_move.x -= sinf(rot.y - (D3DX_PI * 0.5f)) * PLAY_MOVE;
		m_move.z -= cosf(rot.y - (D3DX_PI * 0.5f)) * PLAY_MOVE;

		// 目標向きを更新
		m_destRot.y = D3DXToRadian(270) + rot.y;
	}
}

//============================================================
//	向き処理
//============================================================
void CPlayer::Rot(void)
{
	// 変数を宣言
	D3DXVECTOR3 rot = GetRotation();	// 向き
	float fDiffRot = 0.0f;	// 差分向き

	// 目標向きの正規化
	useful::NormalizeRot(m_destRot.y);

	// 目標向きまでの差分を計算
	fDiffRot = m_destRot.y - rot.y;

	// 差分向きの正規化
	useful::NormalizeRot(fDiffRot);

	// 向きの更新
	rot.y += fDiffRot * PLAY_REV_ROTA;

	// 向きの正規化
	useful::NormalizeRot(rot.y);

	// 向きを反映
	SetRotation(rot);
}

//============================================================
//	ジャンプ処理
//============================================================
void CPlayer::Jump(void)
{
	// ポインタを宣言
	CInputKeyboard	*pKeyboard = CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad = CManager::GetPad();				// パッド

	// ジャンプ操作
	if (m_bJump == false)
	{ // ジャンプしていない場合

		if (pKeyboard->GetTrigger(DIK_SPACE))
		{ // ジャンプの操作が行われた場合

			// 上移動量
			m_move.y += PLAY_JUMP;

			// ジャンプしている状態にする
			m_bJump = true;
		}
	}
}

//============================================================
//	着地処理
//============================================================
void CPlayer::Land(D3DXVECTOR3& rPos)
{
	// 着地判定
	if (CManager::GetField()->LandPosition(rPos, m_move))
	{ // プレイヤーが着地していた場合

		// ジャンプしていない状態にする
		m_bJump = false;
	}
	else
	{ // 着地していない場合

		// ジャンプしている状態にする
		m_bJump = true;
	}
}
