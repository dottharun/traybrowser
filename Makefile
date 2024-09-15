SHELL := /bin/sh -xe

CC = clang
CFLAGS = -std=c17 -Wall -Wextra -lraylib
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = src/main.c
OUTDIR = out
OUT = $(OUTDIR)/traybrowser

$(shell mkdir -p $(OUTDIR))

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(FLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OUTDIR)/*

.PHONY: all clean
