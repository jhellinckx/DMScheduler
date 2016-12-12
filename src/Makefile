CXXFLAGS = -std=c++11 -g -O0 -Wpedantic -Wall -Wextra -Winit-self -Winline -Wconversion -Weffc++ -Wctor-dtor-privacy -Woverloaded-virtual -Wconversion -Wsign-promo
LDFLAGS = -lm

ALL_SRC = $(wildcard *.cpp)
ALL_OBJ = $(patsubst %.cpp, %.o, ${ALL_SRC})

GEN_MAIN = task_generator.o
SIM_MAIN = sim_dm.o
STUDY_MAIN = study_dm.o

SIM_EXEC = simDM
GEN_EXEC = taskGenerator
STUDY_EXEC = studyDM

SIM_OBJ = $(filter-out ${GEN_MAIN} ${STUDY_MAIN}, ${ALL_OBJ})
GEN_OBJ = $(filter-out ${SIM_MAIN} ${STUDY_MAIN}, ${ALL_OBJ})
STUDY_OBJ = $(filter-out ${SIM_MAIN} ${GEN_MAIN}, ${ALL_OBJ})

all: sim gen study

sim: ${SIM_OBJ}
	${CXX} -o ${SIM_EXEC} $^ ${LDFLAGS}

gen: ${GEN_OBJ}
	${CXX} -o ${GEN_EXEC} $^ ${LDFLAGS}

study: ${STUDY_OBJ}
	${CXX} -o ${STUDY_EXEC} $^ ${LDFLAGS}

.PHONY: clean mrproper

clean:
	rm -f *.o

mrproper: clean
	rm -f ${SIM_EXEC} ${GEN_EXEC} ${STUDY_EXEC}
