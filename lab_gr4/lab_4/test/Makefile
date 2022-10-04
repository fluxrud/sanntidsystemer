TARGET = test
CFLAGS =  -lpthread -std=gnu11
LDFLAGS =  -lpthread -std=gnu11
SOURCES = $(shell find -name "*.c")

all: $(TARGET)

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	clang $(LDFLAGS) -o $@ $^
%.o : %.c
	clang $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(TARGET) $(OBJECT)
