# Sample Makefile
# CC - compiler
# OBJ - compiled source files that should be linked
# COPT - compiler flags
# BIN - binary
CC=gcc
OBJ=server.o dns.o client.o pqueue.o log.o global.o cache.o
COPT=-Wall -Wpedantic -g -lpthread
BIN_PHASE=dns_svr

# Running "make" with no argument will make the first target in the file
all: $(BIN_PHASE) 

# Rules of the form
#     target_to_be_made : dependencies_to_be_up-to-date_first
#     <tab>commands_to_make_target
# (Note that spaces will not work.)

$(BIN_PHASE): main.c $(OBJ)
	$(CC) -o $(BIN_PHASE) main.c $(OBJ) $(COPT)


# Wildcard rule to make any  .o  file,
# given a .c and .h file with the same leading filename component
%.o: %.c %.h
	$(CC) -c $< $(COPT) -g

format:
	clang-format -i *.c *.h

clean:
	rm -rf *.o $(BIN_PHASE) dns_svr.log
