SHELL := /bin/sh -xe
MAKEFLAGS += -B

CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wno-unused-label -Wno-unused-parameter -Iexternal -g
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = src/main.c
OUTDIR = out
OUT = $(OUTDIR)/traybrowser
EXTERNALDIR = external

$(shell mkdir -p $(OUTDIR) $(EXTERNALDIR))

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

stb:
	wget -NP $(EXTERNALDIR) https://raw.githubusercontent.com/nothings/stb/master/stb_ds.h

deps: stb

compile-db:
	bear -- make all

clean:
	rm -rf $(OUTDIR)/*

distclean:
	rm -rf $(OUTDIR)/*
	rm -rf $(EXTERNALDIR)/*

.PHONY: all clean deps stb distclean compile-db
