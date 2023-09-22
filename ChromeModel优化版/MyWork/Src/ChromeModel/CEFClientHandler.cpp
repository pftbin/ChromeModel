#include "pch.h"
#include "CEFClientHandler.h"

CEFClientHandler::CEFClientHandler(Delegate* delegate)
    : m_delegate(delegate)
{
}

CEFClientHandler::~CEFClientHandler()
{
}

//CefDisplayHandler
void CEFClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    CEF_REQUIRE_UI_THREAD();

    // Only update the address for the main (top-level) frame.
    if (frame->IsMain())
    {
        if (m_delegate != nullptr)
            m_delegate->OnSetAddress(url);
    }
}

void CEFClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnSetTitle(title);
}

void CEFClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnSetFullscreen(fullscreen);
}

//CefLifeSpanHandler
void CEFClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnBrowserCreated(browser);
}

bool CEFClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access)
{
    //点击新链接，在当前窗口打开
    switch (target_disposition)
    {
    case WOD_NEW_FOREGROUND_TAB:
    case WOD_NEW_BACKGROUND_TAB:
    case WOD_NEW_POPUP:
    case WOD_NEW_WINDOW:
        browser->GetMainFrame()->LoadURL(target_url);
        return true; //cancel create    
    }

    return false;
}

bool CEFClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnBrowserClosing(browser);

    return false;
}

void CEFClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnBrowserClosed(browser);
}

//CefLoadHandler
void CEFClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_delegate != nullptr)
        m_delegate->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

void CEFClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
    frame->LoadURL("www.blank.com");//错误信息网页
    //frame->LoadString(ss.str(), failedUrl);
}

//CefRequestHandler+CefResourceRequestHandler
cef_return_value_t CEFClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
    CefString url = request->GetURL();
    CString strurl; strurl = url.ToWString().c_str();
    if (strurl.Find(_T("updateaction")) != -1)
    {
        //捕获请求的内容
        CefString method = request->GetMethod();
        CString strmethod; strmethod = method.ToWString().c_str();
        if (strmethod.Find(_T("POST")) != -1)
        {
            CString strHead, strBody;

            CefRequest::HeaderMap headerMap;
            request->GetHeaderMap(headerMap);
            for (CefRequest::HeaderMap::iterator itHead = headerMap.begin(); itHead != headerMap.end(); ++itHead)
            {
                CefString cstrKey = itHead->first;
                CefString cstrValue = itHead->second;

                strHead += cstrKey.ToWString().c_str();
                strHead += _T(":");
                strHead += cstrValue.ToWString().c_str();
                strHead += _T("\n\n");
            }

            CefRefPtr<CefPostData> PostData = request->GetPostData();
            CefPostData::ElementVector elementVec;
            PostData->GetElements(elementVec);
            for (CefPostData::ElementVector::iterator itElem = elementVec.begin(); itElem != elementVec.end(); ++itElem)
            {
                CString strElemBytes;
                WCHAR* pUniElemBytes(NULL);
                std::string sElemBytes = ReadPostElementBytes((*itElem));

                UTF8ToUnicode((char*)sElemBytes.c_str(), &pUniElemBytes);
                strElemBytes = pUniElemBytes;

                strBody += strElemBytes;
                strBody += _T("\n\n");
            }
        }
    }

	return RV_CONTINUE;//继续请求
}

void CEFClientHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response, URLRequestStatus status, int64 received_content_length)
{
}

////////////////////////////////////////////////////////////
std::string CEFClientHandler::ReadPostElementBytes(CefRefPtr<CefPostDataElement> element)
{
    std::string sBytes;
    if (element->GetType() == PDE_TYPE_BYTES)
    {
        size_t byte_count = element->GetBytesCount();
        char* bytes = new char[byte_count + 1];
        element->GetBytes(byte_count + 1, (void*)(bytes));
        bytes[byte_count] = '\0';

        sBytes = bytes;
        delete[] bytes;
    }

    return sBytes;
}

void CEFClientHandler::DetachDelegate()
{
    m_delegate = nullptr;
}