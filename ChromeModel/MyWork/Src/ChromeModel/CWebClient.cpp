#include "pch.h"
#include "CWebClient.h"

CWebClient::CWebClient(CWnd* pParent /*= nullptr*/)
{
	m_pParent = pParent;
	m_BrowserId = -1;
	m_bLocalList = FALSE;

	m_Browser = nullptr;
	m_strHead = _T("");
	m_strBody = _T("");
	m_bIsEditing = FALSE;
}

CWebClient::~CWebClient(void)
{
}