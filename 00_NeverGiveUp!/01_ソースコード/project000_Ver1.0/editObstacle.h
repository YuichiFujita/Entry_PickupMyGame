//============================================================
//
//	エディット障害物ヘッダー [editObstacle.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_OBSTACLE_H_
#define _EDIT_OBSTACLE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "obstacle.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス
class CObjectMeshCube;		// オブジェクトメッシュキューブクラス

//************************************************************
//	クラス定義
//************************************************************
// エディット障害物クラス
class CEditObstacle
{
public:
	// コンストラクタ
	CEditObstacle();

	// デストラクタ
	~CEditObstacle();

	// 障害物情報構造体
	struct SInfo
	{
		CObstacle *pObstacle;	// 障害物情報
		CObstacle::EType type;	// 障害物種類
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
	static CEditObstacle *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditObstacle *&prEditObstacle);	// 破棄

private:
	// メンバ関数
	void UpdateCollSize(void);		// 判定大きさの更新
	void UpdateChangeType(void);	// 種類変更の更新
	void UpdateChangeState(void);	// 特殊状態変更の更新
	void UpdateChangeDodge(void);	// 回避法変更の更新
	void CreateObstacle(void);		// 障害物生成
	void ReleaseObstacle(void);		// 障害物破棄

	void DeleteCollisionObstacle(const bool bRelase);	// 障害物の削除判定
	void InitAllColorObstacle(void);					// 障害物の色全初期化

	void SaveObstacle(void);	// 障害物保存
	void Save(void);			// 保存

	// メンバ変数
	CEditStageManager *m_pEdit;		// エディットステージの情報
	CObjectMeshCube *m_pCollision;	// 当たり判定の情報
	SInfo m_obstacle;	// 障害物配置情報
};

#endif	// _EDIT_OBSTACLE_H_
