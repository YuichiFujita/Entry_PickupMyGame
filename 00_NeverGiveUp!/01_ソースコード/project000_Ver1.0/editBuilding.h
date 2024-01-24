//============================================================
//
//	エディットビルヘッダー [editBuilding.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EDIT_BUILDING_H_
#define _EDIT_BUILDING_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "building.h"

//************************************************************
//	前方宣言
//************************************************************
class CEditStageManager;	// エディットステージマネージャークラス

//************************************************************
//	クラス定義
//************************************************************
// エディットビルクラス
class CEditBuilding
{
public:
	// コンストラクタ
	CEditBuilding();

	// デストラクタ
	~CEditBuilding();

	// ビル情報構造体
	struct SInfo
	{
		CBuilding *pBuilding;		// ビル情報
		CBuilding::EType type;		// ビル種類
		CBuilding::ECollision coll;	// ビル判定
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
	static CEditBuilding *Create(CEditStageManager *pEdit);	// 生成
	static HRESULT Release(CEditBuilding *&prEditBuilding);	// 破棄

private:
	// メンバ関数
	void UpdateChangeType(void);	// 種類変更の更新
	void UpdateChangeColl(void);	// 判定変更の更新
	void UpdateChangeScale(void);	// 拡大率変更の更新
	void CreateBuilding(void);		// ビル生成
	void ReleaseBuilding(void);		// ビル破棄
	void CreateRotaEffect(void);	// 方向表示エフェクト生成

	void DeleteCollisionBuilding(const bool bRelase);	// ビルの削除判定
	void InitAllColorBuilding(void);					// ビルの色全初期化

	// メンバ変数
	CEditStageManager *m_pEdit;	// エディットステージの情報
	SInfo m_building;	// ビル配置情報
};

#endif	// _EDIT_BUILDING_H_
