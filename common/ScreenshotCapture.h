#pragma once

#pragma comment(lib, "gdiplus.lib")

class CCapture
{
public:
	CCapture(VOID);
	~CCapture(VOID);
	CCapture(const CCapture&) = delete;
	CCapture& operator=(const CCapture&) = delete;

	BOOL InitCapture(IN DWORD dwPid, IN UINT uquality = 50);
	BOOL CaptureProcess(VOID);

private:
	Gdiplus::GdiplusStartupInput m_gdipInput;
	ULONG_PTR m_gdipToken = NULL;
	DWORD m_dwPid = 0;
	ULONG m_ulQuality = 0;
	BOOL m_bInitialized = FALSE;
	std::wstring m_wstrCaptureSavePath;
	CLSID m_clsid{};
	Gdiplus::EncoderParameters m_params{};

	HWND FindProcessWindow();
	BOOL GetEncoderClsid(IN const std::wstring& wstrEncoderName, CLSID* clsid);
};

