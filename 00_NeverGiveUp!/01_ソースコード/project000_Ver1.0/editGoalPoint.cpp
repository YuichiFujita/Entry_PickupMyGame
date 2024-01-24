//============================================================
//
//	エディットゴールポイント処理 [editGoalPoint.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editGoalPoint.h"
#include "manager.h"
#include "input.h"
#include "collision.h"
#include "editStageManager.h"
#include "objectMeshCube.h"
#include "stage.h"

//************************************************************
//	マクロ定義
//************************************************************
#define KEY_TRIGGER		(DIK_LSHIFT)	// トリガー化キー
#define NAME_TRIGGER	("LSHIFT")		// トリガー化表示

#define KEY_CREATE		(DIK_0)	// 生成キー
#define NAME_CREATE		("0")	// 生成表示
#define KEY_RELEASE		(DIK_9)	// 破棄キー
#define NAME_RELEASE	("9")	// 破棄表示

#define KEY_XSIZE_UP	(DIK_T)	// X拡大キー
#define NAME_XSIZE_UP	("T")	// X拡大表示
#define KEY_YSIZE_UP	(DIK_Y)	// Y拡大キー
#define NAME_YSIZE_UP	("Y")	// Y拡大表示
#define KEY_ZSIZE_UP	(DIK_U)	// Z拡大キー
#define NAME_ZSIZE_UP	("U")	// Z拡大表示

#define KEY_XSIZE_DOWN	(DIK_G)	// X縮小キー
#define NAME_XSIZE_DOWN	("G")	// X縮小表示
#define KEY_YSIZE_DOWN	(DIK_H)	// Y縮小キー
#define NAME_YSIZE_DOWN	("H")	// Y縮小表示
#define KEY_ZSIZE_DOWN	(DIK_J)	// Z縮小キー
#define NAME_ZSIZE_DOWN	("J")	// Z縮小表示

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXVECTOR3 INIT_COLL_SIZE = D3DXVECTOR3(60.0f, 60.0f, 60.0f);	// ゴール判定の大きさ

	namespace coll
	{
		const int	PRIORITY	= 2;	// 当たり判定の優先順位
		const float	SIZE_MOVE	= 2.0f;	// 当たり判定の大きさ変更量

		const D3DXCOLOR COLL_COL = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f);	// 当たり判定の色
		const D3DXCOLOR DRAW_COL = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f);	// 設置位置の色
	}
}

//************************************************************
//	親クラス [CEditGoalPoint] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditGoalPoint::CEditGoalPoint()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;	// エディットステージの情報
	m_pSet = NULL;	// 設置予定位置の情報
	memset(&m_goalPoint, 0, sizeof(m_goalPoint));	// ゴールポイント配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditGoalPoint::~CEditGoalPoint()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditGoalPoint::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;	// エディットステージの情報
	m_pSet = NULL;	// 設置予定位置の情報
	m_goalPoint.pGoalPoint = NULL;	// ゴールポイント情報
	m_goalPoint.pCollision = NULL;	// 当たり判定の情報
	m_goalPoint.bSet = false;		// 配置状況

	//--------------------------------------------------------
	//	ゴールポイントの生成・設定
	//--------------------------------------------------------
	// ゴールポイントの生成
	m_goalPoint.pGoalPoint = CGoalPoint::Create(VEC3_ZERO, INIT_COLL_SIZE);
	if (m_goalPoint.pGoalPoint == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	//--------------------------------------------------------
	//	設置予定位置の生成・設定
	//--------------------------------------------------------
	// 設置予定位置の生成
	m_pSet = CObjectMeshCube::Create
	( // 引数
		VEC3_ZERO,									// 位置
		VEC3_ZERO,									// 向き
		m_goalPoint.pGoalPoint->GetVec3Sizing(),	// 大きさ
		coll::COLL_COL,								// キューブ色
		XCOL_WHITE,									// 縁取り色
		CObjectMeshCube::BORDER_OFF,				// 縁取り状態
		0.0f,										// 縁取り太さ
		CObjectMeshCube::TEXSTATE_ONE,				// テクスチャ状態
		CObjectMeshCube::ETexState(NONE_IDX),		// テクスチャ種類
		CObjectMeshCube::ORIGIN_DOWN				// 原点
	);
	if (m_pSet == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pSet->SetPriority(coll::PRIORITY);

	//--------------------------------------------------------
	//	当たり判定の生成・設定
	//--------------------------------------------------------
	// 当たり判定の生成
	m_goalPoint.pCollision = CObjectMeshCube::Create
	( // 引数
		VEC3_ZERO,									// 位置
		VEC3_ZERO,									// 向き
		m_goalPoint.pGoalPoint->GetVec3Sizing(),	// 大きさ
		coll::DRAW_COL,								// キューブ色
		XCOL_WHITE,									// 縁取り色
		CObjectMeshCube::BORDER_OFF,				// 縁取り状態
		0.0f,										// 縁取り太さ
		CObjectMeshCube::TEXSTATE_ONE,				// テクスチャ状態
		CObjectMeshCube::ETexState(NONE_IDX),		// テクスチャ種類
		CObjectMeshCube::ORIGIN_DOWN				// 原点
	);
	if (m_goalPoint.pCollision == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_goalPoint.pCollision->SetPriority(coll::PRIORITY);

	// 表示をOFFにする
	SetDisp(false);

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
void CEditGoalPoint::Uninit(void)
{
#if _DEBUG

	if (m_pSet != NULL)
	{ // 設置予定位置の情報が使用されている場合

		// 設置予定位置の情報の終了
		m_pSet->Uninit();
	}
	else { assert(false); }	// 非使用中

	if (m_goalPoint.pCollision != NULL)
	{ // 当たり判定の情報が使用されている場合

		// 当たり判定の情報の終了
		m_goalPoint.pCollision->Uninit();
	}
	else { assert(false); }	// 非使用中

#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditGoalPoint::Update(void)
{
#if _DEBUG

	if (m_pEdit == NULL)
	{ // エディットステージが存在しない場合

		// 処理を抜ける
		assert(false);
		return;
	}

	// 判定大きさの更新
	UpdateCollSize();

	// ゴールポイントの生成・位置変更
	CreateGoalPoint();

	// 設置予定位置の更新
	UpdateSet();
	
	// 当たり判定の更新
	UpdateColl();

#endif	// _DEBUG
}

//============================================================
//	表示の設定処理
//============================================================
void CEditGoalPoint::SetDisp(const bool bDisp)
{
	// 変数を宣言
	bool bDispGoal = (m_goalPoint.bSet) ? bDisp : false;	// ゴールポイントの描画状況

	// 設置予定位置の自動更新・自動描画を表示状況に合わせる
	m_pSet->SetEnableUpdate(bDisp);
	m_pSet->SetEnableDraw(bDisp);

	// ゴールポイントの自動更新・自動描画をOFFにする
	m_goalPoint.pGoalPoint->SetEnableUpdate(bDispGoal);
	m_goalPoint.pGoalPoint->SetEnableDraw(bDispGoal);

	// 当たり判定の自動更新・自動描画をOFFにする
	m_goalPoint.pCollision->SetEnableUpdate(bDispGoal);
	m_goalPoint.pCollision->SetEnableDraw(bDispGoal);
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditGoalPoint::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "判定拡大：[%s/%s/%s+%s]\n", NAME_XSIZE_UP, NAME_YSIZE_UP, NAME_ZSIZE_UP, NAME_TRIGGER);
	pDebug->Print(CDebugProc::POINT_RIGHT, "判定縮小：[%s/%s/%s+%s]\n", NAME_XSIZE_DOWN, NAME_YSIZE_DOWN, NAME_ZSIZE_DOWN, NAME_TRIGGER);
	pDebug->Print(CDebugProc::POINT_RIGHT, "設置：[%s]\n", NAME_CREATE);
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditGoalPoint::DrawDebugInfo(void)
{
	// ポインタを宣言
	//CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報
}

//============================================================
//	保存処理
//============================================================
void CEditGoalPoint::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	ゴールポイントの配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_GOALPOINTSET\n\n");

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
	
					if (pObjCheck->GetLabel() != CObject::LABEL_GOALPOINT)
					{ // オブジェクトラベルがゴールポイントではない場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// ゴールポイントの情報を取得
					D3DXVECTOR3 posGoal = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 sizeGoal = pObjCheck->GetVec3Sizing();	// 大きさ
	
					// 情報を書き出し
					fprintf(pFile, "	GOALPOINTSET\n");
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posGoal.x, posGoal.y, posGoal.z);
					fprintf(pFile, "		SIZE = %.2f %.2f %.2f\n", sizeGoal.x, sizeGoal.y, sizeGoal.z);
					fprintf(pFile, "	END_GOALPOINTSET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_GOALPOINTSET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditGoalPoint *CEditGoalPoint::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditGoalPoint *pEditGoalPoint = NULL;	// エディットゴールポイント生成用

	if (pEditGoalPoint == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditGoalPoint = new CEditGoalPoint;	// エディットゴールポイント
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditGoalPoint != NULL)
	{ // 使用されている場合
		
		// エディットゴールポイントの初期化
		if (FAILED(pEditGoalPoint->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditGoalPoint;
			pEditGoalPoint = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditGoalPoint->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditGoalPoint;
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
HRESULT CEditGoalPoint::Release(CEditGoalPoint *&prEditGoalPoint)
{
#if _DEBUG

	if (prEditGoalPoint != NULL)
	{ // 使用中の場合

		// エディットゴールポイントの終了
		prEditGoalPoint->Uninit();

		// メモリ開放
		delete prEditGoalPoint;
		prEditGoalPoint = NULL;

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
//	判定大きさの更新処理
//============================================================
void CEditGoalPoint::UpdateCollSize(void)
{
	// 変数を宣言
	D3DXVECTOR3 size = m_pSet->GetVec3Sizing();	// 大きさ

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 判定サイズを変更
	if (!m_pKeyboard->IsPress(KEY_TRIGGER))
	{
		if (m_pKeyboard->IsPress(KEY_ZSIZE_UP))
		{
			size.z += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_ZSIZE_DOWN))
		{
			size.z -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_XSIZE_UP))
		{
			size.x += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_XSIZE_DOWN))
		{
			size.x -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_YSIZE_UP))
		{
			size.y += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_YSIZE_DOWN))
		{
			size.y -= coll::SIZE_MOVE;
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_ZSIZE_UP))
		{
			size.z += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_ZSIZE_DOWN))
		{
			size.z -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_XSIZE_UP))
		{
			size.x += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_XSIZE_DOWN))
		{
			size.x -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_YSIZE_UP))
		{
			size.y += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_YSIZE_DOWN))
		{
			size.y -= coll::SIZE_MOVE;
		}
	}

	// 大きさを反映
	m_pSet->SetVec3Sizing(size);
}

//============================================================
//	ゴールポイントの生成処理
//============================================================
void CEditGoalPoint::CreateGoalPoint(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// ゴールポイントを配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		// 変数を宣言
		D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
		D3DXVECTOR3 sizeGoal = m_pSet->GetVec3Sizing();		// ゴールの大きさ

		// ゴールポイントの位置を反映
		m_goalPoint.pGoalPoint->SetVec3Position(posEdit);

		// ゴールポイントの大きさを反映
		m_goalPoint.pGoalPoint->SetVec3Sizing(sizeGoal);

		// 当たり判定の位置を反映
		m_goalPoint.pCollision->SetVec3Position(posEdit);

		// 当たり判定の大きさを反映
		m_goalPoint.pCollision->SetVec3Sizing(sizeGoal);

		// 配置した状態にする
		m_goalPoint.bSet = true;

		// 表示をONにする
		SetDisp(true);

		// 未保存を設定
		m_pEdit->UnSave();
	}
}

//============================================================
//	設置予定位置の更新処理
//============================================================
void CEditGoalPoint::UpdateSet(void)
{
	// 設置予定の位置を反映
	m_pSet->SetVec3Position(m_pEdit->GetVec3Position());

	// 設置予定の大きさを反映
	m_pSet->SetVec3Sizing(m_pSet->GetVec3Sizing());
}

//============================================================
//	当たり判定の更新処理
//============================================================
void CEditGoalPoint::UpdateColl(void)
{
	// 当たり判定の位置を反映
	m_goalPoint.pCollision->SetVec3Position(m_goalPoint.pGoalPoint->GetVec3Position());

	// 当たり判定の大きさを反映
	m_goalPoint.pCollision->SetVec3Sizing(m_goalPoint.pGoalPoint->GetVec3Sizing());
}
