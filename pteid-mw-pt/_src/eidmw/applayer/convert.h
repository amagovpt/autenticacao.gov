#ifndef __J2K_CONVERT_H
#define __J2K_CONVERT_H

#include <openjpeg.h>

/* Component precision clipping */
void clip_component(opj_image_comp_t* component, OPJ_UINT32 precision);
/* Component precision scaling */
void scale_component(opj_image_comp_t* component, OPJ_UINT32 precision);

/* planar / interleaved conversions */
typedef void(*convert_32s_CXPX)(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
	OPJ_SIZE_T length);
extern const convert_32s_CXPX convert_32s_CXPX_LUT[5];
typedef void(*convert_32s_PXCX)(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length, OPJ_INT32 adjust);
extern const convert_32s_PXCX convert_32s_PXCX_LUT[5];
/* bit depth conversions */
typedef void(*convert_XXx32s_C1R)(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length);
extern const convert_XXx32s_C1R convert_XXu32s_C1R_LUT[9]; /* up to 8bpp */
typedef void(*convert_32sXXx_C1R)(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length);
extern const convert_32sXXx_C1R convert_32sXXu_C1R_LUT[9]; /* up to 8bpp */

#endif /* __J2K_CONVERT_H */

