all:
	gcc -fPIC -c plugin_ICR10.c -o plugin_ICR10.o
	gcc -shared plugin_ICR10.o -o plugin_ICR10.so

	gcc -fPIC -c plugin_TMD700.c -o plugin_TMD700.o
	gcc -shared plugin_TMD700.o -o plugin_TMD700.so -lm -lD700

	gcc -fPIC -c plugin_PCR100.c -o plugin_PCR100.o
	gcc -shared plugin_PCR100.o -o plugin_PCR100.so -lpcr100

	gcc -fPIC -c plugin_print.c -o plugin_print.o
	gcc -shared plugin_print.o -o plugin_print.so

	gcc -fPIC -c plugin_test.c -o plugin_test.o
	gcc -shared plugin_test.o -o plugin_test.so

clean:
	rm *.o *.so *~

install:
	cp *.so /usr/local/lib/gsat/plugins
