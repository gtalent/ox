#include <memphis/memfs.hpp>

using namespace memphis;

int main() {
	const auto size = 1024;
	uint8_t volume[size];
	uint32_t err;
	initFs(volume, size, false);
	MemFs memfs(volume, volume + size, &err);
	return err;
}
