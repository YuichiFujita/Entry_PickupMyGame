//============================================================
//
//	敵方向表示ヘッダー [enemyWay.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_WAY_H_
#define _ENEMY_WAY_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object3D.h"

//************************************************************
//	クラス定義
//************************************************************
// 敵方向表示クラス
class CEnemyWay : public CObject3D
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_WAY = 0,	// 方向表示
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// 状態列挙
	typedef enum
	{
		STATE_SPAWN = 0,	// 生成状態
		STATE_NORMAL,		// 通常状態
		STATE_DELETE,		// 破棄状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CEnemyWay(const float fMaxLimitRadius);

	// デストラクタ
	~CEnemyWay();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CEnemyWay *Create(CObject *pObject, const float fMaxLimitRadius);	// 生成

	// メンバ関数
	void Delete(void);	// 削除
	void SetObject(CObject *pObject);	// オブジェクト設定

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	CObject *m_pObject;	// 方向表示オブジェクト
	STATE m_state;		// 状態
	const float m_fMaxLimitRadius;	// オブジェクトの最大制限位置
};

#endif	// _ENEMY_WAY_H_
