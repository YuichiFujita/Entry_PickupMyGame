//============================================================
//
//	ビルヘッダー [building.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _BUILDING_H_
#define _BUILDING_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectMeshCube.h"

//************************************************************
//	クラス定義
//************************************************************
// ビルクラス
class CBuilding : public CObjectMeshCube
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_00 = 0,	// ビル00テクスチャ
		TYPE_01,		// ビル01テクスチャ
		TYPE_02,		// ビル02テクスチャ
		TYPE_DOWN,		// ビル下テクスチャ
		TYPE_CENTER,	// ビル中間テクスチャ
		TYPE_UP,		// ビル上テクスチャ
		TYPE_MAX		// この列挙型の総数
	};

	// 当たり判定列挙
	enum ECollision
	{
		COLLISION_NONE = 0,	// 当たり判定無し
		COLLISION_GROUND,	// 地面
		COLLISION_CEILING,	// 天井
		COLLISION_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CBuilding();

	// デストラクタ
	~CBuilding();

	// ステータス情報構造体
	struct SStatusInfo
	{
		D3DXVECTOR3 size;	// 大きさ
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画
	void SetType(const int nType) override;		// 種類設定
	int GetType(void) const override;			// 種類取得
	void SetState(const int nState) override;	// 当たり判定設定
	int GetState(void) const override;			// 当たり判定取得
	void SetScale(const float fScale) override;	// 拡大率設定
	float GetScale(void) const override;		// 拡大率取得

	// 静的メンバ関数
	static CBuilding *Create	// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot,	// 向き
		const ECollision collision,	// 当たり判定
		const float fScale = 1.0f	// 拡大率
	);

private:
	// 静的メンバ変数
	static const char *mc_apTextureFile[][6];	// テクスチャ定数
	static SStatusInfo m_aStatusInfo[];			// ステータス情報

	// メンバ変数
	ECollision m_collision;	// 当たり判定
	SStatusInfo m_status;	// ステータス
	EType m_type;			// 種類
	float m_fScale;			// 拡大率
};

#endif	// _BUILDING_H_
