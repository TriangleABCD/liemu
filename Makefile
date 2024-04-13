CXX = g++
CXXFLAGS = -O3 -Wall -Wimplicit-fallthrough

SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.hpp)
OBJS=$(patsubst src/%.cpp, obj/%.o, $(SRCS))

liemu: $(OBJS)
	@$(CXX) $(CXXFLAGS) -lm -o $@ $^ $(LDFLAGS)

$(OBJS): obj/%.o: src/%.cpp $(HDRS)
	@mkdir -p $$(dirname $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

run: liemu
	@./liemu

clean:
	@rm -rf liemu obj/

.PHONY: clean run
