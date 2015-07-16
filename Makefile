SRCS = cson.c ./parser/JSON_parser.c
OBJS = cson.o ./parser/JSON_parser.o

TEST_SRCS = test.c
TEST_OBJS = test.o
TEST_BIN = test

JSON_PARSER_SRCS = json-parser.c
JSON_PARSER_OBJS = json-parser.o
JSON_PARSER_BIN = json-parser

EXAMPLE_SRCS = examples/parse_read_create.c
EXAMPLE_OBJS = examples/parse_read_create.o
EXAMPLE_BIN = examples/parse_read_create

STATIC_LIB_LOCATION = libcson.a
DYNAMIC_LIB_LOCATION = libcson.so

INSTALL_LIB_DIR = /usr/local/lib
INSTALL_INCLUDE_DIR = /usr/local/include
CSON_DIR = cson

#CROSS_COMPILE := /home/vassik/arrowhead/demo2/crossenv/gcc-linaro-arm-linux/bin/arm-linux-gnueabi-
CROSS_COMPILE :=

GCC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

%.o : %.c
	$(GCC) -pedantic -Wall -Werror -fPIC -g -std=c99 -I. -I./include -UNDEBUG -DDEBUG=1 -c -o $@ $<

all : staticlib dynamiclib
	
staticlib : $(OBJS)
	$(AR) crs $(STATIC_LIB_LOCATION) $(OBJS)

dynamiclib : $(OBJS)
	$(GCC) -shared -rdynamic -o $(DYNAMIC_LIB_LOCATION) $(OBJS)

install: staticlib dynamiclib
	install -d $(INSTALL_INCLUDE_DIR)/$(CSON_DIR)
	install $(STATIC_LIB_LOCATION) $(INSTALL_LIB_DIR)
	install $(DYNAMIC_LIB_LOCATION) $(INSTALL_LIB_DIR)
	cp -r ./include/wh/cson/*.h $(INSTALL_INCLUDE_DIR)/$(CSON_DIR)
	ldconfig

uninstall:
	rm -rf $(INSTALL_INCLUDE_DIR)/$(CSON_DIR)
	rm -rf $(INSTALL_LIB_DIR)/$(STATIC_LIB_LOCATION)
	rm -rf $(INSTALL_LIB_DIR)/$(DYNAMIC_LIB_LOCATION)
	
example: $(EXAMPLE_OBJS)
	$(GCC) -o $(EXAMPLE_BIN) -pedantic -Wall -Werror -fPIC -g $(EXAMPLE_OBJS) -lcson

clean :
	rm -rf $(OBJS) $(DYNAMIC_LIB_LOCATION) $(STATIC_LIB_LOCATION) $(TEST_OBJS) $(TEST_BIN) $(JSON_PARSER_OBJS) $(JSON_PARSER_BIN)

testbin : $(TEST_OBJS)
	$(GCC) -o $(TEST_BIN) -pedantic -Wall -Werror -fPIC -g $(TEST_OBJS)

jsonparserbin : $(JSON_PARSER_OBJS)
	$(GCC) -o $(JSON_PARSER_BIN) -pedantic -Wall -Werror -fPIC -g $(JSON_PARSER_OBJS) $(STATIC_LIB_LOCATION)

test: testbin jsonparserbin
	@echo "Running MUST-PASS tests..."
	@echo -e "\t./$(TEST_BIN)"; \
		./$(TEST_BIN) 1>/dev/null 2>&1 \
		|| { x=$$?; echo "Basic test app failed with code [$$x]."; exit $$x; }
	@for i in json/pass-*.json checker/test/pass*.json; do \
		echo -e "\t$$i"; \
		./$(JSON_PARSER_BIN) -f $$i 1>/dev/null || exit; \
	done
	@echo "Running MUST-FAIL tests..."
	@for i in json/fail-*.json checker/test/fail*.json; do \
		echo -ne "\t$$i"; \
		./$(JSON_PARSER_BIN) -f $$i 1>/dev/null 2>&1 && exit 100; \
		echo " successfully failed."; \
	done; \
	true

testrun:
	@echo "Running MUST-PASS tests..."
	@echo -e "\t./$(TEST_BIN)"; \
		./$(TEST_BIN) 1>/dev/null 2>&1 \
		|| { x=$$?; echo "Basic test app failed with code [$$x]."; exit $$x; }
	@for i in json/pass-*.json checker/test/pass*.json; do \
		echo -e "\t$$i"; \
		./$(JSON_PARSER_BIN) -f $$i 1>/dev/null || exit; \
	done
	@echo "Running MUST-FAIL tests..."
	@for i in json/fail-*.json checker/test/fail*.json; do \
		echo -ne "\t$$i"; \
		./$(JSON_PARSER_BIN) -f $$i 1>/dev/null 2>&1 && exit 100; \
		echo " successfully failed."; \
	done; \
	true
