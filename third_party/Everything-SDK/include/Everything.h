
#ifndef _EVERYTHING_DLL_
#define _EVERYTHING_DLL_

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EVERYTHING_OK						0
#define EVERYTHING_ERROR_MEMORY				1
#define EVERYTHING_ERROR_IPC				2
#define EVERYTHING_ERROR_REGISTERCLASSEX	3
#define EVERYTHING_ERROR_CREATEWINDOW		4
#define EVERYTHING_ERROR_CREATETHREAD		5
#define EVERYTHING_ERROR_INVALIDINDEX		6
#define EVERYTHING_ERROR_INVALIDCALL		7

#ifndef EVERYTHINGAPI
#define EVERYTHINGAPI __stdcall
#endif

#ifndef EVERYTHINGUSERAPI
#define EVERYTHINGUSERAPI __declspec(dllexport)
#endif

// write search state
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetSearchW(LPCWSTR lpString);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetSearchA(LPCSTR lpString);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetMatchPath(BOOL bEnable);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetMatchCase(BOOL bEnable);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetMatchWholeWord(BOOL bEnable);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetRegex(BOOL bEnable);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetMax(DWORD dwMax);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetOffset(DWORD dwOffset);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetReplyWindow(HWND hWnd);
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetReplyID(DWORD nId);

// read search state
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_GetMatchPath(void);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_GetMatchCase(void);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_GetMatchWholeWord(void);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_GetRegex(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetMax(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetOffset(void);
EVERYTHINGUSERAPI LPCSTR EVERYTHINGAPI Everything_GetSearchA(void);
EVERYTHINGUSERAPI LPCWSTR EVERYTHINGAPI Everything_GetSearchW(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetLastError(void);
EVERYTHINGUSERAPI HWND EVERYTHINGAPI Everything_GetReplyWindow(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetReplyID(void);

// execute query
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_QueryA(BOOL bWait);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_QueryW(BOOL bWait);

// query reply
BOOL EVERYTHINGAPI Everything_IsQueryReply(UINT message,WPARAM wParam,LPARAM lParam,DWORD nId);

// write result state
EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SortResultsByPath(void);

// read result state
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetNumFileResults(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetNumFolderResults(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetNumResults(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetTotFileResults(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetTotFolderResults(void);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetTotResults(void);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_IsVolumeResult(DWORD nIndex);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_IsFolderResult(DWORD nIndex);
EVERYTHINGUSERAPI BOOL EVERYTHINGAPI Everything_IsFileResult(DWORD nIndex);
EVERYTHINGUSERAPI LPCWSTR EVERYTHINGAPI Everything_GetResultFileNameW(DWORD nIndex);
EVERYTHINGUSERAPI LPCSTR EVERYTHINGAPI Everything_GetResultFileNameA(DWORD nIndex);
EVERYTHINGUSERAPI LPCWSTR EVERYTHINGAPI Everything_GetResultPathW(DWORD nIndex);
EVERYTHINGUSERAPI LPCSTR EVERYTHINGAPI Everything_GetResultPathA(DWORD nIndex);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetResultFullPathNameA(DWORD nIndex,LPSTR buf,DWORD bufsize);
EVERYTHINGUSERAPI DWORD EVERYTHINGAPI Everything_GetResultFullPathNameW(DWORD nIndex,LPWSTR wbuf,DWORD wbuf_size_in_wchars);

#ifdef UNICODE
#define Everything_SetSearch Everything_SetSearchW
#define Everything_GetSearch Everything_GetSearchW
#define Everything_Query Everything_QueryW
#define Everything_GetResultFileName Everything_GetResultFileNameW
#define Everything_GetResultPath Everything_GetResultPathW
#define Everything_GetResultFullPathName Everything_GetResultFullPathNameW
#else
#define Everything_SetSearch Everything_SetSearchA
#define Everything_GetSearch Everything_GetSearchA
#define Everything_Query Everything_QueryA
#define Everything_GetResultFileName Everything_GetResultFileNameA
#define Everything_GetResultPath Everything_GetResultPathA
#define Everything_GetResultFullPathName Everything_GetResultFullPathNameA
#endif

#ifdef __cplusplus
}
#endif

#endif

