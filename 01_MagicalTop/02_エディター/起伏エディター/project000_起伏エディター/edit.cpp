//============================================================
//
//	エディット処理 [edit.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "edit.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "collision.h"
#include "debugproc.h"
#include "player.h"
#include "field.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SAVE_TXT	"data\\TXT\\save.txt"	// セーブテキスト相対パス

#define INIT_BRUSH_RADIUS	(100.0f)	// 筆の太さの初期値
#define INIT_VTX_UP			(2.0f)		// 頂点上昇量の初期値

#define CHANGE_RAD	(1.0f)		// 筆の太さ変更量
#define MIN_RAD		(10.0f)		// 筆の太さの最小値
#define MAX_RAD		(1000.0f)	// 筆の太さの最大値
#define CHANGE_UP	(0.05f)		// 上昇量変更量
#define MIN_UP		(0.5f)		// 上昇量の最小値
#define MAX_UP		(50.0f)		// 上昇量の最大値

//************************************************************
//	親クラス [CEdit] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CEdit::CEdit()
{
	// メンバ変数をクリア
	m_fBrushRadius = 0.0f;	// 筆の太さ
	m_fVtxUp = 0.0f;		// 頂点の上昇量
	m_bAutoNor = false;		// 法線の自動設定状況
}

//============================================================
//	デストラクタ
//============================================================
CEdit::~CEdit()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CEdit::Init(void)
{
	// メンバ変数を初期化
	m_fBrushRadius = INIT_BRUSH_RADIUS;	// 筆の太さ
	m_fVtxUp = INIT_VTX_UP;				// 頂点の上昇量
	m_bAutoNor = false;					// 法線の自動設定状況

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CEdit::Uninit(void)
{

}

//============================================================
//	更新処理
//============================================================
void CEdit::Update(void)
{
	// ポインタを宣言
	CInputKeyboard *pKey = CManager::GetKeyboard();	// キーボード情報

	// 操作説明表示
	DrawManual();

	// ブラシ設定
	SetBursh();

	// 上昇量設定
	SetVtxUp();

	// 頂点設定
	SetVtx();

	// 法線設定
	SetNor();

	if (pKey->GetPress(DIK_F2))
	{ // [F2] が押された場合

		// 起伏の保存
		Save();
	}
	if (pKey->GetPress(DIK_F3))
	{ // [F3] が押された場合

		// 起伏の読込
		Load();
	}
}

//============================================================
//	生成処理
//============================================================
CEdit *CEdit::Create(void)
{
	// ポインタを宣言
	CEdit *pEdit = NULL;	// エディット生成用

	if (UNUSED(pEdit))
	{ // 使用されていない場合

		// メモリを確保
		pEdit = new CEdit;	// エディット
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pEdit))
	{ // 確保に成功している場合

		// エディットの初期化
		if (FAILED(pEdit->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pEdit;
			pEdit = NULL;

			// 失敗を返す
			return NULL;
		}

		// 確保したアドレスを返す
		return pEdit;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CEdit::Release(CEdit *&prEdit)
{
	if (USED(prEdit))
	{ // 使用中の場合

		// エディットの終了
		prEdit->Uninit();

		// メモリ開放
		delete prEdit;
		prEdit = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
}

//============================================================
//	ブラシ設定処理
//============================================================
void CEdit::SetBursh(void)
{
	// ポインタを宣言
	CInputKeyboard *pKey = CManager::GetKeyboard();	// キーボード情報

	if (pKey->GetPress(DIK_3))
	{ // [3] が押された場合

		// 筆の太さを加算
		m_fBrushRadius += CHANGE_RAD;
	}
	if (pKey->GetPress(DIK_4))
	{ // [4] が押された場合

		// 筆の太さを減算
		m_fBrushRadius -= CHANGE_RAD;
	}

	// 筆の太さの補正
	useful::LimitNum(m_fBrushRadius, MIN_RAD, MAX_RAD);
}

//============================================================
//	上昇量設定処理
//============================================================
void CEdit::SetVtxUp(void)
{
	// ポインタを宣言
	CInputKeyboard *pKey = CManager::GetKeyboard();	// キーボード情報

	if (pKey->GetPress(DIK_5))
	{ // [5] が押された場合

		// 上昇量を加算
		m_fVtxUp += CHANGE_UP;
	}
	if (pKey->GetPress(DIK_6))
	{ // [6] が押された場合

		// 上昇量を減算
		m_fVtxUp -= CHANGE_UP;
	}

	// 上昇量の補正
	useful::LimitNum(m_fVtxUp, MIN_UP, MAX_UP);
}

//============================================================
//	頂点設定処理
//============================================================
void CEdit::SetVtx(void)
{
	// ポインタを宣言
	CInputKeyboard *pKey = CManager::GetKeyboard();	// キーボード情報
	CPlayer *pPlayer = CManager::GetPlayer();		// プレイヤー情報
	CField *pField = CManager::GetField();			// フィールド情報

	if (pKey->GetPress(DIK_0) || pKey->GetPress(DIK_9))
	{ // [0 or 9] が押された場合

		// 変数を宣言
		D3DXVECTOR3 posVtx;	// 頂点位置
		D3DXVECTOR3 posPlayer = pPlayer->GetPosition();	// プレイヤー位置
		int nNumVtx = pField->GetNumVertex();			// 頂点数
		int nUp = (pKey->GetPress(DIK_0)) ? 1 : -1;		// 上昇量の計算用

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // 頂点数分繰り返す

			// 頂点位置を取得
			posVtx = pField->GetMeshVertexPosition(nCntVtx);

			if (collision::Circle2D(posPlayer, posVtx, m_fBrushRadius, 0.0f))
			{ // 範囲内の場合

				// 変数を宣言
				float fLength = sqrtf(((posPlayer.x - posVtx.x) * (posPlayer.x - posVtx.x)) + ((posPlayer.z - posVtx.z) * (posPlayer.z - posVtx.z)) * 0.5f);	// 頂点との距離
				float fVtxUp = (m_fVtxUp * -((fLength / m_fBrushRadius) - 1.0f)) * nUp;	// 頂点上昇量

				// 頂点の高さを変更
				pField->SetMeshVertexPosition(nCntVtx, D3DXVECTOR3(posVtx.x, posVtx.y + fVtxUp, posVtx.z));
			}
		}
	}
}

//============================================================
//	法線設定処理
//============================================================
void CEdit::SetNor(void)
{
	// ポインタを宣言
	CInputKeyboard *pKey = CManager::GetKeyboard();	// キーボード情報
	CField *pField = CManager::GetField();			// フィールド情報

	if (pKey->GetTrigger(DIK_1))
	{ // [1] が押された場合

		// 自動正規化状況を切り替える
		m_bAutoNor = (m_bAutoNor) ? false : true;
	}

	if (m_bAutoNor)
	{ // 自動正規化がONの場合

		// 法線の設定・正規化
		pField->NormalizeNormal();
	}
	else
	{ // 自動正規化がOFFの場合

		if (pKey->GetTrigger(DIK_2))
		{ // [2] が押された場合

			// 法線の設定・正規化
			pField->NormalizeNormal();	
		}
	}
}

//============================================================
//	操作説明表示処理
//============================================================
void CEdit::DrawManual(void)
{
	// ポインタを宣言
	CDebugProc *pDebug = CManager::GetDebugProc();	// デバッグ表示情報

	// 操作説明を表示
	pDebug->Print("========================================\n");
	pDebug->Print("【保存】[F2]\n");
	pDebug->Print("【読込】[F3]\n");
	pDebug->Print("========================================\n");
	pDebug->Print("【頂点上昇】[0]\n");
	pDebug->Print("【頂点下降】[9]\n");
	pDebug->Print("【自動法線 ON/OFF】[1]\n");
	pDebug->Print("【法線の正規化】   [2]\n");
	pDebug->Print("【筆太さ増加】[3]\n");
	pDebug->Print("【筆太さ減少】[4]\n");
	pDebug->Print("【上昇量増加】[5]\n");
	pDebug->Print("【上昇量減少】[6]\n");
	pDebug->Print("========================================\n");

	// 法線の自動正規化状況を表示
	if (m_bAutoNor)
	{ // 自動正規化がONの場合
		pDebug->Print("【自動法線】ON\n");
	}
	else
	{ // 自動正規化がOFFの場合
		pDebug->Print("【自動法線】OFF\n");
	}

	// 筆の太さ表示
	pDebug->Print("【筆太さ】%f\n", m_fBrushRadius);

	// 上昇量表示
	pDebug->Print("【上昇量】%f\n", m_fVtxUp);
}

//============================================================
//	保存処理
//============================================================
void CEdit::Save(void)
{
	// 変数を宣言
	POSGRID2 part = GRID2_ONE;	// 分割数

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ
	CField *pField = CManager::GetField();	// フィールド情報

	// ファイルを書き込み形式で開く
	pFile = fopen(SAVE_TXT, "w");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		// 見出し
		fprintf(pFile, "#==============================================================================\n");
		fprintf(pFile, "#\n");
		fprintf(pFile, "#	地形保存テキスト [save.txt]\n");
		fprintf(pFile, "#	Author：you\n");
		fprintf(pFile, "#\n");
		fprintf(pFile, "#==============================================================================\n");
		fprintf(pFile, "<>*************<> ここから下をコピーし [field.txt] に張り付け <>*************<>\n");
		fprintf(pFile, "\n");

		// 起伏情報の開始地点をテキストに書き出し
		fprintf(pFile, "TERRAINSET\n\n");
		fprintf(pFile, "	FIELDSET\n");

		// 分割数の書き出し
		part = pField->GetPattern();
		fprintf(pFile, "		# 分割数\n");
		fprintf(pFile, "		PART = %d %d\n\n", part.x, part.y);

		// 頂点座標のずれ量の書き出し
		fprintf(pFile, "		# 頂点座標のずれ量\n");
		fprintf(pFile, "		GAPSET\n");

		for (int nCntHeight = 0; nCntHeight < part.y + 1; nCntHeight++)
		{ // 縦の分割数 +1回繰り返す

			// 空白
			fprintf(pFile, "		");

			for (int nCntWidth = 0; nCntWidth < part.x + 1; nCntWidth++)
			{ // 横の分割数 +1回繰り返す

				// 変数を宣言
				int nNumVtx = nCntWidth + (nCntHeight * (part.x + 1));	// 現在の頂点番号
				D3DXVECTOR3 posGap = D3DXVECTOR3						// 頂点座標のずれ量
				( // 引数
					0.0f,
					-(pField->GetGapPosition(nNumVtx).y - pField->GetMeshVertexPosition(nNumVtx).y),
					0.0f
				);

				// 頂点座標のずれ量を書き出し
				fprintf(pFile, "%.3f %.3f %.3f , ", posGap.x, posGap.y, posGap.z);
			}

			// 改行
			fprintf(pFile, "\n");
		}

		// 起伏情報の終了地点をテキストに書き出し
		fprintf(pFile, "	END_FIELDSET\n\n");
		fprintf(pFile, "END_TERRAINSET\n");

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "起伏の書き出しに失敗！", "警告！", MB_ICONWARNING);
	}
}

//============================================================
//	読込処理
//============================================================
void CEdit::Load(void)
{
	// 変数を宣言
	POSGRID2 part = GRID2_ZERO;		// 分割数の代入用
	D3DXVECTOR3 pos = VEC3_ZERO;	// 頂点座標のずれ量の代入用
	int nEnd = 0;					// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ
	CField *pField = CManager::GetField();	// フィールド情報

	// ファイルを読み込み形式で開く
	pFile = fopen(SAVE_TXT, "r");

	if (pFile != NULL)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// 地形の設定
			if (strcmp(&aString[0], "TERRAINSET") == 0)
			{ // 読み込んだ文字列が TERRAINSET の場合

				do
				{ // 読み込んだ文字列が END_TERRAINSET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "FIELDSET") == 0)
					{ // 読み込んだ文字列が FIELDSET の場合

						do
						{ // 読み込んだ文字列が END_FIELDSET ではない場合ループ

							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);

							if (strcmp(&aString[0], "PART") == 0)
							{ // 読み込んだ文字列が PART の場合

								fscanf(pFile, "%s", &aString[0]);	// = を読み込む (不要)
								fscanf(pFile, "%d", &part.x);		// 分割数Xを読み込む
								fscanf(pFile, "%d", &part.y);		// 分割数Yを読み込む

								// 分割数の設定
								pField->SetPattern(part);
							}
							else if (strcmp(&aString[0], "GAPSET") == 0)
							{ // 読み込んだ文字列が GAPSET の場合

								for (int nCntVtx = 0; nCntVtx < pField->GetNumVertex(); nCntVtx++)
								{ // 頂点数分繰り返す

									fscanf(pFile, "%f", &pos.x);		// 頂点座標のずれ量Xを読み込む
									fscanf(pFile, "%f", &pos.y);		// 頂点座標のずれ量Yを読み込む
									fscanf(pFile, "%f", &pos.z);		// 頂点座標のずれ量Zを読み込む
									fscanf(pFile, "%s", &aString[0]);	// , を読み込む (不要)

									// 頂点座標のずれ量の設定
									pField->SetMeshVertexPosition(nCntVtx, pField->GetMeshVertexPosition(nCntVtx) + pos);
								}
							}
						} while (strcmp(&aString[0], "END_FIELDSET") != 0);	// 読み込んだ文字列が END_FIELDSET ではない場合ループ

						// 法線の設定・正規化
						pField->NormalizeNormal();
					}
				} while (strcmp(&aString[0], "END_TERRAINSET") != 0);	// 読み込んだ文字列が END_TERRAINSET ではない場合ループ
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(NULL, "起伏の読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
