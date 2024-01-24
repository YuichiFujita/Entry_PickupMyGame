//============================================================
//
//	魔法マネージャー処理 [magicManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "magicManager.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "camera.h"
#include "sceneGame.h"
#include "object.h"
#include "lockCursor.h"
#include "objectGauge3D.h"
#include "multiModel.h"
#include "player.h"
#include "target.h"
#include "enemy.h"
#include "stage.h"
#include "collision.h"
#include "particle3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define MANA_FRAME	(10)	// マナ変動フレーム

#define GAUGE_PLUS_Y	(120.0f)	// ゲージY位置加算量
#define GAUGE_GAUGESIZE	(D3DXVECTOR3(110.0f, 15.0f, 0.0f))	// ゲージ大きさ
#define GAUGE_FRONTCOL	(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f))	// 表ゲージ色
#define GAUGE_BACKCOL	(D3DXCOLOR(0.0f, 0.0f, 0.5f, 1.0f))	// 裏ゲージ色

#define HEAL_CNT	(60)	// 回復状態に移行するまでのカウンター
#define NORMAL_CNT	(40)	// 通常状態に移行するまでのカウンター

#define HEALCNT_AREAMUL	(10)	// セーフエリア外での回復カウンター設定用係数
#define NUM_DEADZONE	(100)	// デッドゾーンの値

#define HEAL_EFF_CNT	(15)	// 回復時のエフェクト生成フレーム
#define HEAL_EFF_COL	(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f))	// 回復時のエフェクト色

//************************************************************
//	親クラス [CMagicManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CMagicManager::CMagicManager()
{
	// メンバ変数をクリア
	m_pMana = NULL;			// マナの情報
	m_state = STATE_NORMAL;	// 状態
	m_nCounterEffect = 0;	// エフェクト管理カウンター
	m_nCounterMagic = 0;	// 魔法管理カウンター
	m_nCounterState = 0;	// 状態管理カウンター
	m_nCounterHeal = 0;		// 回復管理カウンター
	m_bHeal = false;		// 回復状況
}

//============================================================
//	デストラクタ
//============================================================
CMagicManager::~CMagicManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CMagicManager::Init(void)
{
	// メンバ変数を初期化
	m_pMana = NULL;			// マナの情報
	m_state = STATE_NORMAL;	// 状態
	m_nCounterEffect = 0;	// エフェクト管理カウンター
	m_nCounterMagic = 0;	// 魔法管理カウンター
	m_nCounterState = 0;	// 状態管理カウンター
	m_nCounterHeal = 0;		// 回復管理カウンター
	m_bHeal = true;			// 回復状況

	// マナの生成
	m_pMana = CObjectGauge3D::Create
	( // 引数
		CObject::LABEL_GAUGE,			// オブジェクトラベル
		NULL,							// ゲージ表示オブジェクト
		0,								// 最大マナ
		MANA_FRAME,						// マナ変動フレーム
		GAUGE_PLUS_Y,					// 表示Y位置の加算量
		GAUGE_GAUGESIZE,				// ゲージ大きさ
		GAUGE_FRONTCOL,					// 表ゲージ色
		GAUGE_BACKCOL,					// 裏ゲージ色
		true,							// 枠描画状況
		CObjectGauge3D::TYPE_PLAYER,	// 枠種類
		GAUGE_GAUGESIZE					// 枠大きさ
	);
	if (UNUSED(m_pMana))
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
void CMagicManager::Uninit(void)
{

}

//============================================================
//	更新処理
//============================================================
void CMagicManager::Update(void)
{
	if (CScene::GetPlayer()->GetState() != CPlayer::STATE_NONE
	&&  CScene::GetPlayer()->GetState() != CPlayer::STATE_FADEIN
	&&  CScene::GetPlayer()->GetState() != CPlayer::STATE_FADEOUT)
	{ // プレイヤーが何もしない状態ではない場合

		if (m_nCounterMagic > 0)
		{ // カウンターが 0より大きい場合

			// カウンターを減算
			m_nCounterMagic--;
		}

		switch (m_state)
		{ // 状態ごとの処理
		case STATE_NORMAL:	// 通常状態

			if (m_nCounterState < HEAL_CNT)
			{ // カウンターが一定値より小さい場合

				if (CScene::GetStage()->GetAreaPlayer() == CStage::AREA_SAFE)
				{ // セーフエリアにいる場合

					// カウンターを加算
					m_nCounterState++;
				}
				else
				{ // セーフエリアにいない場合

					// カウンターを初期化
					m_nCounterState = 0;
				}
			}
			else
			{ // カウンターが一定値以上の場合

				// 状態管理カウンターを初期化
				m_nCounterState = 0;

				// エフェクト管理カウンターを設定
				m_nCounterEffect = HEAL_EFF_CNT;

				// 状態を変更
				m_state = STATE_HEAL;	// 回復状態
			}

			break;

		case STATE_ATTACK:	// 攻撃状態

			if (m_nCounterState < NORMAL_CNT)
			{ // カウンターが一定値より小さい場合

				// カウンターを加算
				m_nCounterState++;
			}
			else
			{ // カウンターが一定値以上の場合

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を変更
				m_state = STATE_NORMAL;	// 通常状態
			}

			break;

		case STATE_HEAL:	// 回復状態

			if (CScene::GetStage()->GetAreaPlayer() == CStage::AREA_SAFE)
			{ // セーフエリアにいる場合

				if (m_pMana->GetNum() < m_pMana->GetMaxNum())
				{ // マナが減っている場合

					if (m_bHeal)
					{ // 回復する状況の場合

						// マナを回復
						m_pMana->AddNum(1);

						if (m_nCounterEffect < HEAL_EFF_CNT)
						{ // カウンターが一定値より小さい場合

							// カウンターを加算
							m_nCounterEffect++;
						}
						else
						{ // カウンターが一定値以上の場合

							// カウンターを初期化
							m_nCounterEffect = 0;

							// パーティクル3Dオブジェクトを生成
							CParticle3D::Create(CParticle3D::TYPE_HEAL, CScene::GetPlayer()->GetPosition(), HEAL_EFF_COL);
						}
					}
				}
			}
			else
			{ // セーフエリアにいない場合

				// 状態を変更
				m_state = STATE_NORMAL;	// 通常状態
			}

			break;

		default:	// 例外処理
			assert(false);
			break;
		}
	}
}

//============================================================
//	魔法の発射処理
//============================================================
CMagicManager::Shot CMagicManager::ShotMagic(void)
{
	// 変数を宣言
	D3DXVECTOR3 rotCamera = CManager::GetCamera()->GetRotation();	// カメラ向き
	CMagic::StatusInfo status = CMagic::GetStatusInfo();			// 魔法ステータス
	int nNumShot = status.aLevel[CMagic::GetLevelStatus(CMagic::LEVELINFO_NUM)].nNumBunnle;	// バブル発射数

	Shot bShot = { 0.0f, false, false };	// 射撃状況
	float fVecRot = 0.0f;	// 射撃方向
	float fAddRot = 0.0f;	// 射撃加算向き

	if (nNumShot % 2 == 0)
	{ // 発射数が偶数の場合

		// 加算向きを減算
		fAddRot -= status.fShotAngle * 0.5f;
		useful::NormalizeRot(fAddRot);	// 向き正規化
	}

	for (int nCntMagic = 0; nCntMagic < (int)(nNumShot * 0.5f); nCntMagic++)
	{ // 発射数の半分繰り返す

		// 加算向きを加算
		fAddRot += status.fShotAngle;
		useful::NormalizeRot(fAddRot);	// 向き正規化
	}

	// ポインタを宣言
	CInputKeyboard	*pKeyboard	= CManager::GetKeyboard();	// キーボード
	CInputPad		*pPad		= CManager::GetPad();		// パッド

	// 発射方向の設定
	if (pKeyboard->GetPress(DIK_I))
	{
		// 発射操作をした状態にする
		bShot.bControl = true;

		if (pKeyboard->GetPress(DIK_J))
		{
			// 発射向きを保存
			bShot.fRot = D3DXToRadian(135);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(135) + rotCamera.y;
		}
		else if (pKeyboard->GetPress(DIK_L))
		{
			// 発射向きを設定
			bShot.fRot = D3DXToRadian(225);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(225) + rotCamera.y;
		}
		else
		{
			// 発射向きを設定
			bShot.fRot = D3DXToRadian(180);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(180) + rotCamera.y;
		}
	}
	else if (pKeyboard->GetPress(DIK_K))
	{
		// 発射操作をした状態にする
		bShot.bControl = true;

		if (pKeyboard->GetPress(DIK_J))
		{
			// 発射向きを設定
			bShot.fRot = D3DXToRadian(45);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(45) + rotCamera.y;
		}
		else if (pKeyboard->GetPress(DIK_L))
		{
			// 発射向きを設定
			bShot.fRot = D3DXToRadian(315);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(315) + rotCamera.y;
		}
		else
		{
			// 発射向きを設定
			bShot.fRot = D3DXToRadian(0);

			// 射撃方向を設定
			fVecRot = D3DXToRadian(0) + rotCamera.y;
		}
	}
	else if (pKeyboard->GetPress(DIK_J))
	{
		// 発射操作をした状態にする
		bShot.bControl = true;

		// 発射向きを設定
		bShot.fRot = D3DXToRadian(90);

		// 射撃方向を設定
		fVecRot = D3DXToRadian(90) + rotCamera.y;
	}
	else if (pKeyboard->GetPress(DIK_L))
	{
		// 発射操作をした状態にする
		bShot.bControl = true;

		// 発射向きを設定
		bShot.fRot = D3DXToRadian(270);

		// 射撃方向を設定
		fVecRot = D3DXToRadian(270) + rotCamera.y;
	}
	else if (pPad->GetPressRStickX() >  NUM_DEADZONE
	||		 pPad->GetPressRStickX() < -NUM_DEADZONE
	||		 pPad->GetPressRStickY() >  NUM_DEADZONE
	||		 pPad->GetPressRStickY() < -NUM_DEADZONE)
	{
		// 発射操作をした状態にする
		bShot.bControl = true;

		// 発射向きを設定
		bShot.fRot = pPad->GetPressRStickRot() + D3DXToRadian(270);

		// 射撃方向を設定
		fVecRot = pPad->GetPressRStickRot() + D3DXToRadian(270) + rotCamera.y;
	}

	// 向きを補正
	useful::NormalizeRot(bShot.fRot);

	// 射撃方向に加算向きを加算
	fVecRot += fAddRot;
	useful::NormalizeRot(fVecRot);	// 向き正規化

	if (bShot.bControl)
	{ // 魔法の発射操作していた場合

		if (m_nCounterMagic <= 0 && CScene::GetTarget()->GetState() != CTarget::STATE_DESTROY)
		{ // クールタイムが終了している且つ、ターゲットが破壊されていない場合

			// ポインタを宣言
			CPlayer *pPlayer = CScene::GetPlayer();	// プレイヤー

			// 変数を宣言
			D3DXVECTOR3 magicPos, vecMove;	// 発射位置・移動方向
			D3DXMATRIX  mtxTrans, mtxWorld;	// 計算用マトリックス

			for (int nCntMagic = 0; nCntMagic < nNumShot; nCntMagic++)
			{ // 発射数分繰り返す

				if (m_pMana->GetNum() > 0)
				{ // マナがある場合

					// ワールドマトリックスの初期化
					D3DXMatrixIdentity(&mtxWorld);

					// 位置を反映
					D3DXMatrixTranslation(&mtxTrans, status.shotPos.x, status.shotPos.y, status.shotPos.z);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

					// マトリックスを掛け合わせる
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &pPlayer->GetMultiModel(status.nShotParts)->GetMtxWorld());

					// マトリックスから発射位置を求める
					magicPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

					// 移動方向を設定
					vecMove = D3DXVECTOR3(sinf(fVecRot), 0.0f, cosf(fVecRot));

					// 魔法オブジェクトの生成
					CMagic::Create
					( // 引数
						magicPos,	// 位置
						VEC3_ZERO,	// 向き
						vecMove		// 移動方向
					);

					// マナを消費
					m_pMana->AddNum(-1);

					// 向きを減算
					fVecRot -= status.fShotAngle;
					useful::NormalizeRot(fVecRot);	// 向き正規化
				}

				// カウンターを設定
				m_nCounterMagic = status.aLevel[CMagic::GetLevelStatus(CMagic::LEVELINFO_RAPID)].nCoolTime;

				// カウンターを初期化
				m_nCounterState = 0;

				// 状態を設定
				m_state = STATE_ATTACK;	// 攻撃状態

				// 発射した状態にする
				bShot.bShot = true;
			}

			// サウンドの再生
			CManager::GetSound()->Play(CSound::LABEL_SE_PLAYER_SHOT);	// プレイヤー射撃音 (泡)
		}
	}

	// 射撃状況を返す
	return bShot;
}

//============================================================
//	マナ回復状況の設定処理
//============================================================
void CMagicManager::SetEnableHealMana(const bool bHeal)
{
	// 引数の回復状況をマナの回復状況に設定
	m_bHeal = bHeal;
}

//============================================================
//	マナ描画状況の設定処理
//============================================================
void CMagicManager::SetEnableManaDraw(const bool bDraw)
{
	// 引数の描画状況をマナの描画状況に設定
	m_pMana->SetEnableDraw(bDraw);
}

//============================================================
//	マナの加算処理
//============================================================
void CMagicManager::AddMana(const int nAdd)
{
	// マナを加算
	m_pMana->AddNum(nAdd);
}

//============================================================
//	マナの設定処理
//============================================================
void CMagicManager::SetMana(const int nMana)
{
	// マナを設定
	m_pMana->SetNum(nMana);
}

//============================================================
//	最大マナの設定処理
//============================================================
void CMagicManager::SetMaxMana(const int nMana)
{
	// 引数の最大マナを設定
	m_pMana->SetMaxNum(nMana);
}

//============================================================
//	マナ残量取得処理
//============================================================
int CMagicManager::GetMana(void) const
{
	// マナの残量を返す
	return m_pMana->GetNum();
}

//============================================================
//	最大マナ取得処理
//============================================================
int CMagicManager::GetMaxMana(void) const
{
	// 最大マナを返す
	return m_pMana->GetMaxNum();
}

//============================================================
//	生成処理
//============================================================
CMagicManager *CMagicManager::Create(CObject *pPlayer, const int nMaxMana)
{
	// ポインタを宣言
	CMagicManager *pMagicManager = NULL;	// 魔法マネージャー生成用

	if (UNUSED(pMagicManager))
	{ // 使用されていない場合

		// メモリ確保
		pMagicManager = new CMagicManager;	// 魔法マネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pMagicManager))
	{ // 使用されている場合
		
		// 魔法マネージャーの初期化
		if (FAILED(pMagicManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pMagicManager;
			pMagicManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// マナの最大値を設定
		pMagicManager->m_pMana->SetMaxNum(nMaxMana);

		// マナ表示オブジェクトを設定
		pMagicManager->m_pMana->SetGaugeObject(pPlayer);

		// 確保したアドレスを返す
		return pMagicManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CMagicManager::Release(CMagicManager *&prMagicManager)
{
	if (USED(prMagicManager))
	{ // 使用中の場合

		// 魔法マネージャーの終了
		prMagicManager->Uninit();

		// メモリ開放
		delete prMagicManager;
		prMagicManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}
