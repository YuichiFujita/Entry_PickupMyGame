//============================================================
//
//	バブルヘッダー [bubble.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _BUBBLE_H_
#define _BUBBLE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// バブルクラス
class CBubble : public CObjectModel
{
public:
	// モデル列挙
	typedef enum
	{
		MODEL_BUBBLE = 0,	// バブル
		MODEL_BOLD,			// 縁取り
		MODEL_MAX			// この列挙型の総数
	}MODEL;

	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_CHANGE,		// サイズ変動状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CBubble(const int nMaxLevel, const D3DXVECTOR3& rMaxScale);

	// デストラクタ
	~CBubble();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	float GetRadius(void) const;	// 半径取得

	// 静的メンバ関数
	static CBubble *Create	// 生成
	( // 引数
		CObject *pObject,		// 親オブジェクト
		const int nMaxLevel,	// 最大レベル
		const D3DXVECTOR3& rMaxScale,	// 最大拡大率
		const D3DXVECTOR3& rPos,		// 位置
		const float fPosUp,				// Y位置加算量
		const D3DXVECTOR3& rRot = VEC3_ZERO,	// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 拡大率
	);

	// メンバ関数
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	void SetRotation(const D3DXVECTOR3& rRot);	// 向き設定
	void SetScaling(const D3DXVECTOR3& rScale);	// 大きさ設定
	void AddLevel(const int nAdd);				// レベル加算
	void SetLevel(const int nNum);				// レベル設定
	int GetLevel(void) const;					// レベル取得
	float GetMaxRadius(void) const;				// 最大半径取得
	int GetMaxLevel(void) const;				// 最大レベル取得
	void SetPositionUp(const float fUp);		// Y位置加算量設定
	float GetPositionUp(void) const;			// Y位置加算量取得

private:
	// メンバ関数
	void SetParentObject(CObject *pObject);	// 親オブジェクト設定

	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	CObjectModel *m_pBold;		// 縁取りモデル
	D3DXVECTOR3 m_currentScale;	// 現在の拡大率
	D3DXVECTOR3 m_destScale;	// 目標の拡大率
	CObject *m_pParentObject;	// 親オブジェクト
	float m_fPosUp;	// バブルのY位置加算量
	int m_nLevel;	// 大きさレベル

	const D3DXVECTOR3 m_maxScale;	// 最大拡大率定数
	const int m_nMaxLevel;			// 最大レベル定数
};

#endif	// _BUBBLE_H_
