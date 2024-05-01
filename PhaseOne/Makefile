build: clean


	gcc -c src/dataStructures/queue.c
	
	gcc -c src/dataStructures/queue.c

	gcc -c src/dataStructures/minHeap.c

	gcc -c src/SystemComponents/UI.c

	gcc -c src/SystemComponents/headers.c

	gcc -c src/SystemComponents/clk.c 

	gcc -c src/SystemComponents/processUnits.c
	
	gcc -c src/SystemComponents/scheduler.c 
    
	gcc -c src/SystemComponents/process_generator.c
	
	gcc -c src/SystemComponents/process.c  
	
	gcc clk.o UI.o headers.o processUnits.o queue.o minHeap.o -o clk.out
	
	gcc process_generator.o UI.o headers.o processUnits.o queue.o minHeap.o -o process_generator.out

	gcc scheduler.o  UI.o headers.o processUnits.o queue.o minHeap.o -lm -o scheduler.out

	gcc process.o UI.o headers.o processUnits.o queue.o minHeap.o -o process.out
	
	gcc test_generator.c -o test_generator.out

run:
	@./process_generator.out ./processes.txt

clean:
	@rm -f *.out *.o *.log *.perf 


