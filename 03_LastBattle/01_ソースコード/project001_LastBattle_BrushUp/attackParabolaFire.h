//============================================================
//
//	カーブ攻撃炎ヘッダー [attackParabolaFire.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ATTACK_PARABOLA_FIRE_H_
#define _ATTACK_PARABOLA_FIRE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "fire.h"

//************************************************************
//	クラス定義
//************************************************************
// カーブ攻撃炎クラス
class CAttackParabolaFire : public CFire
{
public:
	// コンストラクタ
	explicit CAttackParabolaFire(const D3DXVECTOR3& rPosDest);

	// デストラクタ
	~CAttackParabolaFire() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CAttackParabolaFire *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rCurPos,		// 生成位置
		const D3DXVECTOR3& rDestPos,	// 目標位置
		const float fMove				// 移動量
	);

private:
	// メンバ関数
	void InitParabolaParam(const D3DXVECTOR3& rCurPos);	// 炎移動パラメーターの初期化
	void UpdateParabolaMove(void);	// 炎移動量の更新

	// メンバ変数
	const D3DXVECTOR3 m_posDest;	// 目標位置
	D3DXVECTOR3 m_posOrigin;		// 原点位置
	float m_fLength;	// 移動距離
	float m_fPhi;		// 方位角
	float m_fTheta;		// 仰角
	float m_fMove;		// 移動量
	int m_nCounterTime;	// 攻撃時間管理カウンター
};

#endif	// _ATTACK_PARABOLA_FIRE_H_
