//============================================================
//
//	エディットセーブポイントヘッダー [editSavePoint.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_SAVEPOINT_H_
#define _EDIT_SAVEPOINT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "savePoint.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス

//************************************************************
//	クラス定義
//************************************************************
// エディットセーブポイントクラス
class CEditSavePoint
{
public:
	// コンストラクタ
	CEditSavePoint();

	// デストラクタ
	~CEditSavePoint();

	// セーブポイント情報構造体
	struct SInfo
	{
		CSavePoint *pSavePoint;	// セーブポイント情報
	};

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void SetDisp(const bool bDisp);	// 表示設定
	void DrawDebugControl(void);	// 操作表示描画
	void DrawDebugInfo(void);		// 情報表示描画
	void Save(FILE *pFile);			// 保存

	// 静的メンバ関数
	static CEditSavePoint *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditSavePoint *&prEditSavePoint);	// 破棄

private:
	// メンバ関数
	void CreateSavePoint(void);		// セーブポイント生成
	void ReleaseSavePoint(void);	// セーブポイント破棄
	void CreateRotaEffect(void);	// 方向表示エフェクト生成

	void DeleteCollisionSavePoint(const bool bRelase);	// セーブポイントの削除判定
	void InitAllColorSavePoint(void);					// セーブポイントの色全初期化

	// メンバ変数
	CEditStageManager *m_pEdit;	// エディットステージの情報
	SInfo m_savePoint;	// セーブポイント配置情報
};

#endif	// _EDIT_SAVEPOINT_H_
