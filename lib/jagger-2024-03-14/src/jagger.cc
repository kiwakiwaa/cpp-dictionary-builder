// Jagger -- C++ implementation of Pattern-based Japanese Morphological Analyzer
//  $Id: jagger.cc 2070 2024-03-14 07:54:57Z ynaga $
// Copyright (c) 2022 Naoki Yoshinaga <ynaga@iis.u-tokyo.ac.jp>
#include <jagger.h>

namespace jagger {
  class tagger {
  private:
    ccedar::da_  _da;  // there may be cache friendly alignment
    uint16_t*    _c2i; // UTF8 char and BOS -> id
    feat_info_t* _p2f; // pattern id -> feature (info)
    char*        _fs;  // feature strings
    std::vector <std::pair <void*, size_t> > _mmaped;
    void* _read_array (const std::string& fn) {
      int fd = ::open (fn.c_str (), O_RDONLY);
      ERR_IF (fd == -1, "no such file: %s", fn.c_str ());
      const size_t size = ::lseek (fd, 0, SEEK_END); // get size
      ::lseek (fd, 0, SEEK_SET);
      void *data = ::mmap (0, size, PROT_READ, MAP_SHARED, fd, 0);
      ::close (fd);
      _mmaped.push_back (std::make_pair (data, size));
      return data;
    }
  public:
    tagger () : _da (), _c2i (0), _p2f (0), _fs (0), _mmaped () {}
    ~tagger () {
      for (size_t i = 0; i < _mmaped.size (); ++i)
        ::munmap (_mmaped[i].first, _mmaped[i].second);
    }
    void read_model (const std::string& m) { // read patterns
      _da.set_array (_read_array (m + ".da"));
      _c2i = static_cast <uint16_t*> (_read_array (m + ".c2i"));
      _p2f = static_cast <feat_info_t*> (_read_array (m + ".p2f"));
      _fs  = static_cast <char*> (_read_array (m + ".fs"));
    }
    void write_feature (simple_writer& writer, const bool concat, const feat_info_t finfo) const {
      IF_COMPACT (writer.write (&_fs[finfo.core_feat_offset], finfo.core_feat_len));
      if (concat) { // as unknown words
        IF_NOT_COMPACT (writer.write (&_fs[finfo.feat_offset], finfo.core_feat_len));
        writer.write (",*,*,*\n", 7);
      } else
        writer.write (&_fs[finfo.feat_offset], finfo.feat_len);
    }
    template <const bool TAGGING, const bool TTY>
    void run () const {
      union { struct { uint32_t shift : MAX_PATTERN_BITS, ctype : 4, id : 20; bool concat : 1; }; int r; } s_prev = {}, s = {};
      feat_info_t finfo = { _c2i[CP_MAX + 1] }; // BOS
      simple_reader reader;
      simple_writer writer;
      for (;! reader.eob ();) {
        if (*reader.ptr () == '\n') { // EOS
          if (s_prev.r)
            if (TAGGING) write_feature (writer, s_prev.concat, finfo);
          writer.write (TAGGING ? "EOS\n" : "\n", TAGGING ? 4 : 1);
          s.shift = 1;
          s_prev.r = 0; // *
          finfo.ti = _c2i[CP_MAX + 1]; // BOS
          if (TTY) writer.flush (); // line buffering
        } else {
          s.r = _da.longestPatternSearch (reader.ptr (), reader.end (), finfo.ti, _c2i);
          if (! s.shift) s.shift = u8_len (reader.ptr ());
          if (s_prev.r &&  // word that may concat with the future context
              ! (s.concat = (s_prev.ctype == s.ctype && // char type mismatch
                             s_prev.ctype != OTHER &&   // kanji, symbol
                             (s_prev.ctype != KANA || s_prev.shift + s.shift < 18)))) {
            if (TAGGING)
              write_feature (writer, s_prev.concat, finfo);
            else
              writer.write (" ", 1);
          }
          finfo = _p2f[s.id];
          s_prev = s; // *
          writer.write (reader.ptr (), s.shift);
        }
        reader.advance (s.shift);
        if (! TTY && ! writer.writable (1 << MAX_FEATURE_BITS)) writer.flush ();
        if (TTY && reader.eob ()) reader.read ();
        if (! TTY && ! reader.readable (1 << MAX_PATTERN_BITS)) reader.read ();
      }
      if (s_prev.r) {
        if (TAGGING) write_feature (writer, s_prev.concat, finfo);
        writer.write (TAGGING ? "EOS\n" : "\n", TAGGING ? 4 : 1);
      }
    }
  };
}

int main (int argc, char** argv) {
  std::string m (JAGGER_DEFAULT_MODEL "/patterns");
  bool tagging = true;
  { // options (minimal)
    extern char *optarg;
    for (int opt = 0; (opt = getopt (argc, argv, "m:u:wh")) != -1;)
      switch (opt) {
        case 'm': m = optarg; m += "/patterns"; break;
        case 'w': tagging = false; break;
        case 'h': errx (1, "Pattern-based Jappanese Morphological Analyzer\nCopyright (c) 2023- Naoki Yoshinaga, All rights reserved.\n\nUsage: %s [-m dir w] < input\n\nOptions:\n -m dir\tdirectory for compiled patterns (default: " JAGGER_DEFAULT_MODEL ")\n -w\tperform only segmentation\n", argv[0]);
      }
  }
  jagger::tagger jagger;
  jagger.read_model (m);
  if (::isatty (0) == 1) { // interactive IO
    if (tagging) jagger.run <true, true>  (); else jagger.run <false, true>  ();
  } else // batch
    if (tagging) jagger.run <true, false> (); else jagger.run <false, false> ();
  return 0;
}
