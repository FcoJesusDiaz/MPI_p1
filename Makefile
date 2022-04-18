DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRDEBUG := debug/

CFLAGS := -I$(DIRHEA) -c -Wall
LDLIBS := -lpthread -lrt -lm
CC := mpicc

all : dirs toroidal

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRDEBUG)

debugging:
	$(CC) -I$(DIRHEA) -g $(DIRSRC)manager.c -o $(DIRDEBUG)manager $(LDLIBS)

toroidal: $(DIROBJ)toroidal.o $(DIROBJ)app_init.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

toroidal_test:
	mpirun -n 16 --hostfile mpi_config ./$(DIREXE)toroidal

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRDEBUG) $(DIRHEA)*~ $(DIRSRC)*~