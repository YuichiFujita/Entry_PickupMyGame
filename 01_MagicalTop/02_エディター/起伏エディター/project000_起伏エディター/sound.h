//============================================================
//
//	サウンドヘッダー [sound.h]
//	Author：藤田勇一
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include "main.h"

//************************************************************
//	クラス定義
//************************************************************
// サウンドクラス
class CSound
{
public:
	// コンストラクタ
	CSound();

	// デストラクタ
	~CSound();

	// サウンドラベル列挙
	typedef enum
	{
		LABEL_BGM_000 = 0,	// BGM (仮)
		LABEL_SE_SHOT,		// SE (仮)
		LABEL_MAX,			// この列挙型の総数
	}LABEL;

	// サウンド情報構造体
	typedef struct
	{
		char *pFilename;	// ファイル名
		int   nCntLoop;		// ループカウント
	}SOUNDINFO;

	// メンバ関数
	HRESULT Init(HWND hWnd);	// 初期化
	void Uninit(void);			// 終了
	HRESULT Play(LABEL label);	// 再生
	void Stop(LABEL label);		// 停止
	void Stop(void);			// 全停止

	// 静的メンバ関数
	static CSound *Create(HWND hWnd);			// 生成
	static HRESULT Release(CSound *&pSound);	// 破棄

private:
	// メンバ関数
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);	// チャンクチェック
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);	// チャンクデータ読込

	// メンバ変数
	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice    *m_apSourceVoice[LABEL_MAX];	// ソースボイス
	BYTE *m_apDataAudio[LABEL_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[LABEL_MAX];						// オーディオデータサイズ
	static const SOUNDINFO mc_aSoundInfo[LABEL_MAX];	// サウンド情報定数
};

#endif	// _SOUND_H_
