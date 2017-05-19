PWD := ${shell pwd}

all: main
clean:
	rm -rf *.po *.so *.o main

api-x.po: api-x.c
	gcc -Wall -Werror -g -fPIC -c -o api-x.po api-x.c

libapi-x.so: api-x.po
	gcc -Wall -Werror -g -shared -o libapi-x.so api-x.po

api-y.po: api-y.c
	gcc -Wall -Werror -g -fPIC -c -o api-y.po api-y.c

libapi-y.so: api-y.po
	gcc -Wall -Werror -g -shared -o libapi-y.so api-y.po

pure-interface.po: pure-interface.c
	gcc -Wall -Werror -g -fPIC -c -o pure-interface.po pure-interface.c

libpure-interface.so: pure-interface.po libapi-x.so libapi-y.so
	gcc -Wall -Werror -g -shared -o libpure-interface.so pure-interface.po -ldl

main.o: main.c
	gcc -Wall -Werror -g -c -o main.o main.c

main: main.o libpure-interface.so
	gcc main.o -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface -o main

.PHONY: all clean

