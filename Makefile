DATE = $(shell date +%Y%m%d)
NAME = robopolibs-$(DATE)

# compile latex source
package:
	make clean -C ./example
	make pdfquiet -C ./doc
	rm -rf -- ./export
	mkdir ./export
	cp ./doc/robopolinux.pdf ./export/robopoly-linux.pdf
	cp ./example/* ./export/
	tar cvzf ./$(NAME).tar.gz ./export
	rm -rf -- ./export

# until documentation is ready, package without pdf
nodoc:
	make clean -C ./example
	rm -rf -- ./export
	mkdir ./export
	cp ./example/* ./export/
	tar cvzf ./$(NAME).tar.gz ./export
	rm -rf -- ./export

upload:
	scp ./$(NAME).tar.gz andrew@watsons.ch:/var/www/robopoly/
	echo "Latest build available at http://watsons.ch/robopoly/$(NAME).tar.gz"
