SRC_SERVER = server.c
SRC_GNL = gnl.c
SRC_ITOA = itoa.c

TARGET = server

all: $(TARGET)

$(TARGET): $(SRC_SERVER)
	$(CC) $(SRC_SERVER) $(SRC_GNL) $(SRC_ITOA) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
