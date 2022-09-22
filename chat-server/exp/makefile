compiler = g++ -std=c++17
test_dep = test.cpp server.hpp net_stream.hpp
net_stream_dep = net_stream.cpp net_stream.hpp

objects = net_stream.o test.o

server:$(objects)
	$(compiler) $(objects) -o server -lpthread
test: $(test_dep)
	$(compiler) -c test.cpp -o test.o

net_stream.o :$(net_stream_dep)
	$(compiler) -c net_stream.cpp -o net_stream.o
run: 
	./server

clean:
	rm $(objects) server