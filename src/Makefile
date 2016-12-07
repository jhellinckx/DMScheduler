CXXFLAGS = -std=c++11 -g -O0 -Wpedantic -Wall -Wextra -Winit-self -Winline -Wconversion -Weffc++ -Wctor-dtor-privacy -Woverloaded-virtual -Wconversion -Wsign-promo
LDFLAGS = -lm

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, ${SRC})
OBJ_TG = task.o generator.o task_generator.o

all: main taskGenerator

main: ${OBJ}
	${CXX} -o $@ $^ ${LDFLAGS}

taskGenerator: ${OBJ_TG}
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper

clean:
	rm -f *.o

mrproper: clean
	rm -f main
