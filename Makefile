CXX = g++
CXXFLAGS = -O3 -Wall -Wimplicit-fallthrough

SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.hpp)
OBJS=$(patsubst src/%.cpp, obj/%.o, $(SRCS))

NAME=liemu

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -lm -o $@ $^ $(LDFLAGS)

$(OBJS): obj/%.o: src/%.cpp $(HDRS)
	@mkdir -p $$(dirname $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(NAME)
	@./$(NAME)

clean:
	@rm -rf $(NAME) obj/

.PHONY: clean run
