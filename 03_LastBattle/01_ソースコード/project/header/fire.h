//============================================================
//
//	炎ヘッダー [fire.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _FIRE_H_
#define _FIRE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CShadow;	// 影クラス

//************************************************************
//	クラス定義
//************************************************************
// 炎クラス
class CFire : public CObject
{
public:
	// コンストラクタ
	CFire();

	// デストラクタ
	~CFire() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3 &rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得

	// 静的メンバ関数
	static CFire *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 生成位置
		const int nLife = NONE_IDX	// 寿命
	);
	static CListManager<CFire> *GetList(void);		// リスト取得

	// メンバ関数
	void SetMove(const D3DXVECTOR3& rMove)	{ m_move = rMove; }	// 移動量設定
	D3DXVECTOR3 GetMove(void) const			{ return m_move; }	// 移動量取得
	void SetLife(const int nLife)	{ m_nLife = nLife; }	// 寿命設定
	int GetLife(void) const			{ return m_nLife; }		// 寿命取得

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ関数
	void CollisionPlayer(void);	// プレイヤーとの当たり判定

	// 静的メンバ変数
	static CListManager<CFire> *m_pList;	// オブジェクトリスト

	// メンバ変数
	CListManager<CFire>::AIterator m_iterator;	// イテレーター
	CShadow *m_pShadow;	// 影の情報
	D3DXVECTOR3	m_pos;	// 位置
	D3DXVECTOR3	m_move;	// 移動量
	int m_nLife;		// 寿命
};

#endif	// _FIRE_H_
