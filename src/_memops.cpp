#include "_memops.hpp"

namespace memphis {

void memcpy(void *src, void *dest, int size) {
	char *srcBuf = (char*) src;
	char *dstBuf = (char*) dest;
	for (int i = 0; i < size; i++) {
		dstBuf[i] = (char) srcBuf[i];
	}
}

void memset(void *ptr, char val, int size) {
	char *buf = (char*) ptr;
	for (int i = 0; i < size; i++) {
		buf[i] = val;
	}
}

}
