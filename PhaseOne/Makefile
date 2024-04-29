build:
	@gcc process_generator.c processUnits.c ./DataStructures/queue.c -o process_generator.out
	@gcc clk.c -o clk.out
	@gcc scheduler.c -o scheduler.out -lm
	@gcc process.c -o process.out
	@gcc test_generator.c -o test_generator.out

clean:
	@rm -f *.out  processes.txt

all: clean build

run:
	@./process_generator.out ./processes.txt

runclk:
	@./clk.out


test:
	@gcc process_generator.c -o process_generator.out
	@gcc clk.c -o clk.out
	@gcc scheduler.c -o scheduler.out
	@gcc test_generator.c -o test_generator.out
	@./clk.out
	@./process_generator.out 

testprocessgen:
	# @gcc test_generator.c -o test_generator.o
	@gcc process_generator.c -o process_generator.o
	# @./test_generator.o
	@./process_generator.o ./processes.txt