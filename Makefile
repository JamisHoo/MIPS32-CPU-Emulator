SOURCES = $(shell ls src/*.cc)
HEADERS = $(shell ls src/*.h)

.PHONY: all
all: cpu cpu.html

CXXFLAGS = -std=c++11 -O2

cpu: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

cpu.html: $(SOURCES) $(HEADERS)
	em++ $(CXXFLAGS) $(SOURCES) --embed-file OS_image/disk0 --embed-file OS_image/rom -s TOTAL_MEMORY=$$((64*1024*1024)) -o $@

.PHONY: clean
clean:
	$(RM) cpu cpu.js cpu.html cpu.html.mem cpu.data
