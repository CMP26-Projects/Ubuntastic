build: clean


	gcc -c ds/queue.c

	gcc -c ds/minHeap.c

	gcc -c UI.c

	gcc -c headers.c

	gcc -c clk.c 

	gcc -c scheduler.c 
    
	gcc -c process_generator.c
	
	gcc -c process.c  
	
	gcc clk.o UI.o headers.o  queue.o minHeap.o -o clk.out
	
	gcc process_generator.o UI.o headers.o  queue.o minHeap.o -o gen.out

	gcc scheduler.o  UI.o headers.o  queue.o minHeap.o -lm -o scheduler.out

	gcc process.o UI.o headers.o  queue.o minHeap.o -o process.out
	
	gcc systemTests/test_generator.c -o systemTests/test.out

run: build
	@./gen.out ./systemTests/test-5.txt 1 0


clean:
	@rm -f *.out *.o outputFiles/scheduler.log outputFiles/scheduler.perf systemTests/test_generator.out 
