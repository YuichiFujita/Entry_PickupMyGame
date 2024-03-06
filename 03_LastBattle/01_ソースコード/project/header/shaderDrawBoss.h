//============================================================
//
//	ボス描画シェーダーヘッダー [shaderDrawBoss.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SHADER_DRAWBOSS_H_
#define _SHADER_DRAWBOSS_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "shader.h"

//************************************************************
//	クラス定義
//************************************************************
// ボス描画シェーダークラス
class CDrawBossShader : public CShader
{
public:
	// テクスチャ列挙
	enum ETexture
	{
		TEXTURE_TOON = 0,	// トゥーンマップテクスチャ
		TEXTURE_MAX			// この列挙型の総数
	};

	// コンストラクタ
	CDrawBossShader();

	// デストラクタ
	~CDrawBossShader() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了

	void SetLightDirect(D3DXMATRIX *pMtxWorld, const int nLightID) override;	// ライト方向ベクトル設定
	void SetTexture(const LPDIRECT3DTEXTURE9 *pTexture) override;	// テクスチャ設定 (ポインタ)
	void SetTexture(const int nTextureID) override;					// テクスチャ設定 (インデックス)
	void SetMaterial(const D3DMATERIAL9& rMaterial) override;		// マテリアル設定
	void SetDiffuse(const D3DXCOLOR& rDiffuse) override;			// 拡散光設定
	void SetAmbient(const D3DXCOLOR& rAmbient) override;			// 環境光設定
	void SetEmissive(const D3DXCOLOR& rEmissive) override;			// 放射光設定
	void SetOnlyDiffuse(const D3DXCOLOR& rDiffuse) override;		// 拡散光のみ設定

	// 静的メンバ関数
	static CDrawBossShader *Create(void);		// 生成
	static CDrawBossShader *GetInstance(void);	// 取得
	static void Release(void);					// 破棄

private:
	// メンバ関数
	void SetToonMapTexture(const ETexture texture);	// トゥーンマップテクスチャ設定
	void SetCropTexture(const int nTextureID);		// 切り抜きテクスチャ設定

	// 静的メンバ変数
	static CDrawBossShader *m_pShader;	// シェーダー情報

	// メンバ変数
	D3DXHANDLE m_pTextureToon;	// トゥーンマップテクスチャ
	D3DXHANDLE m_pTextureCrop;	// 切り抜きテクスチャ
	D3DXHANDLE m_pDirectLight;	// 平行光源の方向ベクトル
	D3DXHANDLE m_pDiffuse;		// 拡散光
	D3DXHANDLE m_pAmbient;		// 環境光
	D3DXHANDLE m_pEmissive;		// 放射光
	D3DXHANDLE m_pUseTexture;	// テクスチャの使用状況
};

#endif	// _SHADER_DRAWBOSS_H_
