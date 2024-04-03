//============================================================
//
//	魔法陣ヘッダー [magicCircle.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MAGIC_CIRCLE_H_
#define _MAGIC_CIRCLE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "objectMeshCircle.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectMeshTube;	// オブジェクトメッシュチューブクラス

//************************************************************
//	クラス定義
//************************************************************
// 魔法陣クラス
class CMagicCircle : public CObjectMeshCircle
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_NORMAL = 0,	// 通常
		TEXTURE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CMagicCircle();

	// デストラクタ
	~CMagicCircle() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定

	// 静的メンバ関数
	static CMagicCircle *Create		// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const float fRadius,		// 半径
		const float fCropRadius		// 切り抜き半径
	);
	static CListManager<CMagicCircle> *GetList(void);	// リスト取得

	// メンバ関数
	void DrawCrop(void);	// 切り抜き用の描画
	void SetCropRadius(const float fRadius);	// 切り抜き半径設定

private:
	// 静的メンバ変数
	static CListManager<CMagicCircle> *m_pList;	// オブジェクトリスト

	// メンバ変数
	CListManager<CMagicCircle>::AIterator m_iterator;	// イテレーター
	CObjectMeshTube *m_pCropTube;	// 切り抜き用チューブの情報
};

#endif	// _MAGIC_CIRCLE_H_
