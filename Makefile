CC=g++
CXXFLAGS=-g -Wall -std=c++0x
LDLIBS1=-lGL -lGLU -lglut
LDLIBS2=-lprotobuf
LDLIBS3=-lpthread

all:	1_message.pb.cc snake_calculate snake_display
1_message.pb.cc: 1_message.proto
	protoc --cpp_out=. 1_message.proto

snake_calculate:	snake_calculate.cc snake.h 1_message.pb.cc
		$(CC) $(CXXFLAGS) $^ $(LDLIBS2) -o $@
snake_display:      snake_display.cc snake.h 1_message.pb.cc
		$(CC) $(CXXFLAGS) $^ $(LDLIBS1) $(LDLIBS2) $(LDLIBS3) -o $@
clean:	
	rm  -f 1_message.pb.cc 1_message.pb.h snake_calculate snake_display
