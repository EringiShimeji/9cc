#include "9cc.h"
void debug_token(const char *label) {
	printf("%s: ", label);
	for (Token *tok = token; tok; tok = tok->next) {
		printf("[%.*s] ", tok->len, tok->str);
	}
	println("");
}
