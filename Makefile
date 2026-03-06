CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wimplicit-fallthrough -I/usr/include/readline

SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.hpp)
OBJS=$(patsubst src/%.cpp, obj/%.o, $(SRCS))
READLINE_LIB=-L/usr/lib -lreadline

NAME=liemu

INST_NAME=fib5

INSTS=emu_bin/$(INST_NAME).bin

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -lm -o $@ $^ $(READLINE_LIB) $(LDFLAGS)

$(OBJS): obj/%.o: src/%.cpp $(HDRS)
	mkdir -p $$(dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(NAME) $(INSTS)
	./$(NAME) $(INSTS)

gdb: $(NAME) $(INSTS)
	gdb --args ./$(NAME) $(INSTS)

clean:
	rm -rf $(NAME) obj/

LOG ?= "."

gitpush:
	git add -A
	git commit -m "$(LOG)"
	git push

.PHONY: clean run gitpush gdb
