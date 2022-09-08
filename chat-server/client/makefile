objects=json_parser.o main.o

compiler= g++ -std=c++17

main_dep = ./main.cpp

json_parser_dep = ./json_parser.cpp ./json_parser.hpp

client:$(objects)
	$(compiler) -o client $(objects) -lpthread

main.o:$(main_dep)
	$(compiler) -c ./main.cpp 

json_parser.o:$(json_parser_dep)
	$(compiler) -c ./json_parser.cpp

clean:
	rm client $(objects);

run:
	./client