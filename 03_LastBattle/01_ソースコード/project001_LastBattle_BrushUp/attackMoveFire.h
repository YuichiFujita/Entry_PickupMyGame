//============================================================
//
//	直進攻撃炎ヘッダー [attackMoveFire.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ATTACK_MOVE_FIRE_H_
#define _ATTACK_MOVE_FIRE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "fire.h"

//************************************************************
//	クラス定義
//************************************************************
// 直進攻撃炎クラス
class CAttackMoveFire : public CFire
{
public:
	// コンストラクタ
	CAttackMoveFire();

	// デストラクタ
	~CAttackMoveFire() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CAttackMoveFire *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,		// 生成位置
		const D3DXVECTOR3& rVecMove,	// 移動方向
		const float fMove,				// 移動量
		const float fAddMove = 0.0f,	// 加速量
		const int nLife = NONE_IDX		// 寿命
	);

	// メンバ関数
	void SetMoveDir(const D3DXVECTOR3& rMove);	// 移動方向の設定
	D3DXVECTOR3 GetMoveDir(void) const			{ return m_dirMove; }		// 移動方向の取得
	void SetMoveValue(const float fMove)		{ m_fMove = fMove; }		// 移動量の設定
	float GetMoveValue(void) const				{ return m_fMove; }			// 移動量の取得
	void SetAddMoveValue(const float fAddMove)	{ m_fAddMove = fAddMove; }	// 加速量の設定
	float GetAddMoveValue(void) const			{ return m_fAddMove; }		// 加速量の取得

private:
	// メンバ関数
	void UpdateMove(void);	// 移動の更新

	// メンバ関数
	D3DXVECTOR3 m_dirMove;	// 移動方向ベクトル
	float m_fMove;		// 移動量
	float m_fAddMove;	// 加速量
};

#endif	// _ATTACK_MOVE_FIRE_H_
