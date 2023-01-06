SOURCES = src/*.cc

build:
	g++ -I include/ -g -Wall -lpthread -o run $(SOURCES) `pkg-config --cflags --libs libserial`
