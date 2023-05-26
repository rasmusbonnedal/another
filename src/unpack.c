
#include <stdint.h>

struct UnpackCtx {
	int size;
	uint32_t crc;
	uint32_t bits;
	uint8_t *dst;
	const uint8_t *src;
};

static uint32_t READ_BE_UINT32(const uint8_t *b) {
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

static int nextBit(struct UnpackCtx *uc) {
	int carry = (uc->bits & 1) != 0;
	uc->bits >>= 1;
	if (uc->bits == 0) { // getnextlwd
		uc->bits = READ_BE_UINT32(uc->src); uc->src -= 4;
		uc->crc ^= uc->bits;
		carry = (uc->bits & 1) != 0;
		uc->bits = (1 << 31) | (uc->bits >> 1);
	}
	return carry;
}

static int getBits(struct UnpackCtx *uc, int count) { // rdd1bits
	int bits = 0;
	for (int i = 0; i < count; ++i) {
		bits |= nextBit(uc) << (count - 1 - i);
	}
	return bits;
}

static void copyLiteral(struct UnpackCtx *uc, int bitsCount, int len) { // getd3chr
	int count = getBits(uc, bitsCount) + len + 1;
	uc->size -= count;
	if (uc->size < 0) {
		count += uc->size;
		uc->size = 0;
	}
	for (int i = 0; i < count; ++i) {
		*(uc->dst - i) = (uint8_t)getBits(uc, 8);
	}
	uc->dst -= count;
}

static void copyReference(struct UnpackCtx *uc, int bitsCount, int count) { // copyd3bytes
	uc->size -= count;
	if (uc->size < 0) {
		count += uc->size;
		uc->size = 0;
	}
	const int offset = getBits(uc, bitsCount);
	for (int i = 0; i < count; ++i) {
		*(uc->dst - i) = *(uc->dst - i + offset);
	}
	uc->dst -= count;
}

uint32_t bytekiller_unpack(uint8_t *dst, int dstSize, const uint8_t *src, int srcSize) {
	struct UnpackCtx uc;
	uc.src = src + srcSize - 4;
	uc.size = READ_BE_UINT32(uc.src); uc.src -= 4;
	if (uc.size > dstSize) {
		return 0;
	}
	uc.dst = dst + uc.size - 1;
	uc.crc = READ_BE_UINT32(uc.src); uc.src -= 4;
	uc.bits = READ_BE_UINT32(uc.src); uc.src -= 4;
	uc.crc ^= uc.bits;
	do {
		if (!nextBit(&uc)) {
			if (!nextBit(&uc)) {
				copyLiteral(&uc, 3, 0);
			} else {
				copyReference(&uc, 8, 2);
			}
		} else {
			switch (getBits(&uc, 2)) {
			case 3:
				copyLiteral(&uc, 8, 8);
				break;
			case 2:
				copyReference(&uc, 12, getBits(&uc, 8) + 1);
				break;
			case 1:
				copyReference(&uc, 10, 4);
				break;
			case 0:
				copyReference(&uc, 9, 3);
				break;
			}
		}
	} while (uc.size > 0);
	return uc.crc;
}