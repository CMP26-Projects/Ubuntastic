build: clean

	gcc -c src/SystemComponents/headers.c

	gcc src/SystemComponents/clk.c headers.o -o clk.out
	
	gcc -c src/dataStructures/node.c
	gcc -c src/dataStructures/queue.c
	gcc -c src/dataStructures/linkedList.c
	gcc -c src/dataStructures/minHeap.c

	gcc -c src/SystemComponents/processUnits.c
	
	gcc -c src/SystemComponents/scheduler.c 
    
	gcc -c src/SystemComponents/process_generator.c
	
	gcc process_generator.o node.o headers.o queue.o processUnits.o -o process_generator.out

	gcc scheduler.o headers.o node.o linkedList.o queue.o minHeap.o processUnits.o -lm -o scheduler.out

	gcc src/SystemComponents/process.c headers.o -o process.out

	gcc test_generator.c -o test_generator.out

	@./process_generator.out ./processes.txt 0 2

clean:
	@rm -f *.out *.o *.log *.perf 


