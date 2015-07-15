SRCS = cson.c ./parser/JSON_parser.c
OBJS = cson.o ./parser/JSON_parser.o

TEST_SRCS = test.c
TEST_OBJS = test.o
TEST_BIN = test

JSON_PARSER_SRCS = json-parser.c
JSON_PARSER_OBJS = json-parser.o
JSON_PARSER_BIN = json-parser


STATIC_LIB_LOCATION = libcson.a
DYNAMIC_LIB_LOCATION = libcson.so

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
