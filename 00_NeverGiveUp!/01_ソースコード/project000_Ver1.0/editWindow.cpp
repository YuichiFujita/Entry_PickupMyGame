//============================================================
//
//	エディット窓処理 [editWindow.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editWindow.h"
#include "manager.h"
#include "input.h"
#include "collision.h"
#include "editStageManager.h"
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
#define KEY_TYPE		(DIK_2)	// 種類変更キー
#define NAME_TYPE		("2")	// 種類変更表示
#define KEY_SCALE_UP	(DIK_3)	// 拡大率上昇キー
#define NAME_SCALE_UP	("3")	// 拡大率上昇表示
#define KEY_SCALE_DOWN	(DIK_4)	// 拡大率下降キー
#define NAME_SCALE_DOWN	("4")	// 拡大率下降表示

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const D3DXCOLOR	INIT_COL	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 配置前の色
	const float CHANGE_SCALE	= 0.05f;	// 拡大率変更量
}

//************************************************************
//	親クラス [CEditWindow] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditWindow::CEditWindow()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;	// エディットステージの情報
	memset(&m_window, 0, sizeof(m_window));	// 窓配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditWindow::~CEditWindow()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditWindow::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;	// エディットステージの情報
	m_window.pWindow = NULL;	// 窓情報
	m_window.type = CWindow::TYPE_NORMAL;	// 窓種類

	// 窓の生成
	m_window.pWindow = CWindow::Create(m_window.type, VEC3_ZERO, VEC3_ZERO);
	if (m_window.pWindow == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 色を設定
	m_window.pWindow->SetColor(INIT_COL);

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
void CEditWindow::Uninit(void)
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditWindow::Update(void)
{
#if _DEBUG

	if (m_pEdit == NULL)
	{ // エディットステージが存在しない場合

		// 処理を抜ける
		assert(false);
		return;
	}

	// 種類変更の更新
	UpdateChangeType();

	// 拡大率変更の更新
	UpdateChangeScale();

	// 窓の生成
	MyCreateWindow();

	// 窓の破棄
	ReleaseWindow();

	// 位置を反映
	m_window.pWindow->SetVec3Position(m_pEdit->GetVec3Position());

	// 向きを反映
	m_window.pWindow->SetVec3Rotation(m_pEdit->GetVec3Rotation());

	// 種類を反映
	m_window.pWindow->SetType(m_window.type);

#endif	// _DEBUG
}

//============================================================
//	表示の設定処理
//============================================================
void CEditWindow::SetDisp(const bool bDisp)
{
	// 自動更新・自動描画を表示状況に合わせる
	m_window.pWindow->SetEnableUpdate(bDisp);	// 更新
	m_window.pWindow->SetEnableDraw(bDisp);		// 描画

	if (bDisp)
	{ // 表示ONの場合

		// 位置を反映
		m_window.pWindow->SetVec3Position(m_pEdit->GetVec3Position());
	}
	else
	{ // 表示OFFの場合

		// 窓の色の全初期化
		InitAllColorWindow();

		// 位置をステージの範囲外に設定
		D3DXVECTOR3 outLimit = D3DXVECTOR3(0.0f, 0.0f, CScene::GetStage()->GetStageLimit().fNear - m_window.pWindow->GetVec3Sizing().z);
		m_window.pWindow->SetVec3Position(outLimit);
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditWindow::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "種類変更：[%s]\n", NAME_TYPE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "拡大率上昇：[%s]\n", NAME_SCALE_UP);
	pDebug->Print(CDebugProc::POINT_RIGHT, "拡大率下降：[%s]\n", NAME_SCALE_DOWN);
	pDebug->Print(CDebugProc::POINT_RIGHT, "削除：[%s]\n", NAME_RELEASE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "設置：[%s]\n", NAME_CREATE);
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditWindow::DrawDebugInfo(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "%d：[種類]\n", m_window.type);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f：[拡大率]\n", m_window.pWindow->GetScale());
}

//============================================================
//	保存処理
//============================================================
void CEditWindow::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	窓の配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_WINDOWSET\n\n");

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
	
					if (pObjCheck->GetLabel() != CObject::LABEL_WINDOW)
					{ // オブジェクトラベルが窓ではない場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}
	
					if (pObjCheck == (CObject*)m_window.pWindow)
					{ // 同じアドレスだった場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// 窓の情報を取得
					D3DXVECTOR3 posBuild = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 rotBuild = pObjCheck->GetVec3Rotation();	// 向き
					int nType = pObjCheck->GetType();		// 種類
					float fScale = pObjCheck->GetScale();	// 拡大率
	
					// 情報を書き出し
					fprintf(pFile, "	WINDOWSET\n");
					fprintf(pFile, "		TYPE = %d\n", nType);
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posBuild.x, posBuild.y, posBuild.z);
					fprintf(pFile, "		ROT = %.2f %.2f %.2f\n", rotBuild.x, rotBuild.y, rotBuild.z);
					fprintf(pFile, "		SCALE = %.2f\n", fScale);
					fprintf(pFile, "	END_WINDOWSET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_WINDOWSET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditWindow *CEditWindow::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditWindow *pEditWindow = NULL;	// エディット窓生成用

	if (pEditWindow == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditWindow = new CEditWindow;	// エディット窓
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditWindow != NULL)
	{ // 使用されている場合
		
		// エディット窓の初期化
		if (FAILED(pEditWindow->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditWindow;
			pEditWindow = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditWindow->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditWindow;
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
HRESULT CEditWindow::Release(CEditWindow *&prEditWindow)
{
#if _DEBUG

	if (prEditWindow != NULL)
	{ // 使用中の場合

		// エディット窓の終了
		prEditWindow->Uninit();

		// メモリ開放
		delete prEditWindow;
		prEditWindow = NULL;

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
//	種類変更の更新処理
//============================================================
void CEditWindow::UpdateChangeType(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 種類を変更
	if (m_pKeyboard->IsTrigger(KEY_TYPE))
	{
		m_window.type = (CWindow::EType)((m_window.type + 1) % CWindow::TYPE_MAX);
	}
}

//============================================================
//	拡大率変更の更新処理
//============================================================
void CEditWindow::UpdateChangeScale(void)
{
	// 変数を宣言
	float fScale = m_window.pWindow->GetScale();	// 窓拡大率

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 拡大率を変更
	if (!m_pKeyboard->IsPress(KEY_TRIGGER))
	{
		if (m_pKeyboard->IsPress(KEY_SCALE_UP))
		{
			fScale += CHANGE_SCALE;
		}
		if (m_pKeyboard->IsPress(KEY_SCALE_DOWN))
		{
			fScale -= CHANGE_SCALE;
		}
	}
	else
	{
		if (m_pKeyboard->IsTrigger(KEY_SCALE_UP))
		{
			fScale += CHANGE_SCALE;
		}
		if (m_pKeyboard->IsTrigger(KEY_SCALE_DOWN))
		{
			fScale -= CHANGE_SCALE;
		}
	}

	// 窓拡大率を反映
	m_window.pWindow->SetScale(fScale);
}

//============================================================
//	窓の生成処理
//============================================================
void CEditWindow::MyCreateWindow(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き
	D3DXCOLOR colBuild = XCOL_WHITE;	// 色保存用
	float fScale = 0.0f;	// 拡大率

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 窓を配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		//----------------------------------------------------
		//	窓の情報を配置用に変更
		//----------------------------------------------------
		// 自動更新・自動描画をONにする
		m_window.pWindow->SetEnableUpdate(true);
		m_window.pWindow->SetEnableDraw(true);

		// 色を設定
		m_window.pWindow->SetColor(XCOL_WHITE);

		// 拡大率を保存
		fScale = m_window.pWindow->GetScale();

		// 未保存を設定
		m_pEdit->UnSave();

		//----------------------------------------------------
		//	新しい窓の生成
		//----------------------------------------------------
		// 窓の生成
		m_window.pWindow = CWindow::Create(m_window.type, posEdit, rotEdit);
		assert(m_window.pWindow != NULL);

		// 色を設定
		m_window.pWindow->SetColor(INIT_COL);

		// 拡大率を設定
		m_window.pWindow->SetScale(fScale);
	}
}

//============================================================
//	窓の破棄処理
//============================================================
void CEditWindow::ReleaseWindow(void)
{
	// 変数を宣言
	bool bRelease = false;	// 破棄状況

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 窓を削除
	if (m_pKeyboard->IsTrigger(KEY_RELEASE))
	{
		// 破棄する状態を設定
		bRelease = true;
	}

	// 窓の削除判定
	DeleteCollisionWindow(bRelease);
}

//============================================================
//	窓の削除判定
//============================================================
void CEditWindow::DeleteCollisionWindow(const bool bRelase)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();				// エディットの位置
	D3DXVECTOR3 sizeEdit = m_window.pWindow->GetVec3Sizing();	// エディット窓の大きさ

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
				D3DXVECTOR3 posBuild = VEC3_ZERO;	// 窓位置
				D3DXVECTOR3 sizeBuild = VEC3_ZERO;	// 窓大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_WINDOW)
				{ // オブジェクトラベルが窓ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_window.pWindow)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 窓の位置を取得
				posBuild = pObjCheck->GetVec3Position();

				// 窓の大きさを取得
				sizeBuild = pObjCheck->GetVec3Sizing();

				// 球体の当たり判定
				if (collision::Circle3D
				( // 引数
					posEdit,							// 判定位置
					posBuild,							// 判定目標位置
					(sizeBuild.x + sizeBuild.z) * 0.5f,	// 判定半径
					(sizeEdit.x + sizeEdit.z) * 0.5f	// 判定目標半径
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

						// 赤を設定
						pObjCheck->SetColor(XCOL_RED);
					}
				}
				else
				{ // 判定外だった場合

					// 通常色を設定
					pObjCheck->SetColor(XCOL_WHITE);
				}

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}

//============================================================
//	窓の色の全初期化処理
//============================================================
void CEditWindow::InitAllColorWindow(void)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_WINDOW)
				{ // オブジェクトラベルが窓ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_window.pWindow)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 通常色を設定
				pObjCheck->SetColor(XCOL_WHITE);

				// 次のオブジェクトへのポインタを代入
				pObjCheck = pObjectNext;
			}
		}
	}
}
