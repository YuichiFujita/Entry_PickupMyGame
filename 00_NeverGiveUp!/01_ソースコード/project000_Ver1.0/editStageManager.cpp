//============================================================
//
//	エディットステージマネージャー処理 [editStageManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editStageManager.h"
#include "manager.h"
#include "input.h"
#include "camera.h"
#include "editBuilding.h"
#include "editWindow.h"
#include "editSignboard.h"
#include "editObstacle.h"
#include "editSavePoint.h"
#include "editGoalPoint.h"

//************************************************************
//	マクロ定義
//************************************************************
#define KEY_DOUBLE		(DIK_LCONTROL)	// 二重化キー
#define NAME_DOUBLE		("LCTRL")		// 二重化表示
#define KEY_TRIGGER		(DIK_LSHIFT)	// トリガー化キー
#define NAME_TRIGGER	("LSHIFT")		// トリガー化表示

#define KEY_SAVE	(DIK_F9)	// 保存キー
#define NAME_SAVE	("F9")		// 保存表示

#define KEY_CHANGE_THING	(DIK_1)	// 配置物変更キー
#define NAME_CHANGE_THING	("1")	// 配置物変更表示

#define KEY_CAMERA	(DIK_1)	// カメラ位置変更キー
#define NAME_CAMERA	("1")	// カメラ位置変更表示

#define KEY_MOVE_UP		(DIK_UP)	// 移動量上昇キー
#define NAME_MOVE_UP	("↑")		// 移動量上昇表示
#define KEY_MOVE_DOWN	(DIK_DOWN)	// 移動量下降キー
#define NAME_MOVE_DOWN	("↓")		// 移動量下降表示

#define KEY_FAR		(DIK_W)	// 奥移動キー
#define NAME_FAR	("W")	// 奥移動表示
#define KEY_NEAR	(DIK_S)	// 手前移動キー
#define NAME_NEAR	("S")	// 手前移動表示
#define KEY_RIGHT	(DIK_D)	// 右移動キー
#define NAME_RIGHT	("D")	// 右移動表示
#define KEY_LEFT	(DIK_A)	// 左移動キー
#define NAME_LEFT	("A")	// 左移動表示
#define KEY_UP		(DIK_E)	// 上移動キー
#define NAME_UP		("E")	// 上移動表示
#define KEY_DOWN	(DIK_Q)	// 下移動キー
#define NAME_DOWN	("Q")	// 下移動表示

#define KEY_ROTA_RIGHT	(DIK_Z)	// 右回転キー
#define NAME_ROTA_RIGHT	("Z")	// 右回転表示
#define KEY_ROTA_LEFT	(DIK_C)	// 左回転キー
#define NAME_ROTA_LEFT	("C")	// 左回転表示

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char* SAVE_TXT	= "data\\TXT\\save_stage.txt";	// ステージセーブテキスト
	const float INIT_MOVE	= 40.0f;	// 配置物の初期移動量
	const float CHANGE_MOVE = 10.0f;	// 配置物の移動量の変動量
	const float MIN_MOVE	= 10.0f;	// 配置物の最小移動量
	const float MAX_MOVE	= 200.0f;	// 配置物の最大移動量
}

//************************************************************
//	親クラス [CEditStageManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditStageManager::CEditStageManager()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pBuilding		= NULL;		// エディットビルの情報
	m_pWindow		= NULL;		// エディット窓の情報
	m_pSignboard	= NULL;		// エディット看板の情報
	m_pObstacle		= NULL;		// エディット障害物の情報
	m_pSavePoint	= NULL;		// エディットセーブポイントの情報
	m_pGoalPoint	= NULL;		// エディットゴールポイントの情報
	m_pos	= VEC3_ZERO;		// 位置
	m_rot	= VEC3_ZERO;		// 向き
	m_fMove	= 0.0f;				// 位置移動量
	m_thing	= THING_BUILDING;	// 配置物
	m_bSave	= false;			// 保存状況
	m_bEdit	= false;			// エディット状況

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditStageManager::~CEditStageManager()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditStageManager::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pBuilding		= NULL;		// エディットビルの情報
	m_pWindow		= NULL;		// エディット窓の情報
	m_pSignboard	= NULL;		// エディット看板の情報
	m_pObstacle		= NULL;		// エディット障害物の情報
	m_pSavePoint	= NULL;		// エディットセーブポイントの情報
	m_pGoalPoint	= NULL;		// エディットゴールポイントの情報
	m_pos	= VEC3_ZERO;		// 位置
	m_rot	= VEC3_ZERO;		// 向き
	m_fMove	= INIT_MOVE;		// 位置移動量
	m_thing	= THING_BUILDING;	// 配置物
	m_bSave = false;			// 保存状況
	m_bEdit	= false;			// エディット状況

#if 1	// TODO：障害物検証用の初期化

	m_pos = D3DXVECTOR3(-2880.0f, 1120.0f, -4000.0f);		// 位置
	m_thing = THING_OBSTACLE;	// 配置物

#endif

	// エディットビルの生成
	m_pBuilding = CEditBuilding::Create(this);
	if (m_pBuilding == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// エディット窓の生成
	m_pWindow = CEditWindow::Create(this);
	if (m_pWindow == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// エディット看板の生成
	m_pSignboard = CEditSignboard::Create(this);
	if (m_pSignboard == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// エディット障害物の生成
	m_pObstacle = CEditObstacle::Create(this);
	if (m_pObstacle == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// エディットセーブポイントの生成
	m_pSavePoint = CEditSavePoint::Create(this);
	if (m_pSavePoint == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// エディットゴールポイントの生成
	m_pGoalPoint = CEditGoalPoint::Create(this);
	if (m_pGoalPoint == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 成功を返す
	return S_OK;

#else	// NDEBUG

	// 成功を返す
	return S_OK;

#endif	// _DEBUG
}

//============================================================
//	終了処理
//============================================================
void CEditStageManager::Uninit(void)
{
#if _DEBUG

	if (m_pBuilding != NULL)
	{ // エディットビルが使用されている場合

		// エディットビルの破棄
		CEditBuilding::Release(m_pBuilding);
	}
	else { assert(false); }	// 非使用中

	if (m_pWindow != NULL)
	{ // エディット窓が使用されている場合

		// エディット窓の破棄
		CEditWindow::Release(m_pWindow);
	}
	else { assert(false); }	// 非使用中

	if (m_pSignboard != NULL)
	{ // エディット看板が使用されている場合

		// エディット看板の破棄
		CEditSignboard::Release(m_pSignboard);
	}
	else { assert(false); }	// 非使用中

	if (m_pObstacle != NULL)
	{ // エディット障害物が使用されている場合

		// エディット障害物の破棄
		CEditObstacle::Release(m_pObstacle);
	}
	else { assert(false); }	// 非使用中

	if (m_pSavePoint != NULL)
	{ // エディットセーブポイントが使用されている場合

		// エディットセーブポイントの破棄
		CEditSavePoint::Release(m_pSavePoint);
	}
	else { assert(false); }	// 非使用中

	if (m_pGoalPoint != NULL)
	{ // エディットゴールポイントが使用されている場合

		// エディットゴールポイントの破棄
		CEditGoalPoint::Release(m_pGoalPoint);
	}
	else { assert(false); }	// 非使用中

#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditStageManager::Update(void)
{
#if _DEBUG

	if (!m_bEdit)
	{ // エディットモードではない場合

		// 処理を抜ける
		return;
	}

	// 配置物変更の更新
	UpdateChangeThing();

	// 移動量変更の更新
	UpdateChangeMove();

	// 位置の更新
	UpdatePosition();

	// 向きの更新
	UpdateRotation();

	// カメラ位置変更の更新
	UpdatePosCamera();

	switch (m_thing)
	{ // 配置物ごとの処理
	case THING_BUILDING:	// ビル

		if (m_pBuilding != NULL)
		{ // エディットビルが使用されている場合

			// エディットビルの更新
			m_pBuilding->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_WINDOW:		// 窓

		if (m_pWindow != NULL)
		{ // エディット窓が使用されている場合

			// エディット窓の更新
			m_pWindow->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SIGNBOARD:	// 看板

		if (m_pSignboard != NULL)
		{ // エディット看板が使用されている場合

			// エディット看板の更新
			m_pSignboard->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_OBSTACLE:	// 障害物

		if (m_pObstacle != NULL)
		{ // エディット障害物が使用されている場合

			// エディット障害物の更新
			m_pObstacle->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SAVEPOINT:	// セーブポイント

		if (m_pSavePoint != NULL)
		{ // エディットセーブポイントが使用されている場合

			// エディットセーブポイントの更新
			m_pSavePoint->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_GOALPOINT:	// ゴールポイント

		if (m_pGoalPoint != NULL)
		{ // エディットゴールポイントが使用されている場合

			// エディットゴールポイントの更新
			m_pGoalPoint->Update();
		}
		else { assert(false); }	// 非使用中

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// ステージ保存
	SaveStage();

	// 操作表示の描画
	DrawDebugControl();

	// 情報表示の描画
	DrawDebugInfo();

#endif	// _DEBUG
}

//============================================================
//	未保存の設定処理
//============================================================
void CEditStageManager::UnSave(void)
{
	// 保存していない状況にする
	m_bSave = false;
}

//============================================================
//	エディット状況の設定処理
//============================================================
void CEditStageManager::SetEnableEdit(const bool bEdit)
{
	// 引数のエディット状況にする
	m_bEdit = bEdit;

	switch (m_thing)
	{ // 配置物ごとの処理
	case THING_BUILDING:	// ビル

		if (m_pBuilding != NULL)
		{ // エディットビルが使用されている場合

			// エディットビルの表示の設定
			m_pBuilding->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_WINDOW:		// 窓

		if (m_pWindow != NULL)
		{ // エディット窓が使用されている場合

			// エディット窓の表示の設定
			m_pWindow->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SIGNBOARD:	// 看板

		if (m_pSignboard != NULL)
		{ // エディット看板が使用されている場合

			// エディット看板の表示の設定
			m_pSignboard->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_OBSTACLE:	// 障害物

		if (m_pObstacle != NULL)
		{ // エディット障害物が使用されている場合

			// エディット障害物の表示の設定
			m_pObstacle->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SAVEPOINT:	// セーブポイント

		if (m_pSavePoint != NULL)
		{ // エディットセーブポイントが使用されている場合

			// エディットセーブポイントの表示の設定
			m_pSavePoint->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_GOALPOINT:	// ゴールポイント

		if (m_pGoalPoint != NULL)
		{ // エディットゴールポイントが使用されている場合

			// エディットゴールポイントの表示の設定
			m_pGoalPoint->SetDisp(m_bEdit);
		}
		else { assert(false); }	// 非使用中

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	エディット状況取得処理
//============================================================
bool CEditStageManager::IsEdit(void) const
{
	// エディット状況を返す
	return m_bEdit;
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CEditStageManager::GetVec3Position(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	向き取得処理
//============================================================
D3DXVECTOR3 CEditStageManager::GetVec3Rotation(void) const
{
	// 向きを返す
	return m_rot;
}

//============================================================
//	生成処理
//============================================================
CEditStageManager *CEditStageManager::Create(void)
{
#if _DEBUG

	// ポインタを宣言
	CEditStageManager *pEditStageManager = NULL;	// エディットステージマネージャー生成用

	if (pEditStageManager == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditStageManager = new CEditStageManager;	// エディットステージマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditStageManager != NULL)
	{ // 使用されている場合
		
		// エディットステージマネージャーの初期化
		if (FAILED(pEditStageManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditStageManager;
			pEditStageManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pEditStageManager;
	}
	else { assert(false); return NULL; }	// 確保失敗

#else	// NDEBUG

	// NULLを返す
	return NULL;

#endif	// _DEBUG
}

//============================================================
//	破棄処理
//============================================================
HRESULT CEditStageManager::Release(CEditStageManager *&prEditStageManager)
{
#if _DEBUG

	if (prEditStageManager != NULL)
	{ // 使用中の場合

		// エディットステージマネージャーの終了
		prEditStageManager->Uninit();

		// メモリ開放
		delete prEditStageManager;
		prEditStageManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中

#else	// NDEBUG

	// 成功を返す
	return S_OK;

#endif	// _DEBUG
}

//============================================================
//	配置物変更の更新処理
//============================================================
void CEditStageManager::UpdateChangeThing(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 配置物を変更
	if (!m_pKeyboard->IsPress(KEY_DOUBLE))
	{
		if (m_pKeyboard->IsTrigger(KEY_CHANGE_THING))
		{
			switch (m_thing)
			{ // 配置物ごとの処理
			case THING_BUILDING:	// ビル

				if (m_pBuilding != NULL)
				{ // エディットビルが使用されている場合

					// エディットビルの表示の設定
					m_pBuilding->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_WINDOW:		// 窓

				if (m_pWindow != NULL)
				{ // エディット窓が使用されている場合

					// エディット窓の表示の設定
					m_pWindow->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_SIGNBOARD:	// 看板

				if (m_pSignboard != NULL)
				{ // エディット看板が使用されている場合

					// エディット看板の表示の設定
					m_pSignboard->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_OBSTACLE:	// 障害物

				if (m_pObstacle != NULL)
				{ // エディット障害物が使用されている場合

					// エディット障害物の表示の設定
					m_pObstacle->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_SAVEPOINT:	// セーブポイント

				if (m_pSavePoint != NULL)
				{ // エディットセーブポイントが使用されている場合

					// エディットセーブポイントの表示の設定
					m_pSavePoint->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_GOALPOINT:	// ゴールポイント

				if (m_pGoalPoint != NULL)
				{ // エディットゴールポイントが使用されている場合

					// エディットゴールポイントの表示の設定
					m_pGoalPoint->SetDisp(false);
				}
				else { assert(false); }	// 非使用中

				break;

			default:	// 例外処理
				assert(false);
				break;
			}

			// 配置物の変更
			m_thing = (EThing)((m_thing + 1) % THING_MAX);

			switch (m_thing)
			{ // 配置物ごとの処理
			case THING_BUILDING:	// ビル

				if (m_pBuilding != NULL)
				{ // エディットビルが使用されている場合

					// エディットビルの表示の設定
					m_pBuilding->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_WINDOW:		// 窓

				if (m_pWindow != NULL)
				{ // エディット窓が使用されている場合

					// エディット窓の表示の設定
					m_pWindow->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_SIGNBOARD:	// 看板

				if (m_pSignboard != NULL)
				{ // エディット看板が使用されている場合

					// エディット看板の表示の設定
					m_pSignboard->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_OBSTACLE:	// 障害物

				if (m_pObstacle != NULL)
				{ // エディット障害物が使用されている場合

					// エディット障害物の表示の設定
					m_pObstacle->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_SAVEPOINT:	// セーブポイント

				if (m_pSavePoint != NULL)
				{ // エディットセーブポイントが使用されている場合

					// エディットセーブポイントの表示の設定
					m_pSavePoint->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			case THING_GOALPOINT:	// ゴールポイント

				if (m_pGoalPoint != NULL)
				{ // エディットゴールポイントが使用されている場合

					// エディットゴールポイントの表示の設定
					m_pGoalPoint->SetDisp(true);
				}
				else { assert(false); }	// 非使用中

				break;

			default:	// 例外処理
				assert(false);
				break;
			}
		}
	}
}

//============================================================
//	移動量変更の更新処理
//============================================================
void CEditStageManager::UpdateChangeMove(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 移動量を変更
	if (!m_pKeyboard->IsPress(KEY_TRIGGER))
	{
		if (m_pKeyboard->IsPress(KEY_MOVE_UP))
		{
			m_fMove += CHANGE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_MOVE_DOWN))
		{
			m_fMove -= CHANGE_MOVE;
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_MOVE_UP))
		{
			m_fMove += CHANGE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_MOVE_DOWN))
		{
			m_fMove -= CHANGE_MOVE;
		}
	}

	// 移動量を補正
	useful::LimitNum(m_fMove, MIN_MOVE, MAX_MOVE);
}

//============================================================
//	位置の更新処理
//============================================================
void CEditStageManager::UpdatePosition(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 位置を変更
	if (!m_pKeyboard->IsPress(KEY_TRIGGER))
	{
		if (m_pKeyboard->IsPress(KEY_FAR))
		{
			m_pos.z += m_fMove;
		}
		if (m_pKeyboard->IsPress(KEY_NEAR))
		{
			m_pos.z -= m_fMove;
		}
		if (m_pKeyboard->IsPress(KEY_RIGHT))
		{
			m_pos.x += m_fMove;
		}
		if (m_pKeyboard->IsPress(KEY_LEFT))
		{
			m_pos.x -= m_fMove;
		}
		if (m_pKeyboard->IsPress(KEY_UP))
		{
			m_pos.y += m_fMove;
		}
		if (m_pKeyboard->IsPress(KEY_DOWN))
		{
			m_pos.y -= m_fMove;
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_FAR))
		{
			m_pos.z += m_fMove;
		}
		if (m_pKeyboard->IsTrigger(KEY_NEAR))
		{
			m_pos.z -= m_fMove;
		}
		if (m_pKeyboard->IsTrigger(KEY_RIGHT))
		{
			m_pos.x += m_fMove;
		}
		if (m_pKeyboard->IsTrigger(KEY_LEFT))
		{
			m_pos.x -= m_fMove;
		}
		if (m_pKeyboard->IsTrigger(KEY_UP))
		{
			m_pos.y += m_fMove;
		}
		if (m_pKeyboard->IsTrigger(KEY_DOWN))
		{
			m_pos.y -= m_fMove;
		}
	}
}

//============================================================
//	向きの更新処理
//============================================================
void CEditStageManager::UpdateRotation(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 向きを変更
	if (m_pKeyboard->IsTrigger(KEY_ROTA_RIGHT))
	{
		m_rot.y += HALF_PI;
	}
	if (m_pKeyboard->IsTrigger(KEY_ROTA_LEFT))
	{
		m_rot.y -= HALF_PI;
	}

	// 向きを正規化
	useful::Vec3NormalizeRot(m_rot);
}

//============================================================
//	カメラ位置変更の更新処理
//============================================================
void CEditStageManager::UpdatePosCamera(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// カメラ位置を変更
	if (m_pKeyboard->IsPress(KEY_DOUBLE))
	{
		if (m_pKeyboard->IsTrigger(KEY_CAMERA))
		{
			// ポインタを宣言
			CCamera *pCamera = CManager::GetInstance()->GetCamera();	// カメラ情報

#if _DEBUG

			// 配置物の位置に注視点を設定
			pCamera->SetThingLook();

#endif	// _DEBUG

		}
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditStageManager::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "======================================\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "[エディット操作]　\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "======================================\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "ステージ保存：[%s+%s]\n", NAME_DOUBLE, NAME_SAVE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "移動：[%s/%s/%s/%s/%s/%s+%s]\n", NAME_FAR, NAME_LEFT, NAME_NEAR, NAME_RIGHT, NAME_UP, NAME_DOWN, NAME_TRIGGER);
	pDebug->Print(CDebugProc::POINT_RIGHT, "移動量変更：[%s/%s]\n", NAME_MOVE_UP, NAME_MOVE_DOWN);
	pDebug->Print(CDebugProc::POINT_RIGHT, "回転：[%s/%s]\n", NAME_ROTA_RIGHT, NAME_ROTA_LEFT);
	pDebug->Print(CDebugProc::POINT_RIGHT, "配置物カメラ：[%s+%s]\n", NAME_DOUBLE, NAME_CAMERA);
	pDebug->Print(CDebugProc::POINT_RIGHT, "配置物変更：[%s]\n", NAME_CHANGE_THING);

	switch (m_thing)
	{ // 配置物ごとの処理
	case THING_BUILDING:	// ビル

		if (m_pBuilding != NULL)
		{ // エディットビルが使用されている場合

			// エディットビルの操作表示
			m_pBuilding->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_WINDOW:		// 窓

		if (m_pWindow != NULL)
		{ // エディット窓が使用されている場合

			// エディット窓の操作表示
			m_pWindow->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SIGNBOARD:	// 看板

		if (m_pSignboard != NULL)
		{ // エディット看板が使用されている場合

			// エディット看板の操作表示
			m_pSignboard->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_OBSTACLE:	// 障害物

		if (m_pObstacle != NULL)
		{ // エディット障害物が使用されている場合

			// エディット障害物の操作表示
			m_pObstacle->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SAVEPOINT:	// セーブポイント

		if (m_pSavePoint != NULL)
		{ // エディットセーブポイントが使用されている場合

			// エディットセーブポイントの操作表示
			m_pSavePoint->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_GOALPOINT:	// ゴールポイント

		if (m_pGoalPoint != NULL)
		{ // エディットゴールポイントが使用されている場合

			// エディットゴールポイントの操作表示
			m_pGoalPoint->DrawDebugControl();
		}
		else { assert(false); }	// 非使用中

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditStageManager::DrawDebugInfo(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報
	static char* apThing[] = { "ビル", "窓", "看板", "障害物", "セーブポイント", "ゴールポイント" };	// 配置物

	// 配置物数の不一致
	assert((sizeof(apThing) / sizeof(apThing[0])) == THING_MAX);

	pDebug->Print(CDebugProc::POINT_RIGHT, "======================================\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "[エディット情報]　\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "======================================\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, (m_bSave) ? "保存済：[保存状況]\n" : "未保存：[保存状況]\n");
	pDebug->Print(CDebugProc::POINT_RIGHT, "%s：[配置物]\n", apThing[m_thing]);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f %f %f：[位置]\n", m_pos.x, m_pos.y, m_pos.z);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f %f %f：[向き]\n", m_rot.x, m_rot.y, m_rot.z);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f：[移動量]\n", m_fMove);

	switch (m_thing)
	{ // 配置物ごとの処理
	case THING_BUILDING:	// ビル

		if (m_pBuilding != NULL)
		{ // エディットビルが使用されている場合

			// エディットビルの情報表示
			m_pBuilding->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_WINDOW:		// 窓

		if (m_pWindow != NULL)
		{ // エディット窓が使用されている場合

			// エディット窓の情報表示
			m_pWindow->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SIGNBOARD:	// 看板

		if (m_pSignboard != NULL)
		{ // エディット看板が使用されている場合

			// エディット看板の情報表示
			m_pSignboard->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_OBSTACLE:	// 障害物

		if (m_pObstacle != NULL)
		{ // エディット障害物が使用されている場合

			// エディット障害物の情報表示
			m_pObstacle->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_SAVEPOINT:	// セーブポイント

		if (m_pSavePoint != NULL)
		{ // エディットセーブポイントが使用されている場合

			// エディットセーブポイントの情報表示
			m_pSavePoint->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	case THING_GOALPOINT:	// ゴールポイント

		if (m_pGoalPoint != NULL)
		{ // エディットゴールポイントが使用されている場合

			// エディットゴールポイントの情報表示
			m_pGoalPoint->DrawDebugInfo();
		}
		else { assert(false); }	// 非使用中

		break;

	default:	// 例外処理
		assert(false);
		break;
	}
}

//============================================================
//	ステージ保存処理
//============================================================
void CEditStageManager::SaveStage(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// ステージを保存
	if (m_pKeyboard->IsPress(KEY_DOUBLE))
	{
		if (m_pKeyboard->IsTrigger(KEY_SAVE))
		{
			// 保存処理
			Save();

			// 保存した状態にする
			m_bSave = true;
		}
	}
}

//============================================================
//	保存処理
//============================================================
void CEditStageManager::Save(void)
{
	// ポインタを宣言
	FILE *pFile = NULL;	// ファイルポインタ

	// ファイルを書き出し形式で開く
	pFile = fopen(SAVE_TXT, "w");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		// 見出しを書き出し
		fprintf(pFile, "#==============================================================================\n");
		fprintf(pFile, "#\n");
		fprintf(pFile, "#	ステージセーブテキスト [save_stage.txt]\n");
		fprintf(pFile, "#	Author : you\n");
		fprintf(pFile, "#\n");
		fprintf(pFile, "#==============================================================================\n");
		fprintf(pFile, "---------->--<---------- ここから下を コピーし貼り付け ---------->--<----------\n\n");

		// ビルの保存
		m_pBuilding->Save(pFile);

		// 窓の保存
		m_pWindow->Save(pFile);

		// 看板の保存
		m_pSignboard->Save(pFile);

		// 障害物の保存
		m_pObstacle->Save(pFile);

		// セーブポイントの保存
		m_pSavePoint->Save(pFile);

		// ゴールポイントの保存
		m_pGoalPoint->Save(pFile);

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "ステ－ジセーブファイルの書き出しに失敗！", "警告！", MB_ICONWARNING);
	}
}
