CXXFLAGS = -std=c++11 -g -O0 -Wpedantic -Wall -Wextra -Winit-self -Winline -Wconversion -Weffc++ -Wctor-dtor-privacy -Woverloaded-virtual -Wconversion -Wsign-promo
LDFLAGS = -lm

ALL_SRC = $(wildcard *.cpp)
ALL_OBJ = $(patsubst %.cpp, %.o, ${ALL_SRC})

GEN_OBJ = task.o generator.o task_generator.o

GEN_MAIN = task_generator.o

MAIN_OBJ = $(filter-out ${GEN_MAIN}, ${ALL_OBJ})

all: main taskGenerator

main: ${MAIN_OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

taskGenerator: ${GEN_OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper

clean:
	rm -f *.o

mrproper: clean
	rm -f main
