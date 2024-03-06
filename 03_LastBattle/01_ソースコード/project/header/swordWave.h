//============================================================
//
//	剣波動ヘッダー [swordWave.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SWORD_WAVE_H_
#define _SWORD_WAVE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "wave.h"

//************************************************************
//	クラス定義
//************************************************************
// 剣波動クラス
class CSwordWave : public CWave
{
public:
	// コンストラクタ
	CSwordWave();

	// デストラクタ
	~CSwordWave() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;	// 描画

	// 静的メンバ関数
	static CSwordWave *Create	// 生成
	( // 引数
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const SGrow& rGrow,			// 成長量
		const SGrow& rAddGrow,		// 成長加速量
		const float fHoleRadius,	// 穴の半径
		const float fThickness		// 太さ
	);

	void SetAddGrow(const SGrow& rGrow)	{ m_addGrow = rGrow; }	// 成長情報設定
	SGrow GetAddGrow(void) const		{ return m_addGrow; }	// 成長情報取得

private:
	// メンバ関数
	void CollisionEnemy(void);	// 敵との当たり判定

	// メンバ変数
	SGrow m_addGrow;	// 成長加速量
};

#endif	// _SWORD_WAVE_H_
