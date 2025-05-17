# Yomitan辞書ビルダー

Yomitan互換の辞書ファイルを作成・管理するためのC++23ライブラリです。このプロジェクトでは、リッチなHTML要素を含む構造化された辞書エントリーをプログラムで簡単に作成するためのインターフェースを提供しています。

## 現状

まだ開発中のプロジェクトなので、不完全な部分があります。現在はコア機能を実装している段階で、TODOリストにはまだ多くの項目があります。

## 特徴

- 単語と読みを含む辞書エントリーの作成
- エントリーのコンテンツ用の階層的なHTML要素の構築
- Yomitan互換のJSON形式での辞書のエクスポート
- リンク、スタイル付きテキスト、その他のコンテンツのサポート

## 必要環境

- C++23対応コンパイラ
- JSONハンドリング用の[Glaze](https://github.com/stephenberry/glaze)
- CMake 3.22以上

## インストール方法

リポジトリをクローンし、CMakeでビルドします：

```bash
git clone https://github.com/kiwakiwaa/yomitan-dictionary-builder
cd yomitan-dictionary-builder
mkdir build && cd build
cmake ..
make
```

## 使い方
### HTML要素の作成
HTML要素は辞書エントリーの構成要素です。このライブラリでは、Yomitan互換の要素のみが使用されるよう確認しています。

```c++
#include "core/html_element.h"

// テキストコンテンツを持つシンプルな要素を作成
auto spanElement = createHtmlElement("span", "例文テキスト");

// 入れ子の要素を作成
auto divElement = createHtmlElement("div");
divElement->addContent(spanElement);

// リンクにhref属性を追加
auto linkElement = createHtmlElement("a", "ここをクリック", "https://example.com");

// data属性を追加
std::map<std::string, std::string> dataAttrs = {{"リンクG", ""}, {"type", "example"}};
spanElement->setData(dataAttrs);
```

### 辞書エントリーの構築
```c++
#include "core/dicentry.h"

// 単語と読み仮名で基本的なエントリーを作成
DicEntry entry{"漢字", "かんじ"};

// コンテンツ要素を追加
auto definitionElement = createHtmlElement("div", "日本語の文章で使われる中国由来の文字");
entry.addElement(definitionElement);

// 例文を追加
auto exampleElement = createHtmlElement("div");
exampleElement->addContent("日本語を勉強するなら漢字も勉強しなければなりません。");
entry.addElement(exampleElement);
```

### 辞書の作成とエクスポート
```c++
#include "core/yomitan_dictionary.h"

// 新しい辞書を作成
YomitanDictionary dictionary{"大漢和辞典"};

// 辞書にエントリーを追加
auto entry = std::make_unique<DicEntry>("豊葦原瑞穂国", "とよあしはらのみずほのくに");
entry->addElement(createHtmlElement("div", "日本の古称"));
dictionary.addEntry(entry);

// ディレクトリにエクスポート
dictionary.exportDictionary("/path/to/output/directory");
```

## 今後の取り組み
- メモリ効率のためのチャンク単位のディスク書き込み
- スペースを含むファイルパスの適切な処理
- メモリ使用量に基づく自動フラッシュ
- 適切な場所でのshared_ptrからunique_ptrへの変換