SRC_DIR=./src
SRC=$(wildcard $(SRC_DIR)/*.cc)
HEADERS=$(wildcard $(SRC_DIR)/.h)
OBJ=$(SRC:.cc=.o)
CXX_FLAGS=-std=c++14 -O3

all: simulator

simulator: ${OBJ} ${HEADERS}
	g++ $^ -o $@ ${CXX_FLAGS}
%.o: %.cc %{HEADERS}
	g++ -c $^ -o $@ ${CXX_FLAGS}


run: simulator
	./simulator

clean:
	rm -rf ${OBJ}
	rm simulator
