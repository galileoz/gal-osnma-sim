#################
# run option
#################
TESTVECT_DIR = ./tv/
FILE=./gal.bin

TESTVECT= $(TESTVECT_DIR)configuration_A/13_DEC_2020_GST_09_00_01_fixed.csv
POSITION = 48.8435155,2.4297700,60
SAMP_RATE=2600000

################################
# build option
################################
CC=gcc
ifeq ($(OS), Windows_NT)
CfLAGS=-Wall -Wextra -Wunused -g -o0
LFLAGS=-lm -g 
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
	CfLAGS= -I/usr/local/include/libhackrf -I/usr/lib/x86_64-linux-gnu -Wall  -g -o0 -Ofast
	LFLAGS= -L/usr/local/lib -lusb-1.0 -lm -lhackrf -lpthread -g 	
	endif
	ifeq ($(UNAME_S),Darwin)
	CfLAGS=-I/opt/local/include/ -Wall  -g -Ofast -o0
	LFLAGS= -L/opt/local/lib/ -lm -lusb-1.0 -lhackrf -g
	endif
endif

SRC_DIR = src/
OBJ_DIR = obj/
INCLUDE_DIR = include/
SRC_LIST = $(wildcard $(SRC_DIR)*.c)
OBJ_LIST =$(subst $(SRC_DIR),$(OBJ_DIR), $(SRC_LIST:.c=.o))

EXEC=gal-osnma-sim

all: $(EXEC)

	
$(EXEC):  $(OBJ_LIST)
	$(CC) $(CfLAGS) -I$(INCLUDE_DIR) -o $@  $^ main.c $(LFLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p obj
	$(CC) $(CfLAGS) -I$(INCLUDE_DIR) -o $@ -c $<


run: $(EXEC)
ifeq ($(OS), Windows_NT)
	./$< -l $(POSITION) -v $(TESTVECT) 
else
	./$< -l $(POSITION) -s $(SAMP_RATE) -v $(TESTVECT) -b 8 -k
endif
 

clean:
	rm -rf  *.o
	rm -rf $(OBJ_DIR)*.o

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean doc mrproper hackrf echo

