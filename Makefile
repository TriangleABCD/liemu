CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wimplicit-fallthrough -I/usr/include/readline

SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.hpp)
OBJS=$(patsubst src/%.cpp, obj/%.o, $(SRCS))
READLINE_LIB=-L/usr/lib -lreadline

NAME=liemu

INSTS=$(wildcard insts/*.inst)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -lm -o $@ $^ $(READLINE_LIB) $(LDFLAGS)

$(OBJS): obj/%.o: src/%.cpp $(HDRS)
	@mkdir -p $$(dirname $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(NAME) $(INSTS)
	@./$(NAME) $(INSTS)

clean:
	@rm -rf $(NAME) obj/

.PHONY: clean run
