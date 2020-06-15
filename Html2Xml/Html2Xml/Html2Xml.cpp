// Html2Xml.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cctype>

using namespace std;


//開始タグ名を取り出す
string getStartTagName(const string& tag) {
	bool in = false;
	string ret = "";
	for (int i = 1; i < tag.size(); i++) {
		if (in) {
			if (!isalpha(tag[i])) {
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
			if (isalpha(tag[i])) {
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
	for (int i = 2; i < tag.size(); i++) {
		if (in) {
			if (!isalpha(tag[i])) {
				return ret;
			}
			else {
				ret += tag[i];
			}
		}
		else {
			if (isalpha(tag[i])) {
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

void CreateAttlibutes(Node& newNode,const vector<string>& tagStrings) {
	for (size_t i = 1; i < tagStrings.size(); i++) {
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
	for (int i = 1; i < tagStr.size(); i++) {
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
			if (tagStr[i] != ' ' &&  tagStr[i] != '\t' && tagStr[i] != '\n') {
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


bool ChkOmissionTag(const string& tagName) {
	string chk = tagName;
	transform(chk.begin(), chk.end(), chk.begin(), ::tolower);
	vector<string> vec = {
		"li",
		"dt",
		"dd",
		"p",
		"rt",
		"rp",
		"optgroup",
		"option",
		"colgroup",
		"thead",
		"tbody",
		"tfoot"
		"tr",
		"td",
		"th"
	};
	for (auto& v : vec) {
		if (chk == v) {
			return true;
		}
	}
	return false;
}

size_t CreateNodes(Node& node,size_t tokenIndex) {
	while (tokenVec.size() > tokenIndex) {
		if (isStartTag(tokenVec[tokenIndex])) {
			string startTagName = getStartTagName(tokenVec[tokenIndex]);
			if (startTagName == string("")) {
				//コメントやDocType宣言
				tokenIndex++;
				continue;
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
	for (int i = 0; i < html.size(); i++) {
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

void OutNodes(const Node& node) {
	if (node.m_type == NodeType::typeRoot|| node.m_type == NodeType::typeTag) {
		if (node.m_type == NodeType::typeRoot) {
			cout << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		}
		if (node.m_nodeName != "") {
			cout << '<' << node.m_nodeName;
			if (node.m_attlibutes.size() > 0) {
				auto it = node.m_attlibutes.begin();
				while (it != node.m_attlibutes.end()) {
					cout << ' ' << (*it).first << '=' << '"' << (*it).second << '"';
					it++;
				}
			}
			cout << '>';
		}
		size_t i = 0;
		while (i < node.m_children.size()) {
			if (node.m_children[i].m_type == NodeType::typeTag) {
				OutNodes(node.m_children[i]);
			}
			else if (node.m_children[i].m_type == NodeType::typeSimpleTag) {
				cout << '<' << node.m_children[i].m_nodeName;
				if (node.m_children[i].m_attlibutes.size() > 0) {
					auto it = node.m_children[i].m_attlibutes.begin();
					while (it != node.m_children[i].m_attlibutes.end()) {
						cout << ' ' << (*it).first << '=' << '"' << (*it).second << '"';
						it++;
					}
				}
				cout << " />";
			}
			else if (node.m_children[i].m_type == NodeType::typeText) {
				cout << node.m_children[i].m_text;
			}
			i++;
		}
		if (node.m_nodeName != "") {
			cout << "</" << node.m_nodeName << '>';
		}
	}
}
/*
閉じタグ無し
area、base、br、col、command、embed、hr、img、input、keygen、link、meta、param、source、track、wbr
*/

/*
終了タグを省略できる要素
li、dt、dd、p、rt、rp、optgroup、option、colgroup、thead、tbody、tfoot、tr、td、th
*/


int main()
{
	ifstream ifs("test.html");
	string line;
	string html;
	while (getline(ifs, line)) {
		html += line;
		html += "\n";
	}
	CreateTokens(html);
	CreateNodes(rootNode, 0);
	OutNodes(rootNode);
}

