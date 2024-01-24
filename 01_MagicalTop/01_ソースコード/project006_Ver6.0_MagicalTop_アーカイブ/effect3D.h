//============================================================
//
//	エフェクト3Dヘッダー [effect3D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EFFECT3D_H_
#define _EFFECT3D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectBillboard.h"

//************************************************************
//	クラス定義
//************************************************************
// エフェクト3Dクラス
class CEffect3D : public CObjectBillboard
{
public:
	// 種類列挙
	typedef enum
	{
		TYPE_NORMAL = 0,	// 通常テクスチャ
		TYPE_BUBBLE,		// バブルテクスチャ
		TYPE_SMOKE,			// 煙テクスチャ
		TYPE_HEAL,			// 回復テクスチャ
		TYPE_LEAF,			// 葉っぱテクスチャ
		TYPE_PIECE_S,		// かけらテクスチャ (小)
		TYPE_PIECE_M,		// かけらテクスチャ (中)
		TYPE_PIECE_L,		// かけらテクスチャ (大)
		TYPE_MAX			// この列挙型の総数
	}TYPE;

	// コンストラクタ
	CEffect3D();
	CEffect3D(const TYPE type, const LABEL label);

	// デストラクタ
	~CEffect3D();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CEffect3D *Create	// 生成
	( // 引数
		const TYPE type,					// テクスチャ
		const D3DXVECTOR3& rPos,			// 位置
		const D3DXVECTOR3& rMove,			// 移動量
		const D3DXVECTOR3& rRot,			// 向き
		const D3DXCOLOR& rCol,				// 色
		const int nLife,					// 寿命
		const float fRadius,				// 半径
		const float fSubSize,				// 半径の減算量
		const bool bAdd = true,				// 加算合成状況
		const LABEL label = LABEL_EFFECT3D	// オブジェクトラベル
	);

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	D3DXVECTOR3	m_move;		// 移動量
	const TYPE	m_type;		// 種類定数
	int		m_nLife;		// 寿命
	float	m_fSubSize;		// 大きさの減算量
	float	m_fSubAlpha;	// 透明度の減算量
	bool	m_bAdd;			// 加算合成状況
};

#endif	// _EFFECT3D_H_
