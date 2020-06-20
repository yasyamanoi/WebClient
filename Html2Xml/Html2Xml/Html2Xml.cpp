// Html2Xml.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "HtmlParser.h"
/*
void getHttpPage(LPCTSTR pszServerName, LPCTSTR pszFileName,int port,string& ret)
{
	CInternetSession session(_T("My Session"));
	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	try
	{
		CString strServerName;
		INTERNET_PORT nPort = port;
		DWORD dwRet = 0;
		if (nPort == 80) {
			pServer = session.GetHttpConnection(pszServerName, nPort);
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, pszFileName);
		}
		else if (nPort == 443) {
			pServer = session.GetHttpConnection(pszServerName, INTERNET_FLAG_SECURE, nPort);
			DWORD httpsFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, pszFileName, NULL, 1, NULL, _T("HTTP/1.1"), httpsFlags);
		}
		else {
			return;
		}
		pFile->SendRequest();
		pFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK)
		{
			CHAR szBuff[1030];
			UINT readSize;
			while ((readSize = pFile->Read(szBuff, 1024)) > 0)
			{
				szBuff[readSize] = '\0';
				ret += szBuff;
			}
		}
		delete pFile;
		delete pServer;
	}
	catch (CInternetException* pEx)
	{
		//catch errors from WinInet
		TCHAR pszError[64];
		pEx->GetErrorMessage(pszError, 64);
		_tprintf_s(_T("%63s"), pszError);
	}
	session.Close();
}
*/

int main(int argc,char* argv[])
{
	if (argc == 3) {
		string inHtml = argv[1];
		cout << inHtml << endl;
/*
		string html;
		regex pat("^http.*");
		if (regex_match(inHtml, pat)) {
			//WEBから取り込む
			CString cstrUrl(inHtml.c_str());
			DWORD dwServiceType;
			CString strServer, strObj;
			INTERNET_PORT nPort;
			::AfxParseURL(cstrUrl, dwServiceType, strServer, strObj, nPort);
			getHttpPage(strServer, strObj, nPort, html);
		}
		else {
			//ファイルから取り込む
			ifstream ifs(inHtml);
			string line;
			while (getline(ifs, line)) {
				html += line;
				html += "\n";
			}
		}
*/
		webclient::HtmlParser parser;
		if (!parser.readHtml(inHtml)) {
			cout << "HTMLの読み込みに失敗しました\n";
		}
		else {
			if (!parser.outXml(argv[2])) {
				cout << "XMLの保存に失敗しました\n";
			}
		}
		return 0;
	}
	else {
		cout << "パラメータが足りません\n";
		cout << "Html2Xml 読み込むHTML 書きだすXMLファイル " << endl;
		return 0;
	}
	return 0;
}

