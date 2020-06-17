#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <regex>

#include <SDKDDKVer.h>
#include <afxinet.h>


using namespace std;

struct StartEnd {
	string::size_type m_start;
	string::size_type m_end;
};

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



class HtmlParser {
	vector<string> m_tokenVec;
	Node m_rootNode;

	void outNodes(Node& node, ofstream& ofs);
	Node createSingleTagNode(const string& tagStr);
	bool chkOmissionTag(const string& tagName, const string& nextTagName);
	bool chkSimpleTag(const string& tagName);
	void createAttlibutes(Node& newNode, const vector<string>& tagStrings);
	string getStartTagName(const string& tag);
	string getEndTagName(const string& tag);
	bool isStartTag(const string& str);
	bool isEndTag(const string& str);
	size_t createNodes(Node& node, size_t tokenIndex);
	void createTokens(const string& html);
	bool getStartEndComment(const string& html,vector<StartEnd>& out);
	void removeComment(const string& html,string& outString);
	bool removeExtraTags(const string& chk, const string& tag, string& out);
	int strReplace(string& tgt, const string& chk, const string& change);
	int strSingleReplace(string& tgt, const string& chk, const string& change);
public:
	HtmlParser() {}
	~HtmlParser() {}
	bool pars(const string& html);
	bool output(const string& outfilename);

};