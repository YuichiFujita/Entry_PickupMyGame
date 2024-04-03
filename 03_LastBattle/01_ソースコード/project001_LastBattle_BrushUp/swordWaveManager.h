//============================================================
//
//	剣波動マネージャーヘッダー [swordWaveManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SWORDWAVE_MANAGER_H_
#define _SWORDWAVE_MANAGER_H_

//************************************************************
//	クラス定義
//************************************************************
// 剣波動マネージャークラス
class CSwordWaveManager
{
public:
	// コンストラクタ
	CSwordWaveManager();

	// デストラクタ
	~CSwordWaveManager();

	// 波動生成タイミング構造体
	struct STiming
	{
		D3DXVECTOR3 posOffset;	// 生成位置オフセット
		D3DXVECTOR3 rotOffset;	// 生成向きオフセット
		int nMotion;	// 生成モーション
		int nFrame;		// 生成フレーム
	};

	// メンバ関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);	// 終了
	void Update(void);	// 更新

	void AddTiming	// 生成タイミング追加
	( // 引数
		const D3DXVECTOR3& rPosOffset,	// 生成位置オフセット
		const D3DXVECTOR3& rRotOffset,	// 生成向きオフセット
		const int nMotion,	// 生成モーション
		const int nFrame	// 生成フレーム
	);
	void AddTiming(const STiming& rTiming);	// 生成タイミング追加

	// 静的メンバ関数
	static CSwordWaveManager *Create(void);	// 生成
	static void Release(CSwordWaveManager *&prSwordWaveManager);	// 破棄

private:
	// メンバ変数
	std::vector<STiming> m_spawnTiming;	// 生成タイミング情報
};

#endif	// _SWORDWAVE_MANAGER_H_
