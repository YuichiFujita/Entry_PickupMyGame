//============================================================
//
//	モデルヘッダー [model.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _MODEL_H_
#define _MODEL_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include <vector>
#include <string>

//************************************************************
//	定数宣言
//************************************************************
namespace model
{
	const int MAX_NUM = 256;	// モデルの最大数
}

//************************************************************
//	クラス定義
//************************************************************
// モデルクラス
class CModel
{
public:
	// コンストラクタ
	CModel();

	// デストラクタ
	~CModel();

	// モデル構造体
	struct SModel
	{
		LPD3DXMESH		pMesh;			// メッシュ (頂点情報) へのポインタ
		LPD3DXBUFFER	pBuffMat;		// マテリアルへのポインタ
		DWORD			dwNumMat;		// マテリアルの数
		D3DXVECTOR3		vtxMin;			// 最小の頂点座標
		D3DXVECTOR3		vtxMax;			// 最大の頂点座標
		D3DXVECTOR3		size;			// 大きさ
		float			fRadius;		// 半径
		int				*pTextureID;	// テクスチャインデックス
	};

	// メンバ関数
	int Regist(const char *pFileName);	// モデル登録
	SModel *GetModel(const int nID);	// モデル情報取得

	// 静的メンバ関数
	static CModel *Create(void);			// 生成
	static void Release(CModel *&pModel);	// 破棄

private:
	// メンバ関数
	HRESULT Load(void);		// モデル生成
	void Unload(void);		// モデル破棄
	void LoadSetup(void);	// セットアップ
	HRESULT LoadXFileModel(const int nID, const char *pFileName);	// xファイルの読み込み
	HRESULT LoadTextureModel(const int nID);	// テクスチャの読み込み
	void SetCollisionModel(const int nID);		// 当たり判定の作成

	// メンバ変数
	SModel m_aModel[model::MAX_NUM];		// モデルへのポインタ
	std::vector<std::string> m_sFileName;	// 読み込んだテクスチャファイル名

	// 静的メンバ変数
	static int m_nNumAll;	// モデルの総数
};

#endif	// _MODEL_H_
