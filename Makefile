all: Diskinfo Disklist Diskget Diskput

clean:
	-rm -rf diskinfo disklist diskget diskput *.o *.exe

Diskinfo:
	gcc diskinfo.c -o diskinfo

Disklist:
	gcc disklist.c -o disklist

Diskget:
	gcc diskget.c -o diskget

Diskput:
	gcc diskput.c -o diskput
