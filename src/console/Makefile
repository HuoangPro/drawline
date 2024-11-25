CXXFLAGS += -std=c++14
INCLUDE_DIR = ../../include

all: main.cpp
	g++ ${CXXFLAGS} -o robotdraw main.cpp -I${INCLUDE_DIR}

clean:
	rm -rf robot *.o

.PHONY: all clean