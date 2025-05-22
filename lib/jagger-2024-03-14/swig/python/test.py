#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys, re, jdepp

opt    = jdepp.option (len (sys.argv), [c.encode ('utf-8') for c in sys.argv])
parser = jdepp.parser (opt)

parser.init ()

## parse raw text (J.DepP must be configured with --enable-standalone)
# """
# text = "彼は彼女にもう一度会いたいと思った。"

## if you configure --enable-standalone, this would work
# sys.stdout.write ("Sentence:\n\t%s\n" % text)
# sys.stdout.write ("Result:\n%s\n" % parser.parse_tostr (text))
# s = parser.parse (text)
# """

## parse pos-tagged text
tagged_text = """\
彼	名詞,普通名詞,*,*,彼,かれ,代表表記:彼/かれ 漢字読み:訓 カテゴリ:人
は	助詞,副助詞,*,*,は,は,*
彼女	名詞,普通名詞,*,*,彼女,かのじょ,代表表記:彼女/かのじょ カテゴリ:人
に	助詞,格助詞,*,*,に,に,*
もう	副詞,*,*,*,もう,もう,代表表記:もう/もう 数量修飾
一	名詞,数詞,*,*,一,いち,カテゴリ:数量
度	接尾辞,名詞性名詞助数辞,*,*,度,ど,代表表記:度/ど 準内容語
会い	動詞,*,子音動詞ワ行,基本連用形,会う,あい,代表表記:会う/あう 反義:動詞:分かれる/わかれる;動詞:別れる/わかれる
たい	接尾辞,形容詞性述語接尾辞,イ形容詞アウオ段,基本形,たい,たい,連語
と	助詞,格助詞,*,*,と,と,*
思った	動詞,*,子音動詞ワ行,タ形,思う,おもった,代表表記:思う/おもう 補文ト
。	特殊,句点,*,*,。,。,連語
EOS
"""

# sys.stdout.write (parser.parse_from_postagged_tostr (tagged_text))

s = parser.parse_from_postagged (tagged_text)

sys.stdout.write ("Sentence:\n\t%s\n" % s.str ())

print ("Tokens:")
for m in s.tokens ():
    sys.stderr.write ("\t%s\t%s\n" % (m.str (), m.feature))

print ("Dependencies ((dependents ->) token => head):")
i = 0
for b in s.chunks ():
    head  = b.head ()
    dpnds = b.dependents ()
    sys.stdout.write ("\t")
    if not dpnds.empty ():
        sys.stdout.write ("%s -> " % ', '.join (d.str () for d in dpnds))
    sys.stdout.write ("[%s]" % b.str ())
    if head:
        sys.stdout.write (" => %s" % head.str ())
    sys.stdout.write ("\n")
sys.stdout.write ("\n")

## read a tree from parsed text
parsed_text = """
# S-ID: 1; J.DepP
* 0 2D
彼	名詞,普通名詞,*,*,彼,かれ,代表表記:彼/かれ 漢字読み:訓 カテゴリ:人
は	助詞,副助詞,*,*,は,は,*
* 1 2D
彼女	名詞,普通名詞,*,*,彼女,かのじょ,代表表記:彼女/かのじょ カテゴリ:人
に	助詞,格助詞,*,*,に,に,*
* 2 3D
会い	動詞,*,子音動詞ワ行,基本連用形,会う,あい,代表表記:会う/あう 反義:動詞:分かれる/わかれる;動詞:別れる/わかれる
たい	接尾辞,形容詞性述語接尾辞,イ形容詞アウオ段,基本形,たい,たい,連語
と	助詞,格助詞,*,*,と,と,*
* 3 -1D
思った	動詞,*,子音動詞ワ行,タ形,思う,おもった,代表表記:思う/おもう 補文ト
。	特殊,句点,*,*,。,。,連語
EOS
"""

s = parser.read_result (parsed_text);

sys.stdout.write ("Sentence: %s\n" % s.str ())
