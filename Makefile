.SECONDARY:

CPPFLAGS := -I"."
CXXFLAGS := -std=c++11 -O2


ccfiles := $(wildcard *.cc)
gperffiles := $(wildcard *.gperf)
ofiles := $(patsubst %.cc,%.o,$(ccfiles))
ofiles += $(patsubst %.gperf,%.o,$(gperffiles))


gitversiontree: $(ofiles)
	g++ $^ -o $@

clean:
	rm -rf gitversiontree $(ofiles) $(patsubst %.gperf,%.h,$(gperffiles)) $(patsubst %.gperf,%.cc,$(gperffiles))

utils.o: options.h

%.h: %.cc
	
%.cc: %.gperf
	gperf $< --output-file=$@
	./gperf_header.sh $@ $< $(basename $@).h


PREFIX ?= ~/bin

.PHONY: install
install: gitversiontree
	@mkdir -p $(PREFIX)
	@ln -srf gitversiontree $(PREFIX)/

.PHONY: uninstall
uninstall:
	@rm $(PREFIX)/gitversiontree
