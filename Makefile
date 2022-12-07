source = MiniSQL.cpp
target = MiniSQL

CXX = g++
CXXFLAG = -Wall -g
ASAN = -fsanitize=address

LIB =

all:
	$(CXX) $(source) $(CXXFLAG) $(ASAN) -o $(target) $(LIB)

clean:
	rm $(target)