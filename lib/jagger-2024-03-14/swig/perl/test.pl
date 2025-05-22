#!/usr/bin/env perl
use strict;
use jdepp;

unshift (@ARGV, '\$0');
my $opt    = new jdepp::option ($#ARGV + 1, \@ARGV);

my $parser = new jdepp::parser ($opt);

$parser->init ();

## parse raw text (J.DepP must be configured with --enable-standalone)
# =begin
# my $text = "彼は彼女にもう一度会いたいと思った。";
# printf "Sentence:\n\t%s\n",  $text;
# printf "Result:\n%s\n", $parser->parse_tostr ($text);
# $s = $parser->parse ($text);
# =cut

## parse pos-tagged text
my $tagged_text = << "__EOS__";
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
__EOS__

# printf $parser->parse_from_postagged_tostr ($tagged_text)

my $s = $parser->parse_from_postagged ($tagged_text);

# print text
printf "Sentence:\n\t%s\n", $s->str ();

# enumerate tokens
print "Tokens:\n";
my $ms = $s->tokens ();
for (my $i = 0; $i < $s->{token_num}; ++$i) {
    my $m = $s->token ($i);
    printf "\t%s\t%s\n", $m->str (), $m->{feature};
}

# enumerate dependencies around each chunk
print "Dependencies ((dependents ->) token => head):\n";
my $bs = $s->chunks ();
for (my $i = 0; $i < $s->{chunk_num}; ++$i) {
    my $b = $bs->get ($i);
    my $head  = $b->head ();
    my $dpnds = $b->dependents ();
    printf "\t";
    if (! $dpnds->empty ()) {
        printf "%s", $dpnds->get (0)->str();
        for (my $j = 1; $j < $dpnds->size (); ++$j) {
            printf ", %s", $dpnds->get ($j)->str();
        }
        printf " -> ";
    }
    printf "[%s]", $b->str ();
    if ($head) { printf " => %s", $head->str (); }
    printf "\n";
}
print "\n";

## read a tree from parsed text
my $parsed_text = << "__EOS__";
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
__EOS__

$s = $parser->read_result ($parsed_text);

printf "Sentence: %s\n", $s->str ();
