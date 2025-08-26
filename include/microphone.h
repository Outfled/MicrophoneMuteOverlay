#pragma once

extern HANDLE g_hMutedEvent;
extern HANDLE g_hUnmutedEvent;

DWORD GetMicrophoneMuted( BOOL *pbMuted );