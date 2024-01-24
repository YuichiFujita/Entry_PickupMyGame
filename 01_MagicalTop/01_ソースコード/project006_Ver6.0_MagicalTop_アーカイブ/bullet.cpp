//============================================================
//
//	弾処理 [bullet.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "bullet.h"
#include "manager.h"
#include "scene.h"
#include "sceneGame.h"
#include "renderer.h"
#include "sound.h"
#include "texture.h"

#include "collision.h"
#include "target.h"
#include "player.h"
#include "field.h"

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CBullet::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\bullet000.png",	// 弾テクスチャ
};

//************************************************************
//	子クラス [CBullet] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CBullet::CBullet(const int nDamage) : CObjectBillboard(CObject::LABEL_BULLET), m_nDamage(nDamage)
{
	// メンバ変数をクリア
	m_move  = VEC3_ZERO;	// 移動量
	m_type  = TYPE_ENEMY;	// 種類
	m_nLife = 0;			// 寿命
}

//============================================================
//	デストラクタ
//============================================================
CBullet::~CBullet()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CBullet::Init(void)
{
	// メンバ変数を初期化
	m_move  = VEC3_ZERO;	// 移動量
	m_type  = TYPE_ENEMY;	// 種類
	m_nLife = 0;			// 寿命

	// オブジェクトビルボードの初期化
	if (FAILED(CObjectBillboard::Init()))
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
void CBullet::Uninit(void)
{
	// オブジェクトビルボードの終了
	CObjectBillboard::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CBullet::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();	// 位置

	if (m_nLife > 0)
	{ // 寿命が残っている場合

		// カウンターを減算
		m_nLife--;
	}
	else
	{ // 寿命が来た場合

		// オブジェクトの終了
		Uninit();

		// 関数を抜ける
		return;
	}

	// 移動量を加算
	pos += m_move;

	// 地面との当たり判定
	if (pos.y <= CScene::GetField()->GetPositionHeight(pos))
	{ // 地面に当たっている場合

		// オブジェクトの終了
		Uninit();

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_HIT);	// 弾着弾音

		// 関数を抜ける
		return;
	}

	// ターゲットとの当たり判定
	if (CollisionTarget())
	{ // ターゲットに当たっている場合

		// オブジェクトの終了
		Uninit();

		// サウンドの再生
		CManager::GetSound()->Play(CSound::LABEL_SE_HIT);	// 弾着弾音

		// 関数を抜ける
		return;
	}

	// 位置を設定
	CObjectBillboard::SetPosition(pos);

	// オブジェクトビルボードの更新
	CObjectBillboard::Update();
}

//============================================================
//	描画処理
//============================================================
void CBullet::Draw(void)
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
//	移動量の設定処理
//============================================================
void CBullet::SetMove(D3DXVECTOR3 vec, const float fMove)
{
	// ベクトルの正規化
	D3DXVec3Normalize(&vec, &vec);

	// 移動量を設定
	m_move = vec * fMove;
}

//============================================================
//	種類の設定処理
//============================================================
void CBullet::SetType(const TYPE type)
{
	// 引数の種類を代入
	m_type = type;
}

//============================================================
//	寿命の設定処理
//============================================================
void CBullet::SetLife(const int nLife)
{
	// 引数の寿命を代入
	m_nLife = nLife;
}

//============================================================
//	生成処理
//============================================================
CBullet *CBullet::Create
(
	const TYPE type,			// 種類
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXCOLOR& rCol,		// 色
	const D3DXVECTOR3& rVec,	// 移動方向
	const float fMove,			// 移動速度
	const int nLife,			// 寿命
	const int nDamage			// 攻撃力
)
{
	// 変数を宣言
	int nTextureID;	// テクスチャインデックス

	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ
	CBullet *pBullet = NULL;	// 弾生成用

	if (UNUSED(pBullet))
	{ // 使用されていない場合

		// メモリ確保
		pBullet = new CBullet(nDamage);	// 弾
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pBullet))
	{ // 確保に成功している場合

		// 弾の初期化
		if (FAILED(pBullet->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pBullet;
			pBullet = NULL;

			// 失敗を返す
			return NULL;
		}

		// テクスチャを登録
		nTextureID = pTexture->Regist(mc_apTextureFile[type]);

		// テクスチャを割当
		pBullet->BindTexture(nTextureID);

		// 位置を設定
		pBullet->SetPosition(rPos);

		// 大きさを設定
		pBullet->SetScaling(rSize);

		// 色を設定
		pBullet->SetColor(rCol);

		// 種類を設定
		pBullet->SetType(type);

		// 移動量を設定
		pBullet->SetMove(rVec, fMove);

		// 寿命の設定
		pBullet->SetLife(nLife);

		// Zテストを設定
		pBullet->SetFunc(D3DCMP_LESSEQUAL);

		// Zバッファの使用状況を設定
		pBullet->SetZEnable(true);

		// 確保したアドレスを返す
		return pBullet;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	ターゲットとの当たり判定
//============================================================
bool CBullet::CollisionTarget(void)
{
	// 変数を宣言
	bool bHit = false;	// 判定状況

	// ポインタを宣言
	CTarget *pTarget = CScene::GetTarget();	// ターゲット情報

	if (USED(pTarget))
	{ // ターゲットが使用されている場合

		// 変数を宣言
		D3DXVECTOR3 sizeBullet = GetScaling();

		// ターゲットとの当たり判定
		bHit = collision::Circle3D
		( // 引数
			GetPosition(),							// 弾位置
			pTarget->GetPosition(),					// ターゲット位置
			(sizeBullet.x + sizeBullet.y) * 0.5f,	// 弾半径
			pTarget->GetRadius()					// ターゲット半径
		);

		if (bHit)
		{ // 当たっていた場合

			// ターゲットのヒット処理
			pTarget->Hit(m_nDamage);
		}
	}

	// 判定状況を返す
	return bHit;
}

//============================================================
//	プレイヤーとの当たり判定
//============================================================
bool CBullet::CollisionPlayer(void)
{
	// 変数を宣言
	bool bHit = false;	// 判定状況

	// ポインタを宣言
	CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー情報

	if (pPlayer->GetState() != CPlayer::STATE_DEATH)
	{ // プレイヤーが使用されている場合

		// 変数を宣言
		D3DXVECTOR3 sizeBullet = GetScaling();

		// プレイヤーとの当たり判定
		bHit = collision::Circle3D
		( // 引数
			GetPosition(),							// 弾位置
			pPlayer->GetPosition(),					// プレイヤー位置
			(sizeBullet.x + sizeBullet.y) * 0.5f,	// 弾半径
			pPlayer->GetRadius()					// プレイヤー半径
		);

		if (bHit)
		{ // 当たっていた場合

			// プレイヤーのヒット処理
			pPlayer->Hit(m_nDamage);
		}
	}

	// 判定状況を返す
	return bHit;
}
