//============================================================
//
//	セーブポイントヘッダー [savePoint.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SAVEPOINT_H_
#define _SAVEPOINT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// セーブポイントクラス
class CSavePoint : public CObjectModel
{
public:
	// モデル列挙
	enum EModel
	{
		MODEL_SAVEPOINT = 0,	// セーブポイントモデル
		MODEL_MAX				// この列挙型の総数
	};

	// 状態列挙
	enum EState
	{
		STATE_NORMAL = 0,	// 通常状態
		STATE_SAVE,			// セーブ状態
		STATE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CSavePoint();

	// デストラクタ
	~CSavePoint();

	// セーブポイント情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画
	int GetIndex(void) const override;		// 自身のセーブポイントインデックス取得
	float GetRadius(void) const override;	// 半径取得

	// 静的メンバ関数
	static CSavePoint *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot		// 向き
	);
	static int GetNumAll(void);		// 総数取得
	static int GetSavePointID(void);	// セーブポイントインデックス取得
	static SInfo GetSavePointInfo(void);	// セーブポイント情報取得

private:
	// メンバ関数
	void CollisionPlayer(void);	// プレイヤーとの当たり判定

	// 静的メンバ関数
	static CObject *GetSavePoint(void);	// 生成済みセーブポイント取得

	// 静的メンバ変数
	static const char *mc_apModelFile[];	// モデル定数
	static CObject *m_pCurrentSave;			// 現在のセーブポイントへのポインタ

	static int m_nNumAll;	// セーブポイントの総数

	// メンバ変数
	EState m_state;				// 状態
	int m_nCounterState;		// 状態管理カウンター
	const int m_nThisSaveID;	// 自身のセーブポイントインデックス
};

#endif	// _SAVEPOINT_H_
