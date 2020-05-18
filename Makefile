SRC_DIR=src
LIB_DIR=lib
LIB=$(wildcard $(LIB_DIR)/*.cc)
LIB_OBJ=$(LIB:.cc=.o)
SRC=$(wildcard $(SRC_DIR)/*.cc)
SRC_OBJ=$(SRC:.cc=.o)
#MACRO=-DVERBOSE
CXX_FLAGS=-std=c++14 -O3 ${MACRO}

TMSL=${SRC_DIR}/tmsl.cc
TMSL_BP=${SRC_DIR}/tmsl.cc

all: tmsl tmsl_bp 

tmsl: ${SRC_DIR}/tmsl.o ${LIB_OBJ}
	g++ $^ -o $@ ${CXX_FLAGS} -I${LIB_DIR}
tmsl_bp: ${SRC_DIR}/tmsl_bp.o ${LIB_OBJ}
	g++ $^ -o $@ ${CXX_FLAGS} -I${LIB_DIR}
%.o: %.cc
	g++ -c $^ -o $@ ${CXX_FLAGS} -I${LIB_DIR}

clean:
	rm -rf ${LIB_OBJ} ${SRC_OBJ}
	rm tmsl tmsl_bp