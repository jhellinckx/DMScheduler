TARGET = report.pdf

.PHONY: all

all: ${TARGET}

clean: 
		rm -f *.log *.aux *.out *.d

mrproper: clean
		rm -f ${TARGET}


%.pdf: %.tex
		pdflatex $<
