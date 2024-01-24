//============================================================
//
//	ロックオン表示ヘッダー [lockCursor.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _LOCKCURSOR_H_
#define _LOCKCURSOR_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "object.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectBillboard;	// オブジェクトビルボードクラス

//************************************************************
//	クラス定義
//************************************************************
// ロックオン表示クラス
class CLockCursor : public CObject
{
public:
	// テクスチャ列挙
	typedef enum
	{
		TEXTURE_LOCK000 = 0,	// ロックオン表示(魔法陣)のテクスチャ
		TEXTURE_LOCK001,		// ロックオン表示(上四角)のテクスチャ
		TEXTURE_LOCK002,		// ロックオン表示(下四角)のテクスチャ
		TEXTURE_MAX				// この列挙型の総数
	}TEXTURE;

	// コンストラクタ
	CLockCursor();

	// デストラクタ
	~CLockCursor();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 静的メンバ関数
	static CLockCursor *Create		// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const bool bDraw = true		// 描画状況
	);

	// メンバ関数
	void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	void SetLockObject(CObject *pObject);		// ロックオン設定
	void SetEnableDisp(const bool bDisp);		// ロックオン表示設定
	void SetEnableUpdate(const bool bUpdate);	// 更新状況設定
	void SetEnableDraw(const bool bDraw);		// 描画状況設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	const CObject *GetLockObject(void) const;	// ロックオン取得

private:
	// メンバ変数
	CObjectBillboard *m_apBilboard[TEXTURE_MAX];	// ビルボードの情報
	CObject *m_pLock;	// ロックオンオブジェクトの情報

	// 静的メンバ変数
	static const char *mc_apTextureFile[];	// テクスチャ定数
};

#endif	// _LOCKCURSOR_H_
