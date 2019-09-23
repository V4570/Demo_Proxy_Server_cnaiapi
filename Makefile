# Makefile for Comer CNAI Simple TCP API
# Linux Version
DIR = $(CURDIR)
DEFS	= -DLINUX
INC_DIR	= $(DIR)/h
INCLUDE = -I$(INC_DIR)
LD	= ld
CC	= gcc
LIBS	= 
FLAGS   = -c -g

CNAIAPI_LIB = cnaiapi.o

API_DIR = $(DIR)/api
API_SRC = 	appname_to_appnum.c	\
		cname_to_comp.c		\
		await_contact.c		\
		make_contact.c		\
		send_eof.c		\
		cnaiapi_init.c

API_OBJ = $(API_SRC:.c=.o)

APP_DIR = $(DIR)/app
APP_SRC = 	proxy_server.c \
			readln.c

APP_OBJ = $(APP_SRC:.c=.o)
APP_EXE = proxy_server

CFILES = $(API_DIR)/*.c $(APP_DIR)/*.c

apps: $(APP_EXE)

proxy_server: cnaiapi.o proxy_server.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) proxy_server.o readln.o

$(APP_OBJ):
	$(CC) -c $(INCLUDE) $(FLAGS) $(DEFS) $(APP_DIR)/$(@:.o=.c)
$(CNAIAPI_LIB): $(API_OBJ)
	$(LD) -r -o $(CNAIAPI_LIB) $(API_OBJ)
$(API_OBJ):
	$(CC) $(INCLUDE) $(FLAGS) $(DEFS) $(API_DIR)/$(@:.o=.c)

clean:
	\rm -rf *.o core $(CNAIAPI_LIB) $(APP_EXE)

# API Dependancies
appname_to_appnum.o: $(API_DIR)/appname_to_appnum.c $(INC_DIR)/cnaiapi.h
await_contact.o: $(API_DIR)/await_contact.c $(INC_DIR)/cnaiapi.h
cname_to_comp.o: $(API_DIR)/cname_to_comp.c $(INC_DIR)/cnaiapi.h
make_contact.o: $(API_DIR)/make_contact.c $(INC_DIR)/cnaiapi.h
send_eof.o: $(API_DIR)/send_eof.c $(INC_DIR)/cnaiapi.h

# Application Dependancies
proxy_server.o: $(APP_DIR)/proxy_server.c $(INC_DIR)/cnaiapi.h
readln.o: $(APP_DIR)/readln.c $(INC_DIR)/cnaiapi.h