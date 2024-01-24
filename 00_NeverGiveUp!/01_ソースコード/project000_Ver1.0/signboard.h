//============================================================
//
//	看板ヘッダー [signboard.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SIGNBOARD_H_
#define _SIGNBOARD_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectMeshCube.h"

//************************************************************
//	前方宣言
//************************************************************
class CMultiModel;	// マルチモデルクラス

//************************************************************
//	クラス定義
//************************************************************
// 看板クラス
class CSignboard : public CObjectMeshCube
{
public:
	// モデル列挙
	enum EModel
	{
		MODEL_STAND,	// スタンドモデル
		MODEL_MAX		// この列挙型の総数
	};

	// 種類列挙
	enum EType
	{
		TYPE_RIGHT = 0,	// 右テクスチャ
		TYPE_LEFT,		// 左テクスチャ
		TYPE_UP,		// 上テクスチャ
		TYPE_DOWN,		// 下テクスチャ
		TYPE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CSignboard();

	// デストラクタ
	~CSignboard();

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画
	void SetType(const int nType) override;		// 種類設定
	int GetType(void) const override;			// 種類取得
	void SetScale(const float fScale) override;	// 拡大率設定
	float GetScale(void) const override;		// 拡大率取得

	// 静的メンバ関数
	static CSignboard *Create	// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const float fScale = 1.20f,			// 拡大率
		const D3DXCOLOR& rCol = XCOL_WHITE	// 色
	);

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[][6];	// テクスチャ定数
	static const char *mc_apModelFile[];		// モデル定数

	// メンバ変数
	CMultiModel *m_pStand;	// 看板スタンドの情報
	EType m_type;			// 種類
	float m_fScale;			// 拡大率
};

#endif	// _SIGNBOARD_H_
