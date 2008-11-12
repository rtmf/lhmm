default : lhmm

#include deps
include configuration
SOURCES:=$(shell find src -name \*.cpp)
OBJECTS:=$(patsubst src/%.cpp,build/%.o,$(SOURCES))
#DEPENDENCIES:=$(patsubst src/%.cpp,deps/%.d,$(SOURCES))

# Default make target
lhmm : $(OBJECTS) $(oglcon)
	#$(oglcon)
	$(CXX) $^ $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

htest : build/dict.o build/hash.o build/htest.o
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

build/htest.o : tests/htest.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -I src -o $@ -c $<

build/%.o : src/%.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

# Build and include dependencies
#deps/%.d: src/%.c
#	$(SHELL) -ec 'mkdir -p `dirname $@`'
#	$(SHELL) -ec '$(CXX) $(CFLAGS) $(CXXFLAGS) -M $< | perl -pe "s!src/(.*)\.o!deps/\1.d!g" > $@'

clean ::
	@-(rm -rf deps build lhmm lhmm.exe ; mkdir build) 2>/dev/null

#TODO - change the lhmm.exe to something in configure perhaps

macosx : LHMM.app/Contents/MacOS/LHMM
LHMM.app/Contents/MacOS/LHMM : lhmm
	mkdir -p LHMM.app/Contents/MacOS
	mkdir -p LHMM.app/Contents/Frameworks
	mkdir -p LHMM.app/Contents/Resources
	cp -r data/ LHMM.app/Contents/Resources/
	cp -r /Library/Frameworks/SDL.framework/ LHMM.app/Contents/Frameworks/
	cp -r /Library/Frameworks/SDL_net.framework/ LHMM.app/Contents/Frameworks/
	cp lhmm LHMM.app/Contents/MacOS/LHMM

