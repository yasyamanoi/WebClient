// Html2Xml.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <regex>

#include <SDKDDKVer.h>
//#include <atlbase.h>
//#include <atlstr.h>
#include <afxinet.h>


using namespace std;


//tgtをchkを検索してchangeに置き換える関数
void StrReplace(string& tgt, const string& chk, const string& change) {
	auto index = tgt.find(chk);
	while (index != string::npos) {
		tgt.replace(index, chk.size(), change);
		index = tgt.find(chk, index + change.size());
	}
}



//開始タグ名を取り出す
string getStartTagName(const string& tag) {
	bool in = false;
	string ret = "";
	for (size_t i = 1; i < tag.size(); i++) {
		if (in) {
			if (!isalnum(tag[i])) {
				return ret;
			}
			else {
				ret += tag[i];
			}
		}
		else {
			if (tag[i] == '!') {
				//DocType宣言もしくはコメント
				//空白を返す
				return string();
			}
			if (isalnum(tag[i])) {
				ret = tag[i];
				in = true;
			}
		}
	}
	return ret;
}

string getEndTagName(const string& tag) {
	bool in = false;
	string ret = "";
	for (size_t i = 2; i < tag.size(); i++) {
		if (in) {
			if (!isalnum(tag[i])) {
				return ret;
			}
			else {
				ret += tag[i];
			}
		}
		else {
			if (isalnum(tag[i])) {
				ret = tag[i];
				in = true;
			}
		}
	}
	return ret;
}


//開始タグかどうかの検証
bool isStartTag(const string& str) {
	if (str.size() < 2) {
		return false;
	}
	if (str[0] == '<' && str[1] != '/') {
		return true;
	}
	return false;
}

//終了タグかどうかの検証
bool isEndTag(const string& str) {
	if (str.size() < 2) {
		return false;
	}
	if (str[0] == '<' && str[1] == '/') {
		return true;
	}
	return false;
}

vector<string> tokenVec;

enum class NodeType {
	typeRoot,
	typeSimpleTag,
	typeTag,
	typeText
};

struct Node {
	NodeType m_type;
	string m_nodeName;
	map<string, string> m_attlibutes;
	string m_text;
	vector<Node> m_children;
	Node() :m_type(NodeType::typeRoot)
	{}
};

Node rootNode;

struct KeyValue {
	string m_key;
	string m_value;
	bool left = true;
	void Create(const string& keyValueSet) {
		for (size_t i = 0; i < keyValueSet.size(); i++) {
			if (left) {
				if (keyValueSet[i] == '=') {
					left = false;
				}
				else {
					m_key += keyValueSet[i];
				}
			}
			else {
				m_value += keyValueSet[i];
			}
		}
	}
};

void CreateAttlibutes(Node& newNode, const vector<string>& tagStrings) {
	for (size_t i = 1; i < tagStrings.size(); i++) {
		if (tagStrings[i] == "/") {
			continue;
		}
		KeyValue keyval;
		keyval.Create(tagStrings[i]);
		if (keyval.m_value != "") {
			newNode.m_attlibutes[keyval.m_key] = keyval.m_value;
		}
		else {
			newNode.m_attlibutes[keyval.m_key] = keyval.m_key;
		}
	}
}

Node CreateSingleTagNode(const string& tagStr) {
	Node newNode;
	newNode.m_type = NodeType::typeTag;
	newNode.m_nodeName = getStartTagName(tagStr);
	bool in = false;
	bool inin = false;
	vector<string> tagStrings;
	size_t index = 0;
	string ret = "";
	for (size_t i = 1; i < tagStr.size(); i++) {
		if (in) {
			if (inin) {
				if (tagStr[i] == '"') {
					inin = false;
				}
				else if (tagStr[i] == '\n' || tagStr[i] == '\t') {
					tagStrings[index] += ' ';
				}
				else {
					tagStrings[index] += tagStr[i];
				}
			}
			else {
				if (tagStr[i] == '"') {
					inin = true;
				}
				else if (tagStr[i] == ' ' || tagStr[i] == '\t' || tagStr[i] == '\n') {
					in = false;
				}
				else if (tagStr[i] == '/' || tagStr[i] == '>') {
					CreateAttlibutes(newNode, tagStrings);
					return newNode;
				}
				else {
					tagStrings[index] += tagStr[i];
				}
			}
		}
		else {
			if (tagStr[i] != ' ' && tagStr[i] != '\t' && tagStr[i] != '\n') {
				auto count = tagStrings.size();
				tagStrings.push_back(string());
				index = count;
				tagStrings[index] += tagStr[i];
				in = true;
				inin = false;
			}
			else if (tagStr[i] == '>') {
				CreateAttlibutes(newNode, tagStrings);
				return newNode;
			}
		}
	}
	return newNode;
}

bool ChkSimpleTag(const string& tagName) {
	string chk = tagName;
	transform(chk.begin(), chk.end(), chk.begin(), ::tolower);
	vector<string> vec = {
		"area",
		"base",
		"br",
		"col",
		"command",
		"embed",
		"hr",
		"img",
		"input",
		"keygen",
		"link",
		"meta",
		"param",
		"source",
		"track",
		"wbr"
	};
	for (auto& v : vec) {
		if (chk == v) {
			return true;
		}
	}
	return false;
}


bool ChkOmissionTag(const string& tagName, const string& nextTagName) {
	string chk1 = tagName;
	transform(chk1.begin(), chk1.end(), chk1.begin(), ::tolower);
	string chk2 = nextTagName;
	transform(chk2.begin(), chk2.end(), chk2.begin(), ::tolower);
	if (chk1 == "li" && chk2 == "li") {
		return true;
	}
	if (chk1 == "dt") {
		if (chk2 == "dt" || chk2 == "dd") {
			return true;
		}
	}
	if (chk1 == "dd") {
		if (chk2 == "dt" || chk2 == "dd") {
			return true;
		}
	}
	if (chk1 == "rt") {
		if (chk2 == "rt" || chk2 == "rp") {
			return true;
		}
	}
	if (chk1 == "rp") {
		if (chk2 == "rt" || chk2 == "rp") {
			return true;
		}
	}
	if (chk1 == "optgroup" && chk2 == "optgroup") {
		return true;
	}
	if (chk1 == "option") {
		if (chk2 == "option" || chk2 == "optgroup") {
			return true;
		}
	}
	if (chk1 == "thead") {
		if (chk2 == "tbody" || chk2 == "tfoot") {
			return true;
		}
	}
	if (chk1 == "tbody") {
		if (chk2 == "tbody" || chk2 == "tfoot") {
			return true;
		}
	}
	if (chk1 == "td") {
		if (chk2 == "td" || chk2 == "th") {
			return true;
		}
	}
	if (chk1 == "th") {
		if (chk2 == "td" || chk2 == "th") {
			return true;
		}
	}
	//pタグ
	vector<string> vecChkP = {
		"address","article","aside",
		"blockquote","details","div","dl",
		"fieldset","figcaption","figure",
		"footer","form",
		"h1","h2","h3","h4","h5","h6",
		"header","hgroup","hr",
		"main","menu","nav","ol","p","pre","section","table","ul"
	};
	if (chk1 == "p") {
		for (auto& v : vecChkP) {
			if (chk2 == v) {
				return true;
			}
		}
	}
	return false;
}

size_t CreateNodes(Node& node, size_t tokenIndex) {
	while (tokenVec.size() > tokenIndex) {
		if (isStartTag(tokenVec[tokenIndex])) {
			string startTagName = getStartTagName(tokenVec[tokenIndex]);
			if (startTagName == string("")) {
				//コメントやDocType宣言
				tokenIndex++;
				continue;
			}
			if (ChkOmissionTag(node.m_nodeName, startTagName)) {
				//省略可能なタグだった
				//tokenIndexは進めずにリターン
				return tokenIndex;
			}
			Node newNode = CreateSingleTagNode(tokenVec[tokenIndex]);
			if (ChkSimpleTag(newNode.m_nodeName)) {
				newNode.m_type = NodeType::typeSimpleTag;
				node.m_children.push_back(newNode);
				tokenIndex++;
			}
			else {
				auto sz = node.m_children.size();
				node.m_children.push_back(newNode);
				tokenIndex = CreateNodes(node.m_children[sz], tokenIndex + 1);
			}
		}
		else if (isEndTag(tokenVec[tokenIndex])) {
			if (getEndTagName(tokenVec[tokenIndex]) == node.m_nodeName) {
				return tokenIndex + 1;
			}
			else {
				//期待するendTagでなかった
				//tokenIndexは進めずにリターン
				return tokenIndex;
			}
		}
		else {
			Node newNode;
			newNode.m_type = NodeType::typeText;
			newNode.m_text = tokenVec[tokenIndex];
			node.m_children.push_back(newNode);
			tokenIndex++;
		}
	}
	return tokenIndex;
}


void CreateTokens(const string& html) {
	bool textFlg = true;
	string temp;
	for (size_t i = 0; i < html.size(); i++) {
		if (textFlg) {
			if (html[i] == '<') {
				if (temp != "") {
					tokenVec.push_back(temp);
				}
				textFlg = false;
				temp = html[i];
			}
			else {
				temp += html[i];
			}
		}
		else {
			if (html[i] == '>') {
				temp += html[i];
				if (temp != "") {
					tokenVec.push_back(temp);
				}
				temp = "";
				textFlg = true;
			}
			else {
				temp += html[i];
			}
		}
	}

}

void OutNodes(const Node& node,ofstream& ofs) {
	if (node.m_type == NodeType::typeRoot || node.m_type == NodeType::typeTag) {
		if (node.m_type == NodeType::typeRoot) {
			ofs << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		}
		if (node.m_nodeName != "") {
			ofs << '<' << node.m_nodeName;
			if (node.m_attlibutes.size() > 0) {
				auto it = node.m_attlibutes.begin();
				while (it != node.m_attlibutes.end()) {
					ofs << ' ' << (*it).first << '=' << '"' << (*it).second << '"';
					it++;
				}
			}
			ofs << '>';
		}
		size_t i = 0;
		while (i < node.m_children.size()) {
			if (node.m_children[i].m_type == NodeType::typeTag) {
				OutNodes(node.m_children[i], ofs);
			}
			else if (node.m_children[i].m_type == NodeType::typeSimpleTag) {
				ofs << '<' << node.m_children[i].m_nodeName;
				if (node.m_children[i].m_attlibutes.size() > 0) {
					auto it = node.m_children[i].m_attlibutes.begin();
					while (it != node.m_children[i].m_attlibutes.end()) {
						ofs << ' ' << (*it).first << '=' << '"' << (*it).second << '"';
						it++;
					}
				}
				ofs << " />";
			}
			else if (node.m_children[i].m_type == NodeType::typeText) {
				ofs << node.m_children[i].m_text;
			}
			i++;
		}
		if (node.m_nodeName != "") {
			ofs << "</" << node.m_nodeName << '>';
		}
	}
}


void GetHttpPage(LPCTSTR pszServerName, LPCTSTR pszFileName,int port,string& ret)
{
	CInternetSession session(_T("My Session"));
	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	try
	{
		CString strServerName;
		INTERNET_PORT nPort = port;
		DWORD dwRet = 0;

		pServer = session.GetHttpConnection(pszServerName, nPort);
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, pszFileName);
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

void SplitURL2ServerFile(const string& base,string& server,string& file) {
	server.clear();
	file.clear();
	auto sz = base.find(string("://"));
	sz += 3;
	bool flg = false;
	while (sz < base.size()) {
		if (base[sz] == '/') {
			flg = true;
		}
		if (flg) {
			file += base[sz];
		}
		else {
			server += base[sz];
		}
		sz++;
	}
}

int main(int argc,char* argv[])
{
	if (argc == 3) {
		string inHtml = argv[1];
		cout << inHtml << endl;
		string html;
		regex pat("^http.*");
		if (regex_match(inHtml, pat)) {
			//WEBから取り込む
			string server, file;
			SplitURL2ServerFile(inHtml, server, file);
			if (file == "") {
				file = "/";
			}
			CStringW serverW(server.c_str());
			CStringW fileW(file.c_str());
			//今のところportは80固定
			int port = 80;
			GetHttpPage(serverW, fileW, port, html);
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

		CreateTokens(html);
		CreateNodes(rootNode, 0);
		ofstream outputfile(argv[2]);
		OutNodes(rootNode, outputfile);
		outputfile.close();
		cout << argv[2] << "に保存されました" << endl;
	}
	else {
		cout << "パラメータが足りません\n";
		cout << "Html2Xml 読み込むHTML 書きだすXMLファイル " << endl;
		return 0;
	}
	return 0;
}

