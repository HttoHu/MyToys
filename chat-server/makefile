target = bin/server
cc = g++ -std=c++17

SRC= $(wildcard ./src/*.cpp)
OBJ= $(addprefix obj/,$(patsubst %.cpp,%.o,$(notdir $(SRC))))
BIN= ./bin/server

define COM_INS 
@echo "\033[31m 正在编译$@ \033[0m"
@$(cc) -c $< -o $@
endef

all:$(target)

obj/defs.o:$(addprefix ./src/,defs.cpp)
	$(COM_INS)
obj/main.o:$(addprefix ./src/,main.cpp) $(addprefix includes/,server.h json.h)
	$(COM_INS)
obj/server.o:$(addprefix ./src/,server.cpp) $(addprefix includes/,server.h)
	$(COM_INS)
obj/utils.o:$(addprefix ./src/,utils.cpp)
	$(COM_INS)
obj/user.o:$(addprefix ./src/,user.cpp) $(addprefix includes/,server.h json.h)
	$(COM_INS)
obj/json.o:$(addprefix ./src/,json.cpp)
	$(COM_INS)
obj/test.o:$(addprefix ./src/,test.cpp) $(addprefix includes/,server.h)
	$(COM_INS)
$(target):$(OBJ)
	@$(cc) -g $^ -o $(target) -lpthread
	@echo "\033[33m 生成完毕\033[0m"
.PHONY clean:
clean:
	@rm $(OBJ)
run:
	@./bin/server ./config.json

back: 
	zip -r ./back.zip ./*
