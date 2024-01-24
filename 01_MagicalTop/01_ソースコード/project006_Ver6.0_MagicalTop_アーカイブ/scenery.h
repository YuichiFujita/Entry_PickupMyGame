//============================================================
//
//	景色ヘッダー [scenery.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SCENERY_H_
#define _SCENERY_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectMeshCylinder.h"

//************************************************************
//	クラス定義
//************************************************************
// 景色クラス
class CScenery : public CObjectMeshCylinder
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_NORMAL = 0,	// 通常テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// コンストラクタ
	CScenery();

	// デストラクタ
	~CScenery();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CScenery *Create	// 生成
	( // 引数
		const TEXTURE texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXCOLOR& rCol,		// 色
		const POSGRID2& rPart,		// 分割数
		const float fRadius,		// 半径
		const float fHeight,		// 縦幅
		const D3DCULL cull = D3DCULL_CCW,	// カリング状況
		const bool bLight = true			// ライティング状況
	);

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
};

#endif	// _SCENERY_H_
