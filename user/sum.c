#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(void) {
	char buf[128] = {};

	int first_pos = -1, second_pos = -1, pos = 0, n;
	int was_space = 0;
	while ((n = read(0, buf + pos, 1)) > 0) {
		if (pos >= 127) {
			printf("Invalid input: buffer overflow\n");
			return 1;
		}

		if (buf[pos] == ' ' && first_pos == -1) {
			continue;
		}

		if (buf[pos] == '\n') {
			buf[pos] = '\0';
			break;
		}

		if ('0' <= buf[pos] && buf[pos] <= '9') {
			if (first_pos == -1) first_pos = pos;
			else if (was_space == 1 && second_pos == -1) second_pos = pos;
			pos += 1;
			continue;
		}

		if (buf[pos] == ' ') {
			was_space = 1;
			pos += 1;
			continue;
		}

		printf("Invalid input: Invalid symbols\n");
		return 1;
	}

	if (n < 0) {
		printf("read error\n");
		return 1;
	}

	if (first_pos == -1) {
		printf("Invalid input: No first number\n");
		return 1;
	}

	if (was_space == 0) {
		printf("Invalid input: No space\n");
		return 1;
	}

	if (second_pos == -1) {
		printf("Invalid input: No second number\n");
		return 1;
	}

	int a = atoi(buf + first_pos);
	int b = atoi(buf + second_pos);
	int c = add(a, b);
	printf("%d\n", c);
	return 0;
}
