#LD_FLAGS := ""
CC_FLAGS := -std=c++11

CPP_FILES := $(shell find Source -name '*.cpp')
OBJ_FILES := $(CPP_FILES:.cpp=.o)

main: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

obj: $(OBJ_FILES)

%.o: %.cpp
	g++ -c $(CC_FLAGS) -o $@ $< -ISource
