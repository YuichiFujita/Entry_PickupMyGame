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
//	マクロ定義
//************************************************************
#define TIMER_PRIO	(6)	// タイマーの優先順位

#define TIME_NUMMIN	(DWORD)(0)			// 最少タイム
#define TIME_NUMMAX	(DWORD)(99 * 60000)	// 最大タイム

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CTimerManager::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\timer000.png",	// 区切り表示
};

//************************************************************
//	子クラス [CTimerManager] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CTimerManager::CTimerManager()
{
	// メンバ変数をクリア
	memset(&m_apValue[0], 0, sizeof(m_apValue));	// 数値の情報
	memset(&m_apPart[0], 0, sizeof(m_apPart));		// 区切りの情報
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
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	memset(&m_apValue[0], 0, sizeof(m_apValue));	// 数値の情報
	memset(&m_apPart[0], 0, sizeof(m_apPart));		// 区切りの情報
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

	for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の生成
		m_apValue[nCntTimer] = CValue::Create(CValue::TEXTURE_MAGIC_GREEN);
		if (UNUSED(m_apValue[nCntTimer]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apValue[nCntTimer]->SetPriority(TIMER_PRIO);
	}

	for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの生成
		m_apPart[nCntTimer] = CObject2D::Create(VEC3_ZERO);
		if (UNUSED(m_apPart[nCntTimer]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// テクスチャを登録・割当
		m_apPart[nCntTimer]->BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_NORMAL]));

		// 優先順位を設定
		m_apPart[nCntTimer]->SetPriority(TIMER_PRIO);
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
	for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の終了
		m_apValue[nCntTimer]->Uninit();
	}

	// 区切りオブジェクトを破棄
	for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの終了
		m_apPart[nCntTimer]->Uninit();
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

			// 現在の計測ミリ秒を設定
			m_dwTime = timeGetTime() - m_dwTempTime;
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

	for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の更新
		m_apValue[nCntTimer]->Update();
	}

	for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
	{ // 区切りの数分繰り返す

		// 区切りの更新
		m_apPart[nCntTimer]->Update();
	}

	// デバッグ表示
	CManager::GetDebugProc()->Print("タイマー：[%d:%d:%d]\n", m_dwTime / 60000, (m_dwTime / 1000) % 60, m_dwTime % 1000);
	CManager::GetDebugProc()->Print("停止タイマー：[%d:%d:%d]\n", m_dwStopTime / 60000, (m_dwStopTime / 1000) % 60, m_dwStopTime % 1000);
}

//============================================================
//	生成処理
//============================================================
CTimerManager *CTimerManager::Create
(
	const D3DXVECTOR3& rPos,		// 位置
	const D3DXVECTOR3& rSizeValue,	// 数字の大きさ
	const D3DXVECTOR3& rSizePart,	// 区切りの大きさ
	const D3DXVECTOR3& rSpaceValue,	// 数字の空白
	const D3DXVECTOR3& rSpacePart	// 区切りの空白
)
{
	// ポインタを宣言
	CTimerManager *pTimerManager = NULL;	// タイマーマネージャー生成用

	if (UNUSED(pTimerManager))
	{ // 使用されていない場合

		// メモリ確保
		pTimerManager = new CTimerManager;	// タイマーマネージャー
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pTimerManager))
	{ // 確保に成功している場合

		// タイマーマネージャーの初期化
		if (FAILED(pTimerManager->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pTimerManager;
			pTimerManager = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pTimerManager->SetPosition(rPos);

		// 数字の大きさを設定
		pTimerManager->SetScalingValue(rSizeValue);

		// 区切りの大きさを設定
		pTimerManager->SetScalingPart(rSizePart);

		// 数字の空白を設定
		pTimerManager->SetSpaceValue(rSpaceValue);

		// 区切りの空白を設定
		pTimerManager->SetSpacePart(rSpacePart);

		// 確保したアドレスを返す
		return pTimerManager;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	破棄処理
//============================================================
HRESULT CTimerManager::Release(CTimerManager *&prTimerManager)
{
	if (USED(prTimerManager))
	{ // 使用中の場合

		// タイマーマネージャーの終了
		prTimerManager->Uninit();

		// メモリ開放
		delete prTimerManager;
		prTimerManager = NULL;

		// 成功を返す
		return S_OK;
	}
	else { assert(false); return E_FAIL; }	// 非使用中
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
//	数字の大きさの設定処理
//============================================================
void CTimerManager::SetScalingValue(const D3DXVECTOR3& rSize)
{
	// 引数の数字の大きさを設定
	m_sizeValue = rSize;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	区切りの大きさの設定処理
//============================================================
void CTimerManager::SetScalingPart(const D3DXVECTOR3& rSize)
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
//	優先順位の設定処理
//============================================================
void CTimerManager::SetPriority(const int nPriority)
{
	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの優先順位を設定
		for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetPriority(nPriority);
		}

		// 区切りオブジェクトの優先順位を設定
		for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
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
	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの更新状況を設定
		for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetEnableUpdate(bUpdate);
		}

		// 区切りオブジェクトの更新状況を設定
		for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
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
	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの描画状況を設定
		for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
		{ // タイマーの桁数分繰り返す

			m_apValue[nCntTimer]->SetEnableDraw(bDraw);
		}

		// 区切りオブジェクトの描画状況を設定
		for (int nCntTimer = 0; nCntTimer < MAX_PART; nCntTimer++)
		{ // 区切りの数分繰り返す

			m_apPart[nCntTimer]->SetEnableDraw(bDraw);
		}
	}
	else { assert(false); }	// 非使用中
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
//	数字の大きさ取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetScalingValue(void) const
{
	// 数字の大きさを返す
	return m_sizeValue;
}

//============================================================
//	区切りの大きさ取得処理
//============================================================
D3DXVECTOR3 CTimerManager::GetScalingPart(void) const
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

	if (USED(m_apValue[0]) && USED(m_apPart[0]))
	{ // タイマーの先頭数値、タイマーの先頭区切りが使用されている場合

		for (int nCntTimer = 0; nCntTimer < MAX_TIMER + MAX_PART; nCntTimer++)
		{ // タイマーの桁数 + タイマーの区切り数分繰り返す

			if (nCntTimer == MAX_MIN || nCntTimer == MAX_MIN + MAX_SEC + 1)
			{ // 区切りの生成タイミングだった場合

				// ポリゴン生成位置をずらす
				posPoly += spacePart;

				// 区切りの位置を設定
				m_apPart[nNumValue]->SetPosition(posPoly);

				// 区切りの大きさを設定
				m_apPart[nNumValue]->SetScaling(m_sizePart);

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
				m_apValue[nNumPart]->SetPosition(posPoly);

				// 数字の大きさを設定
				m_apValue[nNumPart]->SetScaling(m_sizeValue);

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
	int aNumDivide[MAX_TIMER];	// 分の桁数ごとの分解用

	// 分を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[0], GetMin(), MAX_MIN);

	// 秒を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[MAX_MIN], GetSec(), MAX_SEC);

	// ミリ秒を桁数ごとに分解
	useful::DivideDigitNum(&aNumDivide[MAX_MIN + MAX_SEC], GetMSec(), MAX_MSEC);

	for (int nCntTimer = 0; nCntTimer < MAX_TIMER; nCntTimer++)
	{ // タイマーの桁数分繰り返す

		// 数字の設定
		m_apValue[nCntTimer]->SetNumber(aNumDivide[nCntTimer]);
	}
}
