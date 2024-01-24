//============================================================
//
//	経験値ヘッダー [expOrb.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _EXPORB_H_
#define _EXPORB_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectBillboard.h"

//************************************************************
//	クラス定義
//************************************************************
// 経験値クラス
class CExpOrb : public CObjectBillboard
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_NORMAL = 0,	// 経験値
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// コンストラクタ
	CExpOrb();

	// デストラクタ
	~CExpOrb();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CExpOrb *Create(const D3DXVECTOR3& rPos);	// 生成
	static void RandomSpawn(const int nNum, const D3DXVECTOR3& rPos);	// ランダム生成

private:
	// メンバ関数
	bool CollisionPlayer(void);	// プレイヤーとの当たり判定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
};

#endif	// _EXPORB_H_
