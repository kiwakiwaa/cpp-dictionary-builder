# CPP Dictionary Builder

A C++23 library for converting Japanese html based dictionaries into MDict and Yomitan compatible dictionaries. Features a modular architecture with strategy patterns.

## Features
- **Multi-format support**: MDict, Yomitan
- **Modular design**: Strategy patterns for key extration, link processing, asset handling
- **Configuration driven**: YAML-based dictionary-specific processing
- **Rich content**: HTML elements, images, audio
- **Batch processing**: Efficient handling of large dictionaries

### Supported dictionaries: 
- **RGKA9**: 例解学習漢字辞典［第九版］
- **KJT**: 旺文社漢字典［第四版］
- **NHKACCENT2**: NHK日本語発音アクセント新辞典
- **YDP**: 有斐閣現代心理学辞典

## Quick start

<details>

<summary>Installation</summary>

### Requirements
- C++23 compatible compiler
- CMake 3.22+
- [Glaze](https://github.com/stephenberry/glaze)

### Build
```shell
git clone https://github.com/kiwakiwaa/yomitan-dictionary-builder
cd yomitan-dictionary-builder
mkdir build && cd build
cmake ..
make
```
</details>

<details open>
<summary>Basic Usage</summary>

```c++
#include "yomitan_dictionary_builder/config/config_loader.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_parser.h"

int main()
{
    const auto configLoader = ConfigLoader::loadFromFile("resources/dictionaries.yaml");
    const auto config = configLoader.getDictionaryConfig("YDP")
    
    const auto parser = std::make_unique<MdictParser>(config.parserConfig, config.mDictConfig);
    parser->parse();
    return 0;
}
```
</details>

<details open>
<summary>Configuration example</summary>

```
general:
  author: "Your Name"
  showProgress: true
  parsingBatchSize: 250

dictionaries:
  YDP:
    MDictConfig:
      title: "有斐閣現代心理学辞典"
      appendixLinkIdentifier: "appendix/"
    ParserConfig:
      dictionaryPath: "resources/parsers/YDP/pages"
      indexPath: "resources/parsers/YDP/index/index_d.tsv"
      outputPath: "converted/有斐閣現代心理学辞典"
```
</details>

#### Parser architecture
```text
BaseParser
├── XMLParser
    ├── YomitanParser (Yomitan-specific processing)
    └── MdictParser (MDict format processing)
```
#### Strategy Components

- **KeyExtractionStrategy**: Extracts additional keys from dictionary entries
- **LinkHandlingStrategy**: Process link elements
- **ImageHandlingStrategy**: Handle image elements

<details open> 

<summary>Programmatic Dictionary Creation</summary>

### HTML Elements
```c++
#include "yomitan_dictionary_builder/core/dictionary/html_element.h"

// Create simple element
auto spanElement = createHtmlElement("span", "例文テキスト");

// Create nested elements
auto divElement = createHtmlElement("div");
divElement->addContent(spanElement);

// Add data attributes
std::map<std::string, std::string> dataAttrs = {{"見出しG", ""}, {"type", "example"}};
spanElement->setData(dataAttrs);
```

### Dictionary Entries

```c++
#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"

DicEntry entry{"漢字", "かんじ"};
auto definitionElement = createHtmlElement("div", "日本語の文章で使われる中国由来の文字");
entry.addElement(definitionElement);
```

</details>


#### Project Structure

```text
├── src/                    # Source code
├── include/                # Header files
├── resources/              # Configuration file and dictionary data
├── converted/              # Output directory for converted dictionaries
├── test/                   # Test files
└── lib/                    # Third-party libraries
```


# CPP辞書ビルダー

Yomitan互換の辞書ファイルを作成・管理するためのC++23ライブラリです。複数の辞書フォーマットをYomitanおよびMDict互換の形式に変換するための包括的なフレームワークを提供しています。

## 特徴

まだ開発中のプロジェクトなので、不完全な部分があります。現在はコア機能を実装している段階で、TODOリストにはまだ多くの項目があります。

## 特徴

- **複数フォーマット対応**: MDict、その他のXMLベース辞書フォーマットをサポート
- **モジュラー設計**: 戦略パターンによる柔軟な処理戦略
- **設定駆動**: YAML設定ファイルによる辞書固有の処理設定
- **リッチコンテンツ**: HTML要素、画像、音声などをサポート
- **バッチ処理**: 大規模辞書の効率的な処理


<details> 

<summary>インストール方法</summary>

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

</details>

<details open>

<summary>使い方</summary>

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
auto definitionElement = std::make_shared<HTMLElement>("div", "日本語の文章で使われる中国由来の文字");
entry.addElement(definitionElement);

// 例文を追加
auto exampleElement = std::make_shared<HTMLElement>("div");
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

</details>


## 今後の取り組み
- より多くの辞書フォーマットのサポート
- パフォーマンス最適化
- テストカバレッジの拡充