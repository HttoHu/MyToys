objects=main.o net.o trans.o
# .cpp dependancy
main_dep=./src/main.cpp ./includes/net.hpp ./includes/def.hpp

net_dep=./src/net.cpp ./includes/net.hpp

trans_dep=./src/trans.cpp ./includes/def.hpp
# ==============Finshed ========================
compiler= g++ -std=c++11 
server: $(objects)
	$(compiler) -o server $(objects) -lpthread
	echo "All right!"

trans.o: $(trans_dep)
	echo "compiling trans.cpp"
	$(compiler) -c ./src/trans.cpp

main.o: $(main_dep)
	echo "compiling main.cpp"
	$(compiler) -c ./src/main.cpp

net.o: $(net_dep)
	echo "compiling net.cpp"
	$(compiler) -c ./src/net.cpp

.PHONY:clean
clean:
	rm server $(objects);