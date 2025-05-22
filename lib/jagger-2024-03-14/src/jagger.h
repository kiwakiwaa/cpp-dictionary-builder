// Jagger -- C++ implementation of Pattern-based Japanese Morphological Analyzer
//  $Id: jagger.h 2070 2024-03-14 07:54:57Z ynaga $
// Copyright (c) 2022 Naoki Yoshinaga <ynaga@iis.u-tokyo.ac.jp>
#ifndef JAGGER_H
#define JAGGER_H
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <ccedar_core.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define ERR_IF(condition, format, ...) \
  if (condition) errx (1, __FILE__ " [%d]: " format, __LINE__, __VA_ARGS__)

#ifdef USE_COMPACT_DICT
#define IF_COMPACT(e) e
#define IF_NOT_COMPACT(e)
#else
#define IF_COMPACT(e)
#define IF_NOT_COMPACT(e) e
#endif

static const size_t MAX_KEY_BITS     = 14; // also max POS ID

// compute length of UTF8 character from its first byte
// find UTF8 length from its first byte
static inline int8_t u8_len (const char *p)
{ return "\1\1\1\1\1\1\1\1\1\1\1\1\2\2\3\4"[(*p & 0xff) >> 4]; }

// convert UTF-8 character to code point
__attribute__((always_inline))
static inline int unicode (const char* p, int& b) { // decode
  const int p0 (*p & 0xff);
  if ((p0 & 0xf0) == 0xe0) { b = 3; return ((p0 & 0xf) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f); }
  if (p0 < 0x80)           { b = 1; return p0; }
  if ((p0 & 0xe0) == 0xc0) { b = 2; return ((p0 & 0x1f) << 6) | (p[1] & 0x3f); }
  b = 4;
  return ((p0 & 0x7) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | (p[3] & 0x3f);
}

namespace jagger {
  static const size_t BUF_SIZE = 1 << 17;
  static const size_t CP_MAX   = 0x10ffff;  // limit of unicode code point
  static const size_t MAX_PATTERN_BITS = 7; // bits of pattern length (surface)
  static const size_t MAX_FEATURE_BITS = 9; // bits of feature string
  enum { NUM = 1 << 0, ALPHA = 1 << 1, KANA = 1 << 2, OTHER = 0, ANY = 7 };
  struct feat_info_t { // feature infomation retrieved via value id
    uint32_t ti            : MAX_KEY_BITS;     // 14
    uint32_t core_feat_len : MAX_FEATURE_BITS; //  9 (anyway needed for unk word)
    uint32_t feat_len      : MAX_FEATURE_BITS; //  9 (lex only for compact)
    IF_COMPACT (uint32_t core_feat_offset : 18);
    uint32_t feat_offset   : 28; // (lex only for compact)
  };
  struct pat_info_t {
    std::string surf; // surface
    int ti_prev;      // prev pos id
    int count;
    union { int r; struct { uint32_t shift : 8, ctype : 4, fi : 20; }; };
    pat_info_t (const std::string &surf_, int ti_prev_, int count_, int shift_, int ctype_, int fi_) : surf (surf_), ti_prev (ti_prev_), count (count_), r (0) { shift = shift_, ctype = ctype_, fi = fi_; }
    bool operator< (const pat_info_t& a) const
    { return count < a.count || (count == a.count && surf < a.surf); }
    template <typename T>
    void print (FILE* writer, const T& tbag, const T& fbag) const
    { std::fprintf (writer, "%d\t%s%s\t%d\t%d%s", count, surf.c_str (), ti_prev == -1 ? "\t" : tbag.to_s (ti_prev).c_str (), shift, ctype, fbag.to_s (fi).c_str ()); }
  };
  template <typename T>
  class bag_t { // assign unique id to key
  public:
    typedef typename std::map <T, int>::iterator iter;
    bag_t () : _key2id (), _id2key () {}
    ~bag_t () {}
    size_t size () const { return _id2key.size (); }
    size_t to_i (const char* f, size_t len) { return to_i (T (f, len)); } // string
    size_t to_i (const T& f) {
      std::pair <iter, bool> itb = _key2id.insert (std::make_pair (f, _id2key.size ()));
      if (itb.second) _id2key.push_back (&itb.first->first);
      return static_cast <size_t> (itb.first->second);
    }
    const T& to_s (const size_t fi) const { return *_id2key[fi]; }
    iter begin () { return _key2id.begin (); }
    iter end   () { return _key2id.end (); }
    iter find (const T& s) { return _key2id.find (s); }
    size_t serialize (FILE* fp, std::vector <size_t>& offsets, size_t size = 0) const {
      for (typename std::vector <const T*>::const_iterator it = _id2key.begin (); it != _id2key.end (); ++it) {
        offsets.push_back (size);
        size += std::fwrite ((*it)->c_str (), sizeof (char), (*it)->size(), fp);
      }
      return size;
    }
  private:
    std::map <T, int>      _key2id;
    std::vector <const T*> _id2key;
  };
  class simple_reader {
  private:
    char _buf[BUF_SIZE], *_p, *_q, * const _end;
  public:
    simple_reader () : _buf (), _p (_buf), _q (_p), _end (_buf + BUF_SIZE) { read (); }
    void read () {
      std::memmove (_buf, _p, _q - _p);
      _q -= _p - _buf;
      _p = _buf;
      _q += ::read (0, _q, _end - _q);
    }
    const char* ptr () const { return _p; }
    const char* const end () const { return _q; }
    bool eob () const { return _p == _q; }
    void advance (const int shift) { _p += shift; }
    bool readable (const size_t min) const { return _p + min <= _q; }
  };
  class simple_writer {
  private:
    char _buf[BUF_SIZE], *_p, * const _end;
  public:
    simple_writer () : _buf (), _p (_buf), _end (_buf + BUF_SIZE) {}
    ~simple_writer () { flush (); }
    bool writable (const size_t min) const { return _p + min <= _end; }
    void flush () { _p -= ::write (1, _buf, static_cast <size_t> (_p - _buf)); }
    void write (const char* s, const size_t len) {
      std::memcpy (_p, s, len);
      _p += len;
    }
  };
}

namespace ccedar {
  class da_ : public ccedar::da <int, int, MAX_KEY_BITS> {
  public:
    struct u8_feeder { // feed one UTF-8 character by one while mapping codes
      const char *p, * const p_end;
      u8_feeder (const char *p_, const char *p_end_) : p (p_), p_end (p_end_) {}
      int  read (int &b) const { return p == p_end ? 0 : unicode (p, b); }
      void advance (const int b) { p += b; }
    };
    __attribute__((always_inline))
    int longestPatternSearch (const char* key, const char* const end, int fi_prev, const uint16_t* const c2i, size_t from = 0) const {
      size_t from_ (0), pos (0);
      int n (0), i (0), b (0);
      for (u8_feeder f (key, end); (i = c2i[f.read (b)]); f.advance (b)) {
        const int n_ = traverse (&i, from, pos = 0, 1);
        if (n_ == NO_VALUE) continue;
        if (n_ == NO_PATH)  break;
        from_ = from;
        n = n_;
      }
      // ad-hock matching at the moment; it prefers POS-ending patterns
      if (! fi_prev) return n;
      const node* const array_ = array ();
      for (size_t from__ (0); ; from = array_[from].check) { // hopefully, in the cache
        const int n_ = traverse (&fi_prev, from__ = from, pos = 0, 1);
        if (n_ != NO_VALUE && n_ != NO_PATH) return n_;
        if (from == from_)  return n;
      }
    }
  };
}
#endif
