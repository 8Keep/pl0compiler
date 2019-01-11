all: vm.out

vm.out: main.o vm.o
	gcc -o vm.out main.o vm.o

main.o: main.c
	gcc -c main.c

# Do not remove vm.o
clean:
	rm -f vm.out main.o