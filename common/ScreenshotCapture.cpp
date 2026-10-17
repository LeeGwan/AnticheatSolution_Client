#include "pch.h"
#include "ScreenshotCapture.h"
CCapture::CCapture(VOID) : m_gdipToken(NULL)
{
}

CCapture::~CCapture(VOID)
{
	if (NULL != m_gdipToken)
	{
		Gdiplus::GdiplusShutdown(m_gdipToken);
	}
}

/**
 * @details InitCapture()에서 지정한 PID의 보이는 최상위 창을 캡처하여 지정한 폴더에 JPEG 파일로 저장합니다.
 *          파일명은 "YYYY-MM-DD HH_MM_SS.jpeg" 형식입니다.
 * @return BOOL 저장 성공 시 TRUE, 실패 시 FALSE
 * @warning InitCapture()가 성공한 뒤에 호출해야 합니다.
 *          최소화된 창은 캡처하지 않습니다.
 *          같은 초에 여러 번 호출하면 파일명이 겹칩니다.
 * @see InitCapture()
 * @author 이관호 (2026-10-17) 최초 작성
 */
BOOL CCapture::CaptureProcess(VOID)
{
	BOOL bRtn = FALSE;

	if (FALSE == m_bInitialized)
	{
		DBG_ERROR("CaptureProcess, not initialized");
		return bRtn;
	}

	HWND Targethwnd = FindProcessWindow();
	if (Targethwnd && !IsIconic(Targethwnd))
	{
		RECT rc;
		if (GetWindowRect(Targethwnd, &rc))
		{
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;

			HDC hScreen = GetDC(nullptr);
			HDC hMem = hScreen ? CreateCompatibleDC(hScreen) : nullptr;
			HBITMAP hBmp = (hMem && w > 0 && h > 0) ? CreateCompatibleBitmap(hScreen, w, h) : nullptr;
			if (hBmp)
			{
				HGDIOBJ old = SelectObject(hMem, hBmp);
				BOOL bPrinted = PrintWindow(Targethwnd, hMem, PW_RENDERFULLCONTENT);
				SelectObject(hMem, old);

				if (TRUE == bPrinted)
				{
					Gdiplus::Bitmap bmp(hBmp, nullptr);
					Gdiplus::Status status = bmp.GetLastStatus();
					if (Gdiplus::Status::Ok == status)
					{
						std::wstring wstrImagePath = m_wstrCaptureSavePath + L'\\' + CUserUtil::GetLocalTimeStr() + L".jpeg";
						status = bmp.Save(wstrImagePath.c_str(), &m_clsid, &m_params);
						bRtn = (Gdiplus::Status::Ok == status);
						if (TRUE == bRtn)
						{
							DBG_INFO("CaptureProcess, saved(%s)", wstrImagePath.c_str());
						}
						else
						{
							DBG_ERROR("CaptureProcess, Save failed status(%d) path(%s)", (int)status, wstrImagePath.c_str());
						}
					}
					else
					{
						DBG_ERROR("CaptureProcess, Bitmap create failed status(%d)", (int)status);
					}
				}
				else
				{
					DBG_ERROR("CaptureProcess, PrintWindow failed hwnd(%p)", Targethwnd);
				}

				DeleteObject(hBmp);
			}
			else
			{
				DBG_ERROR("CaptureProcess, CreateCompatibleBitmap failed hScreen(%p) hMem(%p) w(%d) h(%d)", hScreen, hMem, w, h);
			}

			if (hMem)
			{
				DeleteDC(hMem);
			}
			if (hScreen)
			{
				ReleaseDC(nullptr, hScreen);
			}
		}
		else
		{
			DBG_ERROR("CaptureProcess, GetWindowRect failed hwnd(%p) lastErr(%lu)", Targethwnd, GetLastError());
		}
	}
	else if (Targethwnd)
	{
		DBG_WARN("CaptureProcess, window minimized hwnd(%p)", Targethwnd);
	}
	return bRtn;
}

/**
 * @details m_dwPid 프로세스가 소유한 최상위 창 중에서 화면에 보이고 소유자 창이 없는 첫 번째 창을 찾습니다.
 * @param[in] 타겟 프로세스의 PID
 * @param[in] 이미지의 화질
 * @return HWND 찾은 창의 핸들, 없으면 nullptr
 * @warning 멀티 프로세스 앱(브라우저, Steam 등)은 창을 가진 프로세스가 따로 있어서 PID에 따라 찾지 못할 수 있습니다.
 *          보이는 창이 여러 개면 메인 창이 아닌 다른 창이 선택될 수 있습니다.
 * @see CaptureProcess()
 * @author 이관호 (2026-10-17) 최초 작성
 */
BOOL CCapture::InitCapture(IN DWORD dwPid, IN UINT uquality)
{
	BOOL bRet = FALSE;
	m_bInitialized = FALSE;

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	DBG_INFO("InitCapture In, dwPid(%lu) uquality(%u)", dwPid, uquality);
	if (NULL == m_gdipToken)
	{
		Gdiplus::Status status = Gdiplus::GdiplusStartup(&m_gdipToken, &m_gdipInput, nullptr);
		if (Gdiplus::Status::Ok != status)
		{
			DBG_ERROR("InitCapture, GdiplusStartup failed status(%d) lastErr(%lu)", (int)status, GetLastError());
			m_gdipToken = NULL;
		}
	}

	if (NULL != m_gdipToken)
	{
		bRet = GetEncoderClsid(L"image/jpeg", &m_clsid);
		if (TRUE == bRet)
		{
			m_dwPid = dwPid;
			m_ulQuality = (uquality > 100) ? 100 : uquality;
			m_params.Count = 1;
			m_params.Parameter[0].Guid = Gdiplus::EncoderQuality;
			m_params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			m_params.Parameter[0].NumberOfValues = 1;
			m_params.Parameter[0].Value = &m_ulQuality;
			m_wstrCaptureSavePath = CUserUtil::GetCurrentProcessDir() + L"\\capture";
			bRet = CUserUtil::CreateDir(m_wstrCaptureSavePath);
			DBG_INFO("InitCapture, bRet(%d) m_wstrCaptureSavePath(%s)", bRet, m_wstrCaptureSavePath.c_str());
			m_bInitialized = bRet;
		}
		else
		{
			DBG_ERROR("InitCapture, GetEncoderClsid(image/jpeg) failed");
		}
	}
	return bRet;
}

HWND CCapture::FindProcessWindow()
{
	HWND hwnd = nullptr;
	while ((hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) != nullptr)
	{
		DWORD dwWindowpid = 0;
		GetWindowThreadProcessId(hwnd, &dwWindowpid);

		if (dwWindowpid == m_dwPid && IsWindowVisible(hwnd) && !GetWindow(hwnd, GW_OWNER))
		{
			DBG_INFO("FindProcessWindow, pid(%lu) hwnd(%p)", m_dwPid, hwnd);
			return hwnd;
		}

	}
	DBG_ERROR("FindProcessWindow, not found pid(%lu)", m_dwPid);
	return nullptr;
}

/**
 * @details 이미지 형식 문자열에 해당하는 GDI+ 이미지 인코더의 CLSID를 찾습니다.
 * @param[in] wstrEncoderName 이미지 형식 문자열 (예: L"image/jpeg", L"image/png")
 * @param[out] clsid 찾은 인코더의 CLSID
 * @return BOOL 찾으면 TRUE, 없거나 조회에 실패하면 FALSE
 * @warning GdiplusStartup() 이후에 호출해야 합니다.
 *          FALSE를 반환하면 clsid 값은 변경되지 않습니다.
 * @see InitCapture()
 * @author 이관호 (2026-10-17) 최초 작성
 */
BOOL CCapture::GetEncoderClsid(IN const std::wstring& wstrEncoderName, CLSID* clsid)
{
	BOOL bRet = FALSE;
	UINT uNum = 0;
	UINT uSize = 0;
	Gdiplus::Status status = Gdiplus::GetImageEncodersSize(&uNum, &uSize);
	if (Gdiplus::Status::Ok == status && 0 < uSize)
	{
		std::vector<BYTE> buf(uSize);
		auto* codecs = reinterpret_cast<Gdiplus::ImageCodecInfo*>(buf.data());
		status = Gdiplus::GetImageEncoders(uNum, uSize, codecs);
		if (Gdiplus::Status::Ok == status)
		{
			for (UINT i = 0; i < uNum; ++i)
			{
				if (0 == wcscmp(codecs[i].MimeType, wstrEncoderName.c_str()))
				{
					*clsid = codecs[i].Clsid;
					bRet = TRUE;
					break;
				}
			}
			if (FALSE == bRet)
			{
				DBG_ERROR("GetEncoderClsid, encoder not found(%s)", wstrEncoderName.c_str());
			}
		}
		else
		{
			DBG_ERROR("GetEncoderClsid, GetImageEncoders failed status(%d)", (int)status);
		}
	}
	else
	{
		DBG_ERROR("GetEncoderClsid, GetImageEncodersSize failed status(%d) size(%u)", (int)status, uSize);
	}
	return bRet;
}
