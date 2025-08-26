#pragma once

#include <vector>
#include "game_overlay.h"

#define REG_VALUE_OVERLAY_ENABLED	"In-GameOverlay"
#define REG_VALUE_OVERLAY_ANCHOR	"OverlayAnchor"

#define REG_STATUS_ENABLED			(DWORD)0x00000001
#define REG_STATUS_DISABLED			(DWORD)0x00000000

#define INGAME_OVERLAY_ANCHOR_RIGHT			0x00000001
#define INGAME_OVERLAY_ANCHOR_LEFT			0x00000000
#define INGAME_OVERLAY_ANCHOR_BOTTOM		0x00000002
#define INGAME_OVERLAY_ANCHOR_TOP			0x00000000

/*
* From:
*	(Solution) OutfledMicrophoneMute
*		include
*			-----> microphone_thread.h
*/
#define MICROPHONE_MUTED_EVENT			L"Global\\OutfledMicrophoneMute_MutedEvent"
#define MICROPHONE_UNMUTED_EVENT		L"Global\\OutfledMicrophoneMute_UnmutedEvent"

BOOL IsOverlayEnabled();
BOOL IsMediaSlideshowOverlayEnabled();

LRESULT GetAppRegistryValue( LPCSTR lpszValueName, LPVOID lpResult, DWORD cbSize );
LRESULT QueryMediaSlideshowFiles(std::vector<LPSTR> &rgMediaFiles);
LRESULT QueryMediaDisplaySize(PDWORD pdwWidth, PDWORD pdwHeight);
LRESULT QueryMediaDisplayTimeLength(PDWORD pdwTimeLength);
LRESULT QuerySelectedDisplayMedia(LPSTR *ppszMediaFile);