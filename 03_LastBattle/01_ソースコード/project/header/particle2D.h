//============================================================
//
//	パーティクル2Dヘッダー [particle2D.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _PARTICLE2D_H_
#define _PARTICLE2D_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	クラス定義
//************************************************************
// パーティクル2Dクラス
class CParticle2D : public CObject
{
public:
	// コンストラクタ
	CParticle2D();

	// デストラクタ
	~CParticle2D() override;

	// 種類列挙
	enum EType
	{
		TYPE_NONE = 0,	// なし
		TYPE_DAMAGE,	// ダメージ
		TYPE_ITEM,		// アイテム
		TYPE_GETITEM,	// アイテム取得
		TYPE_MAX		// この列挙型の総数
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetType(const int nType) override;					// 種類設定
	int GetType(void) const override;						// 種類取得
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得
	void SetColor(const D3DXCOLOR& rCol) override;			// 色設定
	D3DXCOLOR GetColor(void) const override;				// 色取得

	// 静的メンバ関数
	static CParticle2D *Create	// 生成
	( // 引数
		const EType type,					// 種類
		const D3DXVECTOR3& rPos,			// 位置
		const D3DXCOLOR& rCol = XCOL_WHITE	// 色
	);

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ関数
	void Damage(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol);	// ダメージ
	void Item(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol);		// アイテム
	void GetItem(const D3DXVECTOR3& rPos, const D3DXCOLOR& rCol);	// アイテム取得

	// メンバ変数
	D3DXVECTOR3 m_pos;	// 位置
	D3DXCOLOR m_col;	// 色
	EType m_type;		// 種類
	int  m_nLife;		// 寿命
};

#endif	// _PARTICLE2D_H_
