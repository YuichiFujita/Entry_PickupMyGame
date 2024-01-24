//============================================================
//
//	弾ヘッダー [bullet.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _BULLET_H_
#define _BULLET_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectBillboard.h"

//************************************************************
//	クラス定義
//************************************************************
// 弾クラス
class CBullet : public CObjectBillboard
{
public:
	// 種類列挙
	typedef enum
	{
		TYPE_ENEMY = 0,	// 敵の弾
		TYPE_MAX		// この列挙型の総数
	}TYPE;

	// コンストラクタ
	CBullet(const int nDamage);

	// デストラクタ
	~CBullet();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CBullet *Create	// 生成
	( // 引数
		const TYPE type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rSize,	// 大きさ
		const D3DXCOLOR& rCol,		// 色
		const D3DXVECTOR3& rVec,	// 移動方向
		const float fMove,			// 移動速度
		const int nLife,			// 寿命
		const int nDamage			// 攻撃力
	);

	// メンバ関数
	void SetMove(D3DXVECTOR3 vec, const float fMove);	// 移動量設定
	void SetType(const TYPE type);	// 種類設定
	void SetLife(const int nLife);	// 寿命設定

private:
	// メンバ関数
	bool CollisionTarget(void);	// ターゲットとの当たり判定
	bool CollisionPlayer(void);	// プレイヤーとの当たり判定

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数

	// メンバ変数
	D3DXVECTOR3 m_move;		// 移動量
	TYPE m_type;			// 種類
	int m_nLife;			// 寿命
	const int m_nDamage;	// 攻撃力定数
};

#endif	// _BULLET_H_
