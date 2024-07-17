#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
	FILE* f = popen(argv[1], "r");
	char buf[1024];
	char line[1024];
	int n = 0;

	while (fgets(buf, 1024, f) != NULL) {
		n += strlen(buf);

		if (n > 1024)
			break;

		strcat(line, buf);
	}

	printf("%s", line);

	return 0;
}
