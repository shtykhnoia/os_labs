CXX = gcc
CXXFLAGS = -std=c11 -fPIC -shared -O2
APPFLAGS = -std=c11 -O2
LDFLAGS = -ldl

# RPATH настройки
RPATH = -Wl,-rpath,\$$ORIGIN

# Библиотеки
LIBS = libimpl1.so libimpl2.so

all: $(LIBS) program1 program2

libimpl1.so: impl1.c
	$(CXX) $(CXXFLAGS) -o $@ $< -lm

libimpl2.so: impl2.c
	$(CXX) $(CXXFLAGS) -o $@ $< -lm

program1: program1.c
	$(CXX) $(APPFLAGS) -o $@ program1.c -L. -limpl1 $(RPATH) -lm

program2: program2.c
	$(CXX) $(APPFLAGS) -o $@ program2.c -L. $(LDFLAGS) $(RPATH)

clean:
	rm -f $(LIBS) program1 program2