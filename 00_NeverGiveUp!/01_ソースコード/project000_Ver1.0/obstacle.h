//============================================================
//
//	障害物ヘッダー [obstacle.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"
#include "objectModel.h"

//************************************************************
//	クラス定義
//************************************************************
// 障害物クラス
class CObstacle : public CObjectModel
{
public:
	// 種類列挙
	enum EType
	{
		TYPE_BOX = 0,		// 箱モデル
		TYPE_BENCH,			// ベンチモデル
		TYPE_JUMPPAD,		// ジャンプパッドモデル
		TYPE_FENCE,			// フェンスモデル
		TYPE_BLOCK,			// ブロックモデル
		TYPE_WALL,			// 壁モデル
		TYPE_BLUE_BOX,		// 青箱モデル
		TYPE_CAR,			// 車モデル
		TYPE_WHEEL,			// タイヤモデル
		TYPE_DRUMTOWER,		// ドラム缶タワーモデル
		TYPE_PIPEFENCE,		// パイプフェンスモデル
		TYPE_STRUCTURE_00,	// ストラクチャーモデル（赤）
		TYPE_STRUCTURE_01,	// ストラクチャーモデル（青・黄）
		TYPE_STRUCTURE_02,	// ストラクチャーモデル（紫）
		TYPE_STRUCTURE_03,	// ストラクチャーモデル（赤・青）
		TYPE_STRUCTURE_04,	// ストラクチャーモデル（黄・緑）
		TYPE_STRUCTURE_05,	// ストラクチャーモデル（装飾なし：赤）
		TYPE_STRUCTURE_06,	// ストラクチャーモデル（装飾なし：灰）
		TYPE_STRUCTURE_07,	// ストラクチャーモデル（装飾なし：紫）
		TYPE_STRUCTURE_08,	// ストラクチャーモデル（装飾なし：青）
		TYPE_STRUCTURE_09,	// ストラクチャーモデル（装飾なし：緑）
		TYPE_STRUCTURE_10,	// ストラクチャーモデル（装飾なし：黄）
		TYPE_STRUCTURE_11,	// ストラクチャーモデル（灰）
		TYPE_STRUCTURE_12,	// ストラクチャーモデル（青）
		TYPE_WOODGROUND,	// 着地板
		TYPE_MAX			// この列挙型の総数
	};

	// 特殊状態列挙
	enum EState
	{
		STATE_NONE = 0,	// 特殊状態無し
		STATE_JUMP,		// 触れるとジャンプする
		STATE_LAND,		// 着地できる
		STATE_MAX		// この列挙型の総数
	};

	// 回避列挙
	enum EDodge
	{
		DODGE_NONE = 0,	// 回避方法無し
		DODGE_JUMP,		// ジャンプ回避
		DODGE_SLIDE,	// スライディング回避
		DODGE_MAX		// この列挙型の総数
	};

	// コンストラクタ
	CObstacle();

	// デストラクタ
	~CObstacle();

	// ステータス構造体
	struct SStatusInfo
	{
		D3DXVECTOR3 size;	// 大きさ
		EState state;		// 特殊状態
		EDodge dodge;		// 回避法
	};

	// オーバーライド関数
	HRESULT Init(void) override;	// 初期化
	void Uninit(void) override;		// 終了
	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画

	int GetType(void) const override;		// 種類取得
	int GetState(void) const override;		// 特殊状態取得
	int GetDodge(void) const override;		// 回避法取得
	float GetAngle(void) const override;	// 角度取得
	float GetLength(void) const override;	// 長さ取得

	D3DXVECTOR3 GetVec3Sizing(void) const override;	// 大きさ取得

	// 静的メンバ関数
	static void LoadSetup(void);	// セットアップ
	static CObstacle *Create		// 生成
	( // 引数
		const EType type,			// 種類
		const D3DXVECTOR3& rPos,	// 位置
		const D3DXVECTOR3& rRot		// 向き
	);
	static void SetStatusInfo(const SStatusInfo info, const int nID);	// ステータス情報設定
	static SStatusInfo GetStatusInfo(const int nID);					// ステータス情報取得

	// メンバ関数
	void SetType(const EType type);	// 種類設定

private:
	// 静的メンバ変数
	static const char *mc_apModelFile[];		// モデル定数
	static SStatusInfo m_aStatusInfo[TYPE_MAX];	// ステータス情報

	// メンバ変数
	SStatusInfo m_status;	// ステータス
	EType m_type;			// 種類
	float m_fAngle;			// 対角線の角度
	float m_fLength;		// 対角線の長さ
};

#endif	// _OBSTACLE_H_
