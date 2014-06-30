#include <stdio.h>
#include <string.h>
#include "example.h"
#include "secure_zero.h"
#include "secure_compare.h"

/* includes, and implementations, implementations_count */
typedef struct implementation_t {
	const char *name;
	int (*init)(void);
	void (*fuzz)(void);
	void (*bench)(void);
} implementation_t;

#define make_impl(name) {#name, name##_init, name##_fuzz, name##_bench}

static const implementation_t implementations[] = {
	make_impl(example),
	make_impl(secure_compare),
	make_impl(secure_zero)
};

static size_t implementations_count = (sizeof(implementations) / sizeof(implementation_t));

static int
help(void) {
	if (implementations_count > 1) {
		size_t i;
		printf("usage: util [");
		for (i = 0; i < implementations_count; i++) {
			printf("%s", implementations[i].name);
			if (i < (implementations_count - 1))
				printf(",");
		}
		printf("] [fuzz,bench]\n\n");
	} else {
		printf("usage: util [fuzz,bench]\n\n");
	}
	return 1;
}

int main(int argc, const char *argv[]) {
	const implementation_t *sel = implementations, *end = sel + implementations_count;
	size_t action_arg = 1;

	if (implementations_count == 0) {
		printf("no implementations available\n");
		return 1;
	}

	if (argc < ((implementations_count > 1) ? 3 : 2))
		return help();

	if (implementations_count > 1) {
		while (sel < end) {
			if (strcmp(argv[1], sel->name) == 0)
				break;
			sel++;
		}

		if (sel == end)
			return help();

		action_arg = 2;
	}

	if (sel->init() != 0) {
		printf("%s failed to initialize\n", sel->name);
		return 1;
	}

	if (strcmp(argv[action_arg], "fuzz") == 0)
		sel->fuzz();
	else if (strcmp(argv[action_arg], "bench") == 0)
		sel->bench();
	else
		return help();

	return 0;
}
