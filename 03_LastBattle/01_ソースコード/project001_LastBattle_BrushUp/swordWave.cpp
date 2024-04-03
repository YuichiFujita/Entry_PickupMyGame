//============================================================
//
//	剣波動処理 [swordWave.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "swordWave.h"
#include "manager.h"
#include "sound.h"
#include "player.h"
#include "collision.h"
#include "enemy.h"
#include "collSphere.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const POSGRID2	PART		= POSGRID2(64, 1);	// 分割数
	const POSGRID2	TEX_PART	= POSGRID2(1, 1);	// テクスチャ分割数
	const float		SUB_ALPHA	= 0.05f;			// 剣波動の透明度減算量
	const int		DMG_WAVE	= 15;				// 剣波動のダメージ量
}

//************************************************************
//	子クラス [CSwordWave] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSwordWave::CSwordWave() :
	m_addGrow	(SGrow(0.0f, 0.0f, 0.0f))	// 成長加速量
{

}

//============================================================
//	デストラクタ
//============================================================
CSwordWave::~CSwordWave()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSwordWave::Init(void)
{
	// メンバ変数を初期化
	m_addGrow = SGrow(0.0f, 0.0f, 0.0f);	// 成長加速量

	// 波動の初期化
	if (FAILED(CWave::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 分割数の設定
	if (FAILED(SetPattern(PART)))
	{ // 分割数の設定に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// テクスチャの設定
	SetTexture(TEXTURE_SWORD);

	// テクスチャ分割数の設定
	SetTexPattern(TEX_PART);

	// レンダーステートの情報を取得
	CRenderState *pRenderState = GetRenderState();

	// αブレンドの設定
	pRenderState->SetAlphaBlend(CRenderState::BLEND_ADD);

	// Zテストの設定
	pRenderState->SetZFunc(D3DCMP_ALWAYS);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSwordWave::Uninit(void)
{
	// 波動の終了
	CWave::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CSwordWave::Update(void)
{
	// 波動の更新
	CWave::Update();

	// 敵との当たり判定
	CollisionEnemy();

	// 成長を加速させる
	SGrow curGrow = GetGrow();	// 現在の成長情報
	SetGrow(SGrow
	( // 引数
		curGrow.fAddHoleRadius	+ m_addGrow.fAddHoleRadius,	// 穴の半径の成長量
		curGrow.fAddThickness	+ m_addGrow.fAddThickness,	// 太さの成長量
		curGrow.fSubAlpha		+ m_addGrow.fSubAlpha		// 透明度の成長量
	));
}

//============================================================
//	描画処理
//============================================================
void CSwordWave::Draw(CShader *pShader)
{
	// 波動の描画
	CWave::Draw(pShader);
}

//============================================================
//	生成処理
//============================================================
CSwordWave *CSwordWave::Create
(
	const D3DXVECTOR3& rPos,	// 位置
	const D3DXVECTOR3& rRot,	// 向き
	const SGrow& rGrow,			// 成長量
	const SGrow& rAddGrow,		// 成長加速量
	const float fHoleRadius,	// 穴の半径
	const float fThickness		// 太さ
)
{
	// 剣波動の生成
	CSwordWave *pSwordWave = new CSwordWave;
	if (pSwordWave == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 剣波動の初期化
		if (FAILED(pSwordWave->Init()))
		{ // 初期化に失敗した場合

			// 剣波動の破棄
			SAFE_DELETE(pSwordWave);
			return nullptr;
		}

		// 位置を設定
		pSwordWave->SetVec3Position(rPos);

		// 向きを設定
		pSwordWave->SetVec3Rotation(rRot);

		// 成長情報を設定
		pSwordWave->SetGrow(rGrow);

		// 成長加速情報を設定
		pSwordWave->SetAddGrow(rAddGrow);

		// 穴の半径を設定
		pSwordWave->SetHoleRadius(fHoleRadius);

		// 太さを設定
		pSwordWave->SetThickness(fThickness);

		// 確保したアドレスを返す
		return pSwordWave;
	}
}

//============================================================
//	敵との当たり判定
//============================================================
void CSwordWave::CollisionEnemy(void)
{
	CListManager<CEnemy> *pList = CEnemy::GetList();	// リストマネージャー
	if (pList == nullptr)		 { return; }	// リスト未使用
	if (pList->GetNumAll() <= 0) { return; }	// 敵が存在しない

	std::list<CEnemy*> list = pList->GetList();	// 敵リスト
	for (const auto& rEnemy : list)
	{ // リストのすべてを繰り返す

		int nNumParts = rEnemy->GetNumModel();	// パーツ数
		for (int nCntColl = 0; nCntColl < nNumParts; nCntColl++)
		{ // パーツ数分繰り返す

			int nCntArray = 0;	// 判定情報の要素番号
			CCollSphere *pColl = rEnemy->GetCollision(nCntColl);			// 円判定情報
			std::vector<CCollSphere::SInfo> vector = pColl->GetVector();	// 円判定配列
			for (const auto& rColl : vector)
			{ // 配列の要素数分繰り返す

				// 敵の判定位置を計算
				D3DXVECTOR3 posCollEnemy = pColl->CalcWorldPosition(nCntArray);

				// 要素番号を次に
				nCntArray++;

				// 攻撃の当たり判定
				bool bHit = collision::Circle3D
				( // 引数
					GetVec3Position(),	// 判定位置
					posCollEnemy,		// 判定目標位置
					GetHoleRadius() + GetThickness(),	// 判定半径
					rColl.fRadius						// 判定目標半径
				);
				if (bHit)
				{ // 攻撃が当たった場合

					// 敵のヒット処理
					if (rEnemy->Hit(DMG_WAVE))
					{ // 敵に攻撃できた場合

						// 剣ヒット音の再生
						PLAY_SOUND(CSound::LABEL_SE_SWORD_HIT);
					}

					// 関数を抜ける
					return;
				}
			}
		}
	}
}
