//============================================================
//
//	ステージヘッダー [stage.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _STAGE_H_
#define _STAGE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	前方宣言
//************************************************************
class CHurricane;	// ハリケーンクラス
class CObject3D;	// オブジェクト3Dクラス

//************************************************************
//	クラス定義
//************************************************************
// ステージクラス
class CStage
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_BARRIER = 0,	// バリア表示
		TEXTURE_AREA,			// エリア表示
		TEXTURE_MAX				// この列挙型の総数
	}TEXTURE;

	// エリア列挙
	typedef enum
	{
		AREA_NONE = -1,	// エリアなし
		AREA_SAFE,		// セーフエリア
		AREA_1,			// 第1エリア
		AREA_2,			// 第2エリア
		AREA_3,			// 第3エリア
		AREA_MAX		// エリアの総数
	}AREA;

	// 制限列挙
	typedef enum
	{
		LIMIT_BOX = 0,	// 矩形範囲
		LIMIT_CIRCLE,	// 円範囲
		LIMIT_MAX		// この列挙型の総数
	}LIMIT;

	// コンストラクタ
	CStage();

	// デストラクタ
	~CStage();

	// ステージ風構造体
	typedef struct
	{
		int nCounter;	// 風量変更カウンター
	}StageWind;

	// ステージエリア構造体
	typedef struct
	{
		D3DXCOLOR col;	// エリア色
		float fRadius;	// エリア半径
	}StageArea;

	// ステージ範囲構造体
	typedef struct
	{
		LIMIT mode;			// 制限モード
		D3DXVECTOR3 center;	// 中心座標
		float fNear;		// 制限位置 (前)
		float fFar;			// 制限位置 (後)
		float fRight;		// 制限位置 (右)
		float fLeft;		// 制限位置 (左)
		float fRadius;		// 制限位置 (半径)
		float fField;		// 制限位置 (地面)
	}StageLimit;

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新

	bool CollisionBarrier(D3DXVECTOR3& rPos, float fRadius);	// バリアとの当たり判定
	void LimitPosition(D3DXVECTOR3& rPos, const float fRadius);	// 位置補正
	bool LandPosition(D3DXVECTOR3& rPos, D3DXVECTOR3& rMove, const float fHeight);	// 範囲外着地

	void SetStageLimit(const StageLimit& rLimit);	// ステージ範囲設定
	StageLimit GetStageLimit(void) const;			// ステージ範囲取得

	void SetStageArea(const int nID, const StageArea& rArea);	// ステージエリア設定
	StageArea GetStageArea(const int nID) const;				// ステージエリア取得
	void SetEnableDrawArea(const bool bDraw);					// ステージエリア描画設定
	AREA GetAreaPlayer(void) const;								// プレイヤーの現在エリア取得

	void SetStageBarrier(const StageArea& rBarrier);	// ステージバリア設定
	StageArea GetStageBarrier(void) const;				// ステージバリア取得
	void SetEnableDrawBarrier(const bool bDraw);		// ステージバリア描画設定
	D3DXVECTOR3 GetStageBarrierPosition(void) const;	// ステージバリアの位置取得

	void SetEnableDrawHurricane(const bool bDraw);	// ハリケーン描画設定
	void SetEnebleCreateWind(const bool bCreate);	// 風生成設定

	// 静的メンバ関数
	static CStage *Create(void);				// 生成
	static HRESULT Release(CStage *&prStage);	// 破棄

private:
	// 静的メンバ関数
	static void LoadSetup(CStage *pStage);	// セットアップ

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	CHurricane	*m_pHurricane;			// ハリケーンの情報
	CObject3D	*m_pStageBarrier;		// バリア表示の情報
	CObject3D	*m_pStageArea;			// エリア表示の情報
	StageWind	m_stageWind;			// 風速
	StageArea	m_stageBarrier;			// バリア
	StageArea	m_aStageArea[AREA_MAX];	// エリア
	StageLimit	m_stageLimit;			// 範囲
	AREA m_area;	// プレイヤーの現在エリア
	bool m_bWind;	// 風生成状況
};

#endif	// _STAGE_H_
