//============================================================
//
//	剣処理 [sword.cpp]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	インクルードファイル
//************************************************************
#include "sword.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "collision.h"
#include "swordWaveManager.h"
#include "orbit.h"
#include "enemy.h"
#include "collSphere.h"
#include "scene.h"
#include "player.h"

//************************************************************
//	定数宣言
//************************************************************
namespace
{
	const char *MODEL_FILE[] =	// モデルファイル
	{
		"data\\MODEL\\PLAYER\\15_sword.x",	// 剣
	};
	const char *SETUP_TXT	= "data\\TXT\\sword.txt";	// セットアップテキスト相対パス
	const float	SUB_ALPHA	= 0.05f;	// 透明度の減算量
	const int	DMG_HIT		= 10;		// ヒット時のダメージ量
	const int	ORBIT_PART	= 18;		// 分割数

	const COrbit::SOffset ORBIT_OFFSET = COrbit::SOffset	// オフセット情報
	(
		D3DXVECTOR3(0.0f, 0.0f, -10.0f),
		D3DXVECTOR3(0.0f, 0.0f, -65.0f),
		D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f)
	);
}

//************************************************************
//	静的メンバ変数宣言
//************************************************************
CSword::SCollInfo CSword::m_collInfo = {};	// 判定情報

//************************************************************
//	スタティックアサート
//************************************************************
static_assert(NUM_ARRAY(MODEL_FILE) == CSword::MODEL_MAX, "ERROR : Model Count Mismatch");

//************************************************************
//	子クラス [CSword] のメンバ関数
//************************************************************
//============================================================
//	コンストラクタ
//============================================================
CSword::CSword() : CMultiModel(LABEL_NONE, DIM_3D, object::DEFAULT_PRIO),
	m_pWaveManager	(nullptr),		// 剣波動生成マネージャー
	m_pOrbit		(nullptr),		// 軌跡の情報
	m_state			(STATE_NONE),	// 状態
	m_bAttack		(false),		// 攻撃状況
	m_nCounterState	(0)				// 状態管理カウンター
{

}

//============================================================
//	デストラクタ
//============================================================
CSword::~CSword()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CSword::Init(void)
{
	// メンバ変数を初期化
	m_pWaveManager	= nullptr;		// 剣波動生成マネージャー
	m_pOrbit		= nullptr;		// 軌跡の情報
	m_state			= STATE_NONE;	// 状態
	m_bAttack		= true;			// 攻撃状況
	m_nCounterState	= 0;			// 状態管理カウンター

	// マルチモデルの初期化
	if (FAILED(CMultiModel::Init()))
	{ // 初期化に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// モデルを登録・割当
	BindModel(MODEL_FILE[MODEL_SWORD]);

	// 剣波動生成マネージャーの生成
	m_pWaveManager = CSwordWaveManager::Create();
	if (m_pWaveManager == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 軌跡の生成
	m_pOrbit = COrbit::Create
	( // 引数
		GetPtrMtxWorld(),	// 親マトリックス
		ORBIT_OFFSET,		// オフセット情報
		ORBIT_PART			// 分割数
	);
	if (m_pOrbit == nullptr)
	{ // 生成に失敗した場合

		// 失敗を返す
		assert(false);
		return E_FAIL;
	}

	// 軌跡を消す
	m_pOrbit->SetState(COrbit::STATE_NONE);

	// 成功を返す
	return S_OK;
}

//============================================================
//	終了処理
//============================================================
void CSword::Uninit(void)
{
	// 判定情報の破棄
	SAFE_DEL_ARRAY(m_collInfo.pColl);

	// 剣波動生成マネージャーの破棄
	SAFE_REF_RELEASE(m_pWaveManager);

	// 軌跡の終了
	SAFE_UNINIT(m_pOrbit);

	// マルチモデルの終了
	CMultiModel::Uninit();
}

//============================================================
//	更新処理
//============================================================
void CSword::Update(void)
{
	if (m_state == STATE_VANISH)
	{ // 消失中の場合

		// 変数を宣言
		float fAlpha = GetAlpha();	// 透明度

		// 透明にしていく
		fAlpha -= SUB_ALPHA;

		if (fAlpha <= 0.0f)
		{ // 透明になり切った場合

			// 何もしない状態にする
			SetState(STATE_NONE);
		}

		// 透明度を反映
		SetAlpha(fAlpha);
	}

	if (m_bAttack)
	{ // 攻撃判定がONの場合

		if (m_pOrbit->GetState() != STATE_NORMAL)
		{ // まだ表示されていない場合

			// 軌跡を出す
			m_pOrbit->SetState(COrbit::STATE_NORMAL);
		}
	}
	else
	{ // 攻撃判定がOFFの場合

		// 軌跡を消す
		m_pOrbit->SetState(COrbit::STATE_VANISH);
	}

	// TODO：剣自体の判定をOFF
#if 0
	// 敵との当たり判定
	CollisionEnemy();
#endif

	// 剣波動生成マネージャーの更新
	m_pWaveManager->Update();

	// 軌跡の更新
	m_pOrbit->Update();

	// マルチモデルの更新
	CMultiModel::Update();
}

//============================================================
//	描画処理
//============================================================
void CSword::Draw(CShader *pShader)
{
	if (m_state != STATE_NONE)
	{ // 何もしない状態以外の場合

		// マルチモデルの描画
		CMultiModel::Draw(pShader);
	}
}

//============================================================
//	状態取得処理
//============================================================
int CSword::GetState(void) const
{
	// 状態を返す
	return m_state;
}

//============================================================
//	生成処理
//============================================================
CSword *CSword::Create
(
	CObject *pParent,			// 親オブジェクト
	const D3DXVECTOR3 &rPos,	// 位置
	const D3DXVECTOR3 &rRot		// 向き
)
{
	// 剣の生成
	CSword *pSword = new CSword;
	if (pSword == nullptr)
	{ // 生成に失敗した場合

		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// 剣の初期化
		if (FAILED(pSword->Init()))
		{ // 初期化に失敗した場合

			// 剣の破棄
			SAFE_DELETE(pSword);
			return nullptr;
		}

		// 親オブジェクトを設定
		pSword->SetParentObject(pParent);

		// 位置を設定
		pSword->SetVec3Position(rPos);

		// 向きを設定
		pSword->SetVec3Rotation(rRot);

		// 確保したアドレスを返す
		return pSword;
	}
}

//============================================================
//	状態の設定処理
//============================================================
void CSword::SetState(const EState state)
{
	if (state == m_state && state != STATE_NORMAL)
	{ // 設定する状態が現在の状態且つ、設定する状態が通常状態の場合

		// 処理を抜ける
		return;
	}

	if (m_state == STATE_NONE && state == STATE_VANISH)
	{ // すでに消えているのに消失させようとしている場合

		// 処理を抜ける
		return;
	}

	// 引数の状態を設定
	m_state = state;

	switch (m_state)
	{ // 状態ごとの処理
	case STATE_NONE:	// 何もしない状態

		// 軌跡の自動描画をOFFにする
		m_pOrbit->SetState(COrbit::STATE_NONE);
		m_pOrbit->SetEnableDraw(false);

	case STATE_VANISH:	// 消失状態
		break;

	case STATE_NORMAL:	// 通常状態

		// 不透明にする
		SetAlpha(1.0f);

		// 軌跡の自動描画をONにする
		m_pOrbit->SetState(COrbit::STATE_NORMAL);
		m_pOrbit->SetEnableDraw(true);

		break;

	default:	// 例外状態
		assert(false);
		break;
	}
}

//============================================================
//	敵との当たり判定
//============================================================
void CSword::CollisionEnemy(void)
{
	CListManager<CEnemy> *pList = CEnemy::GetList();	// リストマネージャー
	if (pList == nullptr)		 { return; }	// リスト未使用
	if (pList->GetNumAll() <= 0) { return; }	// 敵が存在しない
	if (!m_bAttack)				 { return; }	// 攻撃OFF

	D3DXMATRIX mtxWorld = GetMtxWorld();		// ワールドマトリックス
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

				for (int i = 0; i < m_collInfo.nNumColl; i++)
				{ // 判定数分繰り返す

					D3DXVECTOR3 posCollSword;		// 剣の判定位置
					D3DXMATRIX mtxTrans, mtxColl;	// マトリックス計算用

					// オフセットマトリックスを作成
					D3DXMatrixTranslation
					( // 引数
						&mtxTrans,
						m_collInfo.pColl[i].offset.x,
						m_collInfo.pColl[i].offset.y,
						m_collInfo.pColl[i].offset.z
					);

					// 剣のマトリックスと掛け合わせる
					D3DXMatrixMultiply(&mtxColl, &mtxTrans, &mtxWorld);

					// 武器の判定中心位置を設定
					posCollSword = useful::GetMatrixPosition(mtxColl);

					// 攻撃の当たり判定
					bool bHit = collision::Circle3D
					( // 引数
						posCollSword,	// 判定位置
						posCollEnemy,	// 判定目標位置
						m_collInfo.pColl[i].fRadius,	// 判定半径
						rColl.fRadius					// 判定目標半径
					);
					if (bHit)
					{ // 攻撃が当たった場合

						// 敵のヒット処理
						if (rEnemy->Hit(DMG_HIT))
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
}

//============================================================
//	セットアップ処理
//============================================================
void CSword::LoadSetup(void)
{
	// 変数を宣言
	int nCurrentID = 0;	// 現在の読み込み数の保存用
	int nEnd = 0;		// テキスト読み込み終了の確認用

	// 変数配列を宣言
	char aString[MAX_STRING];	// テキストの文字列の代入用

	// ポインタを宣言
	FILE *pFile;	// ファイルポインタ

	// 判定情報の破棄
	SAFE_DEL_ARRAY(m_collInfo.pColl);

	// 静的メンバ変数の情報をクリア
	memset(&m_collInfo, 0, sizeof(m_collInfo));	// 判定情報

	// ファイルを読み込み形式で開く
	pFile = fopen(SETUP_TXT, "r");

	if (pFile != nullptr)
	{ // ファイルが開けた場合

		do
		{ // 読み込んだ文字列が EOF ではない場合ループ

			// ファイルから文字列を読み込む
			nEnd = fscanf(pFile, "%s", &aString[0]);	// テキストを読み込みきったら EOF を返す

			// 判定の設定
			if (strcmp(&aString[0], "COLLISION_SET") == 0)
			{ // 読み込んだ文字列が COLLISION_SET の場合

				// 現在の読み込み数を初期化
				nCurrentID = 0;

				do
				{ // 読み込んだ文字列が END_COLLISION_SET ではない場合ループ

					// ファイルから文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);

					if (strcmp(&aString[0], "NUMCOLL") == 0)
					{ // 読み込んだ文字列が NUMCOLL の場合

						fscanf(pFile, "%s", &aString[0]);			// = を読み込む (不要)
						fscanf(pFile, "%d", &m_collInfo.nNumColl);	// 判定数を読み込む

						if (m_collInfo.nNumColl > 0)
						{ // 判定がある場合

							if (m_collInfo.pColl == nullptr)
							{ // 判定情報が使用されていない場合

								// 判定情報の読み込み数分メモリ確保
								m_collInfo.pColl = new SColl[m_collInfo.nNumColl];

								if (m_collInfo.pColl != nullptr)
								{ // 確保に成功した場合

									// メモリクリア
									memset(m_collInfo.pColl, 0, sizeof(SColl) * m_collInfo.nNumColl);
								}
								else { assert(false); }	// 確保失敗
							}
							else { assert(false); }	// 使用中
						}
						else
						{ // 読み込むものがない場合

							// 処理を抜ける
							break;
						}
					}
					else if (strcmp(&aString[0], "COLLSET") == 0)
					{ // 読み込んだ文字列が COLLSET の場合
			
						do
						{ // 読み込んだ文字列が END_COLLSET ではない場合ループ
			
							// ファイルから文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
			
							if (strcmp(&aString[0], "OFFSET") == 0)
							{ // 読み込んだ文字列が OFFSET の場合

								fscanf(pFile, "%s", &aString[0]);								// = を読み込む (不要)
								fscanf(pFile, "%f", &m_collInfo.pColl[nCurrentID].offset.x);	// 判定オフセットXを読み込む
								fscanf(pFile, "%f", &m_collInfo.pColl[nCurrentID].offset.y);	// 判定オフセットYを読み込む
								fscanf(pFile, "%f", &m_collInfo.pColl[nCurrentID].offset.z);	// 判定オフセットZを読み込む
							}
							else if (strcmp(&aString[0], "RADIUS") == 0)
							{ // 読み込んだ文字列が RADIUS の場合

								fscanf(pFile, "%s", &aString[0]);							// = を読み込む (不要)
								fscanf(pFile, "%f", &m_collInfo.pColl[nCurrentID].fRadius);	// 判定半径を読み込む
							}
						} while (strcmp(&aString[0], "END_COLLSET") != 0);	// 読み込んだ文字列が END_COLLSET ではない場合ループ

						// 読込総数オーバー
						assert(nCurrentID < m_collInfo.nNumColl);

						// 現在の読み込み数を加算
						nCurrentID++;
					}
				} while (strcmp(&aString[0], "END_COLLISION_SET") != 0);	// 読み込んだ文字列が END_COLLISION_SET ではない場合ループ

				// 読込総数の不一致
				assert(nCurrentID == m_collInfo.nNumColl);
			}
		} while (nEnd != EOF);	// 読み込んだ文字列が EOF ではない場合ループ
		
		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{ // ファイルが開けなかった場合

		// エラーメッセージボックス
		MessageBox(nullptr, "剣セットアップの読み込みに失敗！", "警告！", MB_ICONWARNING);
	}
}
