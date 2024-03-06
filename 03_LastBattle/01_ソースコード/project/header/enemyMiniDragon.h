//============================================================
//
//	ミニドラゴンヘッダー [enemyMiniDragon.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ENEMY_MINI_DRAGON_H_
#define _ENEMY_MINI_DRAGON_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "enemySmall.h"

//************************************************************
//	クラス定義
//************************************************************
// ミニドラゴンクラス
class CEnemyMiniDragon : public CEnemySmall
{
public:
	// モデル列挙
	enum EModel
	{
		MODEL_HEAD = 0,	// 通常
		MODEL_MAX		// この列挙型の総数
	};

	// モーション列挙
	enum EMotion
	{
		MOTION_IDOL = 0,	// 待機モーション
		MOTION_MAX			// この列挙型の総数
	};

	// コンストラクタ
	explicit CEnemyMiniDragon(const EType type);

	// デストラクタ
	~CEnemyMiniDragon() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画
	int GetWaistModelID(void) const override;		// 腰モデルのインデックス取得
	int GetHeadModelID(void) const override;		// 頭モデルのインデックス取得

private:
	// オーバーライド関数
	const char *GetModelFileName(const int nModel) const override;	// モデルファイル取得
	void UpdateMotion(void) override;	// モーションの更新
};

#endif	// _ENEMY_MINI_DRAGON_H_
