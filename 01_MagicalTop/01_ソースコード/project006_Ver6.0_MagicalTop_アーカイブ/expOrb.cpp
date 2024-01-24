//============================================================
//
//	経験値処理 [expOrb.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "expOrb.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "texture.h"

#include "collision.h"
#include "player.h"
#include "target.h"
#include "field.h"
#include "stage.h"

//************************************************************
//	マクロ定義
//************************************************************
#define EXP_MOVE		(19.0f)	// 経験値の移動量
#define EXP_REV_ROT		(0.25f)	// 経験値ホーミング時の向き補正係数
#define EXP_PLUS_POSY	(30.0f)	// 経験値の地面からの上昇量
#define EXP_RADIUS		(80.0f)	// 経験値の半径

#define EXP_SPAWN_RADIUS	(250)	// 経験値出現時のランダム範囲
#define LIMIT_RADIUS	(3000.0f)	// 経験値のステージ移動半径

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CExpOrb::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\exp000.png",	// 経験値テクスチャ
};

//************************************************************
//	子クラス [CExpOrb] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CExpOrb::CExpOrb() : CObjectBillboard(CObject::LABEL_EXP)
{

}

//============================================================
//	デストラクタ
//============================================================
CExpOrb::~CExpOrb()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CExpOrb::Init(void)
{
	// 変数を宣言
	int nTextureID;	// テクスチャインデックス

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// オブジェクトビルボードの初期化
	if (FAILED(CObjectBillboard::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録
	nTextureID = pTexture->Regist(mc_apTextureFile[TEXTURE_NORMAL]);

	// テクスチャを割当
	BindTexture(nTextureID);

	// 大きさを設定
	SetScaling(D3DXVECTOR3(EXP_RADIUS, EXP_RADIUS, 0.0f));

	// Zテストを設定
	SetFunc(D3DCMP_LESSEQUAL);

	// Zバッファの使用状況を設定
	SetZEnable(true);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CExpOrb::Uninit(void)
{
	// オブジェクトビルボードの終了
	CObjectBillboard::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CExpOrb::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = CScene::GetPlayer()->GetPosition();	// プレイヤー位置
	D3DXVECTOR3 posTarget = CScene::GetTarget()->GetPosition();	// ターゲット位置
	D3DXVECTOR3 posExp = CObjectBillboard::GetPosition();		// 経験値位置
	D3DXVECTOR3 rotExp = CObjectBillboard::GetRotation();		// 経験値向き
	float fDestRot = 0.0f;	// 目標向き
	float fDiffRot = 0.0f;	// 向き

	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (CScene::GetPlayer()->GetState() != CPlayer::STATE_DEATH && USED(CScene::GetTarget()))
	{ // プレイヤーが死亡していない且つ、ターゲットが使用されている場合

		// プレイヤーの方向を代入
		fDestRot = atan2f(posExp.x - posPlayer.x, posExp.z - posPlayer.z);	// 目標向き

		// 差分向きを求める
		fDiffRot = fDestRot - rotExp.y;
		useful::NormalizeRot(fDiffRot);	// 差分向きの正規化

		// 向きの更新
		rotExp.y += fDiffRot * EXP_REV_ROT;
		useful::NormalizeRot(rotExp.y);	// 向きの正規化

		// 位置を設定
		posExp.x -= sinf(rotExp.y) * EXP_MOVE;
		posExp.y = 0.0f;
		posExp.z -= cosf(rotExp.y) * EXP_MOVE;

		// 地面との当たり判定
		posExp.y = CScene::GetField()->GetPositionHeight(posExp);
		posExp.y += EXP_PLUS_POSY;

		// 円柱の内側制限
		collision::InCirclePillar(posExp, posTarget, EXP_RADIUS, LIMIT_RADIUS);

		// 位置を設定
		CObjectBillboard::SetPosition(posExp);

		// 向きを設定
		CObjectBillboard::SetRotation(rotExp);

		// プレイヤーとの当たり判定
		if (CollisionPlayer())
		{ // プレイヤーに当たっている場合

			// オブジェクトの終了
			Uninit();

			// 関数を抜ける
			return;
		}
	}

	// オブジェクトビルボードの更新
	CObjectBillboard::Update();
}

//============================================================
//	描画処理
//============================================================
void CExpOrb::Draw(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// αテストの参照値設定

	// オブジェクトビルボードの描画
	CObjectBillboard::Draw();

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// αテストの参照値設定
}

//============================================================
//	生成処理
//============================================================
CExpOrb *CExpOrb::Create(const D3DXVECTOR3& rPos)
{
	// ポインタを宣言
	CExpOrb *pExpOrb = NULL;	// 経験値生成用

	if (UNUSED(pExpOrb))
	{ // 使用されていない場合

		// メモリ確保
		pExpOrb = new CExpOrb;	// 経験値
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pExpOrb))
	{ // 確保に成功している場合

		// 経験値の初期化
		if (FAILED(pExpOrb->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pExpOrb;
			pExpOrb = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pExpOrb->SetPosition(rPos);

		// 確保したアドレスを返す
		return pExpOrb;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ランダム生成処理
//============================================================
void CExpOrb::RandomSpawn(const int nNum, const D3DXVECTOR3& rPos)
{
	// 変数を宣言
	D3DXVECTOR3 pos;	// 位置設定用

	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (USED(CScene::GetTarget()))
	{ // ターゲットが使用されている場合

		for (int nCntGrow = 0; nCntGrow < nNum; nCntGrow++)
		{ // 生成数分繰り返す

			// 生成位置を設定
			pos.x = rPos.x + (float)(rand() % (EXP_SPAWN_RADIUS * 2) - EXP_SPAWN_RADIUS + 1);
			pos.y = rPos.y;
			pos.z = rPos.z + (float)(rand() % (EXP_SPAWN_RADIUS * 2) - EXP_SPAWN_RADIUS + 1);

			// 円柱の内側制限
			collision::InCirclePillar(pos, pTarget->GetPosition(), EXP_RADIUS, LIMIT_RADIUS);

			// 経験値オブジェクトの生成
			CExpOrb::Create(pos);
		}
	}
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
bool CExpOrb::CollisionPlayer(void)
{
	// 変数を宣言
	bool bHit = false;	// 判定状況

	// ポインタを宣言
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報

	if (pPlayer->GetState() != CPlayer::STATE_DEATH)
	{ // プレイヤーが使用されている場合

		// プレイヤーとの当たり判定
		bHit = collision::Circle2D
		( // 引数
			GetPosition(),				// 経験値位置
			pPlayer->GetPosition(),		// プレイヤー位置
			EXP_RADIUS,					// 経験値半径
			pPlayer->GetRadius() * 0.5f	// プレイヤー半径
		);

		if (bHit)
		{ // 当たっていた場合

			// プレイヤーの経験値の加算
			CScene::GetPlayer()->AddExp(1);
		}
	}

	// 判定状況を返す
	return bHit;
}
