#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys, codecs, io

# ; f[0]:  pos1
# ; f[1]:  pos2
# ; f[2]:  pos3
# ; f[3]:  pos4
# ; f[4]:  cType
# ; f[5]:  cForm
# ; f[6]:  lForm; 語彙素読み
# ; f[7]:  lemma; 語彙素
# ; f[8]:  orth; 書字形出現形
# ; f[9]:  pron; 出現発音形
# ; f[10]: orthBase; 書字形; skip
# ; f[11]: pronBase; skip (cannot recover from core_SUW.txt)
# ; f[12]: goshu; skip

if len (sys.argv) < 3:
    sys.exit ("Usage: %s BCCWJ_DIR train.JAG [test.JAG]")

train_writer = codecs.open (sys.argv[2], "w", encoding='utf-8')
test_writer  = train_writer if len (sys.argv) < 4 else codecs.open (sys.argv[3], "w", encoding='utf-8')

fmap = dict ()
for line in codecs.open (sys.argv[1] + "/BCCWJ-ANNOTATION-ORDER/rename.rb", encoding='utf-8'):
    if "_core" in line:
        f = line[:-1].split ("\t")
        fmap[f[3]] = f[1]

flag = False
sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding="utf-8")

sent = []
beg = -1
for line in sys.stdin:
    f = line.rstrip ().split ("\t")
    if flag and f[9] == 'B':
        for m in sent:
            writer.write ("%s\t%s,%s,%s,%s,%s,%s,%s\n" % (m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7]))
        if len (sent) > 0:
            writer.write ("EOS\n")
        sent = []
        beg = -1
    # print ([line, f, len (f)])
    flag = True
    pos  = f[16].split ("-")
    pos.extend (["*"] * (4 - len (pos)))
    fid   = f[1]
    lemma = f[12] if f[12] else '*'
    writer = test_writer if fmap[fid] == 'A' else train_writer
    beg_ = int (f[2])
    if len (f) < 18: # space etc.
        assert ','.join (pos) == '空白,*,*,*'
        # sys.stderr.write ("%s\t%s,*,*,*,%s,%s,*\n" % (lemma, ','.join (pos), lemma, lemma))
    else:
        orig  = f[23]
        cType = f[17] if f[17] else '*'
        cForm = f[18] if f[18] else '*'
        lForm = f[13] if f[13] else '*'
        orth  = f[22] if f[22] else '*'
        pron  = f[24] if len (f) > 24 and f[24] else '*'
        if beg == beg_:
            sent[-1][4] += lForm
            sent[-1][5] += lemma
            sent[-1][6] += orth
            sent[-1][7] += pron
        else:
            sent.append ([orig, ','.join (pos), cType, cForm, lForm, lemma, orth, pron])
    beg = beg_
# writer.write ("EOS\n")

train_writer.close
test_writer.close
