# sth - shader to c header #
make:
	gcc -o sth sth.c
install: make
	cp -f sth /usr/local/bin/sth
uninstall:
	rm -f /usr/local/bin/sth
backup:
	cp ./ ../shader-to-header.bak
