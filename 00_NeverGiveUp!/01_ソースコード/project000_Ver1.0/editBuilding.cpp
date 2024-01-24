//============================================================
//
//	エディットビル処理 [editBuilding.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "editBuilding.h"
#include "manager.h"
#include "input.h"
#include "collision.h"
#include "editStageManager.h"
#include "stage.h"
#include "effect3D.h"

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
#define KEY_COLL		(DIK_3)	// 判定変更キー
#define NAME_COLL		("3")	// 判定変更表示

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
}

//************************************************************
//	親クラス [CEditBuilding] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEditBuilding::CEditBuilding()
{
#if _DEBUG

	// メンバ変数をクリア
	m_pEdit = NULL;	// エディットステージの情報
	memset(&m_building, 0, sizeof(m_building));	// ビル配置情報

#endif	// _DEBUG
}

//============================================================
//	デストラクタ
//============================================================
CEditBuilding::~CEditBuilding()
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	初期化処理
//============================================================
HRESULT CEditBuilding::Init(void)
{
#if _DEBUG

	// メンバ変数を初期化
	m_pEdit = NULL;	// エディットステージの情報

	m_building.pBuilding = NULL;					// ビル情報
	m_building.type = CBuilding::TYPE_00;			// ビル種類
	m_building.coll = CBuilding::COLLISION_GROUND;	// ビル判定

	// ビルの生成
	m_building.pBuilding = CBuilding::Create(m_building.type, VEC3_ZERO, VEC3_ZERO, m_building.coll);
	if (m_building.pBuilding == NULL)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 色を設定
	D3DXCOLOR col = m_building.pBuilding->GetColor();	// 元の色を取得
	m_building.pBuilding->SetColor(D3DXCOLOR(col.r, col.g, col.b, INIT_ALPHA));

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
void CEditBuilding::Uninit(void)
{
#if _DEBUG
#endif	// _DEBUG
}

//============================================================
//	更新処理
//============================================================
void CEditBuilding::Update(void)
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

	// 判定変更の更新
	UpdateChangeColl();

	// 拡大率変更の更新
	UpdateChangeScale();

	// 方向表示エフェクトの生成
	CreateRotaEffect();

	// ビルの生成
	CreateBuilding();

	// ビルの破棄
	ReleaseBuilding();

	// 位置を反映
	m_building.pBuilding->SetVec3Position(m_pEdit->GetVec3Position());

	// 向きを反映
	m_building.pBuilding->SetVec3Rotation(m_pEdit->GetVec3Rotation());

	// 種類を反映
	m_building.pBuilding->SetType(m_building.type);

	// 判定を反映
	m_building.pBuilding->SetState(m_building.coll);

#endif	// _DEBUG
}

//============================================================
//	表示の設定処理
//============================================================
void CEditBuilding::SetDisp(const bool bDisp)
{
	// 自動更新・自動描画を表示状況に合わせる
	m_building.pBuilding->SetEnableUpdate(bDisp);	// 更新
	m_building.pBuilding->SetEnableDraw(bDisp);		// 描画

	if (bDisp)
	{ // 表示ONの場合

		// 位置を反映
		m_building.pBuilding->SetVec3Position(m_pEdit->GetVec3Position());
	}
	else
	{ // 表示OFFの場合

		// ビルの色の全初期化
		InitAllColorBuilding();

		// 位置をステージの範囲外に設定
		D3DXVECTOR3 outLimit = D3DXVECTOR3(0.0f, 0.0f, CScene::GetStage()->GetStageLimit().fNear - m_building.pBuilding->GetVec3Sizing().z);
		m_building.pBuilding->SetVec3Position(outLimit);
	}
}

//============================================================
//	操作表示の描画処理
//============================================================
void CEditBuilding::DrawDebugControl(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報

	pDebug->Print(CDebugProc::POINT_RIGHT, "種類変更：[%s]\n", NAME_TYPE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "判定変更：[%s]\n", NAME_COLL);
	pDebug->Print(CDebugProc::POINT_RIGHT, "拡大率上昇：[%s]\n", NAME_SCALE_UP);
	pDebug->Print(CDebugProc::POINT_RIGHT, "拡大率下降：[%s]\n", NAME_SCALE_DOWN);
	pDebug->Print(CDebugProc::POINT_RIGHT, "削除：[%s]\n", NAME_RELEASE);
	pDebug->Print(CDebugProc::POINT_RIGHT, "設置：[%s]\n", NAME_CREATE);
}

//============================================================
//	情報表示の描画処理
//============================================================
void CEditBuilding::DrawDebugInfo(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetInstance()->GetDebugProc();	// デバッグプロックの情報
	static char* apColl[] = { "無し", "地面", "天井" };	// 判定

	// 判定数の不一致
	assert((sizeof(apColl) / sizeof(apColl[0])) == CBuilding::COLLISION_MAX);

	pDebug->Print(CDebugProc::POINT_RIGHT, "%d：[種類]\n", m_building.type);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%s：[判定]\n", apColl[m_building.coll]);
	pDebug->Print(CDebugProc::POINT_RIGHT, "%f：[拡大率]\n", m_building.pBuilding->GetScale());
}

//============================================================
//	保存処理
//============================================================
void CEditBuilding::Save(FILE *pFile)
{
#if _DEBUG

	if (pFile != NULL)
	{ // ファイルが存在する場合

		// 見出しを書き出し
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "#	ビルの配置情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		// 情報開始地点を書き出し
		fprintf(pFile, "STAGE_BUILDINGSET\n\n");

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
	
					if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
					{ // オブジェクトラベルがビルではない場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}
	
					if (pObjCheck == (CObject*)m_building.pBuilding)
					{ // 同じアドレスだった場合
	
						// 次のオブジェクトへのポインタを代入
						pObjCheck = pObjectNext;
	
						// 次の繰り返しに移行
						continue;
					}

					// ビルの情報を取得
					D3DXVECTOR3 posBuild = pObjCheck->GetVec3Position();	// 位置
					D3DXVECTOR3 rotBuild = pObjCheck->GetVec3Rotation();	// 向き
					int nType = pObjCheck->GetType();		// 種類
					int nCollision = pObjCheck->GetState();	// 当たり判定
					float fScale = pObjCheck->GetScale();	// 拡大率
	
					// 情報を書き出し
					fprintf(pFile, "	BUILDINGSET\n");
					fprintf(pFile, "		TYPE = %d\n", nType);
					fprintf(pFile, "		POS = %.2f %.2f %.2f\n", posBuild.x, posBuild.y, posBuild.z);
					fprintf(pFile, "		ROT = %.2f %.2f %.2f\n", rotBuild.x, rotBuild.y, rotBuild.z);
					fprintf(pFile, "		COLL = %d\n", nCollision);
					fprintf(pFile, "		SCALE = %.2f\n", fScale);
					fprintf(pFile, "	END_BUILDINGSET\n\n");

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;
				}
			}
		}

		// 情報終了地点を書き出し
		fprintf(pFile, "END_STAGE_BUILDINGSET\n\n");
	}

#endif	// _DEBUG
}

//============================================================
//	生成処理
//============================================================
CEditBuilding *CEditBuilding::Create(CEditStageManager *pEdit)
{
#if _DEBUG

	// ポインタを宣言
	CEditBuilding *pEditBuilding = NULL;	// エディットビル生成用

	if (pEditBuilding == NULL)
	{ // 使用されていない場合

		// メモリ確保
		pEditBuilding = new CEditBuilding;	// エディットビル
	}
	else { assert(false); return NULL; }	// 使用中

	if (pEditBuilding != NULL)
	{ // 使用されている場合
		
		// エディットビルの初期化
		if (FAILED(pEditBuilding->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEditBuilding;
			pEditBuilding = NULL;

			// 失敗を返す
			return NULL;
		}

		// エディットステージの情報を設定
		pEditBuilding->m_pEdit = pEdit;

		// 確保したアドレスを返す
		return pEditBuilding;
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
HRESULT CEditBuilding::Release(CEditBuilding *&prEditBuilding)
{
#if _DEBUG

	if (prEditBuilding != NULL)
	{ // 使用中の場合

		// エディットビルの終了
		prEditBuilding->Uninit();

		// メモリ開放
		delete prEditBuilding;
		prEditBuilding = NULL;

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
void CEditBuilding::UpdateChangeType(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 種類を変更
	if (m_pKeyboard->IsTrigger(KEY_TYPE))
	{
		m_building.type = (CBuilding::EType)((m_building.type + 1) % CBuilding::TYPE_MAX);
	}
}

//============================================================
//	判定変更の更新処理
//============================================================
void CEditBuilding::UpdateChangeColl(void)
{
	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// 判定を変更
	if (m_pKeyboard->IsTrigger(KEY_COLL))
	{
		m_building.coll = (CBuilding::ECollision)((m_building.coll + 1) % CBuilding::COLLISION_MAX);
	}
}

//============================================================
//	拡大率変更の更新処理
//============================================================
void CEditBuilding::UpdateChangeScale(void)
{
	// 変数を宣言
	float fScale = m_building.pBuilding->GetScale();	// ビル拡大率

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

	// ビル拡大率を反映
	m_building.pBuilding->SetScale(fScale);
}

//============================================================
//	ビルの生成処理
//============================================================
void CEditBuilding::CreateBuilding(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();	// エディットの位置
	D3DXVECTOR3 rotEdit = m_pEdit->GetVec3Rotation();	// エディットの向き
	D3DXCOLOR colBuild = XCOL_WHITE;	// 色保存用
	float fScale = 0.0f;	// 拡大率

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// ビルを配置
	if (m_pKeyboard->IsTrigger(KEY_CREATE))
	{
		//----------------------------------------------------
		//	ビルの情報を配置用に変更
		//----------------------------------------------------
		// 自動更新・自動描画をONにする
		m_building.pBuilding->SetEnableUpdate(true);
		m_building.pBuilding->SetEnableDraw(true);

		// 色を設定
		colBuild = m_building.pBuilding->GetColor();	// 元の色を取得
		m_building.pBuilding->SetColor(D3DXCOLOR(colBuild.r, colBuild.g, colBuild.b, 1.0f));

		// 拡大率を保存
		fScale = m_building.pBuilding->GetScale();

		// 未保存を設定
		m_pEdit->UnSave();

		//----------------------------------------------------
		//	新しいビルの生成
		//----------------------------------------------------
		// ビルの生成
		m_building.pBuilding = CBuilding::Create(m_building.type, posEdit, rotEdit, m_building.coll);
		assert(m_building.pBuilding != NULL);

		// 色を設定
		colBuild = m_building.pBuilding->GetColor();	// 元の色を取得
		m_building.pBuilding->SetColor(D3DXCOLOR(colBuild.r, colBuild.g, colBuild.b, INIT_ALPHA));

		// 拡大率を設定
		m_building.pBuilding->SetScale(fScale);
	}
}

//============================================================
//	ビルの破棄処理
//============================================================
void CEditBuilding::ReleaseBuilding(void)
{
	// 変数を宣言
	bool bRelease = false;	// 破棄状況

	// ポインタを宣言
	CInputKeyboard *m_pKeyboard = CManager::GetInstance()->GetKeyboard();	// キーボード情報

	// ビルを削除
	if (m_pKeyboard->IsTrigger(KEY_RELEASE))
	{
		// 破棄する状態を設定
		bRelease = true;
	}

	// ビルの削除判定
	DeleteCollisionBuilding(bRelease);
}

//============================================================
//	方向表示エフェクトの生成処理
//============================================================
void CEditBuilding::CreateRotaEffect(void)
{
	// 変数を宣言
	D3DXVECTOR3 posEffect = VEC3_ZERO;								// エフェクト位置
	D3DXVECTOR3 posEdit   = m_pEdit->GetVec3Position();				// エディットの位置
	D3DXVECTOR3 rotEdit   = m_pEdit->GetVec3Rotation();				// エディットの向き
	D3DXVECTOR3 sizeBuild = m_building.pBuilding->GetVec3Sizing();	// ビル大きさ
	float fAverageSizeBuild = (sizeBuild.x + sizeBuild.z) * 0.5f;	// ビル大きさ平均

	// エフェクト位置を計算
	posEffect.x = posEdit.x + sinf(rotEdit.y + D3DX_PI) * fAverageSizeBuild;
	posEffect.y = posEdit.y + sizeBuild.y * 2.0f;
	posEffect.z = posEdit.z + cosf(rotEdit.y + D3DX_PI) * fAverageSizeBuild;

	// 方向表示エフェクトを生成
	CEffect3D::Create(posEffect, EFFECT_RADIUS, CEffect3D::TYPE_NORMAL, EFFECT_LIFE);
}

//============================================================
//	ビルの削除判定
//============================================================
void CEditBuilding::DeleteCollisionBuilding(const bool bRelase)
{
	// 変数を宣言
	D3DXVECTOR3 posEdit = m_pEdit->GetVec3Position();				// エディットの位置
	D3DXVECTOR3 sizeEdit = m_building.pBuilding->GetVec3Sizing();	// エディットビルの大きさ

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
				D3DXVECTOR3 posBuild = VEC3_ZERO;	// ビル位置
				D3DXVECTOR3 sizeBuild = VEC3_ZERO;	// ビル大きさ

				// ポインタを宣言
				CObject *pObjectNext = pObjCheck->GetNext();	// 次オブジェクト

				if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
				{ // オブジェクトラベルがビルではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_building.pBuilding)
				{ // 同じアドレスだった場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				// ビルの位置を取得
				posBuild = pObjCheck->GetVec3Position();

				// ビルの大きさを取得
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
//	ビルの色の全初期化処理
//============================================================
void CEditBuilding::InitAllColorBuilding(void)
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

				if (pObjCheck->GetLabel() != CObject::LABEL_BUILDING)
				{ // オブジェクトラベルがビルではない場合

					// 次のオブジェクトへのポインタを代入
					pObjCheck = pObjectNext;

					// 次の繰り返しに移行
					continue;
				}

				if (pObjCheck == (CObject*)m_building.pBuilding)
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
