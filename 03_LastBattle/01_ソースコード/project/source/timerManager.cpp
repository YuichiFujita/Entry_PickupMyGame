//============================================================
//
//	タイマーマネージャー処理 [timerManager.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "timerManager.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "value.h"
#include "object2D.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *TEXTURE_FILE[] =	// テクスチャファイル
	{
		"data\\TEXTURE\\timer000.png",	// 区切り表示
	};

	const int	PRIORITY	= 6;					// タイマーの優先順位
	const DWORD	TIME_NUMMIN	= (DWORD)(0);			// 最少タイム
	const DWORD	TIME_NUMMAX	= (DWORD)(99 * 60000);	// 最大タイム
}

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(TEXTURE_FILE) == CTimerManager::TEXTURE_MAX, "ERROR : Texture Count Mismatch");

//************************************************************
//	子クラス [CTimerManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTimerManager::CTimerManager() :
	m_pos				(VEC3_ZERO),	// 位置
	m_sizeValue			(VEC3_ZERO),	// 数字の大きさ
	m_sizePart			(VEC3_ZERO),	// 区切りの大きさ
	m_spaceValue		(VEC3_ZERO),	// 数字の空白
	m_spacePart			(VEC3_ZERO),	// 区切りの空白
	m_dwStartTime		(0),			// 開始時間
	m_dwTime			(0),			// 経過時間
	m_dwStopStartTime	(0),			// 停止開始時間
	m_dwStopTime		(0),			// 停止時間
	m_dwTempTime		(0),			// 経過時間の計算用
	m_state				(STATE_NONE),	// 計測状態
	m_bStop				(true),			// 計測停止状況
	m_nLimit			(0)				// 制限時間
{
	// メンバ変数をクリア
	memset(&m_apValue[0],	0, sizeof(m_apValue));	// 数値の情報
	memset(&m_apPart[0],	0, sizeof(m_apPart));	// 区切りの情報
}

//============================================================
//	デストラクタ
//============================================================
CTimerManager::~CTimerManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CTimerManager::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = GET_MANAGER->GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apValue[0],	0, sizeof(m_apValue));	// 数値の情報
	memset(&m_apPart[0],	0, sizeof(m_apPart));	// 区切りの情報
	m_pos				= VEC3_ZERO;	// 位置
	m_sizeValue			= VEC3_ZERO;	// 数字の大きさ
	m_sizePart			= VEC3_ZERO;	// 区切りの大きさ
	m_spaceValue		= VEC3_ZERO;	// 数字の空白
	m_spacePart			= VEC3_ZERO;	// 区切りの空白
	m_dwStartTime		= 0;			// 開始時間
	m_dwTime			= 0;			// 経過時間
	m_dwStopStartTime	= 0;			// 停止開始時間
	m_dwStopTime		= 0;			// 停止時間
	m_dwTempTime		= 0;			// 経過時間の計算用
	m_state				= STATE_NONE;	// 計測状態
	m_bStop				= true;			// 計測停止状況
	m_nLimit			= 0;			// 制限時間

	for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の生成
		m_apValue[nCntTimer] = CValue::Create(CValue::TEXTURE_NORMAL);
		if (m_apValue[nCntTimer] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apValue[nCntTimer]->SetPriority(PRIORITY);
	}

	for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの生成
		m_apPart[nCntTimer] = CObject2D::Create(VEC3_ZERO);
		if (m_apPart[nCntTimer] == nullptr)
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apPart[nCntTimer]->BindTexture(pTexture->Regist(TEXTURE_FILE[TEXTURE_PART]));

		// 優先順位を設定
		m_apPart[nCntTimer]->SetPriority(PRIORITY);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CTimerManager::Uninit(void)
{
	// 数字オブジェクトを破棄
	for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の終了
		SAFE_UNINIT(m_apValue[nCntTimer]);
	}

	// 区切りオブジェクトを破棄
	for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの終了
		SAFE_UNINIT(m_apPart[nCntTimer]);
	}
}

//============================================================
//	更新処理
//============================================================
void CTimerManager::Update(void)
{
	switch (m_state)
	{ // 計測状態ごとの処理
	case STATE_NONE:

		// 無し

		break;

	case STATE_MEASURE:

		if (m_bStop == false)
		{ // 計測中の場合

			if (m_nLimit <= 0)
			{ // 制限時間が 0以下の場合

				// 現在の計測ミリ秒を設定
				m_dwTime = timeGetTime() - m_dwTempTime;
			}
			else
			{ // 制限時間が 0より大きい場合

				// 変数を宣言
				long nTime = m_nLimit - (timeGetTime() - m_dwTempTime);	// 現在タイム

				if (nTime > 0)
				{ // タイムが 0より大きい場合

					// 現在の計測ミリ秒を設定
					m_dwTime = nTime;
				}
				else
				{  // タイムが 0以下の場合

					// 現在の計測ミリ秒を設定
					m_dwTime = 0;

					// 計測を終了する
					End();
				}
			}
		}
		else
		{ // 計測停止中の場合

			// 現在の停止ミリ秒を設定
			m_dwStopTime = timeGetTime() - m_dwStopStartTime;
		}

		break;

	case STATE_END:

		// 無し

		break;

	default:	// 例外処理
		assert(false);
		break;
	}

	// 数字のテクスチャ座標の設定
	SetTexNum();

	for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の更新
		m_apValue[nCntTimer]->Update();
	}

	for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの更新
		m_apPart[nCntTimer]->Update();
	}

	// デバッグ表示
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "タイマー：[%d:%d:%d]\n", m_dwTime / 60000, (m_dwTime / 1000) % 60, m_dwTime % 1000);
	GET_MANAGER->GetDebugProc()->Print(CDebugProc::POINT_LEFT, "停止タイマー：[%d:%d:%d]\n", m_dwStopTime / 60000, (m_dwStopTime / 1000) % 60, m_dwStopTime % 1000);
}

//============================================================
//	位置の設定処理
//============================================================
void CTimerManager::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	優先順位の設定処理
//============================================================
void CTimerManager::SetPriority(const int nPriority)
{
	if (m_apValue[0] != nullptr)
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの優先順位を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetPriority(nPriority);
		}

		// 区切りオブジェクトの優先順位を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
		{ // 区切りの数分繰り返す

			m_apPart[nCntTimer]->SetPriority(nPriority);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	更新状況の設定処理
//============================================================
void CTimerManager::SetEnableUpdate(const bool bUpdate)
{
	if (m_apValue[0] != nullptr)
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの更新状況を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetEnableUpdate(bUpdate);
		}

		// 区切りオブジェクトの更新状況を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
		{ // 区切りの数分繰り返す

			m_apPart[nCntTimer]->SetEnableUpdate(bUpdate);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	描画状況の設定処理
//============================================================
void CTimerManager::SetEnableDraw(const bool bDraw)
{
	if (m_apValue[0] != nullptr)
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの描画状況を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetEnableDraw(bDraw);
		}

		// 区切りオブジェクトの描画状況を設定
		for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
		{ // 区切りの数分繰り返す

			m_apPart[nCntTimer]->SetEnableDraw(bDraw);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	生成処理
//============================================================
CTimerManager *CTimerManager::Create
(
	const ETime time,				// 設定タイム
	const long nTime,				// 制限時間
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSizeValue,	// 数字の大きさ
	const D3DXVECTOR3& rSizePart,	// 区切りの大きさ
	const D3DXVECTOR3& rSpaceValue,	// 数字の空白
	const D3DXVECTOR3& rSpacePart	// 区切りの空白
)
{
	// タイマーマネージャーの生成
	CTimerManager *pTimerManager = new CTimerManager;
	if (pTimerManager == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// タイマーマネージャーの初期化
		if (FAILED(pTimerManager->Init()))
		{ // 初期化に失敗した場合

			// タイマーマネージャーの破棄
			SAFE_DELETE(pTimerManager);
			return nullptr;
		}

		// 制限時間を設定
		pTimerManager->SetLimit(time, nTime);

		// 位置を設定
		pTimerManager->SetPosition(rPos);

		// 数字の大きさを設定
		pTimerManager->SetSizingValue(rSizeValue);

		// 区切りの大きさを設定
		pTimerManager->SetSizingPart(rSizePart);

		// 数字の空白を設定
		pTimerManager->SetSpaceValue(rSpaceValue);

		// 区切りの空白を設定
		pTimerManager->SetSpacePart(rSpacePart);

		// 確保したアドレスを返す
		return pTimerManager;
	}
}

//============================================================
//	破棄処理
//============================================================
void CTimerManager::Release(CTimerManager *&prTimerManager)
{
	// タイマーマネージャーの終了
	assert(prTimerManager != nullptr);
	prTimerManager->Uninit();

	// メモリ開放
	SAFE_DELETE(prTimerManager);
}

//============================================================
//	最少タイム取得処理
//============================================================
DWORD CTimerManager::GetMinTime(void)
{
	// 最少タイムを返す
	return TIME_NUMMIN;
}

//============================================================
//	最大タイム取得処理
//============================================================
DWORD CTimerManager::GetMaxTime(void)
{
	// 最大タイムを返す
	return TIME_NUMMAX;
}

//============================================================
//	計測開始処理
//============================================================
void CTimerManager::Start(void)
{
	if (m_state != STATE_MEASURE)
	{ // タイムの計測中ではない場合

		// 開始時刻を取得
		m_dwStartTime = timeGetTime();
		m_dwTempTime  = m_dwStartTime;

		// 非停止状態にする
		EnableStop(false);

		// 計測開始状態にする
		m_state = STATE_MEASURE;
	}
}

//============================================================
//	計測終了処理
//============================================================
void CTimerManager::End(void)
{
	// 計測終了状態にする
	m_state = STATE_END;
}

//============================================================
//	計測停止の有効無効の設定処理
//============================================================
void CTimerManager::EnableStop(const bool bStop)
{
	// 引数の停止状況を代入
	m_bStop = bStop;

	if (bStop)
	{ // 停止する場合

		// 停止開始時刻を取得
		m_dwStopStartTime = timeGetTime();
	}
	else
	{ // 再開する場合

		// 停止時間を加算
		m_dwTempTime += m_dwStopTime;

		// 停止関連の時間を初期化
		m_dwStopStartTime = 0;	// 停止開始時間
		m_dwStopTime = 0;		// 停止時間
	}
}

//============================================================
//	計測状態取得処理
//============================================================
CTimerManager::EState CTimerManager::GetState(void)
{
	// 計測状態を返す
	return m_state;
}

//============================================================
//	ミリ秒の加算処理
//============================================================
bool CTimerManager::AddMSec(long nMSec)
{
	// 変数を宣言
	bool bAdd = true;	// 加算状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 加算できない状態にする
			bAdd = false;
		}

		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 加算できない状態にする
			bAdd = false;
		}
	}

	if (bAdd)
	{ // 加算できる場合

		// 加算量の補正
		useful::LimitNum(nMSec, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 引数のミリ秒を減算
		m_dwTempTime -= (DWORD)nMSec;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 加算の成功失敗を返す
	return bAdd;
}

//============================================================
//	秒の加算処理
//============================================================
bool CTimerManager::AddSec(long nSec)
{
	// 変数を宣言
	bool bAdd = true;	// 加算状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 加算できない状態にする
			bAdd = false;
		}

		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 加算できない状態にする
			bAdd = false;
		}
	}

	if (bAdd)
	{ // 加算できる場合

		// 引数をミリ秒に変換
		nSec *= 1000;

		// 加算量の補正
		useful::LimitNum(nSec, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 引数の秒を減算
		m_dwTempTime -= (DWORD)nSec;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 加算の成功失敗を返す
	return bAdd;
}

//============================================================
//	分の加算処理
//============================================================
bool CTimerManager::AddMin(long nMin)
{
	// 変数を宣言
	bool bAdd = true;	// 加算状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 加算できない状態にする
			bAdd = false;
		}

		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 加算できない状態にする
			bAdd = false;
		}
	}

	if (bAdd)
	{ // 加算できる場合

		// 引数をミリ秒に変換
		nMin *= 60000;

		// 加算量の補正
		useful::LimitNum(nMin, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 引数の分を減算
		m_dwTempTime -= (DWORD)nMin;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 加算の成功失敗を返す
	return bAdd;
}

//============================================================
//	ミリ秒の設定処理
//============================================================
bool CTimerManager::SetMSec(long nMSec)
{
	// 変数を宣言
	bool bSet = true;	// 設定状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 設定できない状態にする
			bSet = false;
		}

		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 設定できない状態にする
			bSet = false;
		}
	}

	if (bSet)
	{ // 設定できる場合

		// 加算量の補正
		useful::LimitNum(nMSec, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 時刻を設定
		m_dwStartTime = timeGetTime() - nMSec;
		m_dwTempTime = m_dwStartTime;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 値を初期化
		m_dwStopStartTime	= 0;	// 停止開始時間
		m_dwStopTime		= 0;	// 停止時間

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 設定の成功失敗を返す
	return bSet;
}

//============================================================
//	秒の設定処理
//============================================================
bool CTimerManager::SetSec(long nSec)
{
	// 変数を宣言
	bool bSet = true;	// 設定状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 設定できない状態にする
			bSet = false;
		}
		
		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 設定できない状態にする
			bSet = false;
		}
	}

	if (bSet)
	{ // 設定できる場合

		// 引数をミリ秒に変換
		nSec *= 1000;

		// 加算量の補正
		useful::LimitNum(nSec, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 時刻を設定
		m_dwStartTime = timeGetTime() - nSec;
		m_dwTempTime = m_dwStartTime;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 値を初期化
		m_dwStopStartTime	= 0;	// 停止開始時間
		m_dwStopTime		= 0;	// 停止時間

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 設定の成功失敗を返す
	return bSet;
}

//============================================================
//	分の設定処理
//============================================================
bool CTimerManager::SetMin(long nMin)
{
	// 変数を宣言
	bool bSet = true;	// 設定状況

	if (m_state == STATE_MEASURE)
	{ // タイムの計測中の場合

		if (m_bStop)
		{ // 停止中の場合

			// 設定できない状態にする
			bSet = false;
		}

		if (m_nLimit > 0)
		{ // 制限時間が 0より大きい場合

			// 設定できない状態にする
			bSet = false;
		}
	}

	if (bSet)
	{ // 設定できる場合

		// 引数をミリ秒に変換
		nMin *= 60000;

		// 加算量の補正
		useful::LimitNum(nMin, -(long)m_dwTime, (long)TIME_NUMMAX);

		// 時刻を設定
		m_dwStartTime = timeGetTime() - nMin;
		m_dwTempTime = m_dwStartTime;

		// 現在の計測ミリ秒を設定
		m_dwTime = timeGetTime() - m_dwTempTime;

		// 値を初期化
		m_dwStopStartTime	= 0;	// 停止開始時間
		m_dwStopTime		= 0;	// 停止時間

		// 数字のテクスチャ座標の設定
		SetTexNum();
	}

	// 設定の成功失敗を返す
	return bSet;
}

//============================================================
//	タイムの取得処理
//============================================================
int CTimerManager::Get(void)
{
	// タイムを返す
	return m_dwTime;
}

//============================================================
//	ミリ秒の取得処理
//============================================================
int CTimerManager::GetMSec(void)
{
	// ミリ秒を返す
	return m_dwTime % 1000;
}

//============================================================
//	秒の取得処理
//============================================================
int CTimerManager::GetSec(void)
{
	// 秒を返す
	return (m_dwTime / 1000) % 60;
}

//============================================================
//	分の取得処理
//============================================================
int CTimerManager::GetMin(void)
{
	// 分を返す
	return m_dwTime / 60000;
}

//============================================================
//	制限時間の取得処理
//============================================================
long CTimerManager::GetLimit(void)
{
	// 制限時間を返す
	return m_nLimit;
}

//============================================================
//	制限時間の設定処理
//============================================================
void CTimerManager::SetLimit(const ETime time, const long nTime)
{
	// 変数を宣言
	long nMSec = 0;	// ミリ秒変換用

	// 引数の制限時間を設定
	switch (time)
	{ // タイムごとの処理
	case TIME_MSEC:	// ミリ秒

		m_nLimit = nTime;	// 制限時間を設定

		break;

	case TIME_SEC:	// 秒

		// 引数をミリ秒に変換
		nMSec = nTime * 1000;
		m_nLimit = nMSec;	// 制限時間を設定

		break;

	case TIME_MIN:	// 分

		// 引数をミリ秒に変換
		nMSec = nTime * 60000;
		m_nLimit = nMSec;	// 制限時間を設定

		break;

	default:
		assert(false);
		break;
	}
}

//============================================================
//	数字の大きさの設定処理
//============================================================
void CTimerManager::SetSizingValue(const D3DXVECTOR3& rSize)
{
	// 引数の数字の大きさを設定
	m_sizeValue = rSize;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	区切りの大きさの設定処理
//============================================================
void CTimerManager::SetSizingPart(const D3DXVECTOR3& rSize)
{
	// 引数の区切りの大きさを設定
	m_sizePart = rSize;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	数字の空白の設定処理
//============================================================
void CTimerManager::SetSpaceValue(const D3DXVECTOR3& rSpace)
{
	// 引数の数字の空白を設定
	m_spaceValue = rSpace;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	区切りの空白の設定処理
//============================================================
void CTimerManager::SetSpacePart(const D3DXVECTOR3& rSpace)
{
	// 引数の区切りの空白を設定
	m_spacePart = rSpace;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	色の設定処理
//============================================================
void CTimerManager::SetColor(const D3DXCOLOR& rCol)
{
	if (m_apValue[0] != nullptr)
	{ // スコアの先頭の数値が使用されている場合

		for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			// 数字の色を設定
			m_apValue[nCntTimer]->SetColor(rCol);
		}

		for (int nCntTimer = 0; nCntTimer < timer::MAX_PART; nCntTimer++)
		{ // 区切りの数分繰り返す

			// 区切りの色を設定
			m_apPart[nCntTimer]->SetColor(rCol);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	数字の大きさ取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetSizingValue(void) const
{
	// 数字の大きさを返す
	return m_sizeValue;
}

//============================================================
//	区切りの大きさ取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetSizingPart(void) const
{
	// 区切りの大きさを返す
	return m_sizePart;
}

//============================================================
//	数字の空白取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetSpaceValue(void) const
{
	// 数字の空白を返す
	return m_spaceValue;
}

//============================================================
//	区切りの空白取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetSpacePart(void) const
{
	// 区切りの空白を返す
	return m_spacePart;
}

//============================================================
//	数字の表示設定処理
//============================================================
void CTimerManager::SetDrawValue(void)
{
	// 変数を宣言
	D3DXVECTOR3 spaceValue = m_spaceValue * 0.5f;	// 数字の空白
	D3DXVECTOR3 spacePart = m_spacePart * 0.5f;		// 区切りの空白
	D3DXVECTOR3 posPoly = m_pos - spaceValue;		// ポリゴン生成位置
	int nNumValue = 0;	// 数字の生成数
	int nNumPart = 0;	// 区切りの生成数

	if (m_apValue[0] != nullptr && m_apPart[0] != nullptr)
	{ // タイマーの先頭数値、タイマーの先頭区切りが使用されている場合

		for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT + timer::MAX_PART; nCntTimer++)
		{ // タイマーの桁数 + タイマーの区切り数分繰り返す

			if (nCntTimer == timer::MAX_MIN || nCntTimer == timer::MAX_MIN + timer::MAX_SEC + 1)
			{ // 区切りの生成タイミングだった場合

				// ポリゴン生成位置をずらす
				posPoly += spacePart;

				// 区切りの位置を設定
				m_apPart[nNumValue]->SetVec3Position(posPoly);

				// 区切りの大きさを設定
				m_apPart[nNumValue]->SetVec3Sizing(m_sizePart);

				// 区切り生成数を加算
				nNumValue++;

				// ポリゴン生成位置をずらす
				posPoly += spacePart;
			}
			else
			{ // 区切りの生成タイミングではない場合

				// ポリゴン生成位置をずらす
				posPoly += spaceValue;

				// 数字の位置を設定
				m_apValue[nNumPart]->SetVec3Position(posPoly);

				// 数字の大きさを設定
				m_apValue[nNumPart]->SetVec3Sizing(m_sizeValue);

				// 数字生成数を加算
				nNumPart++;

				// ポリゴン生成位置をずらす
				posPoly += spaceValue;
			}
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	数字のテクスチャ座標の設定処理
//============================================================
void CTimerManager::SetTexNum(void)
{
	// 変数を宣言
	int aNumDivide[timer::MAX_DIGIT];	// 分の桁数ごとの分解用

	// 分を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[0], GetMin(), timer::MAX_MIN);

	// 秒を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[timer::MAX_MIN], GetSec(), timer::MAX_SEC);

	// ミリ秒を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[timer::MAX_MIN + timer::MAX_SEC], GetMSec(), timer::MAX_MSEC);

	for (int nCntTimer = 0; nCntTimer < timer::MAX_DIGIT; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の設定
		m_apValue[nCntTimer]->SetNumber(aNumDivide[nCntTimer]);
	}
}
