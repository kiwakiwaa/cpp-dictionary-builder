%module jdepp
#define __attribute__(x)
%{
#undef vwarn
#include "tagger.h"
#include "jagger.h"
%}

#ifdef SWIGPERL
%typemap (typecheck) (char **) {
  if (! SvROK ($input) || SvTYPE (SvRV ($input)) != SVt_PVAV)
    croak ("not a list");
}

%typemap (in) (char **) {
  if (! SvROK ($input) || SvTYPE (SvRV ($input)) != SVt_PVAV)
    croak ("not a list");
  int len = av_len (reinterpret_cast <AV *> (SvRV ($input))) + 1;
  $1 = new char*[len + 1];
  int i = 0;
  for (; i < len; ++i) {
    SV ** aval = av_fetch (reinterpret_cast <AV *> (SvRV ($input)), i, 0);
    $1[i] = reinterpret_cast <char *> (SvPV (*aval, PL_na));
  }
  $1[i] = 0;
}
#endif

#ifdef SWIGPYTHON
%typemap (typecheck) (char **) {
  if (! PyList_Check ($input))
    $1 = 0;
  else
    $1 = 1;
}

%typemap (in) (char **) {
  if (! PyList_Check ($input))
    { PyErr_SetString (PyExc_TypeError, "not a list"); return NULL; }
  int len = PyList_Size ($input);
  $1 = new char*[len + 1];
  int i = 0;
  for (; i < len; ++i) {
    PyObject *po = PyList_GetItem ($input, i);
    if (! PyString_Check (po))
      { PyErr_SetString (PyExc_TypeError, "not a string"); delete [] $1; return NULL; }
    $1[i] = PyString_AsString (po);
  }
  $1[i] = 0;
}
#endif

#ifdef SWIGRUBY
%typemap (typecheck) (char **) {
  Check_Type ($input, T_ARRAY);
}

%typemap (in) (char **) {
  Check_Type ($input, T_ARRAY);
  int len = RARRAY_LEN ($input);
  $1 = new char*[len + 1];
  int i = 0;
  for (i = 0; i < len; ++i) {
    VALUE ro = rb_ary_entry ($input, i);
    Check_Type (ro, T_STRING);
    $1[i]= StringValuePtr (ro);
  }
  $1[i] = 0;
}
#endif

#ifdef SWIGLUA
%typemap (typecheck) (char **) {
  if (! lua_istable (L, $input))
    { std::fprintf (stderr, "not a list\n"); return 0; }
}

%typemap (in) (char **) {
  if (! lua_istable (L, $input))
    { std::fprintf (stderr, "not a list\n"); return 0; }
  int i = 1;
  while (1) {
    lua_rawgeti (L, $input, i);
    if (lua_isnil (L, -1)) { lua_pop (L, 1); break; }
    lua_pop (L, 1);
    ++i;
  }
  int len = i - 1;
  $1 = new char*[len + 1];
  for (i = 0; i < len; ++i) {
    lua_rawgeti (L, $input, i + 1);
    if (! lua_isstring (L, -1))
      { std::fprintf (stderr, "not a string\n"); lua_pop (L, 1); return 0; }
    $1[i] = const_cast <char *> (lua_tostring (L, -1));
    lua_pop (L, 1);
  }
  $1[i] = 0;
}
#endif

%typemap(freearg) (char **) { delete [] $1; }

%include "std_vector.i"
%include "std_string.i"
%include "../config.h"
%include "../src/jagger.h"

%template (vectorb) std::vector <const pdep::chunk_t*>;
%template (vectorm) std::vector <const pdep::token_t*>;

