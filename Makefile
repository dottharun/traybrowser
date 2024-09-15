SHELL := /bin/sh -xe

CC = clang
FLAGS = -Wall -Wextra

SRC = src/main.c
OUTDIR = out
OUT = $(OUTDIR)/traybrowser

$(shell mkdir -p $(OUTDIR))

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -rf $(OUTDIR)/*

.PHONY: all clean
