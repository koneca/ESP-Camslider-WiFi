#
# Makefile for the creation of the Camslider test client
#
# by Konrad Grzeca 2016

SRC                            = .
BASE                           = .
OUT                            = out
TARGET   					   = client
SOURCES  					   = $(shell echo *.c)
HEADERS 					   = $(shell echo *.h)

COMPILER                       = gcc
DEFINES                        = -DPOSIX -DGCC
CINCLUDE                       = -I$(SRC)/includes
CFLAGS                         = -Wall $(CINCLUDE) -g -o0s
LIBS                           = # -lpthread -lhwinfo


all: $(TARGET) directories

# create directories

directories:
	@mkdir -p out
    
clean:
	rm -rf out

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) -o $(OUT)/$(TARGET) $(SOURCES) $(LIBS)
    
.PHONY: all clean directories clean

