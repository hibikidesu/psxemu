#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "psx/psx.h"
#include "utils/logger.h"
#include "tests.h"

int run_Psx(char *path) {
	log_Info("Running psx");
	return psx_Run(path);
}

int main(int argc, char *argv[]) {
	int r = 0;
	if (argc > 1) {
		if (strcmp(argv[1], "test") == 0) {
			r = tests_Run();
		} else {
			r = run_Psx(argv[1]);
		}
	} else {
		r = run_Psx("data/SCPH1001.BIN");
	}
	return r;
}
