TARGET = test
CFLAGS = -g -Wall -fsanitize=address
LDFLAGS = -g -fsanitize=address
SOURCES = $(shell find -name "*.c")

all: $(TARGET)

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(TARGET) $(OBJECTS)
