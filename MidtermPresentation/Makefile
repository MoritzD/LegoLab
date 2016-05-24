all: main.tex
	pdflatex $^
	pdflatex $^

.PHONY: clean
clean:
	rm -f *.log *.toc *.nav *.aux *.snm *.vrb *.out *.synctex.gz

.PHONY: cleanall
cleanall: clean
	rm -f *.pdf


