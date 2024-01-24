//============================================================
//
//	プレイヤーヘッダー [player.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// プレイヤークラス
class CPlayer : public CObjectModel
{
public:
	// 種類列挙
	typedef enum
	{
		MODEL_ESCAPEKUN = 0,	// エスケープ君
		MODEL_MAX				// この列挙型の総数
	}MODEL;

	// コンストラクタ
	CPlayer();

	// デストラクタ
	~CPlayer();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CPlayer *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot		// 向き
	);

private:
	// メンバ関数
	void Move(void);	// 移動
	void Rot(void);		// 向き
	void Jump(void);	// ジャンプ
	void Land(D3DXVECTOR3& rPos);	// 着地

	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3	m_move;		// 移動量
	D3DXVECTOR3	m_destRot;	// 目標向き
	bool m_bJump;			// ジャンプ状況
};

#endif	// _PLAYER_H_
