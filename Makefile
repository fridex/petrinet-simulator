# Makefile for ICP project Petri net simulator and editor.
# 2012
# Authors:
# 		Miroslav Lisik		<xlisik00@stud.fit.vutbr.cz>
# 		Fridolin Pokorny	<fridex.devel@gmail.com>

PACKNAME = project

.PHONY: clean pack run runserver doxygen cleandoc

all: pn2012 server2012

doc: doxygen

pn2012:
	@echo ">>>> Building client"
	@cd src/pn2012 && qmake -makefile
	@make -C src/pn2012

server2012:
	@echo ">>>> Building server"
	@cd src/server2012 && qmake -makefile
	@make -C src/server2012

run: pn2012
	@echo ">>>> Running client"
	./src/pn2012/pn2012

runserver: server2012
	@echo ">>>> Running server"
	./src/server2012/server2012 --userdb 'examples/userdb.txt' -p 44444 --projectdb 'examples/projectdb'

doxygen:
	doxygen doxygen.conf

pack:
	@cd .. && tar -zcf $(PACKNAME).tar.gz $(PACKNAME)

cleandoc:
	rm -rf doc/*

clean: cleandoc
	make -C src/server2012 distclean; make -C src/pn2012 distclean; true


help:
	@echo "Makefile for ICP project Petri net simulator and editor."
	@echo "2012"
	@echo "Authors:"
	@echo "	Miroslav Lisik		<xlisik00@stud.fit.vutbr.cz>"
	@echo "	Fridolin Pokorny	<fridex.devel@gmail.com>"
	@echo
	@echo "Options:"
	@echo "	make			- build project"
	@echo "	make run		- run client"
	@echo "	make runserver		- run client server"
	@echo "	make doxygen		- run client server"
	@echo "	make help		- print this simple help"
	@echo "	make pack		- create GZip tar archive"

