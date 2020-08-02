// Html2Xml.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

// Html2Xml.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "HtmlParser.h"

int main(int argc, char* argv[])
{
	if (argc == 3) {
		string inHtml = argv[1];
		cout << inHtml << endl;
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
