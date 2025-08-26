/*
* From:
	OutfledMicrophoneMute
		include
			overlay
		-------> game_overlay.h
		-----> appreg.h
		-----> microphone_thread.h
*/
#pragma once

#define REG_STATUS_ENABLED						(DWORD)0x00000001
#define REG_STATUS_DISABLED						(DWORD)0x00000000

#define INGAME_OVERLAY_STATUS_ENABLED			REG_STATUS_ENABLED
#define INGAME_OVERLAY_STATUS_DISABLED			REG_STATUS_DISABLED

#define INGAME_OVERLAY_ANCHOR_LEFT				0x00000000
#define INGAME_OVERLAY_ANCHOR_TOP				0x00000000
#define INGAME_OVERLAY_ANCHOR_RIGHT				0x00000001
#define INGAME_OVERLAY_ANCHOR_BOTTOM			0x00000002

#define OVERLAY_HELPER_PROCESSID_ARG			L"--pid"
#define OVERLAY_HELPER_MANUAL_MAP_ARG			L"-mm"
#define OVERLAY_HELPER_DEFAULT_INJ_ARG			L"-d"

#define OVERLAY_IMAGE_DEFAULT_WIDTH				(FLOAT)45
#define OVERLAY_IMAGE_DEFAULT_HEIGHT			(FLOAT)45

#define OVERLAY_REG_PATH_OVERLAY					L"SOFTWARE\\Outfled\\Microphone Mute\\Overlay"
#define OVERLAY_REG_VALUE_OVERLAY_IMAGE_WIDTH		"OverlayIconWidth"
#define OVERLAY_REG_VALUE_OVERLAY_IMAGE_HEIGHT		"OverlayIconHeight"
#define OVERLAY_REG_VALUE_BINARY_PATH				L"BinaryPath"
#define OVERLAY_REG_VALUE_SELECTED_MODULE			L"Module"



