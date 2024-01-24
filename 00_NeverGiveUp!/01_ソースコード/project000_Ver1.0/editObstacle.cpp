//============================================================
//
//	エディット障害物処理 [editObstacle.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editObstacle.h"
#include "manager.h"
#include "input.h"
#include "collision.h"
#include "editStageManager.h"
#include "objectMeshCube.h"
#include "stage.h"

//************************************************************
//	マクロ定義
//************************************************************
#define KEY_DOUBLE		(DIK_LCONTROL)	// 二重化キー
#define NAME_DOUBLE		("LCTRL")		// 二重化表示
#define KEY_REVERSE		(DIK_LCONTROL)	// 逆転キー
#define NAME_REVERSE	("LCTRL")		// 逆転表示
#define KEY_TRIGGER		(DIK_LSHIFT)	// トリガー化キー
#define NAME_TRIGGER	("LSHIFT")		// トリガー化表示

#define KEY_SAVE	(DIK_F8)	// 保存キー
#define NAME_SAVE	("F8")		// 保存表示

#define KEY_CREATE		(DIK_0)	// 生成キー
#define NAME_CREATE		("0")	// 生成表示
#define KEY_RELEASE		(DIK_9)	// 破棄キー
#define NAME_RELEASE	("9")	// 破棄表示
#define KEY_TYPE		(DIK_2)	// 種類変更キー
#define NAME_TYPE		("2")	// 種類変更表示
#define KEY_STATE		(DIK_3)	// 特殊状態変更キー
#define NAME_STATE		("3")	// 特殊状態変更表示
#define KEY_DODGE		(DIK_4)	// 回避法変更キー
#define NAME_DODGE		("4")	// 回避法変更表示

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
	const char* SAVE_TXT = "data\\TXT\\save_obstacle.txt";	// 障害物セーブテキスト

	const float	INIT_ALPHA = 0.5f;	// 配置前のα値

	namespace coll
	{
		const int	PRIORITY	= 2;	// 当たり判定の優先順位
		const float	SIZE_MOVE	= 2.0f;	// 当たり判定の大きさ変更量

		const D3DXCOLOR COLL_COL = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f);	// 当たり判定の色
	}
}

//************************************************************
//	親クラス [CEditObstacle] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditObstacle::CEditObstacle()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;			// エディットステージの情報
	m_pCollision = NULL;	// 当たり判定の情報
	memset(&m_obstacle, 0, sizeof(m_obstacle));	// 障害物配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditObstacle::~CEditObstacle()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditObstacle::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;			// エディットステージの情報
	m_pCollision = NULL;	// 当たり判定の情報
	m_obstacle.pObstacle = NULL;			// 障害物情報
	m_obstacle.type = CObstacle::TYPE_BOX;	// 障害物種類

	// 当たり判定の生成
	m_pCollision = CObjectMeshCube::Create
	( // 引数
		VEC3_ZERO,								// 位置
		VEC3_ZERO,								// 向き
		VEC3_ZERO,								// 大きさ
		coll::COLL_COL,							// キューブ色
		XCOL_WHITE,								// 縁取り色
		CObjectMeshCube::BORDER_OFF,			// 縁取り状態
		0.0f,									// 縁取り太さ
		CObjectMeshCube::TEXSTATE_ONE,			// テクスチャ状態
		CObjectMeshCube::ETexState(NONE_IDX),	// テクスチャ種類
		CObjectMeshCube::ORIGIN_DOWN			// 原点
	);
	if (m_pCollision == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 優先順位を設定
	m_pCollision->SetPriority(coll::PRIORITY);

	// 障害物の生成
	m_obstacle.pObstacle = CObstacle::Create(m_obstacle.type, VEC3_ZERO, VEC3_ZERO);
	if (m_obstacle.pObstacle == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 透明度を設定
	m_obstacle.pObstacle->SetAlpha(INIT_ALPHA);

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
void CEditObstacle::Uninit(void)
{
#if _DEBUG

	if (m_pCollision != NULL)
	{ // 当たり判定の情報が使用されている場合

		// 当たり判定の情報の終了
		m_pCollision->Uninit();
	}
	else { assert(false); }	// 非使用中

#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditObstacle::Update(void)
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

	// 種類変更の更新
	UpdateChangeType();

	// 特殊状態変更の更新
	UpdateChangeState();

	// 回避法変更の更新
	UpdateChangeDodge();

	// 障害物の生成
	CreateObstacle();

	// 障害物の破棄
	ReleaseObstacle();

	// 障害物の保存
	SaveObstacle();

	// 位置を反映
	m_obstacle.pObstacle->SetVec3Position(m_pEdit->GetVec3Position());
	m_pCollision->SetVec3Position(m_pEdit->GetVec3Position());

	// 向きを反映
	m_obstacle.pObstacle->SetVec3Rotation(m_pEdit->GetVec3Rotation());
	m_pCollision->SetVec3Rotation(m_pEdit->GetVec3Rotation());

	// 大きさを反映
	D3DXVECTOR3 sizeColl = CObstacle::GetStatusInfo(m_obstacle.type).size * 0.5f;
	m_pCollision->SetVec3Sizing(sizeColl);

#endif	// _DEBUG
}

//============================================================
//	表示の設定処理
//============================================================
void CEditObstacle::SetDisp(const bool bDisp)
{
	// 自動更新・自動描画を表示状況に合わせる
	m_obstacle.pObstacle->SetEnableUpdate(bDisp);	// 更新
	m_pCollision->SetEnableUpdate(bDisp);			// 更新
	m_obstacle.pObstacle->SetEnableDraw(bDisp);		// 描画
	m_pCollision->SetEnableDraw(bDisp);				// 描画

	if (bDisp)
	{ // 表示ONの場合

		// 位置を反映
		m_obstacle.pObstacle->SetVec3Position(m_pEdit->GetVec3Position());
		m_pCollision->SetVec3Position(m_pEdit->GetVec3Position());
	}
	else
	{ // 表示OFFの場合

		// 障害物の色の全初期化
		InitAllColorObstacle();

		// 位置をステージの範囲外に設定
		D3DXVECTOR3 sizeObs = m_obstacle.pObstacle->GetVec3Sizing();
		D3DXVECTOR3 outLimit = D3DXVECTOR3(0.0f, 0.0f, CScene::GetStage()->GetStageLimit().fNear - ((sizeObs.z < sizeObs.x) ? sizeObs.x : sizeObs.z) * 0.5f);
		m_obstacle.pObstacle->SetVec3Position(outLimit);
		m_pCollision->SetVec3Position(outLimit);
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditObstacle::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "障害物保存：[%s+%s]\n", NAME_DOUBLE, NAME_SAVE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "判定拡大：[%s/%s/%s+%s]\n", NAME_XSIZE_UP, NAME_YSIZE_UP, NAME_ZSIZE_UP, NAME_TRIGGER);
	pDebug->Print(CDebugProc::POINT_RIGHT, "判定縮小：[%s/%s/%s+%s]\n", NAME_XSIZE_DOWN, NAME_YSIZE_DOWN, NAME_ZSIZE_DOWN, NAME_TRIGGER);
	pDebug->Print(CDebugProc::POINT_RIGHT, "種類変更：[%s/%s+%s]\n", NAME_TYPE, NAME_REVERSE, NAME_TYPE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "特殊状態変更：[%s]\n", NAME_STATE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "回避法変更：[%s]\n", NAME_DODGE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "削除：[%s]\n", NAME_RELEASE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "設置：[%s]\n", NAME_CREATE);
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditObstacle::DrawDebugInfo(void)
{
	// 変数を宣言
	CObstacle::SStatusInfo info = CObstacle::GetStatusInfo(m_obstacle.type);	// ステ－タス情報

	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();				// デバッグプロックの情報
	static char* apState[] = { "特殊状態無し", "ジャンプ", "着地" };			// 特殊状態
	static char* apDodge[] = { "回避法無し", "ジャンプ", "スライディング" };	// 回避法

	// 特殊状態・回避法数の不一致
	assert((sizeof(apState) / sizeof(apState[0])) == CObstacle::STATE_MAX);
	assert((sizeof(apDodge) / sizeof(apDodge[0])) == CObstacle::DODGE_MAX);

	pDebug->Print(CDebugProc::POINT_RIGHT, "%d：[種類]\n", m_obstacle.type);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%s：[特殊状態]\n", apState[info.state]);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%s：[回避法]\n", apDodge[info.dodge]);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f %f %f：[判定サイズ]\n", info.size.x, info.size.y, info.size.z);
}

//============================================================
//	保存処理
//============================================================
void CEditObstacle::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	障害物の配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_OBSTACLESET\n\n");

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
	
					if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
					{ // オブジェクトラベルが障害物ではない場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}
	
					if (pObjCheck == (CObject*)m_obstacle.pObstacle)
					{ // 同じアドレスだった場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// 障害物の情報を取得
					D3DXVECTOR3 posObs = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 rotObs = pObjCheck->GetVec3Rotation();	// 向き
					int nType = pObjCheck->GetType();	// 種類
	
					// 情報を書き出し
					fprintf(pFile, "	OBSTACLESET\n");
					fprintf(pFile, "		TYPE = %d\n", nType);
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posObs.x, posObs.y, posObs.z);
					fprintf(pFile, "		ROT = %.2f %.2f %.2f\n", rotObs.x, rotObs.y, rotObs.z);
					fprintf(pFile, "	END_OBSTACLESET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_OBSTACLESET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditObstacle *CEditObstacle::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditObstacle *pEditObstacle = NULL;	// エディット障害物生成用

	if (pEditObstacle == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditObstacle = new CEditObstacle;	// エディット障害物
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditObstacle != NULL)
	{ // 使用されている場合
		
		// エディット障害物の初期化
		if (FAILED(pEditObstacle->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditObstacle;
			pEditObstacle = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditObstacle->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditObstacle;
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
HRESULT CEditObstacle::Release(CEditObstacle *&prEditObstacle)
{
#if _DEBUG

	if (prEditObstacle != NULL)
	{ // 使用中の場合

		// エディット障害物の終了
		prEditObstacle->Uninit();

		// メモリ開放
		delete prEditObstacle;
		prEditObstacle = NULL;

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
void CEditObstacle::UpdateCollSize(void)
{
	// 変数を宣言
	CObstacle::SStatusInfo info = CObstacle::GetStatusInfo(m_obstacle.type);	// ステ－タス情報

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 判定サイズを変更
	if (!m_pKeyboard->IsPress(KEY_TRIGGER))
	{
		if (m_pKeyboard->IsPress(KEY_ZSIZE_UP))
		{
			info.size.z += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_ZSIZE_DOWN))
		{
			info.size.z -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_XSIZE_UP))
		{
			info.size.x += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_XSIZE_DOWN))
		{
			info.size.x -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_YSIZE_UP))
		{
			info.size.y += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsPress(KEY_YSIZE_DOWN))
		{
			info.size.y -= coll::SIZE_MOVE;
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_ZSIZE_UP))
		{
			info.size.z += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_ZSIZE_DOWN))
		{
			info.size.z -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_XSIZE_UP))
		{
			info.size.x += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_XSIZE_DOWN))
		{
			info.size.x -= coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_YSIZE_UP))
		{
			info.size.y += coll::SIZE_MOVE;
		}
		if (m_pKeyboard->IsTrigger(KEY_YSIZE_DOWN))
		{
			info.size.y -= coll::SIZE_MOVE;
		}
	}

	// ステ－タス情報を反映
	CObstacle::SetStatusInfo(info, m_obstacle.type);
}

//============================================================
//	種類変更の更新処理
//============================================================
void CEditObstacle::UpdateChangeType(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 種類を変更
	if (m_pKeyboard->IsPress(KEY_REVERSE))
	{
		if (m_pKeyboard->IsTrigger(KEY_TYPE))
		{
			// 種類を変更
			m_obstacle.type = (CObstacle::EType)((m_obstacle.type + (CObstacle::TYPE_MAX - 1)) % CObstacle::TYPE_MAX);

			// 種類を反映
			m_obstacle.pObstacle->SetType(m_obstacle.type);
			m_obstacle.pObstacle->SetAlpha(INIT_ALPHA);	// 透明度を再設定
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_TYPE))
		{
			// 種類を変更
			m_obstacle.type = (CObstacle::EType)((m_obstacle.type + 1) % CObstacle::TYPE_MAX);

			// 種類を反映
			m_obstacle.pObstacle->SetType(m_obstacle.type);
			m_obstacle.pObstacle->SetAlpha(INIT_ALPHA);	// 透明度を再設定
		}
	}
}

//============================================================
//	特殊状態変更の更新処理
//============================================================
void CEditObstacle::UpdateChangeState(void)
{
	// 変数を宣言
	CObstacle::SStatusInfo info = CObstacle::GetStatusInfo(m_obstacle.type);	// ステ－タス情報

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 特殊状態を変更
	if (m_pKeyboard->IsTrigger(KEY_STATE))
	{
		info.state = (CObstacle::EState)((info.state + 1) % CObstacle::STATE_MAX);
	}

	// ステ－タス情報を反映
	CObstacle::SetStatusInfo(info, m_obstacle.type);
}

//============================================================
//	回避法変更の更新処理
//============================================================
void CEditObstacle::UpdateChangeDodge(void)
{
	// 変数を宣言
	CObstacle::SStatusInfo info = CObstacle::GetStatusInfo(m_obstacle.type);	// ステ－タス情報

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 回避法を変更
	if (m_pKeyboard->IsTrigger(KEY_DODGE))
	{
		info.dodge = (CObstacle::EDodge)((info.dodge + 1) % CObstacle::DODGE_MAX);
	}

	// ステ－タス情報を反映
	CObstacle::SetStatusInfo(info, m_obstacle.type);
}

//============================================================
//	障害物の生成処理
//============================================================
void CEditObstacle::CreateObstacle(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 障害物を配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		//----------------------------------------------------
		//	障害物の情報を配置用に変更
		//----------------------------------------------------
		// 自動更新・自動描画をONにする
		m_obstacle.pObstacle->SetEnableUpdate(true);
		m_obstacle.pObstacle->SetEnableDraw(true);

		// マテリアルを再設定
		m_obstacle.pObstacle->ResetMaterial();

		// 未保存を設定
		m_pEdit->UnSave();

		//----------------------------------------------------
		//	新しい障害物の生成
		//----------------------------------------------------
		// 障害物の生成
		m_obstacle.pObstacle = CObstacle::Create(m_obstacle.type, posEdit, rotEdit);
		assert(m_obstacle.pObstacle != NULL);

		// 透明度を設定
		m_obstacle.pObstacle->SetAlpha(INIT_ALPHA);
	}
}

//============================================================
//	障害物の破棄処理
//============================================================
void CEditObstacle::ReleaseObstacle(void)
{
	// 変数を宣言
	bool bRelease = false;	// 破棄状況

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 障害物を削除
	if (m_pKeyboard->IsTrigger(KEY_RELEASE))
	{
		// 破棄する状態を設定
		bRelease = true;
	}

	// 障害物の削除判定
	DeleteCollisionObstacle(bRelease);
}

//============================================================
//	障害物の削除判定
//============================================================
void CEditObstacle::DeleteCollisionObstacle(const bool bRelase)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();				// エディットの位置
	D3DXVECTOR3 sizeEdit = m_obstacle.pObstacle->GetVec3Sizing();	// エディット障害物の大きさ

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
				D3DXVECTOR3 posObs = VEC3_ZERO;	// 障害物位置
				D3DXVECTOR3 sizeObs = VEC3_ZERO;	// 障害物大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
				{ // オブジェクトラベルが障害物ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_obstacle.pObstacle)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 障害物の位置を取得
				posObs = pObjCheck->GetVec3Position();

				// 障害物の大きさを取得
				sizeObs = pObjCheck->GetVec3Sizing();

				// 球体の当たり判定
				if (collision::Circle3D
				( // 引数
					posEdit,	// 判定位置
					posObs,	// 判定目標位置
					((sizeObs.x + sizeObs.z) * 0.5f) * 0.5f,	// 判定半径
					((sizeEdit.x + sizeEdit.z) * 0.5f) * 0.5f	// 判定目標半径
				))
				{ // 判定内だった場合

					if (bRelase)
					{ // 破棄する場合

						// 終了処理
						pObjCheck->Uninit();

						// 未保存を設定
						m_pEdit->UnSave();
					}
					else
					{ // 破棄しない場合

						// 赤マテリアルを全設定
						pObjCheck->SetAllMaterial(material::Red());
					}
				}
				else
				{ // 判定外だった場合

					// マテリアルを再設定
					pObjCheck->ResetMaterial();
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	障害物の色の全初期化処理
//============================================================
void CEditObstacle::InitAllColorObstacle(void)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_OBSTACLE)
				{ // オブジェクトラベルが障害物ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_obstacle.pObstacle)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// マテリアルを再設定
				pObjCheck->ResetMaterial();

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	障害物保存処理
//============================================================
void CEditObstacle::SaveObstacle(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 障害物を保存
	if (m_pKeyboard->IsPress(KEY_DOUBLE))
	{
		if (m_pKeyboard->IsTrigger(KEY_SAVE))
		{
			// 保存処理
			Save();
		}
	}
}

//============================================================
//	保存処理
//============================================================
void CEditObstacle::Save(void)
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
		fprintf(pFile, "#	障害物セーブテキスト [save_obstacle.txt]\n");
		fprintf(pFile, "#	Author : you\n");
		fprintf(pFile, "#\n");
		fprintf(pFile, "#==============================================================================\n");
		fprintf(pFile, "---------->--<---------- ここから下を コピーし貼り付け ---------->--<----------\n");
		fprintf(pFile, "\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	パラメーター情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	特殊状態無し		：0\n");
		fprintf(pFile, "#	触れるとジャンプする：1\n");
		fprintf(pFile, "#	着地できる			：2\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	回避方法無し		：0\n");
		fprintf(pFile, "#	ジャンプ回避		：1\n");
		fprintf(pFile, "#	スライディング回避	：2\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STATUSSET\n\n");

		for (int nCntObs = 0; nCntObs < CObstacle::TYPE_MAX; nCntObs++)
		{ // 障害物の種類数分繰り返す

			// 変数を宣言
			CObstacle::SStatusInfo info = CObstacle::GetStatusInfo(nCntObs);	// ステ－タス情報

			// 情報を書き出し
			fprintf(pFile, "	OBSTACLESET\n");
			fprintf(pFile, "		TYPE = %d\n", nCntObs);
			fprintf(pFile, "		SIZE = %.2f %.2f %.2f\n", info.size.x, info.size.y, info.size.z);
			fprintf(pFile, "		STATE = %d\n", info.state);
			fprintf(pFile, "		DODGE = %d\n", info.dodge);
			fprintf(pFile, "	END_OBSTACLESET\n\n");
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STATUSSET\n\n");

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "障害物セーブファイルの書き出しに失敗！", "警告！", MB_ICONWARNING);
	}
}
