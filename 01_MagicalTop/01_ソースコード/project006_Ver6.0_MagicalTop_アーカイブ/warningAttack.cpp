//============================================================
//
//	攻撃警告表示処理 [warningAttack.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "warningAttack.h"
#include "manager.h"
#include "scene.h"
#include "renderer.h"
#include "texture.h"

//************************************************************
//	マクロ定義
//************************************************************
#define WARNIN_PRIO	(6)		// 警告表示の優先順位
#define FADEIN_CNT	(240)	// フェードイン開始待機フレーム
#define SET_SCALE	(15.0f)	// 拡大率の初期値
#define SUB_SCALE	(1.5f)	// 拡大率の減算量
#define SUB_FADE	(0.04f)	// フェードのα値の減算量

//************************************************************
//	静的メンバ変数宣言
//************************************************************
const char *CWarningAttack::mc_apTextureFile[] =	// テクスチャ定数
{
	"data\\TEXTURE\\warningAttack000.png",	// 警告表示テクスチャ
};

//************************************************************
//	子クラス [CWarningAttack] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CWarningAttack::CWarningAttack() : CObject2D(CObject::LABEL_UI, WARNIN_PRIO)
{
	// メンバ変数をクリア
	m_size =VEC3_ZERO;		// 元の大きさ
	m_state = STATE_NONE;	// 状態
	m_nCounterState = 0;	// 状態管理カウンター
	m_fScale = 0.0f;		// 拡大率
}

//============================================================
//	デストラクタ
//============================================================
CWarningAttack::~CWarningAttack()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CWarningAttack::Init(void)
{
	// ポインタを宣言
	CTexture *pTexture = CManager::GetTexture();	// テクスチャへのポインタ

	// メンバ変数を初期化
	m_size = VEC3_ZERO;		// 元の大きさ
	m_state = STATE_NONE;	// 状態
	m_nCounterState = 0;	// 状態管理カウンター
	m_fScale = 0.0f;		// 拡大率

	// オブジェクト2Dの初期化
	if (FAILED(CObject2D::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャを登録・割当
	BindTexture(pTexture->Regist(mc_apTextureFile[TEXTURE_WARNING]));

	// 描画を停止する
	SetEnableDraw(false);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CWarningAttack::Uninit(void)
{
	// オブジェクト2Dの終了
	CObject2D::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CWarningAttack::Update(void)
{
	// 変数を宣言
	D3DXCOLOR colWarning = GetColor();	// 警告表示の色

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:

		// 無し

		break;

	case STATE_FADEOUT:

		// 拡大率を減算
		m_fScale -= SUB_SCALE;

		if (m_fScale <= 1.0f)
		{ // 拡大率が最小値以下の場合

			// 拡大率を補正
			m_fScale = 1.0f;

			// 状態を設定
			m_state = STATE_NORMAL;	// 通常状態
		}

		break;

	case STATE_NORMAL:

		if (m_nCounterState < FADEIN_CNT)
		{ // カウンターが一定値より小さい場合

			// カウンターを加算
			m_nCounterState++;
		}
		else
		{ // カウンターが一定値以上の場合

			// カウンターを初期化
			m_nCounterState = 0;

			// 状態を設定
			m_state = STATE_FADEIN;	// フェードイン状態
		}

		break;

	case STATE_FADEIN:

		// 透明度を減算
		colWarning.a -= SUB_FADE;

		if (colWarning.a <= 0.0f)
		{ // 透明度が下がり切った場合

			// 透明度を補正
			colWarning.a = 0.0f;

			// 描画を停止する
			SetEnableDraw(false);

			// 状態を設定
			m_state = STATE_NONE;	// 何もしない状態
		}

		break;

	default:
		assert(false);
		break;
	}

	// 大きさを反映
	SetScaling(m_size * m_fScale);

	// 色を反映
	SetColor(colWarning);

	// オブジェクト2Dの更新
	CObject2D::Update();
}

//============================================================
//	描画処理
//============================================================
void CWarningAttack::Draw(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();	// デバイスのポインタ

	// αテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// αテストの設定

	// オブジェクト2Dの描画
	CObject2D::Draw();

	// αテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// αテストの有効 / 無効の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// αテストの設定
}

//============================================================
//	生成処理
//============================================================
CWarningAttack *CWarningAttack::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rSize	// 大きさ
)
{
	// ポインタを宣言
	CWarningAttack *pWarningAttack = NULL;		// 攻撃警告表示生成用

	if (UNUSED(pWarningAttack))
	{ // 使用されていない場合

		// メモリ確保
		pWarningAttack = new CWarningAttack;	// 攻撃警告表示
	}
	else { assert(false); return NULL; }		// 使用中

	if (USED(pWarningAttack))
	{ // 確保に成功している場合

		// 攻撃警告表示の初期化
		if (FAILED(pWarningAttack->Init()))
		{ // 初期化に失敗した場合

			// メモリ開放
			delete pWarningAttack;
			pWarningAttack = NULL;

			// 失敗を返す
			return NULL;
		}

		// 位置を設定
		pWarningAttack->SetPosition(rPos);

		// 大きさを設定
		pWarningAttack->SetScaling(rSize);

		// 元の大きさを設定
		pWarningAttack->SetScalingOrigin(rSize);

		// 確保したアドレスを返す
		return pWarningAttack;
	}
	else { assert(false); return NULL; }	// 確保失敗
}

//============================================================
//	表示処理
//============================================================
void CWarningAttack::SetDisp(void)
{
	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:

		// 拡大率を初期化
		m_fScale = SET_SCALE;

		// 大きさを初期化
		SetScaling(m_size * m_fScale);

		// 色を初期化
		SetColor(XCOL_WHITE);

		// 描画を再開する
		SetEnableDraw(true);

		// 状態を設定
		m_state = STATE_FADEOUT;	// フェードアウト状態

		break;

	case STATE_FADEOUT:

		// 無し

		break;

	case STATE_NORMAL:

		// カウンターを初期化
		m_nCounterState = 0;

		break;

	case STATE_FADEIN:

		// 色を初期化
		SetColor(XCOL_WHITE);

		// 状態を設定
		m_state = STATE_NORMAL;	// 通常状態

		break;

	default:
		assert(false);
		break;
	}
}

//============================================================
//	元の大きさの設定処理
//============================================================
void CWarningAttack::SetScalingOrigin(const D3DXVECTOR3& rSize)
{
	// 引数の大きさを設定
	m_size = rSize;
}
