//============================================================
//
//	エディット看板処理 [editSignboard.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editSignboard.h"
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
#define KEY_TYPE		(DIK_2)	// 種類変更キー
#define NAME_TYPE		("2")	// 種類変更表示

#define KEY_SCALE_UP	(DIK_RIGHT)	// 拡大率上昇キー
#define NAME_SCALE_UP	("→")		// 拡大率上昇表示
#define KEY_SCALE_DOWN	(DIK_LEFT)	// 拡大率下降キー
#define NAME_SCALE_DOWN	("←")		// 拡大率下降表示

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const float	INIT_ALPHA		= 0.5f;		// 配置前のα値
	const float CHANGE_SCALE	= 0.05f;	// 拡大率変更量
	const float	EFFECT_RADIUS	= 30.0f;	// 方向表示エフェクトの半径
	const int	EFFECT_LIFE		= 10;		// 方向表示エフェクトの寿命

	namespace center
	{
		const D3DXVECTOR3	SIZE	= D3DXVECTOR3(280.0f, 20.0f, 280.0f);	// 中心表示の大きさ
		const D3DXCOLOR		COL		= D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);	// 中心表示の色

		const float ADD_SIDE	= 50.0f;	// 中心位置までの横加算量
		const float SUB_DOWN	= 240.0f;	// 中心位置までの縦減算量
	}
}

//************************************************************
//	親クラス [CEditSignboard] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditSignboard::CEditSignboard()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;		// エディットステージの情報
	m_pCenter = NULL;	// 中心の情報
	memset(&m_signboard, 0, sizeof(m_signboard));	// 看板配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditSignboard::~CEditSignboard()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditSignboard::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;		// エディットステージの情報
	m_pCenter = NULL;	// 中心の情報
	m_signboard.pSignboard = NULL;				// 看板情報
	m_signboard.type = CSignboard::TYPE_LEFT;	// 看板種類

	// 中心情報の生成
	m_pCenter = CObjectMeshCube::Create
	( // 引数
		VEC3_ZERO,								// 位置
		VEC3_ZERO,								// 向き
		center::SIZE,							// 大きさ
		center::COL,							// キューブ色
		XCOL_WHITE,								// 縁取り色
		CObjectMeshCube::BORDER_OFF,			// 縁取り状態
		0.0f,									// 縁取り太さ
		CObjectMeshCube::TEXSTATE_ONE,			// テクスチャ状態
		CObjectMeshCube::ETexState(NONE_IDX),	// テクスチャ種類
		CObjectMeshCube::ORIGIN_DOWN			// 原点
	);
	if (m_pCenter == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 看板の生成
	m_signboard.pSignboard = CSignboard::Create(m_signboard.type, VEC3_ZERO, VEC3_ZERO);
	if (m_signboard.pSignboard == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 色を設定
	D3DXCOLOR col = m_signboard.pSignboard->GetColor();	// 元の色を取得
	m_signboard.pSignboard->SetColor(D3DXCOLOR(col.r, col.g, col.b, INIT_ALPHA));

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
void CEditSignboard::Uninit(void)
{
#if _DEBUG

	if (m_pCenter != NULL)
	{ // 中心情報が使用されている場合

		// 中心情報の終了
		m_pCenter->Uninit();
	}
	else { assert(false); }	// 非使用中

#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditSignboard::Update(void)
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

	// 中心表示の更新
	UpdateCenter();

	// 看板の生成
	CreateSignboard();

	// 看板の破棄
	ReleaseSignboard();

	// 位置を反映
	m_signboard.pSignboard->SetVec3Position(m_pEdit->GetVec3Position());

	// 向きを反映
	m_signboard.pSignboard->SetVec3Rotation(m_pEdit->GetVec3Rotation());

	// 種類を反映
	m_signboard.pSignboard->SetType(m_signboard.type);

#endif	// _DEBUG
}

//============================================================
//	表示の設定処理
//============================================================
void CEditSignboard::SetDisp(const bool bDisp)
{
	// 看板の自動更新・自動描画を表示状況に合わせる
	m_signboard.pSignboard->SetEnableUpdate(bDisp);	// 更新
	m_signboard.pSignboard->SetEnableDraw(bDisp);	// 描画

	// 中心表示の自動更新・自動描画を表示状況に合わせる
	m_pCenter->SetEnableUpdate(bDisp);	// 更新
	m_pCenter->SetEnableDraw(bDisp);	// 描画

	if (bDisp)
	{ // 表示ONの場合

		// 位置を反映
		m_signboard.pSignboard->SetVec3Position(m_pEdit->GetVec3Position());
	}
	else
	{ // 表示OFFの場合

		// 看板の色の全初期化
		InitAllColorSignboard();

		// 位置をステージの範囲外に設定
		D3DXVECTOR3 outLimit = D3DXVECTOR3(0.0f, 0.0f, CScene::GetStage()->GetStageLimit().fNear - m_signboard.pSignboard->GetVec3Sizing().z);
		m_signboard.pSignboard->SetVec3Position(outLimit);
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditSignboard::DrawDebugControl(void)
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
void CEditSignboard::DrawDebugInfo(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "%d：[種類]\n", m_signboard.type);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f：[拡大率]\n", m_signboard.pSignboard->GetScale());
}

//============================================================
//	保存処理
//============================================================
void CEditSignboard::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	看板の配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_SIGNBOARDSET\n\n");

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
	
					if (pObjCheck->GetLabel() != CObject::LABEL_SIGNBOARD)
					{ // オブジェクトラベルが看板ではない場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}
	
					if (pObjCheck == (CObject*)m_signboard.pSignboard)
					{ // 同じアドレスだった場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// 看板の情報を取得
					D3DXVECTOR3 posSign = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 rotSign = pObjCheck->GetVec3Rotation();	// 向き
					D3DXCOLOR colSign = pObjCheck->GetColor();	// 色
					int nType = pObjCheck->GetType();			// 種類
					float fScale = pObjCheck->GetScale();		// 拡大率
	
					// 情報を書き出し
					fprintf(pFile, "	SIGNBOARDSET\n");
					fprintf(pFile, "		TYPE = %d\n", nType);
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posSign.x, posSign.y, posSign.z);
					fprintf(pFile, "		ROT = %.2f %.2f %.2f\n", rotSign.x, rotSign.y, rotSign.z);
					fprintf(pFile, "		COL = %.2f %.2f %.2f %.2f\n", colSign.r, colSign.g, colSign.b, colSign.a);
					fprintf(pFile, "		SCALE = %.2f\n", fScale);
					fprintf(pFile, "	END_SIGNBOARDSET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_SIGNBOARDSET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditSignboard *CEditSignboard::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditSignboard *pEditSignboard = NULL;	// エディット看板生成用

	if (pEditSignboard == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditSignboard = new CEditSignboard;	// エディット看板
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditSignboard != NULL)
	{ // 使用されている場合
		
		// エディット看板の初期化
		if (FAILED(pEditSignboard->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditSignboard;
			pEditSignboard = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditSignboard->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditSignboard;
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
HRESULT CEditSignboard::Release(CEditSignboard *&prEditSignboard)
{
#if _DEBUG

	if (prEditSignboard != NULL)
	{ // 使用中の場合

		// エディット看板の終了
		prEditSignboard->Uninit();

		// メモリ開放
		delete prEditSignboard;
		prEditSignboard = NULL;

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
void CEditSignboard::UpdateChangeType(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 種類を変更
	if (m_pKeyboard->IsTrigger(KEY_TYPE))
	{
		m_signboard.type = (CSignboard::EType)((m_signboard.type + 1) % CSignboard::TYPE_MAX);
	}
}

//============================================================
//	拡大率変更の更新処理
//============================================================
void CEditSignboard::UpdateChangeScale(void)
{
	// 変数を宣言
	float fScale = m_signboard.pSignboard->GetScale();	// 看板拡大率

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

	// 看板拡大率を反映
	m_signboard.pSignboard->SetScale(fScale);
}

//============================================================
//	中心表示の更新処理
//============================================================
void CEditSignboard::UpdateCenter(void)
{
	// 変数を宣言
	D3DXVECTOR3 posCenter = VEC3_ZERO;					// 中心位置
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き

	// 中心位置を計算
	posCenter.x = posEdit.x + sinf(rotEdit.y + D3DX_PI) * center::ADD_SIDE;
	posCenter.y = posEdit.y - center::SUB_DOWN;
	posCenter.z = posEdit.z + cosf(rotEdit.y + D3DX_PI) * center::ADD_SIDE;

	// 中心位置を指定
	m_pCenter->SetVec3Position(posCenter);
}

//============================================================
//	看板の生成処理
//============================================================
void CEditSignboard::CreateSignboard(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き
	D3DXCOLOR colSign = XCOL_WHITE;	// 色保存用
	float fScale = 0.0f;	// 拡大率

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 看板を配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		//----------------------------------------------------
		//	看板の情報を配置用に変更
		//----------------------------------------------------
		// 自動更新・自動描画をONにする
		m_signboard.pSignboard->SetEnableUpdate(true);
		m_signboard.pSignboard->SetEnableDraw(true);

		// 色を設定
		colSign = m_signboard.pSignboard->GetColor();	// 元の色を取得
		m_signboard.pSignboard->SetColor(D3DXCOLOR(colSign.r, colSign.g, colSign.b, 1.0f));

		// 拡大率を保存
		fScale = m_signboard.pSignboard->GetScale();

		// 未保存を設定
		m_pEdit->UnSave();

		//----------------------------------------------------
		//	新しい看板の生成
		//----------------------------------------------------
		// 看板の生成
		m_signboard.pSignboard = CSignboard::Create(m_signboard.type, posEdit, rotEdit);
		assert(m_signboard.pSignboard != NULL);

		// 色を設定
		colSign = m_signboard.pSignboard->GetColor();	// 元の色を取得
		m_signboard.pSignboard->SetColor(D3DXCOLOR(colSign.r, colSign.g, colSign.b, INIT_ALPHA));

		// 拡大率を設定
		m_signboard.pSignboard->SetScale(fScale);
	}
}

//============================================================
//	看板の破棄処理
//============================================================
void CEditSignboard::ReleaseSignboard(void)
{
	// 変数を宣言
	bool bRelease = false;	// 破棄状況

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 看板を削除
	if (m_pKeyboard->IsTrigger(KEY_RELEASE))
	{
		// 破棄する状態を設定
		bRelease = true;
	}

	// 看板の削除判定
	DeleteCollisionSignboard(bRelease);
}

//============================================================
//	看板の削除判定
//============================================================
void CEditSignboard::DeleteCollisionSignboard(const bool bRelase)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();				// エディットの位置
	D3DXVECTOR3 sizeEdit = m_signboard.pSignboard->GetVec3Sizing();	// エディット看板の大きさ

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
				D3DXVECTOR3 posSign = VEC3_ZERO;	// 看板位置
				D3DXVECTOR3 sizeSign = VEC3_ZERO;	// 看板大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_SIGNBOARD)
				{ // オブジェクトラベルが看板ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_signboard.pSignboard)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// 看板の位置を取得
				posSign = pObjCheck->GetVec3Position();

				// 看板の大きさを取得
				sizeSign = pObjCheck->GetVec3Sizing();

				// 球体の当たり判定
				if (collision::Circle3D
				( // 引数
					posEdit,							// 判定位置
					posSign,							// 判定目標位置
					(sizeSign.x + sizeSign.z) * 0.5f,	// 判定半径
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
//	看板の色の全初期化処理
//============================================================
void CEditSignboard::InitAllColorSignboard(void)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_SIGNBOARD)
				{ // オブジェクトラベルが看板ではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_signboard.pSignboard)
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
