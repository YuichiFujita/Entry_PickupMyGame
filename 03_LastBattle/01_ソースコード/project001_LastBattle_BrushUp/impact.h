//============================================================
//
//	衝撃波ヘッダー [impact.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _IMPACT_H_
#define _IMPACT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "wave.h"

//************************************************************
//	クラス定義
//************************************************************
// 衝撃波クラス
class CImpact : public CWave
{
public:
	// コンストラクタ
	CImpact();

	// デストラクタ
	~CImpact() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CImpact *Create	// 生成
	( // 引数
		const ETexture texture,		// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXCOLOR& rCol,		// 色
		const SGrow& rGrow,			// 成長量
		const SGrow& rAddGrow,		// 成長加速量
		const float fHoleRadius,	// 穴の半径
		const float fThickness,		// 太さ
		const float fOuterPlusY,	// 外周のY座標加算量
		const float fMaxGrowRadius	// 半径の最大成長量
	);

	void SetAddGrow(const SGrow &rGrow)		{ m_addGrow = rGrow; }			// 成長情報設定
	SGrow GetAddGrow(void) const			{ return m_addGrow; }			// 成長情報取得
	void SetMaxGrowRadius(const float fMax)	{ m_fMaxGrowRadius = fMax; }	// 成長情報設定
	float GetMaxGrowRadius(void) const		{ return m_fMaxGrowRadius; }	// 成長情報取得

private:
	// メンバ関数
	void CollisionPlayer(void);	// プレイヤーとの当たり判定

	// メンバ変数
	float m_fMaxGrowRadius;	// 半径の最大成長量
	SGrow m_addGrow;		// 成長加速量
};

#endif	// _IMPACT_H_
