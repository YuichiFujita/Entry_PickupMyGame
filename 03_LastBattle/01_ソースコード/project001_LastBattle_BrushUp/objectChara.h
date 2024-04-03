//============================================================
//
//	オブジェクトキャラクターヘッダー [objectChara.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECTCHARA_H_
#define _OBJECTCHARA_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "motion.h"

//************************************************************
//	前方宣言
//************************************************************
class CMultiModel;	// マルチモデルクラス
class CCollSphere;	// 円判定クラス

//************************************************************
//	クラス定義
//************************************************************
// オブジェクトキャラクタークラス
class CObjectChara : public CObject
{
public:
	// コンストラクタ
	explicit CObjectChara(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CObjectChara() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3& rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得
	void SetVec3Rotation(const D3DXVECTOR3& rRot) override;	// 向き設定
	D3DXVECTOR3 GetVec3Rotation(void) const override;		// 向き取得
	void SetAllMaterial(const D3DXMATERIAL& rMat) override;	// マテリアル全設定
	void ResetMaterial(void) override;						// マテリアル再設定
	D3DXMATRIX GetMtxWorld(void) const override;			// マトリックス取得
	D3DXMATRIX *GetPtrMtxWorld(void) override;				// マトリックスポインタ取得

	// 静的メンバ関数
	static CObjectChara *Create(const D3DXVECTOR3 &rPos, const D3DXVECTOR3 &rRot = VEC3_ZERO);	// 生成

	// 仮想関数
	virtual void SetMotion(const int nType, const int nBlendFrame = 0);	// モーション設定

	// メンバ関数
	void SetPartsInfo	// パーツ情報設定
	( // 引数
		const int nID,				// パーツインデックス
		const int nParentID,		// 親インデックス
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const char *pFileName		// ファイル名
	);

	void SetModelInfo(void);						// モデル情報設定
	void SetMotionInfo(CMotion::SMotionInfo info);	// モーション情報設定
	void SetMtxWorld(const D3DXMATRIX &rMtxWorld);	// マトリックス設定
	void SetEnableMotionUpdate(const bool bUpdate);	// 更新状況設定

	void SetPartsPosition(const int nPartsID, const D3DXVECTOR3& rPos);	// パーツ位置設定
	D3DXVECTOR3 GetPartsPosition(const int nPartsID) const;				// パーツ位置取得
	void SetPartsRotation(const int nPartsID, const D3DXVECTOR3& rRot);	// パーツ向き設定
	D3DXVECTOR3 GetPartsRotation(const int nPartsID) const;				// パーツ向き取得

	CMultiModel *GetMultiModel(const int nPartsID) const;	// マルチモデル取得
	CCollSphere *GetCollision(const int nPartsID) const;	// 当たり判定取得
	CMotion *GetMotion(void) const;							// モーション取得

	void SetMaterial(const D3DXMATERIAL& rMat, const int nPartsID, const int nMatID);	// マテリアル設定
	void SetAlpha(const float fAlpha);		// 透明度設定
	float GetAlpha(void) const;				// 透明度取得
	float GetMaxAlpha(void) const;			// 最大透明度取得
	int GetNumModel(void) const;			// パーツ総数取得
	int GetMotionType(void) const;			// モーション種類取得
	int GetMotionNumType(void) const;		// モーション種類総数取得
	int GetMotionKey(void) const;			// モーションキー番号取得
	int GetMotionNumKey(void) const;		// モーションキー総数取得
	int GetMotionKeyCounter(void) const;	// モーションキーカウンター取得
	int GetMotionWholeCounter(void) const;	// モーション全体カウンター取得
	int GetMotionWholeFrame(void) const;	// モーション全体フレーム数取得
	int GetMotionCancelFrame(void) const;	// モーションキャンセルフレーム取得
	int GetMotionComboFrame(void) const;	// モーションコンボフレーム取得
	bool IsMotionFinish(void) const;		// モーション終了取得
	bool IsMotionLoop(void) const;			// モーションループ取得
	bool IsMotionCancel(void) const;		// モーションキャンセル取得
	bool IsMotionCombo(void) const;			// モーションコンボ取得
	bool IsWeaponDisp(void) const;			// モーション武器表示取得
	bool IsLeftWeaponCollision(void);		// 左の攻撃判定フラグ取得
	bool IsRightWeaponCollision(void);		// 右の攻撃判定フラグ取得

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ変数
	CMultiModel	*m_apMultiModel[motion::MAX_PARTS];	// モデルの情報
	CCollSphere	*m_apCollision[motion::MAX_PARTS];	// 円判定クラス
	CMotion		*m_pMotion;		// モーションの情報
	D3DXMATRIX	m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3	m_pos;			// 位置
	D3DXVECTOR3	m_rot;			// 向き
	int			m_nNumModel;	// パーツの総数
};

#endif	// _OBJECTCHARA_H_
