//============================================================
//
//	チュートリアルプレイヤーヘッダー [playerTutorial.h]
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
#include "main.h"
#include "player.h"

//************************************************************
//	クラス定義
//************************************************************
// チュートリアルプレイヤークラス
class CPlayerTutorial : public CPlayer
{
public:
	// コンストラクタ
	CPlayerTutorial();

	// デストラクタ
	~CPlayerTutorial();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void HitBlowAway	// 吹っ飛びヒット
	( // 引数
		const D3DXVECTOR3& rPlayerPos,	// プレイヤー位置
		const D3DXVECTOR3& rHitObjPos,	// オブジェクト位置
		const int nDmg					// ダメージ量
	) override;
	void HitVortex		// 渦巻きこまれヒット
	( // 引数
		D3DXVECTOR3& rPlayerPos,	// プレイヤー位置
		const D3DXVECTOR3& rHitPos,	// 当たり判定位置
		const int nDmg				// ダメージ量
	) override;
	void SetDisp(const bool bDisp) override;		// 表示設定
	void SetEnableDraw(const bool bDraw) override;	// 描画状況設定

private:
	// メンバ関数
	MOTION UpdateNormal(void);	// 通常状態時の更新
	MOTION UpdateMove(void);	// 移動量・目標向きの更新

	// メンバ変数
	int m_nOldNumEnemy;	// 過去の敵の総数
};

#endif	// _PLAYER_TUTORIAL_H_
