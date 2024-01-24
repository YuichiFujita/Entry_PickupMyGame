//============================================================
//
//	地面ヘッダー [field.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FIELD_H_
#define _FIELD_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectMeshField.h"
#include "waveManager.h"

//************************************************************
//	クラス定義
//************************************************************
// 地面クラス
class CField : public CObjectMeshField
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_SPRING = 0,	// 春地面テクスチャ
		TEXTURE_SUMMER,		// 夏地面テクスチャ
		TEXTURE_AUTUMN,		// 秋地面テクスチャ
		TEXTURE_WINTER,		// 冬地面テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// 地形列挙
	typedef enum
	{
		TERRAIN_120x120 = 0,	// 120x120分割数の地形
		TERRAIN_MAX				// この列挙型の総数
	}TERRAIN;

	// コンストラクタ
	CField();

	// デストラクタ
	~CField();

	// 地形情報構造体
	typedef struct
	{
		D3DXVECTOR3 *pPosGap;	// 頂点座標のずれ量
		POSGRID2 part;			// 分割数
	}TerrainInfo;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CField *Create	// 生成
	( // 引数
		const TEXTURE texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR2& rSize,	// 大きさ
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const D3DCULL cull = D3DCULL_CCW,	// カリング状況
		const bool bLight = true			// ライティング状況
	);

	// メンバ関数
	void SetTerrain(const TERRAIN terrain);	// 地形設定
	void SetSeason(const CWaveManager::SEASON season);	// 季節設定

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
	static TerrainInfo m_aTerrainInfo[TERRAIN_MAX];	// 地形情報
};

#endif	// _FIELD_H_
