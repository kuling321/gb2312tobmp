objects = test.o gb2312toBMP.o
a : $(objects)
	cc -o a $(objects)
    
gb2312toBMP.o : gb2312toBMP.c gb2312toBMP.h
	cc -c gb2312toBMP.c
    
test.o : test.c gb2312toBMP.h
	cc -c test.c
    
clean :
	rm a $(objects)