#include <math.h>
#include <stdio.h>
#include "filters.h"

void scale3x(uint32_t *dest, uint32_t *src, uint32_t width, uint32_t height)
{
	uint32_t p[3][3];
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint32_t E;
	uint32_t F;
	uint32_t G;
	uint32_t H;
	uint32_t I;
	uint32_t x;
	uint32_t y;
	uint32_t size = width * height;

	for (uint32_t i = 0; i < size; i++) {
		x = i % width;
		y = (i - x) / width;
		
		p[0][0] = 3 * x + 9 * y * width;
		p[0][1] = p[0][0] + 1;
		p[0][2] = p[0][0] + 2;
		p[1][0] = p[0][0] + 3 * width;
		p[1][1] = p[1][0] + 1;
		p[1][2] = p[1][0] + 2;
		p[2][0] = p[0][0] + 6 * width;
		p[2][1] = p[2][0] + 1;
		p[2][2] = p[2][0] + 2;


		if (x == 0) {
			D = i;
		} else {
			D = i - 1;
		}
		E = i;
		if (x == width - 1) {
			F = i;
		} else {
			F = i + 1;
		}
		if (y == 0) {
			A = D;
			B = E;
			C = F;
		} else {
			A = D - width;
			B = E - width;
			C = F - width;
		}
		if (y == height - 1) {
			G = D;
			H = E;
			I = F;
		} else {
			G = D + width;
			H = E + width;
			I = F + width;
		}

		A = src[A];
		B = src[B];
		C = src[C];
		D = src[D];
		E = src[E];
		F = src[F];
		G = src[G];
		H = src[H];
		I = src[I];

		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 3; k++) {
				dest[p[j][k]] = src[i];
			}
		}
		if ( D==B && D!=H && B!=F ) {
			dest[p[0][0]] = D;
		}
		if ( (D==B && D!=H && B!=F && E!=C) || (B==F && B!=D && F!=H && E!=A) ) {
			dest[p[0][1]] = B;
		}
		if ( B==F && B!=D && F!=H ) {
			dest[p[0][2]] = F;
		}
		if ( (H==D && H!=F && D!=B && E!=A) || (D==B && D!=H && B!=F && E!=G) ) {
			dest[p[1][0]] = D;
		}
		dest[p[1][1]] = E;
		if ( (B==F && B!=D && F!=H && E!=I) || (F==H && F!=B && H!=D && E!=C) ) {
			dest[p[1][2]] = F;
		}
		if ( H==D && H!=F && D!=B ) {
			dest[p[2][0]] = D;
		}
		if ( (F==H && F!=B && H!=D && E!=G) || (H==D && H!=F && D!=B && E!=I) ) {
			dest[p[2][1]] = H;
		}
		if ( F==H && F!=B && H!=D ) {
			dest[p[2][2]] = F;
		}
	}
}
