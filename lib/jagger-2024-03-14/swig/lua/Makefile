CXX=g++

all: lua-bindings

lua-bindings: jagger_wrap.o
	$(CXX) -shared -O2 jagger_wrap.o jagger.o $(LIBS) -llua -o jagger.so

luajit-bindings: jagger_wrap.o
	$(CXX) -shared -O2 jagger_wrap.o jagger.o $(LIBS) -lluajit-5.3 -o jagger.so

.cc.o:
	$(CXX) -DHAVE_CONFIG_H -I../.. -I../../src -fPIC -O2 $(CFLAGS) -c $<

clean:
	rm -f *.o jagger.so
