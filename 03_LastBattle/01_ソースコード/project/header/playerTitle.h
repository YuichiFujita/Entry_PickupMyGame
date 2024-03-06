//============================================================
//
//	プレイヤータイトルヘッダー [playerTitle.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PLAYER_TITLE_H_
#define _PLAYER_TITLE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "player.h"

//************************************************************
//	前方宣言
//************************************************************
class CAnim2D;	// アニメーション2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// プレイヤータイトルクラス
class CPlayerTitle : public CPlayer
{
public:
	// 演出列挙
	enum EStag
	{
		STAG_NONE = 0,	// 何もしない
		STAG_FALL,		// 落下
		STAG_LAND,		// 着地
		STAG_IDOL,		// 待機
		STAG_ATTACK,	// 攻撃
		STAG_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CPlayerTitle();

	// デストラクタ
	~CPlayerTitle() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;						// 描画
	void SetStag(const int nStag) override	{ m_stag = (EStag)nStag; }	// 演出状態設定
	int GetStag(void) const override		{ return m_stag; }			// 演出状態取得

private:
	// メンバ関数
	void UpdateStagFall(void);		// 落下
	void UpdateStagLand(void);		// 着地
	void UpdateStagIdol(void);		// 待機
	void UpdateStagAttack(void);	// 攻撃

	// メンバ変数
	CAnim2D *m_pSlash;	// 斬撃の情報
	EStag m_stag;		// 演出状態
};

#endif	// _PLAYER_TITLE_H_
