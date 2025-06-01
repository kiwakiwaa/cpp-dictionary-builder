"""
指定されたフォルダ内の全てのTIFFおよびPDFファイルをPNG形式に変換する
"""

import os as オーエス
from pathlib import Path as パス
from PIL import Image as 画像分析
import fitz as pdf分析モジュール
import argparse as 引数パーサモジュール

class 例外(Exception):
    pass

プリント = print
引数パーサ = 引数パーサモジュール.ArgumentParser
画像分析を開く = 画像分析.open
真 = True
偽 = False
壱 = 1
零 = 0

色空間マップ = {
    'CMYK': lambda 図: 図.convert('RGB'),
    'RGBA': lambda 図: 図.convert('RGBA'),
    'LA': lambda 図: 図.convert('RGBA'),
    'RGB': lambda 図: 図.convert('RGB'),
    'P': lambda 図: 図.convert('RGBA') if 'transparency' in 図.info else 図.convert('RGB')
}

def tiffをpngに変換(入力パス, 出力パス):
    try:
        with 画像分析を開く(入力パス) as 画像:
            色空間変換 = 色空間マップ.get(画像.mode, None)

            if not 色空間変換:
                return 偽

            画像 = 色空間変換(画像)
            画像.save(出力パス, 'PNG')
        return 真

    except 例外 as エラー:
        プリント(f"TIFF変換エラー{入力パス}：{エラー}")
        return 偽


def フォルダ内のファイルを変換(フォルダパス, 元ファイルを保持=真):
    フォルダパス = パス(フォルダパス)

    if not フォルダパス.exists():
        プリント(f"エラー：フォルダ '{フォルダパス}'が存在しないよ")

    if not フォルダパス.is_dir():
        プリント(f"エラー：'{フォルダパス}'はディレクトリじゃない")

    tiff拡張子 = {'.tiff', '.tif'}
    pdf拡張子 = {'.pdf'}

    変換カウント = 零
    エラーカウント = 零

    for ファイルパス in フォルダパス.iterdir():
        if not ファイルパス.is_file():
            continue

        ファイル拡張子 = ファイルパス.suffix.lower()

        if ファイル拡張子 in tiff拡張子:
            出力パス = ファイルパス.with_suffix('.png')
            プリント(f"TIFF変換中：{ファイルパス.name} -> {出力パス}")

            if tiffをpngに変換(ファイルパス, 出力パス):
                変換カウント += 壱
                if not 元ファイルを保持:
                    ファイルパス.unlink()

            else:
                エラーカウント += 壱

        elif ファイル拡張子 in pdf拡張子:
            変換されたファイル一覧 = pdfをpngに変換(ファイルパス, フォルダパス)
            if 変換されたファイル一覧:
                変換カウント += len(変換されたファイル一覧)
                if not 元ファイルを保持:
                    ファイルパス.unlink()

            else:
                エラーカウント += 壱

    プリント("-------------------")
    プリント(f"変換完了よ")
    プリント(f"変換成功：{エラーカウント}ファイル")
    if エラーカウント > 零:
        プリント(f"エラー発生：{エラーカウント}ファイル")


def pdfをpngに変換(入力パス, 出力フォルダ):
    try:
        ベース名 = パス(入力パス).stem
        変換されたファイル一覧 = []

        with pdf分析モジュール.open(入力パス) as pdfドキュメント:

            for 頁番号 in range(pdfドキュメント.page_count):
                頁 = pdfドキュメント[頁番号]

                画素マップ = 頁.get_pixmap(matrix=pdf分析モジュール.Matrix(300/72, 300/72))

                if pdfドキュメント.page_count == 壱:
                    出力パス = オーエス.path.join(出力フォルダ, f"{ベース名}.png")
                    プリント(f"PDF変換中：{入力パス.name} -> {出力パス}")
                else:
                    出力パス = オーエス.path.join(出力フォルダ, f"{ベース名}_{頁番号}頁")
                    プリント(f"PDF変換中：{入力パス.name} -> {出力パス}")

                画素マップ.save(出力パス)
                変換されたファイル一覧.append(出力パス)

        return 変換されたファイル一覧

    except 例外 as エラー:
        プリント(f"PDF変換エラー{入力パス}：{エラー}")
        return []


def 引数を解析():
    パーサー = 引数パーサ(
        description='指定されたフォルダ内のTIFFとPDFファイルをPNGに変換するツールだよ〜'
    )

    パーサー.add_argument(
        'フォルダパス',
        help='変換するフォルダが入っているフォルダのパスを指定して'
    )

    パーサー.add_argument(
        '--元ファイル削除', '-d',
        action='store_true',
        help='変換後に元のファイルを削除するかどうか'
    )

    return パーサー.parse_args()

def メイン():
    引数 = 引数を解析()

    元ファイルを保存 = not 引数.元ファイル削除
    フォルダ内のファイルを変換(引数.フォルダパス, 元ファイルを保存)


if __name__ == "__main__":
    メイン()


