//============================================================
//
//	草ヘッダー [weed.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _WEED_H_
#define _WEED_H_

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
// 草クラス
class CWeed : public CObject3D
{
public:
	// 種類列挙
	typedef enum
	{
		TYPE_SPRING = 0,	// 春草テクスチャ
		TYPE_SUMMER,		// 夏草テクスチャ
		TYPE_AUTUMN,		// 秋草テクスチャ
		TYPE_WINTER,		// 冬草テクスチャ
		TYPE_MAX			// この列挙型の総数
	}TYPE;

	// コンストラクタ
	CWeed();

	// デストラクタ
	~CWeed();

	// ステータス構造体
	typedef struct
	{
		D3DXVECTOR3 size;	// 大きさ
	}StatusInfo;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CWeed *Create	// 生成
	( // 引数
		const TYPE type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR3& rSize	// 大きさ
	);
	static void RandomSpawn	// ランダム生成
	( // 引数
		const int nNum,	// 生成数
		const TYPE type	// 種類
	);
	static void SetSeason(const CWaveManager::SEASON season);	// 季節設定

	// メンバ関数
	void SetType(const int nType);	// 種類設定

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[];		// テクスチャ定数
	static StatusInfo m_aStatusInfo[TYPE_MAX];	// ステータス情報

	// メンバ変数
	CShadow *m_pShadow;	// 影の情報
	TYPE m_type;		// 種類
	float m_fSinRot;	// なびき向き
};

#endif	// _WEED_H_
