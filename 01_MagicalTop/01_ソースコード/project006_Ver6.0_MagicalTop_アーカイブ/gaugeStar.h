//============================================================
//
//	星ゲージヘッダー [gaugeStar.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _GAUGE_STAR_H_
#define _GAUGE_STAR_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	マクロ定義
//************************************************************
#define MAX_STAR	(8)	// 星の最大数

//************************************************************
//	前方宣言
//************************************************************
class CObjectBillboard;	// オブジェクトビルボードクラス

//************************************************************
//	クラス定義
//************************************************************
// 星ゲージクラス
class CGaugeStar : public CObject
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_STAR = 0,	// 星のテクスチャ
		TEXTURE_FRAME,		// 星枠のテクスチャ
		TEXTURE_MAX			// この列挙型の総数
	}TEXTURE;

	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_USAGE,		// ゲージ使用状態
		STATE_HEAL,			// ゲージ回復状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CGaugeStar(const int nHealNumGauge, const int nHealWait, const float fMaxRadius);

	// デストラクタ
	~CGaugeStar();

	// 星構造体
	typedef struct
	{
		CObjectBillboard *pBillboard;	// ビルボード情報
		int nNumGauge;	// ゲージ量
		float fRadius;	// 半径
	}Star;

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void SetEnableUpdate(const bool bUpdate);	// 更新状況設定
	void SetEnableDraw(const bool bDraw);		// 描画状況設定

	// 静的メンバ関数
	static CGaugeStar *Create		// 生成
	( // 引数
		const int nMaxNumGauge,		// 最大ゲージ量
		const float fMaxRadius,		// 半径
		const int nHealNumGauge,	// ゲージ回復量
		const int nHealWait,		// 回復待機カウント
		CObject *pObject,			// 親オブジェクト
		const D3DXVECTOR3& rGap,	// 表示位置の加算量
		const float fDistance,		// 中心からの距離
		const float fFlicker		// 揺らめき量
	);

	// メンバ関数
	bool UseGauge(void);		// ゲージ使用
	void HealNumGauge(void);	// ゲージ全回復
	void SetMaxNumGauge(const int nMax);	// 最大ゲージ量
	void SetStar(const int nNumGauge, const float fRadius);	// 星情報設定
	void SetGapPosition(const D3DXVECTOR3& rGap);	// 表示位置の加算量設定
	void SetDistance(const float fDistance);		// 中心からの距離設定
	void SetFlicker(const float fFlicker);			// 揺らめき量設定

private:
	// メンバ関数
	HRESULT SetDrawInfo(void);	// 描画情報設定
	void SetParentObject(CObject *pObject);	// 親オブジェクト設定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	CObjectBillboard *m_apStarFrame[MAX_STAR];	// 星の枠情報
	CObjectBillboard *m_apStarBG[MAX_STAR];		// 星の背景情報
	Star m_aStar[MAX_STAR];		// 星の情報
	STATE m_state;				// 状態
	int m_nMaxNumGauge;			// 最大ゲージ量
	int m_nCounterState;		// 状態管理カウンター
	int m_nCurrentStar;			// 現在消費中の星
	bool m_bOverheat;			// オーバーヒート状況
	const int m_nHealNumGauge;	// ゲージ回復量定数
	const int m_nHealWait;		// 回復待機カウント定数
	const float m_fMaxRadius;	// 最大半径定数

	CObject *m_pParentObject;	// 親オブジェクト
	D3DXVECTOR3 m_posGap;		// 表示位置の加算量
	float m_fDistance;			// 中心からの距離
	float m_fFlicker;			// 揺らめき量
	float m_fSinRot;			// 距離加減量
	float m_fAddRot;			// 消費中の星方向への現在加算量
	float m_fDestAddRot;		// 消費中の星方向への目標加算量
};

#endif	// _GAUGE_STAR_H_
