CPPFLAGS := -I"."
CXXFLAGS := -std=c++11


ccfiles := $(wildcard *.cc)
ofiles := $(patsubst %.cc,%.o,$(ccfiles))


gitversiontree: $(ofiles)
	g++ $^ -o $@

clean:
	rm -rf $(ofiles)
