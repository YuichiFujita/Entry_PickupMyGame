//============================================================
//
//	出現警告表示ヘッダー [warningSpawn.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WARNING_SPAWN_H_
#define _WARNING_SPAWN_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object2D.h"

//************************************************************
//	クラス定義
//************************************************************
// 出現警告表示クラス
class CWarningSpawn : public CObject2D
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_WARNING = 0,	// 警告表示
		TEXTURE_MAX				// この列挙型の総数
	}TEXTURE;

	// 状態列挙
	typedef enum
	{
		STATE_NONE = 0,	// 何もしない状態
		STATE_FADEOUT,	// フェードアウト状態
		STATE_NORMAL,	// 通常状態
		STATE_FADEIN,	// フェードイン状態
		STATE_MAX		// この列挙型の総数
	}STATE;

	// コンストラクタ
	CWarningSpawn();

	// デストラクタ
	~CWarningSpawn();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CWarningSpawn *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rSize	// 大きさ
	);

	// メンバ関数
	void SetDisp(void);	// 表示設定
	void SetScalingOrigin(const D3DXVECTOR3& rSize);	// 元の大きさ設定

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	D3DXVECTOR3 m_size;		// 元の大きさ
	STATE m_state;			// 状態
	int m_nCounterState;	// 状態管理カウンター
	float m_fScale;			// 拡大率
};

#endif	// _WARNING_SPAWN_H_
