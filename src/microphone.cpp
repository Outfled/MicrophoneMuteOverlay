#include "pch.h"
#include "microphone.h"
#include "overlayreg.h"

HANDLE g_hMutedEvent	= NULL;
HANDLE g_hUnmutedEvent	= NULL;

DWORD GetMicrophoneMuted( BOOL *pbMuted )
{
	DWORD	dwResult;
	HANDLE	hEvent;

	if (!g_hMutedEvent)
	{
		g_hMutedEvent = OpenEventW(SYNCHRONIZE, FALSE, MICROPHONE_MUTED_EVENT);
	}
	if (!g_hUnmutedEvent)
	{
		g_hUnmutedEvent = OpenEventW(SYNCHRONIZE, FALSE, MICROPHONE_UNMUTED_EVENT);
	}
	if (!g_hMutedEvent && !g_hUnmutedEvent)
	{
		return GetLastError();
	}

	*pbMuted = FALSE;
	if (g_hMutedEvent && WAIT_OBJECT_0 == WaitForSingleObject(g_hMutedEvent, 0))
	{
		*pbMuted = TRUE;
	}
	else if (g_hUnmutedEvent && WAIT_OBJECT_0 == WaitForSingleObject(g_hUnmutedEvent, 0))
	{
		*pbMuted = FALSE;
	}

	return NO_ERROR;
}