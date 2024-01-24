//============================================================
//
//	エディットセーブポイント処理 [editSavePoint.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editSavePoint.h"
#include "manager.h"
#include "input.h"
#include "collision.h"
#include "editStageManager.h"
#include "stage.h"
#include "effect3D.h"

//************************************************************
//	マクロ定義
//************************************************************
#define KEY_CREATE		(DIK_0)	// 生成キー
#define NAME_CREATE		("0")	// 生成表示
#define KEY_RELEASE		(DIK_9)	// 破棄キー
#define NAME_RELEASE	("9")	// 破棄表示

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const float	INIT_ALPHA		= 0.5f;		// 配置前のα値
	const float	EFFECT_RADIUS	= 30.0f;	// 方向表示エフェクトの半径
	const int	EFFECT_LIFE		= 10;		// 方向表示エフェクトの寿命
}

//************************************************************
//	親クラス [CEditSavePoint] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditSavePoint::CEditSavePoint()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;	// エディットステージの情報
	memset(&m_savePoint, 0, sizeof(m_savePoint));	// セーブポイント配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditSavePoint::~CEditSavePoint()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditSavePoint::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;	// エディットステージの情報
	m_savePoint.pSavePoint = NULL;	// セーブポイント情報

	// セーブポイントの生成
	m_savePoint.pSavePoint = CSavePoint::Create(VEC3_ZERO, VEC3_ZERO);
	if (m_savePoint.pSavePoint == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 透明度を設定
	m_savePoint.pSavePoint->SetAlpha(INIT_ALPHA);

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
void CEditSavePoint::Uninit(void)
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditSavePoint::Update(void)
{
#if _DEBUG

	if (m_pEdit == NULL)
	{ // エディットステージが存在しない場合

		// 処理を抜ける
		assert(false);
		return;
	}

	// 方向表示エフェクトの生成
	CreateRotaEffect();

	// セーブポイントの生成
	CreateSavePoint();

	// セーブポイントの破棄
	ReleaseSavePoint();

	// 位置を反映
	m_savePoint.pSavePoint->SetVec3Position(m_pEdit->GetVec3Position());

	// 向きを反映
	m_savePoint.pSavePoint->SetVec3Rotation(m_pEdit->GetVec3Rotation());

#endif
}

//============================================================
//	表示の設定処理
//============================================================
void CEditSavePoint::SetDisp(const bool bDisp)
{
	// 自動更新・自動描画を表示状況に合わせる
	m_savePoint.pSavePoint->SetEnableUpdate(bDisp);	// 更新
	m_savePoint.pSavePoint->SetEnableDraw(bDisp);	// 描画

	if (bDisp)
	{ // 表示ONの場合

		// 位置を反映
		m_savePoint.pSavePoint->SetVec3Position(m_pEdit->GetVec3Position());
	}
	else
	{ // 表示OFFの場合

		// セーブポイントの色の全初期化
		InitAllColorSavePoint();
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditSavePoint::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "削除：[%s]\n", NAME_RELEASE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "設置：[%s]\n", NAME_CREATE);
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditSavePoint::DrawDebugInfo(void)
{
	// ポインタを宣言
	//CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報
}

//============================================================
//	保存処理
//============================================================
void CEditSavePoint::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	セーブポイントの配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_SAVEPOINTSET\n\n");

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
	
					if (pObjCheck == (CObject*)m_savePoint.pSavePoint)
					{ // 同じアドレスだった場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// セーブポイントの情報を取得
					D3DXVECTOR3 posSave = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 rotSave = pObjCheck->GetVec3Rotation();	// 向き
	
					// 情報を書き出し
					fprintf(pFile, "	SAVEPOINTSET\n");
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posSave.x, posSave.y, posSave.z);
					fprintf(pFile, "		ROT = %.2f %.2f %.2f\n", rotSave.x, rotSave.y, rotSave.z);
					fprintf(pFile, "	END_SAVEPOINTSET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_SAVEPOINTSET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditSavePoint *CEditSavePoint::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditSavePoint *pEditSavePoint = NULL;	// エディットセーブポイント生成用

	if (pEditSavePoint == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditSavePoint = new CEditSavePoint;	// エディットセーブポイント
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditSavePoint != NULL)
	{ // 使用されている場合
		
		// エディットセーブポイントの初期化
		if (FAILED(pEditSavePoint->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditSavePoint;
			pEditSavePoint = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditSavePoint->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditSavePoint;
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
HRESULT CEditSavePoint::Release(CEditSavePoint *&prEditSavePoint)
{
#if _DEBUG

	if (prEditSavePoint != NULL)
	{ // 使用中の場合

		// エディットセーブポイントの終了
		prEditSavePoint->Uninit();

		// メモリ開放
		delete prEditSavePoint;
		prEditSavePoint = NULL;

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
//	セーブポイントの生成処理
//============================================================
void CEditSavePoint::CreateSavePoint(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き
	D3DXCOLOR colSave = XCOL_WHITE;	// 色保存用

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// セーブポイントを配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		//----------------------------------------------------
		//	セーブポイントの情報を配置用に変更
		//----------------------------------------------------
		// 自動更新・自動描画をONにする
		m_savePoint.pSavePoint->SetEnableUpdate(true);
		m_savePoint.pSavePoint->SetEnableDraw(true);

		// マテリアルを再設定
		m_savePoint.pSavePoint->ResetMaterial();

		// 未保存を設定
		m_pEdit->UnSave();

		//----------------------------------------------------
		//	新しいセーブポイントの生成
		//----------------------------------------------------
		// セーブポイントの生成
		m_savePoint.pSavePoint = CSavePoint::Create(VEC3_ZERO, VEC3_ZERO);
		assert(m_savePoint.pSavePoint != NULL);

		// 透明度を設定
		m_savePoint.pSavePoint->SetAlpha(INIT_ALPHA);
	}
}

//============================================================
//	セーブポイントの破棄処理
//============================================================
void CEditSavePoint::ReleaseSavePoint(void)
{
	// 変数を宣言
	bool bRelease = false;	// 破棄状況

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// セーブポイントを削除
	if (m_pKeyboard->IsTrigger(KEY_RELEASE))
	{
		// 破棄する状態を設定
		bRelease = true;
	}

	// セーブポイントの削除判定
	DeleteCollisionSavePoint(bRelease);
}

//============================================================
//	方向表示エフェクトの生成処理
//============================================================
void CEditSavePoint::CreateRotaEffect(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEffect = VEC3_ZERO;							// エフェクト位置
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();			// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();			// エディットの向き
	float fRadiusSave = m_savePoint.pSavePoint->GetRadius();	// セーブ半径

	// エフェクト位置を計算
	posEffect.x = posEdit.x + sinf(rotEdit.y + D3DX_PI) * fRadiusSave;
	posEffect.y = posEdit.y;
	posEffect.z = posEdit.z + cosf(rotEdit.y + D3DX_PI) * fRadiusSave;

	// 方向表示エフェクトを生成
	CEffect3D::Create(posEffect, EFFECT_RADIUS, CEffect3D::TYPE_NORMAL, EFFECT_LIFE);
}

//============================================================
//	セーブポイントの削除判定
//============================================================
void CEditSavePoint::DeleteCollisionSavePoint(const bool bRelase)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();			// エディットの位置
	float fRadiusEdit = m_savePoint.pSavePoint->GetRadius();	// エディットセーブポイントの半径

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
				D3DXVECTOR3 posSave = VEC3_ZERO;	// セーブポイント位置
				float fRadiusSave = 0.0f;			// セーブポイント半径

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_SAVEPOINT)
				{ // オブジェクトラベルがセーブポイントではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_savePoint.pSavePoint)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// セーブポイントの位置を取得
				posSave = pObjCheck->GetVec3Position();

				// セーブポイントの半径を取得
				fRadiusSave = pObjCheck->GetRadius();

				// 球体の当たり判定
				if (collision::Circle3D
				( // 引数
					posEdit,		// 判定位置
					posSave,		// 判定目標位置
					fRadiusEdit,	// 判定半径
					fRadiusSave		// 判定目標半径
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
//	セーブポイントの色の全初期化処理
//============================================================
void CEditSavePoint::InitAllColorSavePoint(void)
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

				if (pObjCheck == (CObject*)m_savePoint.pSavePoint)
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
