#include "pch.h"
#include "overlayreg.h"

#define OVERLAY_REGKEY_SUBPATH			"SOFTWARE\\Outfled\\Microphone Mute"

#define OVERLAY_MEDIA_BASE_KEY			"SOFTWARE\\Outfled\\Overlay\\Images"
#define OVERLAY_MEDIA_PARENT_KEY		L"SOFTWARE\\Outfled\\Overlay"

static LRESULT GetAppRegistryValue(HKEY hKey, LPCSTR lpszValueName, LPVOID lpResult, DWORD cbSize);
static VOID GetSubKeyCount(HKEY hKey, LPDWORD lpdwSubKeys);

LRESULT GetAppRegistryValue( LPCSTR lpszValueName, LPVOID lpResult, DWORD cbSize )
{
	LRESULT lResult;
	HKEY	hBaseKey;
	DWORD	cbValue;

	lResult = RegOpenKeyEx( HKEY_CURRENT_USER, OVERLAY_REGKEY_SUBPATH, 0, KEY_QUERY_VALUE, &hBaseKey );
	if (lResult == ERROR_SUCCESS)
	{
		lResult = GetAppRegistryValue(hBaseKey, lpszValueName, lpResult, cbSize);
		RegCloseKey(hBaseKey);
	}

	return lResult;
}

LRESULT QueryMediaSlideshowFiles(std::vector<LPSTR> &rgMediaFiles)
{
	DWORD	dwMediaFileCount;
	LRESULT lResult;
	HKEY	hKey;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, OVERLAY_MEDIA_BASE_KEY, 0, KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return lResult;
	}

	GetSubKeyCount(hKey, &dwMediaFileCount);
	for (DWORD i = 0; i < dwMediaFileCount; ++i)
	{
		DWORD	cbReadBytes;
		CHAR	szKeyName[10];
		CHAR	szPathBuffer[512];
		HKEY	hEnumKey;

		hEnumKey	= NULL;
		cbReadBytes = sizeof(szPathBuffer);

		lResult = RegEnumKey(hKey, i, szKeyName, ARRAYSIZE(szKeyName));
		if (lResult == ERROR_SUCCESS) {
			lResult = RegOpenKey(hKey, szKeyName, &hEnumKey);
		}

		if (lResult != ERROR_SUCCESS)
		{
			ZeroMemory(szKeyName, sizeof(szKeyName));
			continue;
		}

		lResult = RegQueryValueExA(hEnumKey, NULL, NULL, NULL, (PBYTE)szPathBuffer, &cbReadBytes);
		if (lResult == ERROR_SUCCESS)
		{
			rgMediaFiles.push_back(_strdup(szPathBuffer));
		}

		RegCloseKey(hEnumKey);
	}

	RegCloseKey(hKey);
	return ERROR_SUCCESS;
}

LRESULT QueryMediaDisplaySize(PDWORD pdwWidth, PDWORD pdwHeight)
{
	LRESULT lResult;
	HKEY	hKey;

	*pdwHeight	= 0;
	*pdwWidth	= 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, OVERLAY_MEDIA_BASE_KEY, 0, KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return lResult;
	}

	lResult = GetAppRegistryValue(hKey, "ImgWidth", pdwWidth, sizeof(DWORD));
	if (lResult == ERROR_SUCCESS) {
		lResult = GetAppRegistryValue(hKey, "ImgHeight", pdwHeight, sizeof(DWORD));
	}

	RegCloseKey(hKey);
	return lResult;
}

LRESULT QueryMediaDisplayTimeLength(PDWORD pdwTimeLength)
{
	LRESULT lResult;
	HKEY	hKey;

	*pdwTimeLength = 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, OVERLAY_MEDIA_BASE_KEY, 0, KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return lResult;
	}

	lResult = GetAppRegistryValue(hKey, "Timer", pdwTimeLength, sizeof(DWORD));
	RegCloseKey(hKey);

	return lResult;
}

LRESULT QuerySelectedDisplayMedia(LPSTR *ppszMediaFile)
{
	LRESULT lResult;
	HKEY	hKey;
	CHAR	szBuffer[10]{};
	CHAR	szPathBuffer[512]{};

	*ppszMediaFile = 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, OVERLAY_MEDIA_BASE_KEY, 0, KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return lResult;
	}

	lResult = GetAppRegistryValue(hKey, "", szBuffer, sizeof(szBuffer));
	if (lResult == ERROR_SUCCESS)
	{
		HKEY	hSelectedMediaKey;
		DWORD	cbReadBytes;

		cbReadBytes = sizeof(szPathBuffer);

		lResult = RegOpenKeyEx(hKey, szBuffer, 0, KEY_READ, &hSelectedMediaKey);
		if (lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueExA(hSelectedMediaKey, NULL, NULL, NULL, (PBYTE)szPathBuffer, &cbReadBytes);
			if (lResult == ERROR_SUCCESS)
			{
				*ppszMediaFile = _strdup(szPathBuffer);
			}

			RegCloseKey(hKey);
		}
	}

	RegCloseKey(hKey);
	return (*ppszMediaFile) ? ERROR_SUCCESS : lResult;
}

BOOL IsOverlayEnabled()
{
	LRESULT lResult;
	DWORD	dwStatus;

	lResult = GetAppRegistryValue( REG_VALUE_OVERLAY_ENABLED, &dwStatus, sizeof( DWORD ) );
	if (lResult == ERROR_SUCCESS && dwStatus == REG_STATUS_ENABLED) {
		return TRUE;
	}

	return FALSE;
}

BOOL IsMediaSlideshowOverlayEnabled()
{
	LRESULT lResult;
	HKEY	hKey;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, OVERLAY_MEDIA_BASE_KEY, 0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD dwEnabledValue;

		lResult = GetAppRegistryValue(hKey, "Enabled", &dwEnabledValue, sizeof(DWORD));
		if (lResult == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return (dwEnabledValue == 1) ? TRUE : FALSE;
		}
	}

	return FALSE;
}

LRESULT GetAppRegistryValue(HKEY hKey, LPCSTR lpszValueName, LPVOID lpResult, DWORD cbSize)
{
	DWORD cbValue;
	LRESULT lResult;
	
	lResult = RegQueryValueEx(hKey, lpszValueName, NULL, NULL, (LPBYTE)lpResult, &(cbValue = cbSize));
	return lResult;
}
VOID GetSubKeyCount(HKEY hKey, LPDWORD lpdwSubKeys)
{
	RegQueryInfoKey(hKey, NULL, NULL, NULL, lpdwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}
