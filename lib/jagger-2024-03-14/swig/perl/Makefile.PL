use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

# add SSE4.2 CXXFLAGS; dirty bit
open (IN, "../../config.h");
close (IN);

WriteMakefile
  ('NAME'    => 'jagger',
   'CC'      => 'g++',
   'LD'      => 'g++',
   'DEFINE'  => '-DHAVE_CONFIG_H',
   'INC'     => '-I../.. -I../../src',
   'OBJECT'  => 'jagger_wrap.o jagger.o'
);
