CC=g++
CFLAGS=-c -Wall -std=c++20

SOURCES=ipkcpd.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ipkcpd

.PHONY: all clean run

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

run:
	./$(EXECUTABLE)