// NodeTest.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <string>
#include <vector>


using namespace std;

string dataStr = "abc<n>test<n>rrr<n>fff</n></n></n>testtest<n>bbb</n>xyz";

enum class NodeType {
    typeTag,
    typeText
};

struct Node {
    NodeType m_type;
    vector<Node> m_children;
};

vector<string> tokenVec;

void CreateTokens() {

}

int main()
{

    std::cout << "Hello World!\n";
}

