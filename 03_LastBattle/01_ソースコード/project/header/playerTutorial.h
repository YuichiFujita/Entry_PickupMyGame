//============================================================
//
//	プレイヤーチュートリアルヘッダー [playerTutorial.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PLAYER_TUTORIAL_H_
#define _PLAYER_TUTORIAL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "player.h"

//************************************************************
//	クラス定義
//************************************************************
// プレイヤーチュートリアルクラス
class CPlayerTutorial : public CPlayer
{
public:
	// コンストラクタ
	CPlayerTutorial();

	// デストラクタ
	~CPlayerTutorial() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

private:
	// オーバーライド関数
	void UpdateNormal(int *pLowMotion, int *pUpMotion) override;	// 通常状態時の更新

	// メンバ変数
	D3DXVECTOR3 m_oldCamRot;	// カメラの過去向き
};

#endif	// _PLAYER_TUTORIAL_H_
