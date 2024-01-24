//============================================================
//
//	バブル処理 [bubble.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "bubble.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "model.h"

//************************************************************
//	マクロ定義
//************************************************************
#define BUBBLE_PRIO	(4)		// バブル表示の優先順位
#define REV_SIZE	(0.25f)	// バブル拡大率の補正係数
#define BOLD_ADD	(0.25f)	// バブルの縁取り加算量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CBubble::mc_apModelFile[] =	// モデル定数
{
	"data\\MODEL\\BUBBLE\\bubble000.x",	// バブルモデル
	"data\\MODEL\\BUBBLE\\bubble001.x",	// バブル縁取りモデル
};

//************************************************************
//	子クラス [CBubble] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CBubble::CBubble(const int nMaxLevel, const D3DXVECTOR3& rMaxScale) : CObjectModel(CObject::LABEL_NONE, BUBBLE_PRIO), m_maxScale(rMaxScale), m_nMaxLevel(nMaxLevel)
{
	// メンバ変数をクリア
	m_pBold			= NULL;			// 縁取りモデル
	m_currentScale	= VEC3_ZERO;	// 現在の拡大率
	m_destScale		= VEC3_ZERO;	// 目標の拡大率
	m_pParentObject	= NULL;			// 親オブジェクト
	m_fPosUp		= 0.0f;			// バブルのY位置加算量
	m_nLevel		= 0;			// 大きさレベル
}

//============================================================
//	デストラクタ
//============================================================
CBubble::~CBubble()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CBubble::Init(void)
{
	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ

	// メンバ変数を初期化
	m_pBold			= NULL;			// 縁取りモデル
	m_currentScale	= VEC3_ZERO;	// 現在の拡大率
	m_destScale		= VEC3_ZERO;	// 目標の拡大率
	m_pParentObject	= NULL;			// 親オブジェクト
	m_fPosUp		= 0.0f;			// バブルのY位置加算量
	m_nLevel		= 0;			// 大きさレベル

	// オブジェクトモデルの初期化
	if (FAILED(CObjectModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 縁取りモデルの生成
	m_pBold = CObjectModel::Create(VEC3_ZERO, VEC3_ZERO);
	if (UNUSED(m_pBold))
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルを登録・割当
	m_pBold->BindModel(pModel->GetModel(pModel->Regist(mc_apModelFile[MODEL_BOLD])));

	// 描画しない設定にする
	m_pBold->SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CBubble::Uninit(void)
{
	// 縁取りモデルの終了
	m_pBold->Uninit();

	// オブジェクトモデルの終了
	CObjectModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CBubble::Update(void)
{
	// 変数を宣言
	D3DXVECTOR3 diffScale = VEC3_ZERO;	// 差分拡大率

	// 差分拡大率を設定
	diffScale = m_destScale - m_currentScale;

	// 拡大率を設定
	m_currentScale += diffScale * REV_SIZE;

	// TODO：ポインタの確認もっときれいに
	if (CObject::CheckUse(m_pParentObject))
	{ // 親オブジェクトが使用されていた場合

		// 変数を宣言
		D3DXVECTOR3 posParent = m_pParentObject->GetPosition();	// 親オブジェクト位置
		D3DXVECTOR3 posBubble = VEC3_ZERO;		// バブル位置
		D3DXVECTOR3 scaleBubble = VEC3_ZERO;	// バブル拡大率

		// バブルの位置を求める
		posBubble = posParent;		// 親オブジェクトの座標代入
		posBubble.y += m_fPosUp;	// Y位置上昇量を加算

		// バブルの拡大率を設定
		scaleBubble = m_currentScale;

		// 位置を設定
		SetPosition(posBubble);

		// 拡大率を設定
		SetScaling(scaleBubble);
	}

	// 縁取りモデルの更新
	m_pBold->Update();

	// オブジェクトモデルの更新
	CObjectModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CBubble::Draw(void)
{
	// オブジェクトモデルの描画
	CObjectModel::Draw();

	// 縁取りモデルの描画
	m_pBold->Draw();
}

//============================================================
//	半径取得処理
//============================================================
float CBubble::GetRadius(void) const
{
	// 変数を宣言
	D3DXVECTOR3 scale = GetScaling();	// 拡大率
	D3DXVECTOR3 size = scale * GetModelData().fRadius;	// 大きさ

	// バブルの大きさの平均サイズを返す
	return (size.x + size.y + size.z) / 3;
}

//============================================================
//	生成処理
//============================================================
CBubble *CBubble::Create
( // 引数
	CObject *pObject,				// 親オブジェクト
	const int nMaxLevel,			// 最大レベル
	const D3DXVECTOR3& rMaxScale,	// 最大拡大率
	const D3DXVECTOR3& rPos,		// 位置
	const float fPosUp,				// Y位置加算量
	const D3DXVECTOR3& rRot,		// 向き
	const D3DXVECTOR3& rScale		// 拡大率
)
{
	// ポインタを宣言
	CModel *pModel = CManager::GetModel();	// モデルへのポインタ
	CBubble *pBubble = NULL;	// バブル生成用

	if (UNUSED(pBubble))
	{ // 使用されていない場合

		// メモリ確保
		pBubble = new CBubble(nMaxLevel, rMaxScale);	// バブル
	}
	else { assert(false); return NULL; }	// 使用中

	if (USED(pBubble))
	{ // 使用されている場合
		
		// バブルの初期化
		if (FAILED(pBubble->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pBubble;
			pBubble = NULL;

			// 失敗を返す
			return NULL;
		}

		// モデルを登録・割当
		pBubble->BindModel(pModel->GetModel(pModel->Regist(mc_apModelFile[MODEL_BUBBLE])));

		// 位置を設定
		pBubble->SetPosition(rPos);

		// 向きを設定
		pBubble->SetRotation(rRot);

		// 大きさを設定
		pBubble->SetScaling(rScale);

		// Y位置加算量の設定
		pBubble->SetPositionUp(fPosUp);

		// 親オブジェクトを設定
		pBubble->SetParentObject(pObject);

		// 確保したアドレスを返す
		return pBubble;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	位置の設定処理
//============================================================
void CBubble::SetPosition(const D3DXVECTOR3& rPos)
{
	// 縁取りモデルの位置を設定
	m_pBold->SetPosition(rPos);

	// オブジェクトモデルの位置を設定
	CObjectModel::SetPosition(rPos);
}

//============================================================
//	向きの設定処理
//============================================================
void CBubble::SetRotation(const D3DXVECTOR3& rRot)
{
	// 縁取りモデルの向きを設定
	m_pBold->SetRotation(rRot);

	// オブジェクトモデルの向きを設定
	CObjectModel::SetRotation(rRot);
}

//============================================================
//	大きさの設定処理
//============================================================
void CBubble::SetScaling(const D3DXVECTOR3& rScale)
{
	// 縁取りモデルの大きさを設定
	m_pBold->SetScaling(D3DXVECTOR3(rScale.x + BOLD_ADD, rScale.y + BOLD_ADD, rScale.z + BOLD_ADD));

	// オブジェクトモデルの大きさを設定
	CObjectModel::SetScaling(rScale);
}

//============================================================
//	レベル加算処理
//============================================================
void CBubble::AddLevel(const int nAdd)
{
	// レベルを引数分加算
	m_nLevel += nAdd;

	// 範囲内制限
	useful::LimitNum(m_nLevel, 0, m_nMaxLevel);

	// バブルの目標拡大率を設定
	m_destScale.x = m_nLevel * (m_maxScale.x / (float)m_nMaxLevel);
	m_destScale.y = m_nLevel * (m_maxScale.y / (float)m_nMaxLevel);
	m_destScale.z = m_nLevel * (m_maxScale.z / (float)m_nMaxLevel);
}

//============================================================
//	レベル設定処理
//============================================================
void CBubble::SetLevel(const int nNum)
{
	// 引数の値をレベルに設定
	m_nLevel = nNum;

	// 範囲内制限
	useful::LimitNum(m_nLevel, 0, m_nMaxLevel);

	// バブルの拡大率を設定
	m_currentScale.x = m_nLevel * (m_maxScale.x / (float)m_nMaxLevel);
	m_currentScale.y = m_nLevel * (m_maxScale.y / (float)m_nMaxLevel);
	m_currentScale.z = m_nLevel * (m_maxScale.z / (float)m_nMaxLevel);
}

//============================================================
//	レベル取得処理
//============================================================
int CBubble::GetLevel(void) const
{
	// レベルを返す
	return m_nLevel;
}

//============================================================
//	最大半径取得処理
//============================================================
float CBubble::GetMaxRadius(void) const
{
	// 変数を宣言
	D3DXVECTOR3 size = m_maxScale * GetModelData().fRadius;	// 大きさ

	// バブルの最大の大きさの平均サイズを返す
	return (size.x + size.y + size.z) / 3;
}

//============================================================
//	最大レベル取得処理
//============================================================
int CBubble::GetMaxLevel(void) const
{
	// 最大レベルを返す
	return m_nMaxLevel;
}

//============================================================
//	Y位置加算量の設定処理
//============================================================
void CBubble::SetPositionUp(const float fUp)
{
	// 引数のY位置加算量を設定
	m_fPosUp = fUp;
}

//============================================================
//	Y位置加算量取得処理
//============================================================
float CBubble::GetPositionUp(void) const
{
	// Y位置加算量を返す
	return m_fPosUp;
}

//============================================================
//	親オブジェクトの設定処理
//============================================================
void CBubble::SetParentObject(CObject *pObject)
{
	// 引数のオブジェクトを設定
	m_pParentObject = pObject;
}
