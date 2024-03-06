//============================================================
//
//	球体判定デバッグ表示ヘッダー [debugCollSphere.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _DEBUG_COLLSPHERE_H_
#define _DEBUG_COLLSPHERE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshSphere.h"

//************************************************************
//	定数宣言
//************************************************************
namespace debugCollSphere
{
	const D3DXVECTOR3 ROT		= VEC3_ZERO;	// 向き
	const D3DXCOLOR DEFAULT_COL	= D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.3f);	// 色
	const D3DXCOLOR CHOICE_COL	= D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.3f);	// 色

	const POSGRID2	PART	= POSGRID2(8, 4);	// 分割数
	const POSGRID2	TEXPART	= POSGRID2(1, 1);	// テクスチャ分割数
}

//************************************************************
//	クラス定義
//************************************************************
// 球体判定デバッグ表示クラス
class CDebugCollSphere : public CObjectMeshSphere
{
public:
	// コンストラクタ
	CDebugCollSphere();

	// デストラクタ
	~CDebugCollSphere() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CDebugCollSphere *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const float fRadius,		// 半径
		const D3DXCOLOR& rCol		= debugCollSphere::DEFAULT_COL,	// 色
		const D3DXVECTOR3& rRot		= debugCollSphere::ROT,			// 向き
		const POSGRID2& rPart		= debugCollSphere::PART,		// 分割数
		const POSGRID2& rTexPart	= debugCollSphere::TEXPART		// テクスチャ分割数
	);
};

#endif	// _DEBUG_COLLSPHERE_H_
