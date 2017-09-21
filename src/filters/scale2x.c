#include <math.h>
#include "scale2x.h"

void scale2x(uint32_t *dest, uint32_t *src, uint32_t width, uint32_t height)
{
	uint32_t p00;
	uint32_t p01;
	uint32_t p10;
	uint32_t p11;
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint32_t size = width * height;

	for (uint32_t i = 0; i < size; i++) {
		p00 = 2 * i + floor(i / width) * 2 * width;
		p01 = p00 + 1;
		p10 = p00 + 2 * width;
		p11 = p10 + 1;

		if (i < width) {
			A = src[i];
		} else {
			A = src[i - width];
		}
		if (i % width == width - 1) {
			B = src[i];
		} else {
			B = src[i + 1];
		}
		if (i == 0) {
			C = src[i];
		} else {
			C = src[i - 1];
		}
		if (i > size - width) {
			D = src[i];
		} else {
			D = src[i + width];
		}

		dest[p00] = src[i];
		dest[p01] = src[i];
		dest[p10] = src[i];
		dest[p11] = src[i];

		if (C == A && C != D && A != B) {
			dest[p00] = A;
		}
		if (A == B && A != C && B != D) {
			dest[p01] = B;
		}
		if (D == C && D != B && C != A) {
			dest[p10] = C;
		}
		if (B == D && B != A && D != C) {
			dest[p11] = D;
		}
	}
}
