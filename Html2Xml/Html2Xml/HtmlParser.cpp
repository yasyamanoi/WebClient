#include "HtmlParser.h"


int HtmlParser::strReplace(string& tgt, const string& chk, const string& change) {
	auto index = tgt.find(chk);
	int repCount = 0;
	while (index != string::npos) {
		tgt.replace(index, chk.size(), change);
		repCount++;
		index = tgt.find(chk, index + change.size());
	}
	return repCount;
}

int HtmlParser::strSingleReplace(string& tgt, const string& chk, const string& change) {
	auto index = tgt.find(chk);
	if (index != string::npos) {
		tgt.replace(index, chk.size(), change);
	}
	return 1;
}



bool HtmlParser::getStartEndComment(const string& html, vector<StartEnd>& out) {
	auto strSz = html.size();
	bool isStart = false;
	StartEnd tmpSE;
	for (size_t i = 0; i < strSz; i++) {
		if (!isStart) {
			if (html[i] == '<') {
				if (i < (strSz - 4)) {
					if (html[i + 1] == '!' && html[i + 2] == '-' && html[i + 3] == '-') {
						tmpSE.m_start = i;
						isStart = true;
					}
				}
				else {
					return false;
				}
			}
		}
		else {
			if (html[i] == '-') {
				if (i < (strSz - 3)) {
					if (html[i + 1] == '-' && html[i + 2] == '>') {
						tmpSE.m_end = i + 2;
						out.push_back(tmpSE);
						isStart = false;
					}
				}
				else {
					return false;
				}
			}
		}
	}
	return true;
}

void HtmlParser::removeComment(const string& html, string& outString) {
	vector<StartEnd> se;
	size_t start = 0;
	if (getStartEndComment(html, se)) {
		size_t sz = se.size();
		for (size_t i = 0; i < sz; i++) {
			outString += html.substr(start, se[i].m_start - start);
			start = se[i].m_end + 1;
		}
		outString += html.substr(start);
	}
}

bool HtmlParser::removeExtraTags(const string& chk, const string& tag, string& out) {
	string startTag = "<" + tag;
	string endTag = "</" + tag;
	string::size_type  top = 0;
	string::size_type  start = 0;
	string::size_type  end = 0;
	while ((start = chk.find(startTag, start)) != string::npos) {
		end = chk.find(endTag, start);
		if (end == string::npos) {
			//失敗
			return false;
		}
		end = chk.find(">", end);
		if (end == string::npos) {
			//失敗
			return false;
		}
		out += chk.substr(top, start - top);
		end++;
		top = end;
		start = top;
	}
	out += chk.substr(end);
	return true;
}

void HtmlParser::createTokens(const string& html) {
	bool textFlg = true;
	string temp;
	for (size_t i = 0; i < html.size(); i++) {
		if (textFlg) {
			if (html[i] == '<') {
				if (temp != "") {
					m_tokenVec.push_back(temp);
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
					m_tokenVec.push_back(temp);
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

bool HtmlParser::isStartTag(const string& str) {
	if (str.size() < 2) {
		return false;
	}
	if (str[0] == '<' && str[1] != '/') {
		return true;
	}
	return false;
}

bool HtmlParser::isEndTag(const string& str) {
	if (str.size() < 2) {
		return false;
	}
	if (str[0] == '<' && str[1] == '/') {
		return true;
	}
	return false;
}

string HtmlParser::getStartTagName(const string& tag) {
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

string HtmlParser::getEndTagName(const string& tag) {
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

void HtmlParser::createAttlibutes(Node& newNode, const vector<string>& tagStrings) {
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

bool HtmlParser::chkSimpleTag(const string& tagName) {
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


bool HtmlParser::chkOmissionTag(const string& tagName, const string& nextTagName) {
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

Node HtmlParser::createSingleTagNode(const string& tagStr) {
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
					createAttlibutes(newNode, tagStrings);
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
				createAttlibutes(newNode, tagStrings);
				return newNode;
			}
		}
	}
	return newNode;
}



size_t HtmlParser::createNodes(Node& node, size_t tokenIndex) {
	while (m_tokenVec.size() > tokenIndex) {
		if (isStartTag(m_tokenVec[tokenIndex])) {
			string startTagName = getStartTagName(m_tokenVec[tokenIndex]);
			if (startTagName == string("")) {
				//DocType宣言
				tokenIndex++;
				continue;
			}
			if (chkOmissionTag(node.m_nodeName, startTagName)) {
				//省略可能なタグだった
				//tokenIndexは進めずにリターン
				return tokenIndex;
			}
			Node newNode = createSingleTagNode(m_tokenVec[tokenIndex]);
			if (chkSimpleTag(newNode.m_nodeName)) {
				newNode.m_type = NodeType::typeSimpleTag;
				node.m_children.push_back(newNode);
				tokenIndex++;
			}
			else {
				auto sz = node.m_children.size();
				node.m_children.push_back(newNode);
				tokenIndex = createNodes(node.m_children[sz], tokenIndex + 1);
			}
		}
		else if (isEndTag(m_tokenVec[tokenIndex])) {
			if (getEndTagName(m_tokenVec[tokenIndex]) == node.m_nodeName) {
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
			newNode.m_text = m_tokenVec[tokenIndex];
			node.m_children.push_back(newNode);
			tokenIndex++;
		}
	}
	return tokenIndex;
}

void HtmlParser::outNodes(Node& node, ofstream& ofs) {
	if (node.m_type == NodeType::typeRoot || node.m_type == NodeType::typeTag) {
		if (node.m_type == NodeType::typeRoot) {
			ofs << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
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
				outNodes(node.m_children[i], ofs);
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
				if (node.m_children[i].m_nodeName != "span") {
					ofs << "\n";
				}
			}
			else if (node.m_children[i].m_type == NodeType::typeText) {
				strReplace(node.m_children[i].m_text, "&", "");
				strReplace(node.m_children[i].m_text, " ", "");
				ofs << node.m_children[i].m_text;
			}
			i++;
		}
		if (node.m_nodeName != "") {
			ofs << "</" << node.m_nodeName << '>';
			if (node.m_nodeName != "span") {
				ofs << "\n";
			}
		}
	}
}

bool HtmlParser::pars(const string& html) {
	vector<StartEnd> se;
	string outString;
	removeComment(html,outString);
	string tmp;
	if (!removeExtraTags(outString, "script", tmp)) {
		cout << "scriptタグの除去に失敗しました\n";
	}
	outString = tmp;
	tmp.clear();
	if (!removeExtraTags(outString, "style", tmp)) {
		cout << "styleタグの除去に失敗しました\n";
	}
	outString = tmp;
	tmp.clear();
	strReplace(outString, "\t", " ");
	strReplace(outString, "\r\n", " ");
	strReplace(outString, "\n", " ");
	while (strReplace(outString, "  ", " ") > 0) {
		;
	}
	createTokens(outString);
	createNodes(m_rootNode, 0);
	return true;
}

bool HtmlParser::output(const string& outfilename) {
	ofstream outputfile(outfilename);
	outNodes(m_rootNode, outputfile);
	outputfile.close();
	cout << outfilename << "に保存されました" << endl;
	return true;
}

