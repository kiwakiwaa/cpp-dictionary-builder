all: perl-bindings ruby-bindings python-bindings lua-bindings

perl-bindings:
	swig -Wall -perl -outdir perl -c++ jdepp.i
	ln -sf ../../src/{timer,classify,kernel,pdep}.cc .
	mv {timer,classify,kernel,pdep}.cc perl
	mv -f jdepp_wrap.cxx perl

python-bindings: 
	swig -Wall -python -builtin -outdir python -c++ jdepp.i
	mv -f jdepp_wrap.cxx python

ruby-bindings:
	swig -Wall -ruby -outdir ruby -c++ jdepp.i
	ln -sf ../../src/{timer,classify,kernel,pdep}.cc .
	mv {timer,classify,kernel,pdep}.cc ruby
	mv -f jdepp_wrap.cxx ruby/jdepp_wrap.cpp

lua-bindings:
	swig -Wall -lua -outdir lua -c++ jdepp.i
	ln -sf ../../src/{timer,classify,kernel,pdep}.cc .
	mv {timer,classify,kernel,pdep}.cc lua
	mv -f jdepp_wrap.cxx lua/jdepp_wrap.cc

clean:
	cd perl   && perl Makefile.PL && make distclean && cd ..
	cd python && python3 setup.py clean --all        && cd ..
	cd ruby   && ruby extconf.rb  && make distclean && cd ..
	cd lua    && make clean && cd ..
	rm -rf */jdepp* */*.cc */src
