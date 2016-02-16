#
# Makefile for the creation of the Camslider test client
#
# by Konrad Grzeca 2016

SRC                            = .
BASE                           = .
OUT                            = out/
MAPFILE                        = eftc0.map
MAPFOLDER                      = ~/eft/mapfile

COMPILER                       = gcc
DEFINES                        = -DPOSIX -DGCC
CINCLUDE                       = -I$(SRC)/includes
CFLAGS                         = -Wall $(CINCLUDE)
LIBS                           = # -lpthread -lhwinfo


all: directories dbg.o list.o client

# create directories

directories:
	@mkdir -p out
    
clean:
	rm -rf out

dbg.o: dbg.c
	@$(CC) -c $(CINCLUDE) $(CFLAGS) $(LIBS) $< -o $(OUT)/$@
	@echo $<

list.o: list.c
	@$(CC) -c $(CINCLUDE) $(CFLAGS) $(LIBS) $< -o $(OUT)/$@
	@echo $<
	
client:  $(SRC)/test-client.c $(OUT)/dbg.o $(OUT)/list.o
	@$(CC)  $(CFLAGS) $(LIBS) $< -o $(OUT)/$@ $(OUT)/dbg.o
	@echo $<

