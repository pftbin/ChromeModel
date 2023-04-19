#include "pch.h"
#include "CWebClient.h"

CWebClient::CWebClient(CWnd* pParent /*= nullptr*/)
{
	m_pParent = pParent;
	m_BrowserId = -1;
	m_bLocalList = FALSE;
}

CWebClient::~CWebClient(void)
{
}