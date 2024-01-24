//============================================================
//
//	エフェクト2Dヘッダー [effect2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EFFECT2D_H_
#define _EFFECT2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object2D.h"

//************************************************************
//	クラス定義
//************************************************************
// エフェクト2Dクラス
class CEffect2D : public CObject2D
{
public:
	// 種類列挙
	typedef enum
	{
		TYPE_NORMAL = 0,	// 通常テクスチャ
		TYPE_MAX			// この列挙型の総数
	}TYPE;

	// コンストラクタ
	CEffect2D();
	CEffect2D(const TYPE type, const LABEL label);

	// デストラクタ
	~CEffect2D();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CEffect2D *Create	// 生成
	( // 引数
		const TYPE type,					// テクスチャ
		const D3DXVECTOR3& rPos,			// 位置
		const D3DXVECTOR3& rMove,			// 移動量
		const D3DXVECTOR3& rRot,			// 向き
		const D3DXCOLOR& rCol,				// 色
		const int nLife,					// 寿命
		const float fRadius,				// 半径
		const float fSubSize,				// 半径の減算量
		const float fSubAlpha,				// 透明度の減算量
		const LABEL label = LABEL_EFFECT2D	// オブジェクトラベル
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
};

#endif	// _EFFECT2D_H_
