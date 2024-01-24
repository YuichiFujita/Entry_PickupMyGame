//============================================================
//
//	エディット看板ヘッダー [editSignboard.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_SIGNBOARD_H_
#define _EDIT_SIGNBOARD_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "signboard.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス
class CObjectMeshCube;		// オブジェクトメッシュキューブクラス

//************************************************************
//	クラス定義
//************************************************************
// エディット看板クラス
class CEditSignboard
{
public:
	// コンストラクタ
	CEditSignboard();

	// デストラクタ
	~CEditSignboard();

	// 看板情報構造体
	struct SInfo
	{
		CSignboard *pSignboard;	// 看板情報
		CSignboard::EType type;	// 看板種類
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
	static CEditSignboard *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditSignboard *&prEditSignboard);	// 破棄

private:
	// メンバ関数
	void UpdateChangeType(void);	// 種類変更の更新
	void UpdateChangeScale(void);	// 拡大率変更の更新
	void UpdateCenter(void);		// 中心表示の更新
	void CreateSignboard(void);		// 看板生成
	void ReleaseSignboard(void);	// 看板破棄

	void DeleteCollisionSignboard(const bool bRelase);	// 看板の削除判定
	void InitAllColorSignboard(void);					// 看板の色全初期化

	// メンバ変数
	CEditStageManager *m_pEdit;	// エディットステージの情報
	CObjectMeshCube *m_pCenter;	// 中心の情報
	SInfo m_signboard;	// 看板配置情報
};

#endif	// _EDIT_SIGNBOARD_H_
