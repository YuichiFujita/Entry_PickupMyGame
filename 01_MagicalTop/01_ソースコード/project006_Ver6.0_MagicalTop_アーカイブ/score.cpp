//============================================================
//
//	スコア処理 [score.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "score.h"
#include "manager.h"
#include "renderer.h"
#include "value.h"

//************************************************************
//	マクロ定義
//************************************************************
#define SCORE_PRIO	(6)	// スコアの優先順位

#define SCO_NUMMIN	(0)			// 最少スコア
#define SCO_NUMMAX	(99999999)	// 最大スコア

//************************************************************
//	子クラス [CScore] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CScore::CScore() : CObject(CObject::LABEL_NUMBER, DEFAULT_PRIO)
{
	// メンバ変数をクリア
	memset(&m_apValue[0], 0, sizeof(m_apValue));	// 数値の情報
	m_pos	= VEC3_ZERO;	// 位置
	m_size	= VEC3_ZERO;	// 大きさ
	m_space	= VEC3_ZERO;	// 空白
	m_nNum	= 0;			// スコア
}

//============================================================
//	デストラクタ
//============================================================
CScore::~CScore()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CScore::Init(void)
{
	// メンバ変数を初期化
	memset(&m_apValue[0], 0, sizeof(m_apValue));	// 数値の情報
	m_pos	= VEC3_ZERO;	// 位置
	m_size	= VEC3_ZERO;	// 大きさ
	m_space	= VEC3_ZERO;	// 空白
	m_nNum	= 0;			// スコア

	for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
	{ // スコアの桁数分繰り返す

		// 数字の生成
		m_apValue[nCntScore] = CValue::Create(CValue::TEXTURE_MAGIC_GREEN);
		if (UNUSED(m_apValue[nCntScore]))
		{ // 生成に失敗した場合

			// 失敗を返す
			assert(false);
			return E_FAIL;
		}

		// 優先順位を設定
		m_apValue[nCntScore]->SetPriority(SCORE_PRIO);
	}

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CScore::Uninit(void)
{
	// 数字オブジェクトを破棄
	for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
	{ // スコアの桁数分繰り返す

		// 数字の終了
		m_apValue[nCntScore]->Uninit();
	}

	// 自身のオブジェクトを破棄
	Release();
}

//============================================================
//	更新処理
//============================================================
void CScore::Update(void)
{
	for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
	{ // スコアの桁数分繰り返す

		// 数字の更新
		m_apValue[nCntScore]->Update();
	}
}

//============================================================
//	描画処理
//============================================================
void CScore::Draw(void)
{

}

//============================================================
//	生成処理
//============================================================
CScore *CScore::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize,	// 大きさ
	const D3DXVECTOR3& rSpace	// 空白
)
{
	// ポインタを宣言
	CScore *pScore = NULL;		// スコア生成用

	if (UNUSED(pScore))
	{ // 使用されていない場合

		// メモリ確保
		pScore = new CScore;	// スコア
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pScore))
	{ // 確保に成功している場合

		// スコアの初期化
		if (FAILED(pScore->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pScore;
			pScore = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pScore->SetPosition(rPos);

		// 大きさを設定
		pScore->SetScaling(rSize);

		// 空白を設定
		pScore->SetSpace(rSpace);

		// 確保したアドレスを返す
		return pScore;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	加算処理
//============================================================
void CScore::Add(const int nNum)
{
	// スコアを引数分加算
	m_nNum += nNum;

	// スコアの補正
	useful::LimitNum(m_nNum, SCO_NUMMIN, SCO_NUMMAX);

	// 数字のテクスチャ座標の設定
	SetTexNum();
}

//============================================================
//	設定処理
//============================================================
void CScore::Set(const int nNum)
{
	// スコアを引数の値に設定
	m_nNum = nNum;

	// スコアの補正
	useful::LimitNum(m_nNum, SCO_NUMMIN, SCO_NUMMAX);

	// 数字のテクスチャ座標の設定
	SetTexNum();
}

//============================================================
//	取得処理
//============================================================
int CScore::Get(void)
{
	// スコアの値を返す
	return m_nNum;
}

//============================================================
//	位置の設定処理
//============================================================
void CScore::SetPosition(const D3DXVECTOR3& rPos)
{
	// 引数の位置を設定
	m_pos = rPos;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	大きさの設定処理
//============================================================
void CScore::SetScaling(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを設定
	m_size = rSize;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	空白の設定処理
//============================================================
void CScore::SetSpace(const D3DXVECTOR3& rSpace)
{
	// 引数の空白を設定
	m_space = rSpace;

	// 数字の表示設定
	SetDrawValue();
}

//============================================================
//	優先順位の設定処理
//============================================================
void CScore::SetPriority(const int nPriority)
{
	// 自身の優先順位を設定
	CObject::SetPriority(nPriority);

	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの優先順位を設定
		for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
		{ // スコアの桁数分繰り返す

			m_apValue[nCntScore]->SetPriority(nPriority);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	更新状況の設定処理
//============================================================
void CScore::SetEnableUpdate(const bool bUpdate)
{
	// 自身の更新状況を設定
	CObject::SetEnableUpdate(bUpdate);

	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの更新状況を設定
		for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
		{ // スコアの桁数分繰り返す

			m_apValue[nCntScore]->SetEnableUpdate(bUpdate);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	描画状況の設定処理
//============================================================
void CScore::SetEnableDraw(const bool bDraw)
{
	// 自身の描画状況を設定
	CObject::SetEnableDraw(bDraw);

	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		// 数字オブジェクトの描画状況を設定
		for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
		{ // スコアの桁数分繰り返す

			m_apValue[nCntScore]->SetEnableDraw(bDraw);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	位置取得処理
//============================================================
D3DXVECTOR3 CScore::GetPosition(void) const
{
	// 位置を返す
	return m_pos;
}

//============================================================
//	大きさ取得処理
//============================================================
D3DXVECTOR3 CScore::GetScaling(void) const
{
	// 大きさを返す
	return m_size;
}

//============================================================
//	空白取得処理
//============================================================
D3DXVECTOR3 CScore::GetSpace(void) const
{
	// 空白を返す
	return m_space;
}

//============================================================
//	数字の表示設定処理
//============================================================
void CScore::SetDrawValue(void)
{
	if (USED(m_apValue[0]))
	{ // スコアの先頭の数値が使用されている場合

		for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
		{ // スコアの桁数分繰り返す

			// 数字の位置を設定
			m_apValue[nCntScore]->SetPosition(m_pos + (m_space * (float)nCntScore));

			// 数字の大きさを設定
			m_apValue[nCntScore]->SetScaling(m_size);
		}
	}
	else { assert(false); }	// 非使用中
}

//============================================================
//	数字のテクスチャ座標の設定処理
//============================================================
void CScore::SetTexNum(void)
{
	// 変数を宣言
	int aNumDivide[MAX_SCORE];	// 桁数ごとの分解用

	// スコアを桁数ごとに分解
	useful::DivideDigitNum
	( // 引数
		&aNumDivide[0],	// 分解結果の格納配列
		m_nNum,			// 分解する数値
		MAX_SCORE		// 分解する数字の桁数
	);

	for (int nCntScore = 0; nCntScore < MAX_SCORE; nCntScore++)
	{ // スコアの桁数分繰り返す

		// 数字の設定
		m_apValue[nCntScore]->SetNumber(aNumDivide[nCntScore]);
	}
}
