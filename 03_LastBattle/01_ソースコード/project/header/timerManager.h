//============================================================
//
//	タイマーマネージャーヘッダー [timerManager.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TIMERMANAGER_H_
#define _TIMERMANAGER_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"

//************************************************************
//	定数宣言
//************************************************************
namespace timer
{
	const int MAX_MIN	= 2;	// タイマーの分の桁数
	const int MAX_SEC	= 2;	// タイマーの秒の桁数
	const int MAX_MSEC	= 3;	// タイマーのミリ秒の桁数
	const int MAX_PART	= 2;	// 区切りの最大数

	const int MAX_DIGIT = (MAX_MIN + MAX_SEC + MAX_MSEC);	// タイマーの桁数 (分・秒・ミリ秒)
}

//************************************************************
//	前方宣言
//************************************************************
class CValue;		// 数字クラス
class CObject2D;	// オブジェクト2Dクラス

//************************************************************
//	クラス定義
//************************************************************
// タイマーマネージャークラス
class CTimerManager
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_PART = 0,	// 区切り表示のテクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// タイム列挙
	enum ETime
	{
		TIME_MSEC,	// ミリ秒
		TIME_SEC,	// 秒
		TIME_MIN,	// 分
		TIME_MAX	// この列挙型の総数
	};

	// 計測列挙
	enum EState
	{
		STATE_NONE = 0,	// 処理なし
		STATE_MEASURE,	// 計測中
		STATE_END,		// 計測終了 
		STATE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CTimerManager();

	// デストラクタ
	virtual ~CTimerManager();

	// 仮想関数
	virtual HRESULT Init(void);	// 初期化
	virtual void Uninit(void);	// 終了
	virtual void Update(void);	// 更新
	virtual void SetPosition(const D3DXVECTOR3& rPos);	// 位置設定
	virtual D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	virtual void SetPriority(const int nPriority);		// 優先順位設定
	virtual void SetEnableUpdate(const bool bUpdate);	// 更新状況設定
	virtual void SetEnableDraw(const bool bDraw);		// 描画状況設定

	// メンバ関数
	void Start(void);	// 計測開始
	void End(void);		// 計測終了
	void EnableStop(const bool bStop);	// 計測停止設定
	EState GetState(void);		// 計測状態取得
	bool AddMSec(long nMSec);	// ミリ秒加算
	bool AddSec(long nSec);		// 秒加算
	bool AddMin(long nMin);		// 分加算
	bool SetMSec(long nMSec);	// ミリ秒設定
	bool SetSec(long nSec);		// 秒設定
	bool SetMin(long nMin);		// 分設定
	int Get(void);				// タイム取得
	int GetMSec(void);			// ミリ秒取得
	int GetSec(void);			// 秒取得
	int GetMin(void);			// 分取得
	long GetLimit(void);		// 制限時間取得
	void SetLimit(const ETime time, const long nTime);	// 制限時間設定
	void SetSizingValue(const D3DXVECTOR3& rSize);		// 区切りの大きさ設定
	void SetSizingPart(const D3DXVECTOR3& rSize);		// 数字の大きさ設定
	void SetSpaceValue(const D3DXVECTOR3& rSpace);		// 区切りの空白設定
	void SetSpacePart(const D3DXVECTOR3& rSpace);		// 数字の空白設定
	void SetColor(const D3DXCOLOR& rCol);				// 色の設定
	D3DXVECTOR3 GetSizingValue(void) const;				// 区切りの大きさ取得
	D3DXVECTOR3 GetSizingPart(void) const;				// 数字の大きさ取得
	D3DXVECTOR3 GetSpaceValue(void) const;				// 区切りの空白取得
	D3DXVECTOR3 GetSpacePart(void) const;				// 数字の空白取得

	// 静的メンバ関数
	static CTimerManager *Create	// 生成
	( // 引数
		const ETime time,				// 設定タイム
		const long nTime,				// 制限時間
		const D3DXVECTOR3& rPos,		// 位置
		const D3DXVECTOR3& rSizeValue,	// 数字の大きさ
		const D3DXVECTOR3& rSizePart,	// 区切りの大きさ
		const D3DXVECTOR3& rSpaceValue,	// 数字の空白
		const D3DXVECTOR3& rSpacePart	// 区切りの空白
	);
	static void Release(CTimerManager *&prTimerManager);	// 破棄
	static DWORD GetMinTime(void);	// 最少タイム取得
	static DWORD GetMaxTime(void);	// 最大タイム取得

private:
	// メンバ関数
	void SetDrawValue(void);	// 数字の表示設定
	void SetTexNum(void);		// 数字のテクスチャ座標設定

	// メンバ変数
	CValue *m_apValue[timer::MAX_DIGIT];	// 数値の情報
	CObject2D *m_apPart[timer::MAX_PART];	// 区切りの情報
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_sizeValue;	// 数字の大きさ
	D3DXVECTOR3 m_sizePart;		// 区切りの大きさ
	D3DXVECTOR3 m_spaceValue;	// 数字の空白
	D3DXVECTOR3 m_spacePart;	// 区切りの空白
	DWORD	m_dwStartTime;		// 開始時間
	DWORD	m_dwTime;			// 経過時間
	DWORD	m_dwStopStartTime;	// 停止開始時間
	DWORD	m_dwStopTime;		// 停止時間
	DWORD	m_dwTempTime;		// 経過時間の計算用
	EState	m_state;			// 計測状態
	bool	m_bStop;			// 計測停止状況
	long	m_nLimit;			// 制限時間
};

#endif	// _TIMERMANAGER_H_
