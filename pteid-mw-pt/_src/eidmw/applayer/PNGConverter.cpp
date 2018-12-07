/*
 * The copyright in this software is being made available under the 2-clauses
 * BSD License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <openjpeg.h>

#include "PNGConverter.h"

/* Component precision scaling */
void clip_component(opj_image_comp_t* component, OPJ_UINT32 precision)
{
	OPJ_SIZE_T i;
	OPJ_SIZE_T len;
	OPJ_UINT32 umax = static_cast<OPJ_UINT32>(static_cast<OPJ_UINT32>(-1));

	len = static_cast<OPJ_SIZE_T>(component->w) * static_cast<OPJ_SIZE_T>(component->h);
	if (precision < 32) {
		umax = (1U << precision) - 1U;
	}

	if (component->sgnd) {
		OPJ_INT32* l_data = component->data;
		OPJ_INT32 max = static_cast<OPJ_INT32>(umax / 2U);
		OPJ_INT32 min = -max - 1;
		for (i = 0; i < len; ++i) {
			if (l_data[i] > max) {
				l_data[i] = max;
			}
			else if (l_data[i] < min) {
				l_data[i] = min;
			}
		}
	}
	else {
		OPJ_UINT32* l_data = reinterpret_cast<OPJ_UINT32*>(component->data);
		for (i = 0; i < len; ++i) {
			if (l_data[i] > umax) {
				l_data[i] = umax;
			}
		}
	}
	component->prec = precision;
}

/* Component precision scaling */
static void scale_component_up(opj_image_comp_t* component,
	OPJ_UINT32 precision)
{
	OPJ_SIZE_T i, len;

	len = static_cast<OPJ_SIZE_T>(component->w) * static_cast<OPJ_SIZE_T>(component->h);
	if (component->sgnd) {
		OPJ_INT64  newMax = static_cast<OPJ_INT64>(1U << (precision - 1));
		OPJ_INT64  oldMax = static_cast<OPJ_INT64>(1U << (component->prec - 1));
		OPJ_INT32* l_data = component->data;
		for (i = 0; i < len; ++i) {
			l_data[i] = static_cast<OPJ_INT32>((static_cast<OPJ_INT64>(l_data[i]) * newMax) / oldMax);
		}
	}
	else {
		OPJ_UINT64  newMax = static_cast<OPJ_UINT64>((1U << precision) - 1U);
		OPJ_UINT64  oldMax = static_cast<OPJ_UINT64>((1U << component->prec) - 1U);
		OPJ_UINT32* l_data = reinterpret_cast<OPJ_UINT32*>(component->data);
		for (i = 0; i < len; ++i) {
			l_data[i] = static_cast<OPJ_UINT32>((static_cast<OPJ_UINT64>(l_data[i]) * newMax) / oldMax);
		}
	}
	component->prec = precision;
	component->bpp = precision;
}
void scale_component(opj_image_comp_t* component, OPJ_UINT32 precision)
{
	int shift;
	OPJ_SIZE_T i, len;

	if (component->prec == precision) {
		return;
	}
	if (component->prec < precision) {
		scale_component_up(component, precision);
		return;
	}
	shift = static_cast<int>(component->prec - precision);
	len = static_cast<OPJ_SIZE_T>(component->w) * static_cast<OPJ_SIZE_T>(component->h);
	if (component->sgnd) {
		OPJ_INT32* l_data = component->data;
		for (i = 0; i < len; ++i) {
			l_data[i] >>= shift;
		}
	}
	else {
		OPJ_UINT32* l_data = reinterpret_cast<OPJ_UINT32*>(component->data);
		for (i = 0; i < len; ++i) {
			l_data[i] >>= shift;
		}
	}
	component->bpp = precision;
	component->prec = precision;
}


/* planar / interleaved conversions */
/* used by PNG/TIFF */
static void convert_32s_C1P1(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
	OPJ_SIZE_T length)
{
	memcpy(pDst[0], pSrc, length * sizeof(OPJ_INT32));
}
static void convert_32s_C2P2(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	OPJ_INT32* pDst0 = pDst[0];
	OPJ_INT32* pDst1 = pDst[1];

	for (i = 0; i < length; i++) {
		pDst0[i] = pSrc[2 * i + 0];
		pDst1[i] = pSrc[2 * i + 1];
	}
}
static void convert_32s_C3P3(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	OPJ_INT32* pDst0 = pDst[0];
	OPJ_INT32* pDst1 = pDst[1];
	OPJ_INT32* pDst2 = pDst[2];

	for (i = 0; i < length; i++) {
		pDst0[i] = pSrc[3 * i + 0];
		pDst1[i] = pSrc[3 * i + 1];
		pDst2[i] = pSrc[3 * i + 2];
	}
}
static void convert_32s_C4P4(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	OPJ_INT32* pDst0 = pDst[0];
	OPJ_INT32* pDst1 = pDst[1];
	OPJ_INT32* pDst2 = pDst[2];
	OPJ_INT32* pDst3 = pDst[3];

	for (i = 0; i < length; i++) {
		pDst0[i] = pSrc[4 * i + 0];
		pDst1[i] = pSrc[4 * i + 1];
		pDst2[i] = pSrc[4 * i + 2];
		pDst3[i] = pSrc[4 * i + 3];
	}
}
const convert_32s_CXPX convert_32s_CXPX_LUT[5] = {
	nullptr,
	convert_32s_C1P1,
	convert_32s_C2P2,
	convert_32s_C3P3,
	convert_32s_C4P4
};

static void convert_32s_P1C1(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length, OPJ_INT32 adjust)
{
	OPJ_SIZE_T i;
	const OPJ_INT32* pSrc0 = pSrc[0];

	for (i = 0; i < length; i++) {
		pDst[i] = pSrc0[i] + adjust;
	}
}
static void convert_32s_P2C2(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length, OPJ_INT32 adjust)
{
	OPJ_SIZE_T i;
	const OPJ_INT32* pSrc0 = pSrc[0];
	const OPJ_INT32* pSrc1 = pSrc[1];

	for (i = 0; i < length; i++) {
		pDst[2 * i + 0] = pSrc0[i] + adjust;
		pDst[2 * i + 1] = pSrc1[i] + adjust;
	}
}
static void convert_32s_P3C3(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length, OPJ_INT32 adjust)
{
	OPJ_SIZE_T i;
	const OPJ_INT32* pSrc0 = pSrc[0];
	const OPJ_INT32* pSrc1 = pSrc[1];
	const OPJ_INT32* pSrc2 = pSrc[2];

	for (i = 0; i < length; i++) {
		pDst[3 * i + 0] = pSrc0[i] + adjust;
		pDst[3 * i + 1] = pSrc1[i] + adjust;
		pDst[3 * i + 2] = pSrc2[i] + adjust;
	}
}
static void convert_32s_P4C4(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length, OPJ_INT32 adjust)
{
	OPJ_SIZE_T i;
	const OPJ_INT32* pSrc0 = pSrc[0];
	const OPJ_INT32* pSrc1 = pSrc[1];
	const OPJ_INT32* pSrc2 = pSrc[2];
	const OPJ_INT32* pSrc3 = pSrc[3];

	for (i = 0; i < length; i++) {
		pDst[4 * i + 0] = pSrc0[i] + adjust;
		pDst[4 * i + 1] = pSrc1[i] + adjust;
		pDst[4 * i + 2] = pSrc2[i] + adjust;
		pDst[4 * i + 3] = pSrc3[i] + adjust;
	}
}
const convert_32s_PXCX convert_32s_PXCX_LUT[5] = {
	nullptr,
	convert_32s_P1C1,
	convert_32s_P2C2,
	convert_32s_P3C3,
	convert_32s_P4C4
};

/* bit depth conversions */
/* used by PNG/TIFF up to 8bpp */
static void convert_1u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(7U)); i += 8U) {
		OPJ_UINT32 val = *pSrc++;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 7);
		pDst[i + 1] = static_cast<OPJ_INT32>((val >> 6) & 0x1U);
		pDst[i + 2] = static_cast<OPJ_INT32>((val >> 5) & 0x1U);
		pDst[i + 3] = static_cast<OPJ_INT32>((val >> 4) & 0x1U);
		pDst[i + 4] = static_cast<OPJ_INT32>((val >> 3) & 0x1U);
		pDst[i + 5] = static_cast<OPJ_INT32>((val >> 2) & 0x1U);
		pDst[i + 6] = static_cast<OPJ_INT32>((val >> 1) & 0x1U);
		pDst[i + 7] = static_cast<OPJ_INT32>(val & 0x1U);
	}
	if (length & 7U) {
		OPJ_UINT32 val = *pSrc++;
		length = length & 7U;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 7);

		if (length > 1U) {
			pDst[i + 1] = static_cast<OPJ_INT32>((val >> 6) & 0x1U);
			if (length > 2U) {
				pDst[i + 2] = static_cast<OPJ_INT32>((val >> 5) & 0x1U);
				if (length > 3U) {
					pDst[i + 3] = static_cast<OPJ_INT32>((val >> 4) & 0x1U);
					if (length > 4U) {
						pDst[i + 4] = static_cast<OPJ_INT32>((val >> 3) & 0x1U);
						if (length > 5U) {
							pDst[i + 5] = static_cast<OPJ_INT32>((val >> 2) & 0x1U);
							if (length > 6U) {
								pDst[i + 6] = static_cast<OPJ_INT32>((val >> 1) & 0x1U);
							}
						}
					}
				}
			}
		}
	}
}
static void convert_2u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(3U)); i += 4U) {
		OPJ_UINT32 val = *pSrc++;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 6);
		pDst[i + 1] = static_cast<OPJ_INT32>((val >> 4) & 0x3U);
		pDst[i + 2] = static_cast<OPJ_INT32>((val >> 2) & 0x3U);
		pDst[i + 3] = static_cast<OPJ_INT32>(val & 0x3U);
	}
	if (length & 3U) {
		OPJ_UINT32 val = *pSrc++;
		length = length & 3U;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 6);

		if (length > 1U) {
			pDst[i + 1] = static_cast<OPJ_INT32>((val >> 4) & 0x3U);
			if (length > 2U) {
				pDst[i + 2] = static_cast<OPJ_INT32>((val >> 2) & 0x3U);

			}
		}
	}
}
static void convert_4u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(1U)); i += 2U) {
		OPJ_UINT32 val = *pSrc++;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 4);
		pDst[i + 1] = static_cast<OPJ_INT32>(val & 0xFU);
	}
	if (length & 1U) {
		OPJ_UINT8 val = *pSrc++;
		pDst[i + 0] = static_cast<OPJ_INT32>(val >> 4);
	}
}
static void convert_6u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(3U)); i += 4U) {
		OPJ_UINT32 val0 = *pSrc++;
		OPJ_UINT32 val1 = *pSrc++;
		OPJ_UINT32 val2 = *pSrc++;
		pDst[i + 0] = static_cast<OPJ_INT32>(val0 >> 2);
		pDst[i + 1] = static_cast<OPJ_INT32>(((val0 & 0x3U) << 4) | (val1 >> 4));
		pDst[i + 2] = static_cast<OPJ_INT32>(((val1 & 0xFU) << 2) | (val2 >> 6));
		pDst[i + 3] = static_cast<OPJ_INT32>(val2 & 0x3FU);

	}
	if (length & 3U) {
		OPJ_UINT32 val0 = *pSrc++;
		length = length & 3U;
		pDst[i + 0] = static_cast<OPJ_INT32>(val0 >> 2);

		if (length > 1U) {
			OPJ_UINT32 val1 = *pSrc++;
			pDst[i + 1] = static_cast<OPJ_INT32>(((val0 & 0x3U) << 4) | (val1 >> 4));
			if (length > 2U) {
				OPJ_UINT32 val2 = *pSrc++;
				pDst[i + 2] = static_cast<OPJ_INT32>(((val1 & 0xFU) << 2) | (val2 >> 6));
			}
		}
	}
}
static void convert_8u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < length; i++) {
		pDst[i] = pSrc[i];
	}
}
const convert_XXx32s_C1R convert_XXu32s_C1R_LUT[9] = {
	nullptr,
	convert_1u32s_C1R,
	convert_2u32s_C1R,
	nullptr,
	convert_4u32s_C1R,
	nullptr,
	convert_6u32s_C1R,
	nullptr,
	convert_8u32s_C1R
};


static void convert_32s1u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(7U)); i += 8U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
		OPJ_UINT32 src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
		OPJ_UINT32 src3 = static_cast<OPJ_UINT32>(pSrc[i + 3]);
		OPJ_UINT32 src4 = static_cast<OPJ_UINT32>(pSrc[i + 4]);
		OPJ_UINT32 src5 = static_cast<OPJ_UINT32>(pSrc[i + 5]);
		OPJ_UINT32 src6 = static_cast<OPJ_UINT32>(pSrc[i + 6]);
		OPJ_UINT32 src7 = static_cast<OPJ_UINT32>(pSrc[i + 7]);

		*pDst++ = static_cast<OPJ_BYTE>((src0 << 7) | (src1 << 6) | (src2 << 5) | (src3 << 4) |
			(src4 << 3) | (src5 << 2) | (src6 << 1) | src7);
	}

	if (length & 7U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = 0U;
		OPJ_UINT32 src2 = 0U;
		OPJ_UINT32 src3 = 0U;
		OPJ_UINT32 src4 = 0U;
		OPJ_UINT32 src5 = 0U;
		OPJ_UINT32 src6 = 0U;
		length = length & 7U;

		if (length > 1U) {
			src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
			if (length > 2U) {
				src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
				if (length > 3U) {
					src3 = static_cast<OPJ_UINT32>(pSrc[i + 3]);
					if (length > 4U) {
						src4 = static_cast<OPJ_UINT32>(pSrc[i + 4]);
						if (length > 5U) {
							src5 = static_cast<OPJ_UINT32>(pSrc[i + 5]);
							if (length > 6U) {
								src6 = static_cast<OPJ_UINT32>(pSrc[i + 6]);
							}
						}
					}
				}
			}
		}
		*pDst++ = static_cast<OPJ_BYTE>((src0 << 7) | (src1 << 6) | (src2 << 5) | (src3 << 4) |
			(src4 << 3) | (src5 << 2) | (src6 << 1));
	}
}

static void convert_32s2u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(3U)); i += 4U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
		OPJ_UINT32 src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
		OPJ_UINT32 src3 = static_cast<OPJ_UINT32>(pSrc[i + 3]);

		*pDst++ = static_cast<OPJ_BYTE>((src0 << 6) | (src1 << 4) | (src2 << 2) | src3);
	}

	if (length & 3U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = 0U;
		OPJ_UINT32 src2 = 0U;
		length = length & 3U;

		if (length > 1U) {
			src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
			if (length > 2U) {
				src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
			}
		}
		*pDst++ = static_cast<OPJ_BYTE>((src0 << 6) | (src1 << 4) | (src2 << 2));
	}
}

static void convert_32s4u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(1U)); i += 2U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);

		*pDst++ = static_cast<OPJ_BYTE>((src0 << 4) | src1);
	}

	if (length & 1U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		*pDst++ = static_cast<OPJ_BYTE>((src0 << 4));
	}
}

static void convert_32s6u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < (length & ~static_cast<OPJ_SIZE_T>(3U)); i += 4U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
		OPJ_UINT32 src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
		OPJ_UINT32 src3 = static_cast<OPJ_UINT32>(pSrc[i + 3]);

		*pDst++ = static_cast<OPJ_BYTE>((src0 << 2) | (src1 >> 4));
		*pDst++ = static_cast<OPJ_BYTE>(((src1 & 0xFU) << 4) | (src2 >> 2));
		*pDst++ = static_cast<OPJ_BYTE>(((src2 & 0x3U) << 6) | src3);
	}

	if (length & 3U) {
		OPJ_UINT32 src0 = static_cast<OPJ_UINT32>(pSrc[i + 0]);
		OPJ_UINT32 src1 = 0U;
		OPJ_UINT32 src2 = 0U;
		length = length & 3U;

		if (length > 1U) {
			src1 = static_cast<OPJ_UINT32>(pSrc[i + 1]);
			if (length > 2U) {
				src2 = static_cast<OPJ_UINT32>(pSrc[i + 2]);
			}
		}
		*pDst++ = static_cast<OPJ_BYTE>((src0 << 2) | (src1 >> 4));
		if (length > 1U) {
			*pDst++ = static_cast<OPJ_BYTE>(((src1 & 0xFU) << 4) | (src2 >> 2));
			if (length > 2U) {
				*pDst++ = static_cast<OPJ_BYTE>(((src2 & 0x3U) << 6));
			}
		}
	}
}
static void convert_32s8u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < length; ++i) {
		pDst[i] = static_cast<OPJ_BYTE>(pSrc[i]);
	}
}
const convert_32sXXx_C1R convert_32sXXu_C1R_LUT[9] = {
	nullptr,
	convert_32s1u_C1R,
	convert_32s2u_C1R,
	nullptr,
	convert_32s4u_C1R,
	nullptr,
	convert_32s6u_C1R,
	nullptr,
	convert_32s8u_C1R
};
