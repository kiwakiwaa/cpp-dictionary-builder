// Jagger -- C++ implementation of Pattern-based Japanese Morphological Analyzer
//  $Id: train_jagger.cc 2070 2024-03-14 07:54:57Z ynaga $
// Copyright (c) 2022 Naoki Yoshinaga <ynaga@iis.u-tokyo.ac.jp>
#include <jagger.h>

namespace jagger {
  static const char* FEAT_UNK = "\x09\xE5\x90\x8D\xE8\xA9\x9E\x2C\xE6\x99\xAE\xE9\x80\x9A\xE5\x90\x8D\xE8\xA9\x9E\x2C\x2A\x2C\x2A";
  static const char* FEAT_NUM = "\x09\xE5\x90\x8D\xE8\xA9\x9E\x2C\xE6\x95\xB0\xE8\xA9\x9E\x2C\x2A\x2C\x2A";
  static const char* FEAT_SYMBOL = "\x09\xE7\x89\xB9\xE6\xAE\x8A\x2C\xE8\xA8\x98\xE5\x8F\xB7\x2C\x2A\x2C\x2A";
  static int UC_SYMBOL_RANGE[][2] = { {0x0021, 0x002F}, {0x003A, 0x0040}, {0x005B, 0x0060}, {0x007B, 0x007E}, {0x00A1, 0x00BF}, {0x00D7, 0x00D7}, {0x00F7, 0x00F7}, {0x2000, 0x206F}, {0x20A0, 0x214F}, {0x2190, 0x2BFF}, {0x3000, 0x3004}, {0x3008, 0x303F}, {0x3200, 0x33FF}, {0xFE30, 0xFE4F}, {0xFE50, 0xFE6B}, {0xFF01, 0xFF0F}, {0xFF1A, 0xFF20}, {0xFF3B, 0xFF40}, {0xFF5B, 0xFF65}, {0xFFE0, 0xFFEF}, {0x10190, 0x1019C}, {0x1F000, 0x1FBFF}, {} }; // Symbol-like Unicode Blocks
  static const char* chars_[] = {"0123456789０１２３４５６７８９〇一二三四五六七八九十百千万億兆数・", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ", "ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヷヸヹヺーヽヾヿ", 0}; // characters for concatenation
  class pattern_builder { // build patterns from training data and dictinary
  private:
    bag_t <std::string> _tbag, _fbag;
    std::vector <pat_info_t> _pi2sf; // pi -> <surf, prev_pos, shift, fi, count>
    std::vector <std::pair <size_t, int> > _ccnt;
    template <typename T>
    static inline void _write_array (const T* const data, const size_t size, const std::string& fn) {
      FILE *fp = std::fopen (fn.c_str (), "wb");
      ERR_IF (! fp, "cannot write to %s", fn.c_str ());
      std::fwrite (data, sizeof (T), size, fp);
      std::fclose (fp);
    }
    static const char* _strchr_n (const char* p, int c, int n) // find nth c
    { do if (n-- && (p = std::strchr (p, c))) ++p; else return --p; while (1); }
    // examine UTF8 sequence p consist of only numeric / alpha / kana characters
    static int check_ctype (const char* p, const int len, const char* char_t, int n = ANY) {
      for (int b (0), offset (0); n && offset < len; offset += b)
        n &= char_t[unicode (p + offset, b)];
      return n;
    }
  public:
    pattern_builder () : _tbag (), _fbag (), _pi2sf (), _ccnt () {}
    ~pattern_builder () {}
    void extract_patterns (const std::string& train, const std::vector <std::string>& dict) {
      bag_t <std::pair <std::string, int> >  pbag; // pattern -> pi
      std::vector <std::map <std::pair <int, int>, int> > pi2sfic; // pi -> <shift, feature> -> count
      std::vector <std::map <int, int> > si2ti2fi; // unseen seed -> features
      std::vector <int> ti2c (1, -1); // counter to set features (core) for unk
      char char_t[CP_MAX + 1] = {0};
      const long max_plen = 1 << MAX_PATTERN_BITS;
      _tbag.to_i ("\tBOS");
      _tbag.to_i (FEAT_UNK);    // t1
      _tbag.to_i (FEAT_NUM);    // t2
      _tbag.to_i (FEAT_SYMBOL); // t3
      if (! dict.empty ()) { // read seeds from dictionary
        std::fprintf (stderr, "reading seed patterns from dictionary...");
        for (std::vector <std::string>::const_iterator it = dict.begin (); it != dict.end (); ++it) {
          FILE* fp = std::fopen (it->c_str (), "r");
          ERR_IF (! fp, "cannot read from %s\n", it->c_str ());
          for (char line[BUF_SIZE]; std::fgets (line, BUF_SIZE, fp); ) {
            const size_t len = std::strlen (line);
            const char *p (line), *surf (*p == '"' ? ++p : p);
            const bool quoted = p != line;
            p = _strchr_n (p, quoted ? '"' : ',' , 1) + quoted;
            ERR_IF (p - surf - quoted > max_plen, "increase MAX_PATTERN_BITS not to skip %s", std::string (line, p - line).c_str ());
            const int pi = pbag.to_i (std::make_pair (std::string (surf, p - surf - quoted), -1));
            char *f = const_cast <char*> (_strchr_n (++p, ',', 3));
            *f = '\t'; // POS starts with '\t'
            p = _strchr_n (f, ',', NUM_POS_FIELD);
            const int ti (_tbag.to_i (f, p - f)), fi (_fbag.to_i (f, line + len - f));
            si2ti2fi.resize (pbag.size ());
            si2ti2fi[pi].insert (std::make_pair (ti, fi)); // may not unique
          }
          std::fclose (fp);
        }
        std::fprintf (stderr, "done; %ld words, %ld features\n", si2ti2fi.size (), _fbag.size ());
      }
      const int num_seed = static_cast <int> (pbag.size ());
      std::fprintf (stderr, "registering concatenating chars and symbols as seed patterns...");
      for (int i (0), b (0); chars_[i]; ++i) // seed from numeric / alpha / kana
        for (const char *p = &chars_[i][0]; *p; pbag.to_i (std::make_pair (std::string (p, b), -1)), p += b)
          char_t[unicode (p, b)] = 1 << i;
      for (int i = 0; UC_SYMBOL_RANGE[i][0]; ++i)
        for (int j = UC_SYMBOL_RANGE[i][0]; j <= UC_SYMBOL_RANGE[i][1]; ++j) {
          int k (j), b ((j > 0xffff) + (j > 0x7ff) + (j > 0x7f));
          char c[5] = { "\0\xc0\xe0\xf0"[b] };
          for (c[0] |= k >> (6 * b); b; k >>= 6)
            c[b--] = 0x80 | (k & 0x3f);
          pbag.to_i (std::make_pair (&c[0], -1));
        }
      std::fprintf (stderr, "done.\n");
      ti2c.resize (_tbag.size (), 0);
      pi2sfic.resize (pbag.size ());
      std::fprintf (stderr, "mining patterns from training data...");
      { // notations follow https://aclanthology.org/2023.acl-short.2/
        std::string cs; // sequence of characters
        std::vector <std::pair <size_t, std::string> > ss; // tokens <len(w), t>
        FILE* fp = std::fopen (train.c_str (), "r");
        ERR_IF (! fp, "cannot read from %s\n", train.c_str ());
        for (char line[BUF_SIZE]; std::fgets (line, BUF_SIZE, fp); ) {
          const size_t len = std::strlen (line);
          if (std::strncmp (line, "EOS\n", 4) == 0) {
            for (size_t i (0), j (0), ti (0), ti_prev (0); j < ss.size (); i += ss[j].first, ti_prev = ti, ++j) {
              const long shift (ss[j].first), fi (_fbag.to_i (ss[j].second));
              ERR_IF (shift >> MAX_PATTERN_BITS, "increase MAX_PATTERN_BITS not to skip %s", cs.substr (i, shift).c_str ()); // for empty dict
              for (int k = shift; i + k <= cs.size () && k <= max_plen; k += u8_len (&cs[i + k])) {
                const int pi_max = pbag.size ();
                const int pi  = pbag.to_i (std::make_pair (cs.substr (i, k), -1));
                const int pi_ = pbag.to_i (std::make_pair (cs.substr (i, k), ti_prev));
                pi2sfic.resize (pbag.size ());
                ++pi2sfic[pi][std::make_pair (shift, fi)];
                ++pi2sfic[pi_][std::make_pair (shift, fi)];
                if (pi >= pi_max) break; // skip pattern extension; heuristics
              }
              const char* fs = ss[j].second.c_str ();
              const int n = pbag.to_i (std::make_pair (cs.substr (i, shift), -1));
              ti = _tbag.to_i (fs, _strchr_n (fs, ',', NUM_POS_FIELD) - fs);
              if (n >= num_seed && check_ctype (&cs[i], shift, char_t) != NUM) { // for unseen tokens
                ti2c.resize (_tbag.size (), 0); // fi -> _fbag.size (); bug fix
                ++ti2c[ti];
                const int pi = pbag.to_i (std::make_pair ("", ti_prev));
                const int fi_unk = _fbag.to_i (_tbag.to_s (ti) + ",*,*,*\n");
                pi2sfic.resize (pbag.size ());
                ++pi2sfic[pi][std::make_pair (0, fi_unk)];
              }
            }
            cs.clear ();
            ss.clear ();
          } else { // token
            const char * const p (line), *f (_strchr_n (p, '\t', 1));
            cs += std::string (p, f - p);
            ss.push_back (std::make_pair (f - p, std::string (f, p + len - f)));
          }
        }
        std::fclose (fp);
      }
      std::fprintf (stderr, "done; %ld pattern candidates\n", pbag.size ());
      { // pruning patterns
        ccedar::da <char, int> patterns;
        for (size_t i = 0; i < CP_MAX + 1 + _tbag.size (); ++i)
          _ccnt.push_back (std::make_pair (0, _ccnt.size ()));
        std::fprintf (stderr, "pruning patterns...");
        long max_ti = std::max_element (ti2c.begin (), ti2c.end ()) - ti2c.begin ();
        for (bag_t <std::pair <std::string, int> >::iter it = pbag.begin (); it != pbag.end (); ++it) {
          const std::pair <std::string, int>& p = it->first;
          const std::string& c = p.first;
          int pi (it->second), ti_prev (it->first.second), shift (c.size ()), fi (0), count (0);
          if (pi2sfic[pi].empty ()) { // unseen patterns
            if (pi < num_seed) { // dictionary words
              const std::map <int, int>& ti2fi = si2ti2fi[pi];
              int ti = 0;
              for (std::map <int, int>::const_iterator jt = ti2fi.begin ();
                   jt != ti2fi.end (); ++jt)
                if (ti2c[jt->first] >= ti2c[ti])
                  ti = jt->first;
              fi = ti2fi.find (ti)->second;
            } else if  (check_ctype (c.c_str (), shift, char_t) == NUM)
              fi = _fbag.to_i (std::string (FEAT_NUM) + ",*,*,*\n");
            else if (check_ctype (c.c_str (), shift, char_t) != OTHER)
              fi = _fbag.to_i (_tbag.to_s (max_ti) + "," + c + "," + c  + ",*\n");
            else
              fi = _fbag.to_i (std::string (FEAT_SYMBOL) + ",*,*,*\n");
          } else { // perform pruning for seen patterns
            const std::map <std::pair <int, int>, int>& sfi2c = pi2sfic[pi];
            std::vector <int> s2c (max_plen + 1, 0);
            for (std::map <std::pair <int, int>, int>::const_iterator jt = sfi2c.begin ();
                 jt != sfi2c.end (); ++jt)
              s2c[jt->first.first] += jt->second;
            shift = -std::distance (s2c.rend (), std::max_element (s2c.rbegin (), s2c.rend ())) - 1;
            for (std::map <std::pair <int, int>, int>::const_iterator jt = sfi2c.begin ();
                 jt != sfi2c.end (); ++jt)
              if (jt->first.first == shift && jt->second > count)
                count = jt->second, // used to s2c
                   fi = jt->first.second;
            const pat_info_t* r = 0;
            for (size_t from (0), pos (0); pos < c.size (); ) {
              const int n_ = patterns.traverse (c.c_str (), from, pos, pos + 1);
              if (n_ == ccedar::NO_VALUE) continue;
              if (n_ == ccedar::NO_PATH)  break;
              r = &_pi2sf[n_];
            }
            if (r && shift == r->shift && fi == r->fi) continue;
          }
          const int ctype = check_ctype (c.c_str (), shift, char_t, shift ? ANY : OTHER); // NUM -> OTHER
          // count each character and prev POS for count-based indexing
          for (int i (0), b (0), len (c.size ()); i < len; i += b)
            _ccnt[unicode (&c[i], b)].first += count + 1;
          if (ti_prev != -1)
            _ccnt[CP_MAX + 1 + ti_prev].first += count + 1;
          else // record surface-only patterns for pruning
            patterns.update (c.c_str (), c.size ()) = static_cast <int> (_pi2sf.size ());
          _pi2sf.push_back (pat_info_t (c, p.second, count, shift, ctype, fi));
        }
      }
      std::fprintf (stderr, "done; %ld -> %ld patterns\n", pi2sfic.size (), _pi2sf.size ());
    }
    void write_patterns (const std::string& m) { // output compiled patterns
      std::fprintf (stderr, "building DA trie from patterns..");
      bag_t <std::pair <int, int> > fsbag;
      bag_t <std::string> fbag; // core (sorted, compressed)
      ccedar::da_ da;
      IF_COMPACT (fbag.to_i (",*,*,*\n")); // f0: features for unk (lex)
      IF_NOT_COMPACT (fbag.to_i (std::string (FEAT_UNK) + ",*,*,*\n")); // f0: unk
      fsbag.to_i (std::make_pair (0, 1)); // unk <f0, t1>
      // save c2i
      std::sort (_ccnt.rbegin (), _ccnt.rend () - 1);
      std::vector <uint16_t> c2i (_ccnt.size ());
      for (size_t i = 1; i < _ccnt.size () && _ccnt[i].first; ++i)
        c2i[_ccnt[i].second] = static_cast <uint16_t> (i);
      _write_array (c2i.data (), CP_MAX + 2, m + ".c2i"); // chop POS except BOS
      FILE* writer = std::fopen (m.c_str (), "w");
      std::sort (_pi2sf.rbegin (), _pi2sf.rend ());
      for (std::vector <pat_info_t>::iterator it = _pi2sf.begin (); it != _pi2sf.end (); ++it) { // output pattern
        it->print (writer, _tbag, _fbag);
        const std::string& fs =_fbag.to_s (it->fi);
        const int ti_prev = it->ti_prev;
        size_t pos = _strchr_n (fs.c_str (), ',', NUM_POS_FIELD) - fs.c_str ();
        const int ti = _tbag.to_i (fs.substr (0, pos)); // core
        IF_NOT_COMPACT (pos = 0);                       // lemma -> core + lemma
        const int fi = fbag.to_i (fs.substr (pos));
        const int pi = fsbag.to_i (std::make_pair (fi, ti));
        // save pattern trie
        std::vector <int> pv;
        for (int i (0), b (0), len (it->surf.size ()); i < len; i += b)
          pv.push_back (c2i[unicode (&it->surf[i], b)]);
        if (ti_prev + 1) pv.push_back (c2i[CP_MAX + 1 + ti_prev]);
        union { struct { uint32_t shift : MAX_PATTERN_BITS, ctype : 4, id: 20; bool : 1; }; int r; } s = { { it->shift, it->ctype, static_cast <uint32_t> (pi) } };
        da.update (&pv[0], pv.size ()) = s.r;
      }
      std::fclose (writer);
      _write_array (da.array (), da.size (), m + ".da");
      // save feature strings
      std::vector <size_t> offsets, offsets_;
      writer = std::fopen ((m + ".fs").c_str (), "wb");
      IF_COMPACT (const size_t base_offset = _tbag.serialize (writer, offsets_));
      fbag.serialize (writer, offsets); // (core +) lemma
      std::fclose (writer);
      // save mapping from feature ID to feature strings
      feat_info_t finfo = {0};
      std::vector <feat_info_t> p2f (fsbag.size (), finfo);
      for (size_t pi = 0; pi < fsbag.size (); ++pi) {
        const int fi (fsbag.to_s (pi).first), ti (fsbag.to_s (pi).second);
        p2f[pi].ti = c2i[CP_MAX + 1 + ti];
        p2f[pi].core_feat_len = _tbag.to_s (ti).size ();
        p2f[pi].feat_len = fbag.to_s (fi).size ();
        IF_COMPACT (p2f[pi].core_feat_offset = offsets_[ti]);
        IF_COMPACT (p2f[pi].feat_offset = base_offset + offsets[fi]);
        IF_NOT_COMPACT (p2f[pi].feat_offset = offsets[fi]);
      }
      _write_array (p2f.data (), p2f.size (), m + ".p2f");
      std::fprintf (stderr, "done.\n");
    }
  };
}

int main (int argc, char** argv) {
  std::string m, train;
  std::vector <std::string> dict;
  { // options (minimal)
    extern char *optarg;
    extern int optind;
    for (int opt = 0; (opt = getopt (argc, argv, "m:d:u:")) != -1; )
      switch (opt) {
        case 'm': m = optarg; m += "/patterns"; break;
        case 'd': dict.insert (dict.begin (), optarg); break;
        case 'u': dict.push_back (optarg); break;
      }
    if (optind == argc || m.empty ()) errx (1, "Extract patterns for Jagger from dictionary and training data\nCopyright (c) 2023- Naoki Yoshinaga, All rights reserved.\n\nUsage: %s [-m dir -d dict -u dict] train\n\nOptions:\n -m dir \tdirectory to store patterns\n -d dict\tdictionary in CSV format\n -u user_dict\tuser-defined dictionary in CSV format\n", argv[0]);
    train = argv[optind];
  }
  jagger::pattern_builder builder;
  builder.extract_patterns (train, dict);
  builder.write_patterns (m);
  return 0;
}
