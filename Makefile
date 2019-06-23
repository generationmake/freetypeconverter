CFLAGS = -g -Wall -Wextra `pkg-config --cflags freetype2`
LDFLAGS = `pkg-config --libs freetype2`

all: freetypeconverter.o
	cc -o freetypeconverter freetypeconverter.o $(LDFLAGS)

freetypeconverter.o: freetypeconverter.c
	cc $(CFLAGS) -c freetypeconverter.c

clean:
	rm -f freetypeconverter freetypeconverter.o
