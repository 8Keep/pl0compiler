OUT_FILE = code_generator.out
STD = c99

all: $(OUT_FILE) vm removeObjectFiles

vm: vm/vm.out

vm/vm.out:
	cd vm/ ; make clean ; make all

$(OUT_FILE): main.o code_generator.o token.o data.o symbol.o
	gcc -o $(OUT_FILE) main.o token.o code_generator.o data.o symbol.o -std=$(STD)

run_cg: all
	cd test/ ; bash run_cg.sh

grade: all
	cd test/ ; bash grader.sh

main.o: main.c
	gcc -c main.c -std=$(STD)

data.o: data.c data.h
	gcc -c data.c -std=$(STD)

code_generator.o: code_generator.c code_generator.h
	gcc -c code_generator.c -std=$(STD)

token.o: token.c token.h
	gcc -c token.c -std=$(STD)

symbol.o: symbol.c symbol.h
	gcc -c symbol.c -std=$(STD)

removeObjectFiles:
	rm -f main.o token.o code_generator.o data.o symbol.o

clean: removeObjectFiles
	rm $(OUT_FILE) vm.out test/io/your_outputs -rf
	cd vm ; make clean
