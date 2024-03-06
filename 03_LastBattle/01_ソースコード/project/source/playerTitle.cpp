//============================================================
//
//	プレイヤータイトル処理 [playerTitle.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "playerTitle.h"
#include "manager.h"
#include "camera.h"
#include "sceneTitle.h"
#include "titleManager.h"
#include "renderer.h"
#include "sound.h"
#include "stage.h"
#include "sword.h"
#include "shadow.h"
#include "anim2D.h"
#include "effect3D.h"
#include "particle3D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* TEXTURE_FILE = "data\\TEXTURE\\slash000.png";	// 斬撃テクスチャ
	const D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 800.0f, 0.0f);	// プレイヤー生成位置

	// 斬撃情報
	namespace slash
	{
		const D3DXVECTOR3 POS	= SCREEN_CENT + D3DXVECTOR3(0.0f, -90.0f, 0.0f);	// 位置
		const D3DXVECTOR3 SIZE	= SCREEN_SIZE;		// 大きさ
		const POSGRID2	  PART	= POSGRID2(6, 6);	// テクスチャ分割数
	}
}

//************************************************************
//	子クラス [CPlayerTitle] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CPlayerTitle::CPlayerTitle() :
	m_pSlash	(nullptr),	// 斬撃の情報
	m_stag		(STAG_NONE)	// 演出状態
{

}

//============================================================
//	デストラクタ
//============================================================
CPlayerTitle::~CPlayerTitle()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPlayerTitle::Init(void)
{
	// メンバ変数を初期化
	m_pSlash = nullptr;	// 斬撃の情報
	m_stag = STAG_FALL;	// 演出状態

	// プレイヤーの初期化
	if (FAILED(CPlayer::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 斬撃の生成
	m_pSlash = CAnim2D::Create
	( // 引数
		slash::PART.x,	// テクスチャの横の分割数
		slash::PART.y,	// テクスチャの縦の分割数
		slash::POS,		// 位置
		slash::SIZE		// 大きさ
	);
	if (m_pSlash == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	m_pSlash->BindTexture(TEXTURE_FILE);

	// ラベルをエフェクトにする
	m_pSlash->SetLabel(LABEL_EFFECT);

	// 変更フレームを設定
	m_pSlash->SetCounter(1);

	// 再生を停止させる
	m_pSlash->SetEnableStop(true);

	// 加算合成にする
	m_pSlash->GetRenderState()->SetAlphaBlend(CRenderState::BLEND_ADD);

	// 位置を設定
	SetVec3Position(POS);

	// 落下モーションを設定
	SetMotion(BODY_LOWER, L_MOTION_FALL);
	SetMotion(BODY_UPPER, U_MOTION_FALL);

	// UI自動描画をOFFにする
	SetEnableDrawUI(false);

	// 自動更新・自動描画をONにする
	SetEnableUpdate(true);
	SetEnableDraw(true);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CPlayerTitle::Uninit(void)
{
	// プレイヤーの終了
	CPlayer::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CPlayerTitle::Update(void)
{
	// オブジェクト分割キャラクターの更新
	CObjectDivChara::Update();

	// 過去位置の更新
	UpdateOldPosition();

	switch (m_stag)
	{ // 演出状態ごとの処理
	case STAG_NONE:
		break;

	case STAG_FALL:

		// 落下演出の更新
		UpdateStagFall();
		break;

	case STAG_LAND:

		// 着地演出の更新
		UpdateStagLand();
		break;

	case STAG_IDOL:

		// 待機演出の更新
		UpdateStagIdol();
		break;

	case STAG_ATTACK:

		// 攻撃演出の更新
		UpdateStagAttack();
		break;

	default:
		assert(false);
		break;
	}

	bool bColl[] =	// 判定ON/OFF状況
	{
		IsLeftWeaponCollision(BODY_UPPER),	// 左手の武器の攻撃判定
		IsRightWeaponCollision(BODY_UPPER),	// 右手の武器の攻撃判定
	};
	for (int nCntSword = 0; nCntSword < player::NUM_SWORD; nCntSword++)
	{ // 剣の数分繰り返す

		CSword *pSword = GetSword(nCntSword);	// 剣の情報

		// 剣の攻撃判定を設定
		pSword->SetEnableAttack(bColl[nCntSword]);

		// 剣の更新
		pSword->Update();
	}

	// 影の更新
	GetShadow()->Update();
}

//============================================================
//	描画処理
//============================================================
void CPlayerTitle::Draw(CShader * /*pShader*/)
{
	// プレイヤーの描画
	CPlayer::Draw();
}

//============================================================
//	落下演出の更新処理
//============================================================
void CPlayerTitle::UpdateStagFall(void)
{
	// 変数を宣言
	D3DXVECTOR3 posPlayer = GetVec3Position();	// プレイヤー位置
	D3DXVECTOR3 rotPlayer = GetVec3Rotation();	// プレイヤー向き

	// ポインタを宣言
	CStage *pStage = CScene::GetStage();				// ステージ情報
	if (pStage == nullptr) { assert(false); return; }	// ステージ非使用中

	// 重力の更新
	UpdateGravity();

	// 位置更新
	UpdatePosition(&posPlayer);

	// 着地判定
	if (UpdateLanding(&posPlayer))
	{ // 着地した場合

		// 着地モーションを設定
		SetMotion(BODY_LOWER, L_MOTION_LAND);
		SetMotion(BODY_UPPER, U_MOTION_LAND);

		// 着地演出にする
		m_stag = STAG_LAND;
	}

	// 向き更新
	UpdateRotation(&rotPlayer);

	// ステージ範囲外の補正
	pStage->LimitPosition(posPlayer, GetRadius());

	// 位置を反映
	SetVec3Position(posPlayer);

	// 向きを反映
	SetVec3Rotation(rotPlayer);
}

//============================================================
//	着地演出の更新処理
//============================================================
void CPlayerTitle::UpdateStagLand(void)
{
	if (IsMotionFinish(BODY_LOWER)
	&&  IsMotionFinish(BODY_UPPER))
	{ // 着地モーションが終了した場合

		// 待機モーションを設定
		SetMotion(BODY_LOWER, L_MOTION_IDOL);
		SetMotion(BODY_UPPER, U_MOTION_IDOL);

		// 待機演出にする
		m_stag = STAG_IDOL;
	}
}

//============================================================
//	待機演出の更新処理
//============================================================
void CPlayerTitle::UpdateStagIdol(void)
{
	CCamera *pCamera = GET_MANAGER->GetCamera();	// カメラ情報
	if (pCamera->GetLook().nCounterForce == 0
	&&  pCamera->GetState() == CCamera::STATE_TITLE_ATK)
	{ // カメラの最終目標になっている場合

		// タイトル攻撃モーションを設定
		SetMotion(BODY_LOWER, L_MOTION_TITLE_ATTACK);
		SetMotion(BODY_UPPER, U_MOTION_TITLE_ATTACK);

		// 攻撃演出にする
		m_stag = STAG_ATTACK;
	}
}

//============================================================
//	攻撃演出の更新処理
//============================================================
void CPlayerTitle::UpdateStagAttack(void)
{
	if (IsMotionFinish(BODY_LOWER))
	{ // モーションが終了した場合

		// 次のモードに遷移
		CSceneTitle::GetTitleManager()->Transition();
	}

	if (GetMotionKey(BODY_LOWER) == 0 && GetMotionKeyCounter(BODY_LOWER) == 1)
	{ // モーションが始まった場合

		// 斬撃を再生
		m_pSlash->SetEnableStop(false);
	}

	if (m_pSlash->GetLoopAnimation() >= 1)
	{ // 一度再生し終わった場合

		// 斬撃を停止
		m_pSlash->SetEnableStop(true);
	}
}
