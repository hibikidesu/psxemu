#include "psx/psx.h"
#include "utils/logger.h"

int main(int argc, char *argv[]) {
	log_Info("Running psx");
	return psx_Run("data/SCPH1001.BIN");
}
