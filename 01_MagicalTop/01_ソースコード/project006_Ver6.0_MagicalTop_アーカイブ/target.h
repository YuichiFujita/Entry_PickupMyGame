//============================================================
//
//	ターゲットヘッダー [target.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TARGET_H_
#define _TARGET_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectModel.h"
#include "model.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectMeshCube;	// オブジェクトメッシュキューブクラス
class CObjectGauge3D;	// オブジェクトゲージ3Dクラス
class CShadow;			// 影クラス
class CWarningAttack;	// 攻撃警告表示クラス

//************************************************************
//	クラス定義
//************************************************************
// ターゲットクラス
class CTarget : public CObjectModel
{
public:
	// モデル列挙
	typedef enum
	{
		MODEL_NORMAL = 0,	// 通常台座
		MODEL_MAX			// この列挙型の総数
	}MODEL;

	// 状態列挙
	typedef enum
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_HEAL,			// 回復状態
		STATE_DESTROY,		// 破壊状態
		STATE_MAX			// この列挙型の総数
	}STATE;

	// コンストラクタ
	CTarget();

	// デストラクタ
	~CTarget();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Hit(const int nDmg);	// ヒット

	// 静的メンバ関数
	static CTarget *Create	// 生成
	( // 引数
		const MODEL model,			// モデル
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 大きさ
	);

	// メンバ関数
	void SetEnableUpdate(const bool bUpdate);	// 更新状況設定
	void SetEnableDraw(const bool bDraw);		// 描画状況設定
	void SetEnableDrawLife(const bool bDraw);	// 体力描画状況設定
	void AddLife(const int nAdd);	// 体力加算
	void SetLife(const int nLife);	// 体力設定
	int GetLife(void) const;		// 体力取得
	int GetMaxLife(void) const;		// 最大体力取得
	int GetState(void) const;		// 状態取得
	float GetRadius(void) const;	// 半径取得

private:
	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数

	// メンバ変数
	CObjectMeshCube *m_pMeshCube;	// メッシュキューブの情報
	CObjectGauge3D *m_pLife;		// 体力の情報
	CShadow *m_pShadow;				// 影の情報
	CWarningAttack *m_pWarning;		// 攻撃警告表示の情報

	STATE m_state;			// 状態
	float m_fSinRot;		// 浮遊向き
	int m_nCounterState;	// 状態管理カウンター
	int m_nCounterHeal;		// 回復管理カウンター
};

#endif	// _TARGET_H_
