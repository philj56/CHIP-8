#include <math.h>
#include <stdio.h>
#include "filters.h"

void scale2x(uint32_t *dest, uint32_t *src, uint32_t width, uint32_t height)
{
	uint32_t dest_p[2][2];
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint32_t x;
	uint32_t y;
	uint32_t size = width * height;

	for (uint32_t i = 0; i < size; i++) {
		x = i % width;
		y = (i - x) / width;
		
		dest_p[0][0] = 2 * x + 4 * y * width;
		dest_p[0][1] = dest_p[0][0] + 1;
		dest_p[1][0] = dest_p[0][0] + 2 * width;
		dest_p[1][1] = dest_p[1][0] + 1;


		if (y == 0) {
			A = src[i];
		} else {
			A = src[i - width];
		}
		if (x == width - 1) {
			B = src[i];
		} else {
			B = src[i + 1];
		}
		if (x == 0) {
			C = src[i];
		} else {
			C = src[i - 1];
		}
		if (y == height - 1) {
			D = src[i];
		} else {
			D = src[i + width];
		}

		dest[dest_p[0][0]] = src[i];
		dest[dest_p[0][1]] = src[i];
		dest[dest_p[1][0]] = src[i];
		dest[dest_p[1][1]] = src[i];

		if (C == A && C != D && A != B) {
			dest[dest_p[0][0]] = A;
		}
		if (A == B && A != C && B != D) {
			dest[dest_p[0][1]] = B;
		}
		if (D == C && D != B && C != A) {
			dest[dest_p[1][0]] = C;
		}
		if (B == D && B != A && D != C) {
			dest[dest_p[1][1]] = D;
		}
	}
}
