
PIPES=$(wildcard frontend_*)


all:frontend backend

backend: backend.c backend.h utils.h utils.c userLib/users_lib.o
	gcc -o backend backend.c backend.h utils.h utils.c userLib/users_lib.o

frontend: frontend.c frontend.h utils.h utils.c
	gcc -o frontend frontend.c frontend.h utils.h utils.c


clean:
	rm frontend
	rm backend
	rm $(PIPES)
