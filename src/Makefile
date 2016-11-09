CXXFLAGS = -std=c++11 -g -O0 -Wpedantic -Wall -Wextra -Winit-self -Winline -Wconversion -Weffc++ -Wctor-dtor-privacy -Woverloaded-virtual -Wconversion -Wsign-promo
TARGET = main
LDFLAGS = -lm

${TARGET}: 
	${CXX} -o $@ $^ ${LDFLAGS} && ./${TARGET}

.PHONY: clean mrproper

clean:	
	rm -f *.o

mrproper: clean
	rm -f ${TARGET}