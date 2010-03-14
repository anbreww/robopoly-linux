# Test makefile for building latex guide
BASE_NAME = robopolinux
TEXSOURCE = $(BASE_NAME).tex
OUTPUT = $(BASE_NAME).pdf
BIBSOURCE = biblio-robopolinux

pdf:
	pdflatex $(TEXSOURCE) && pdflatex $(TEXSOURCE) && evince $(OUTPUT)

bib:
	pdflatex $(TEXSOURCE) && pdflatex $(TEXSOURCE) && bibtex $(BASE_NAME) && pdflatex $(TEXSOURCE) && evince $(OUTPUT)

clean:
	rm -rf *.bbl *.aux *.blg *.log *.toc
