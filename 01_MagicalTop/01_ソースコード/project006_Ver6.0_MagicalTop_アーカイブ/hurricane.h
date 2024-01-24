//============================================================
//
//	ハリケーンヘッダー [hurricane.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _HURRICANE_H_
#define _HURRICANE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectTornado.h"

//************************************************************
//	クラス定義
//************************************************************
// ハリケーンクラス
class CHurricane : public CObjectTornado
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_NORMAL = 0,	// 通常テクスチャ
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CHurricane();

	// デストラクタ
	~CHurricane();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CHurricane *Create(const D3DXVECTOR3& rPos);	// 生成

private:
	// メンバ変数
	STATE m_state;	// 状態
};

#endif	// _HURRICANE_H_
