//============================================================
//
//	マナフラワーヘッダー [flower.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FLOWER_H_
#define _FLOWER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object3D.h"
#include "waveManager.h"

//************************************************************
//	前方宣言
//************************************************************
class CShadow;	// 影クラス

//************************************************************
//	クラス定義
//************************************************************
// マナフラワークラス
class CFlower : public CObject3D
{
public:
	// 種類列挙
	typedef enum
	{
		TYPE_SPRING = 0,	// 春マナフラワーテクスチャ
		TYPE_SUMMER,		// 夏マナフラワーテクスチャ
		TYPE_AUTUMN,		// 秋マナフラワーテクスチャ
		TYPE_WINTER,		// 冬マナフラワーテクスチャ
		TYPE_MAX			// この列挙型の総数
	}TYPE;

	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_DAMAGE,		// ダメージ状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CFlower();

	// デストラクタ
	~CFlower();

	// ステータス構造体
	typedef struct
	{
		D3DXVECTOR3 size;	// 大きさ
		int nLife;			// 寿命
	}StatusInfo;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CFlower *Create	// 生成
	( // 引数
		const TYPE type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR3& rSize,	// 大きさ
		const int nLife				// 体力
	);
	static void RandomSpawn	// ランダム生成
	( // 引数
		const int nNum,	// 生成数
		const TYPE type	// 種類
	);
	static void SetSeason(const CWaveManager::SEASON season);	// 季節設定
	static int GetNumAll(void);		// 総数取得

	// メンバ関数
	void SetType(const int nType);	// 種類設定
	void SetLife(const int nLife);	// 体力設定

private:
	// メンバ関数
	bool CollisionPlayer(const D3DXVECTOR3& rPos);	// プレイヤーとの当たり判定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];		// テクスチャ定数
	static StatusInfo m_aStatusInfo[TYPE_MAX];	// ステータス情報
	static int m_nNumAll;						// マナフラワーの総数

	// メンバ変数
	CShadow *m_pShadow;		// 影の情報
	STATE m_state;			// 状態
	TYPE m_type;			// 種類
	int m_nLife;			// 体力
	int m_nCounterState;	// 状態管理カウンター
};

#endif	// _FLOWER_H_
