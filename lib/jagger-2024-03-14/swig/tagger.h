#include <jagger.h>

namespace jagger {
  struct Node {
    struct Node   prev;     // pointer to previous node
    struct Node   next;     // pointer to next node
    std::string   surface;  // surface of morpheme
    std::string   feature;  // features in CSV format
    unsigned int  length;   // length of surface
  };
  
  class Tagger {
    tagger* _tagger;
    std::vector <Node> node;
    Tagger () : _tagger (new tagger) {}
    std::string parse (std::string str, int len = 0) {
      len = len ? len : static_cast <int> (str.size ());
    }
    std::string parseToString (std::string str, int len = 0)
    { return parse (str, len); }
    Node parseToNode (std::string str, int len) {
      
    }
  };
}
