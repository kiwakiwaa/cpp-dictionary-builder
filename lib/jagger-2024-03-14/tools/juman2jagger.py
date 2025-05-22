#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Usage: juman2jagger.py KC4|KWDLC_DIR test.ids train.JAG [test.JAG] < *.knp
#   convert KC4|KWDLC corpus to Jagger (MeCab) format
import sys, codecs, io, re

if len (sys.argv) < 2:
    sys.exit ("juman2jagger.py KC4|KWDLC_DIR train.JAG dev.JAG test.JAG dev.id test.id < *.knp")

benchmark = len (sys.argv) == 6

train_writer = codecs.open (sys.argv[1], "w", encoding='utf-8')
if benchmark:
    dev_writer   = codecs.open (sys.argv[2], "w", encoding='utf-8')
    test_writer  = codecs.open (sys.argv[3], "w", encoding='utf-8')

dev_ids  = set ()
test_ids = set ()
width = 0
if benchmark:
    for line in open (sys.argv[4]):
        if width == 0:
            width = len (line[:-1])
        elif width != len (line[:-1]):
            sys.exit ("inconsistent id")
        dev_ids.add (line[:-1])

    for line in open (sys.argv[5]):
        if width != len (line[:-1]):
            sys.exit ("inconsistent id")
        test_ids.add (line[:-1])

header, data, head, body, t2c = "", "", [], [], {}
pat_s = re.compile (u' +')
sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding="utf-8")
for line in sys.stdin:
    if line[0] == '#':
        doc_id = line[7:7+width]
        writer = train_writer
        if doc_id in test_ids:
            writer = test_writer
        elif doc_id in dev_ids:
            writer = dev_writer
    elif line[0] == '*' or line[0] == '+':
        pass
    elif line[0] == 'E':
        writer.write (line)
    else:
        f = pat_s.split (line)[:11]
        if f[3] == "未定義語": # following corpus2train.rb in JUMAN++
            f[3],f[5],f[7],f[9]  = "名詞", "普通名詞", "*", "*"
        writer.write ("%s\t%s\n" % (f[0], ','.join (f[3:10:2] + [f[2], f[1], '*'])))

train_writer.close
if benchmark:
    dev_writer.close
    test_writer.close
