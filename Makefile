all: build

build:
ifeq ($(OS),Windows_NT)
	gcc example.c -o example -lgdi32
else
	gcc example.c -o example -lX11
endif
