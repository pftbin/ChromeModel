#pragma once
#include <sstream>
#include <string>

#include "cef_v8.h"
#include "cef_app.h"
#include "cef_parser.h"
#include "cef_client.h"
#include "base/cef_lock.h"
#include "base/cef_bind.h"
#include "wrapper/cef_closure_task.h"
#include "wrapper/cef_helpers.h"

#include "ChromeModelDefine.h"

class CEFClientHandler 
    : public CefClient
    , public CefDisplayHandler
    , public CefLifeSpanHandler
    , public CefLoadHandler
    , public CefRequestHandler
    , public CefResourceRequestHandler
{
public:
    // Implement this interface to receive notification of CEFClientHandler
    // events. The methods of this class will be called on the main thread.
    class Delegate
    {
    public:
        // Called when the browser is created.
        virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;

        // Called when the browser is closing.
        virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) = 0;

        // Called when the browser has been closed.
        virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) = 0;

        // Set the window URL address.
        virtual void OnSetAddress(std::string const& url) = 0;

        // Set the window title.
        virtual void OnSetTitle(std::string const& title) = 0;

        // Set fullscreen mode.
        virtual void OnSetFullscreen(bool const fullscreen) = 0;

        // Set the loading state.
        virtual void OnSetLoadingState(bool const isLoading,
            bool const canGoBack,
            bool const canGoForward) = 0;

    protected:
        virtual ~Delegate() {}
    };

public:
    CEFClientHandler(Delegate* delegate);
    virtual ~CEFClientHandler();

public:
    // CefClient 
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString& request_initiator, bool& disable_default_handling) override { return this;}

    // CefDisplayHandler 
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;

    // CefLifeSpanHandler 
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler 
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;

    //CefRequestHandler+CefResourceRequestHandler
    virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;
    virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response, URLRequestStatus status, int64 received_content_length) override;

    //Other Public
    std::string ReadPostElementBytes(CefRefPtr<CefPostDataElement> element);

    // This object may outlive the Delegate object so it's necessary for the
    // Delegate to detach itself before destruction.
    void DetachDelegate();

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(CEFClientHandler);

    // Include the default locking implementation.
    //IMPLEMENT_LOCKING(CEFClientHandler); //线程锁定义,当前版本无,不使用

private:
    Delegate* m_delegate;
};

