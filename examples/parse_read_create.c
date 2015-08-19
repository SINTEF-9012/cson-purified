/*
 * parse_read_create.c
 *
 *  Created on: Jul 16, 2015
 *      Author: vassik
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "cson/cson.h"


int main(int argc, char* argv[]) {


	const char* json_string = "{\"uuid\": \"00a02b93-25d4-4e35-b134-3028a1993c32\", \"command\": {\"default\": {\"value\": \"true\", \"bvalue\": false, \"intvalue\": 10, \"doublevalue\": 10.1}}}";

	cson_parse_opt opt = cson_parse_opt_empty;
	cson_parse_info inf = cson_parse_info_empty;

	cson_value* root = NULL;

	int rc = cson_parse_string(&root, json_string, strlen(json_string), &opt, &inf);

	if (rc != 0) {
		printf("Error code %d (%s)!\n", rc, cson_rc_string(rc));
		return 1;
	}

	cson_object* obj = cson_value_get_object(root);
	if(obj == NULL) {
		printf("root is not an object\n");
		return 1;
	}

	cson_value* command_value = cson_object_get_sub2(obj, ".command.default.value");
	cson_value* command_value_bool = cson_object_get_sub2(obj, ".command.default.bvalue");
	cson_value* command_value_int = cson_object_get_sub2(obj, ".command.default.intvalue");
	cson_value* command_value_double = cson_object_get_sub2(obj, ".command.default.doublevalue");

	if (command_value == NULL) {
		printf("cannot extract command_value\n");
		return 1;
	}

	if (command_value_bool == NULL) {
		printf("cannot extract command_value_bool\n");
		return 1;
	}

	if (command_value_int == NULL) {
		printf("cannot extract command_value_int\n");
		return 1;
	}

	if (command_value_double == NULL) {
		printf("cannot extract command_value_double\n");
		return 1;
	}

	char bool_value = cson_value_get_bool(command_value_bool);
	printf("command_value_bool=%s\n", bool_value == 1 ? "true" : "false");

	char const * value = cson_value_get_cstr(command_value);
	printf("command_value=%s\n", value);

	uint8_t value_int = cson_value_get_integer(command_value_int);
	printf("command_value_int=%d\n", value_int);

	double value_double = cson_value_get_double(command_value_double);
	printf("command_value_int=%f\n", value_double);

	cson_buffer buf = cson_buffer_empty;
	rc = cson_output_buffer(root, &buf, NULL);
	if(rc != 0) {
		printf("Cannot print the object -> Error code %d (%s)!\n", rc, cson_rc_string(rc));
		return 1;
	}
	char* json = (char*) buf.mem;
	printf("json=%s\n", json);
	buf = cson_buffer_empty;
	free(json);

	//printf("%s\n", json_string);
	cson_value_free(root);
	//cson_value_free(command_value);
	return 0;
}
