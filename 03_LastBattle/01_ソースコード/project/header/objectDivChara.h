//============================================================
//
//	オブジェクト分割キャラクターヘッダー [objectDivChara.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBJECT_DIVCHARA_H_
#define _OBJECT_DIVCHARA_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "object.h"
#include "motion.h"

//************************************************************
//	前方宣言
//************************************************************
class CObjectChara;	// オブジェクトキャラクタークラス
class CMultiModel;	// マルチモデルクラス

//************************************************************
//	クラス定義
//************************************************************
// オブジェクト分割キャラクタークラス
class CObjectDivChara : public CObject
{
public:
	// 身体列挙
	enum EBody
	{
		BODY_LOWER = 0,	// 下半身
		BODY_UPPER,		// 上半身
		BODY_MAX		// この列挙型の総数
	};

	// コンストラクタ
	explicit CObjectDivChara(const CObject::ELabel label = LABEL_NONE, const CObject::EDim dimension = DIM_3D, const int nPriority = object::DEFAULT_PRIO);

	// デストラクタ
	~CObjectDivChara() override;

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(CShader *pShader = nullptr) override;			// 描画
	void SetVec3Position(const D3DXVECTOR3 &rPos) override;	// 位置設定
	D3DXVECTOR3 GetVec3Position(void) const override;		// 位置取得
	void SetVec3Rotation(const D3DXVECTOR3 &rRot) override;	// 向き設定
	D3DXVECTOR3 GetVec3Rotation(void) const override;		// 向き取得
	void SetAllMaterial(const D3DXMATERIAL &rMat) override;	// マテリアル全設定
	void ResetMaterial(void) override;						// マテリアル再設定
	void SetEnableUpdate(const bool bUpdate) override;		// 更新状況設定
	void SetEnableDraw(const bool bDraw) override;			// 描画状況設定

	// 静的メンバ関数
	static CObjectDivChara *Create	// 生成
	( // 引数
		const D3DXVECTOR3 &rPos,				// 位置
		const D3DXVECTOR3 &rRot = VEC3_ZERO,	// 向き
		const int nUpperParentID = 0			// 上半身親インデックス
	);

	// 仮想関数
	virtual void SetMotion	// モーション設定
	( // 引数
		const EBody bodyID,			// 身体インデックス
		const int nType,			// モーション種類
		const int nBlendFrame = 0	// ブレンドフレーム
	);

	// メンバ関数
	void SetPartsInfo	// パーツ情報設定
	( // 引数
		const EBody bodyID,			// 身体インデックス
		const int nID,				// パーツインデックス
		const int nParentID,		// 親インデックス
		const D3DXVECTOR3 &rPos,	// 位置
		const D3DXVECTOR3 &rRot,	// 向き
		const char *pFileName		// ファイル名
	);
	void SetMaterial	// マテリアル設定
	( // 引数
		const D3DXMATERIAL &rMat,	// 設定マテリアル
		const EBody bodyID,			// 身体インデックス
		const int nPartsID,			// パーツインデックス
		const int nMatID			// マテリアルインデックス
	);

	void SetModelInfo(void);	// モデル情報設定
	void SetEnableMotionUpdate(const bool bUpdate);	// 更新状況設定
	void SetMotionInfo(const EBody bodyID, CMotion::SMotionInfo info);	// モーション情報設定

	int  GetMotionType(const EBody bodyID) const;			// モーション種類取得
	int  GetMotionNumType(const EBody bodyID) const;		// モーション種類総数取得
	int  GetMotionKey(const EBody bodyID) const;			// モーションキー番号取得
	int  GetMotionNumKey(const EBody bodyID) const;			// モーションキー総数取得
	int  GetMotionKeyCounter(const EBody bodyID) const;		// モーションキーカウンター取得
	int  GetMotionWholeCounter(const EBody bodyID) const;	// モーション全体カウンター取得
	int  GetMotionWholeFrame(const EBody bodyID) const;		// モーション全体フレーム数取得
	int  GetMotionCancelFrame(const EBody bodyID) const;	// モーションキャンセルフレーム取得
	int  GetMotionComboFrame(const EBody bodyID) const;		// モーションコンボフレーム取得
	bool IsMotionFinish(const EBody bodyID) const;			// モーション終了取得
	bool IsMotionLoop(const EBody bodyID) const;			// モーションループ取得
	bool IsMotionCancel(const EBody bodyID) const;			// モーションキャンセル取得
	bool IsMotionCombo(const EBody bodyID) const;			// モーションコンボ取得
	bool IsWeaponDisp(const EBody bodyID) const;			// モーション武器表示取得
	bool IsLeftWeaponCollision(const EBody bodyID);			// 左の攻撃判定フラグ取得
	bool IsRightWeaponCollision(const EBody bodyID);		// 右の攻撃判定フラグ取得

	void SetPartsPosition(const EBody bodyID, const int nPartsID, const D3DXVECTOR3 &rPos);	// パーツ位置設定
	D3DXVECTOR3 GetPartsPosition(const EBody bodyID, const int nPartsID) const;				// パーツ位置取得
	void SetPartsRotation(const EBody bodyID, const int nPartsID, const D3DXVECTOR3 &rRot);	// パーツ向き設定
	D3DXVECTOR3 GetPartsRotation(const EBody bodyID, const int nPartsID) const;				// パーツ向き取得

	void SetAlpha(const float fAlpha);	// 透明度設定
	float GetAlpha(void) const;			// 透明度取得
	float GetMaxAlpha(void) const;		// 最大透明度取得
	void SetUpperParentID(const int nUpperParentID);		// 上半身の親インデックス設定
	CObjectChara *GetObjectChara(const EBody bodyID) const;	// オブジェクトキャラクター取得
	CMultiModel *GetMultiModel(const EBody bodyID, const int nModelID) const;	// マルチモデル取得

private:
	// オーバーライド関数
	void Release(void) override;	// 破棄

	// メンバ変数
	CObjectChara *m_apBody[BODY_MAX];	// 身体の情報
	int m_nUpperParentID;	// 上半身親インデックス
};

#endif	// _OBJECT_DIVCHARA_H_
