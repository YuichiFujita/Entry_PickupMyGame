//============================================================
//
//	風ヘッダー [wind.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WIND_H_
#define _WIND_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectOrbit.h"

//************************************************************
//	クラス定義
//************************************************************
// 風クラス
class CWind : public CObjectOrbit
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
		STATE_DELETE,		// 消失状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CWind();

	// デストラクタ
	~CWind();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CWind *Create(const D3DXVECTOR3& rPos);	// 生成

	// メンバ変数
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	D3DXMATRIX *GetPtrMtxWorld(void);			// マトリックスポインタ取得

private:
	// メンバ変数
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3	m_pos;		// 位置
	STATE m_state;			// 状態
};

#endif	// _WIND_H_
