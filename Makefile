
CFLAGS = -Wall -Werror

PROGS = CPU JobSch

all: $(PROGS)

CPU: CPU.o
	gcc -o CPU CPU.o 

CPU.o: CPU.c globals.h
	gcc $(CFLAGS) -c CPU.c 

JobSch: JobSch.o scheduler.o communications.o 
	gcc -o JobSch  JobSch.o scheduler.o communications.o

JobSch.o:  JobSch.c communications.h scheduler.h globals.h
	gcc $(CFLAGS) -c JobSch.c
	
scheduler.o:  scheduler.c communications.h scheduler.h globals.h
	gcc $(CFLAGS) -c scheduler.c

communications.o:  communications.c communications.h globals.h
	gcc $(CFLAGS) -c communications.c

clean:
	rm -f *.o $(PROGS)
