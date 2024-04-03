//============================================================
//
//	モデル文字ヘッダー [modelFont.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MODEL_FONT_H_
#define _MODEL_FONT_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// モデル文字クラス
class CModelFont : public CObjectModel
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_BOSS_NAME = 0,	// ボスの名前
		TYPE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CModelFont();

	// デストラクタ
	~CModelFont() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CModelFont *Create	// 生成
	( // 引数
		const EType type,						// 種類
		const D3DXVECTOR3& rPos,				// 位置
		const D3DXVECTOR3& rRot = VEC3_ZERO,	// 向き
		const D3DXVECTOR3& rScale = VEC3_ONE	// 大きさ
	);
};

#endif	// _MODEL_FONT_H_
