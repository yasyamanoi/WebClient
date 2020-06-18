# WebClient  
Webのクライアントツール集です。  
１、Xml2Xml  
手始めに「HTMLをXMLに変換するツール」を作成してみました。  
Xml2XmlソリューションをVS2019で開いて、ビルドしてください。  
C++によるコンソールアプリです。MFCを使用しています。  
ビルドができましたら、実行ファイルのディレクトリに行き、コンソールで  
Xml2Xml もとになるHTML 保存するXMLファイル  
と入力してください。  
例えば、ものになるHTMLが  
「https://hoge.huga.piyo/index.html」  
で、保存するファイルが「index.xml」ならば  
Xml2Xml https://hoge.huga.piyo/index.html index.xml  
となります。  
また、ローカルファイルをXMLにする場合は  
Xml2Xml index.html index.xml  
とすれば、index.htmlをindex.xmlにします。  
【制限事項】  
※scriptタグ、styleタグは削除されます。  
※utf8オンリーです。ただしsjisのサイト等は読めないわけではなくて、  
　XML作成後「XML宣言」の部分のエンコード指定を「encoding="sjis"」などにすればOKです。  
※一応HTML5に仕様に合わせたつもりですが、読めないサイト等あってもご了承ください。  
　あくまで、自分の責任においてお使いください。  
　（MITライセンスなので、ソース改変自由にやってください）
