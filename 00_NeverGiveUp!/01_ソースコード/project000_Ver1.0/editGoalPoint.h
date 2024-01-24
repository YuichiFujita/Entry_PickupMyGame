//============================================================
//
//	エディットゴールポイントヘッダー [editGoalPoint.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_GOALPOINT_H_
#define _EDIT_GOALPOINT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "goalPoint.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス
class CObjectMeshCube;		// オブジェクトメッシュキューブクラス

//************************************************************
//	クラス定義
//************************************************************
// エディットゴールポイントクラス
class CEditGoalPoint
{
public:
	// コンストラクタ
	CEditGoalPoint();

	// デストラクタ
	~CEditGoalPoint();

	// ゴールポイント情報構造体
	struct SInfo
	{
		CGoalPoint *pGoalPoint;			// ゴールポイント情報
		CObjectMeshCube *pCollision;	// 当たり判定の情報
		bool bSet;						// 配置状況
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
	static CEditGoalPoint *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditGoalPoint *&prEditGoalPoint);	// 破棄

private:
	// メンバ関数
	void UpdateCollSize(void);	// 判定大きさの更新
	void CreateGoalPoint(void);	// ゴールポイント生成
	void UpdateSet(void);		// 設置予定位置の更新
	void UpdateColl(void);		// 当たり判定の更新

	// メンバ変数
	CEditStageManager *m_pEdit;	// エディットステージの情報
	CObjectMeshCube *m_pSet;	// 設置予定位置の情報
	SInfo m_goalPoint;			// ゴールポイント配置情報
};

#endif	// _EDIT_GOALPOINT_H_
