//============================================================
//
//	ホーミング攻撃炎ヘッダー [attackHomingFire.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ATTACK_HOMING_FIRE_H_
#define _ATTACK_HOMING_FIRE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "fire.h"

//************************************************************
//	クラス定義
//************************************************************
// ホーミング攻撃炎クラス
class CAttackHomingFire : public CFire
{
public:
	// コンストラクタ
	CAttackHomingFire();

	// デストラクタ
	~CAttackHomingFire() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CAttackHomingFire *Create(const D3DXVECTOR3& rPos);	// 生成

private:
	// メンバ関数
	void UpdateHomingMove(void);	// 炎移動量の更新
	void CalcHoming(float *pCurRot, float fDestRot);	// ホーミングの計算
};

#endif	// _ATTACK_HOMING_FIRE_H_
