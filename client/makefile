target = bin/client
cc = g++ -g -std=c++17

SRC= $(wildcard ./src/*.cpp)
OBJ= $(addprefix obj/,$(patsubst %.cpp,%.o,$(notdir $(SRC))))
BIN= ./bin/server

define COM_INS 
@echo "\033[31m 正在编译$@ \033[0m"
@$(cc) -c $< -o $@
endef

all:$(target)
obj/chat.o:$(addprefix ./src/,chat.cpp defs.h)
	$(COM_INS)
obj/connection.o:$(addprefix ./src/,connection.cpp defs.h)
	$(COM_INS)
obj/defs.o:$(addprefix ./src/,defs.cpp json.h)
	$(COM_INS)
obj/json.o:$(addprefix ./src/,json.cpp json.h)
	$(COM_INS)
obj/main.o:$(addprefix ./src/,main.cpp ui.h defs.h)
	$(COM_INS)
obj/ui.o:$(addprefix ./src/,ui.cpp ui.h json.h defs.h utils.h)
	$(COM_INS)
obj/backup.o: $(addprefix ./src/,backup.cpp json.h)
	$(COM_INS)

$(target):$(OBJ)
	@$(cc) -g $^ -o $(target) -lpthread
	@echo "\033[33m 生成完毕\033[0m"
.PHONY clean:
clean:
	@rm $(OBJ)
run:
	@./bin/client
back: 
	zip -r ./back.zip ./*
