#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys, re, argparse, codecs

parser = argparse.ArgumentParser ()
parser.add_argument ('system', help='system outputs')
parser.add_argument ('gold', help='gold outputs')
parser.add_argument ('-e', help='set error format: seg or pos')
parser.add_argument ('-l', type=int, default=0, help='set level of evaluations')
parser.add_argument ('-t', help='set training data to break down evaluations')
parser.add_argument ('-d', help='set dictonary data to break down evaluations')

args = parser.parse_args ()

def read_results (file_name):
    results = []
    result  = []
    with codecs.open (file_name, encoding='utf-8') as f:
        for line in f:
            if line[:-1] == 'EOS':
                results.append (result)
                result = []
            else:
                w, f = line[:-1].split ("\t", 1)
                result.append ([w] + list (map (lambda x: x if x else "*", f.split (",")[:args.l])))
#                if result[-1][5] == '*':
#                    result[-1][5] = result[-1][0]
    return results

def add_truth_counter (counters_, eqs_):
    counters_[0][0] += 1
    for l in range (args.l):
        if eqs_[l + 1]:
            counters_[0][l + 1] += 1
        else:
            counters_[1][l + 1] += 1
            counters_[2][l + 1] += 1

def add_false_counter (counter_):
    counter_[0] += 1
    for l in range (args.l):
        counter_[l + 1] += 1

def print_seg_err (sw_, gw_):
    sys.stdout.write (" [\x1b[34m%s\x1b[0m" % ' '.join (gw_))
    sys.stdout.write (" -> ")
    sys.stdout.write (" \x1b[31m%s\x1b[0m]" % ' '.join (sw_))

def print_pos_err (sw_, gw_, sf_, gf_):
    for w, f in zip (gw_, gf_):
        sys.stdout.write ("\x1b[34m%s/%s\x1b[0m " % (w, f))
    sys.stdout.write ("->")
    for w, f in zip (sw_, sf_):
        sys.stdout.write (" \x1b[31m%s/%s\x1b[0m" % (w, f))
    sys.stdout.write ("\n")

def print_scores (tp_, fp_, fn_, header_):
    pre = tp_ / (tp_ + fp_)
    rec = tp_ / (tp_ + fn_)
    f1  = 2 * pre * rec / (pre + rec) if pre + rec > 0 else 0
    sys.stderr.write ("%s" % header_)
    sys.stderr.write ("  P / R / F1 = (%.4f, %.4f, %.4f); " % (pre, rec, f1))
    sys.stderr.write ("(tp, fp, fn) = (%6d, %6d, %6d)\n" % (tp_, fp_, fn_))

systems = read_results (args.system)
golds   = read_results (args.gold)

ws = set ([line.split ('\t', 1)[0]
           for line in open (args.t) if line[:-1] != 'EOS'] if args.t else [])

ws = set ([line.split (',', 1)[0]
           for line in open (args.d)] if args.d else [])

tp, fp, fn, utp, ufp, ufn, stp, sfp, sfn = [[0] * (args.l + 1) for i in range (9)]
for system, gold in zip (systems, golds):
    sent = ''.join (f[0] for f in system)
    l = 0
    sl = []
    for f in system:
        sl.append (tuple ([l, l + len (f[0])]))
        l += len (f[0])
    l = 0
    gl = []
    for f in gold:
        gl.append (tuple ([l, l + len (f[0])]))
        l += len (f[0])
    # count
    err_report = args.e and sl != gl
    i = j = 0
    if err_report:
        sys.stdout.write ("# %s\n" % sent)
    while i < len (sl) or j < len (gl):
        i_beg = i
        j_beg = j
        if sl[i] == gl[j]:
            eqs = tuple (system[i][0:l+1] == gold[j][0:l+1]
                         for l in range (args.l + 1))
            add_truth_counter ([tp, fp, fn], eqs)
            if ws:
                add_truth_counter ([stp, sfp, sfn] if sent[sl[i][0]:sl[i][1]] in ws else [utp, ufp, ufn], eqs)
            if err_report:
                if args.e == 'seg':
                    sys.stdout.write (" %s" % sent[sl[i][0]:sl[i][1]])
                else:
                    if system[i][1:] == gold[j][1:]:
                        sys.stdout.write ("%s\t%s\n" % (sent[sl[i][0]:sl[i][1]], ','.join (system[i][1:])))
                    else:
                        print_pos_err ([sent[sl[i][0]:sl[i][1]]],
                                       [sent[gl[j][0]:gl[j][1]]],
                                       [','.join (system[i][1:])],
                                       [','.join (gold[j][1:])])
        else:
            eqs = [True] * (args.l + 1)
            while sl[i][1] != gl[j][1]:
                if sl[i][1] < gl[j][1]:
                    add_false_counter (fp)
                    if ws:
                        add_false_counter (sfp if sent[sl[i][0]:sl[i][1]] in ws else ufp)
                    i += 1
                else:
                    add_false_counter (fn)
                    if ws:
                        add_false_counter (sfn if sent[gl[j][0]:gl[j][1]] in ws else ufn)
                    j += 1
            add_false_counter (fp)
            add_false_counter (fn)
            if ws:
                add_false_counter (sfp if sent[sl[i][0]:sl[i][1]] in ws else ufp)
                add_false_counter (sfn if sent[gl[j][0]:gl[j][1]] in ws else ufn)
            if err_report:
                if args.e == 'seg':
                    print_seg_err ([sent[t[0]:t[1]] for t in sl[i_beg:i+1]],
                                   [sent[t[0]:t[1]] for t in gl[j_beg:j+1]])
                else:
                    print_pos_err ([sent[t[0]:t[1]] for t in sl[i_beg:i+1]],
                                   [sent[t[0]:t[1]] for t in gl[j_beg:j+1]],
                                   [','.join (f[1:]) for f in system[i_beg:i+1]],
                                   [','.join (f[1:]) for f in gold[j_beg:j+1]])
        i += 1
        j += 1
    if err_report:
        sys.stdout.write ("\n" if args.e == 'seg' else "EOS\n\n")

for l in range (args.l + 1):
    print_scores (tp[l], fp[l], fn[l], "LEVEL %d: " % l)
    if ws:
        print_scores (stp[l], sfp[l], sfn[l], "   (seen)")
        print_scores (utp[l], ufp[l], ufn[l], " (unseen)")
