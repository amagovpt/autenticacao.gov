#ifdef __MINGW32__

/*
 * MS specifics
 */

#ifndef _NO_W32_PSEUDO_MODIFIERS
#define IN
#define OUT

#define __in
#define __in_opt
#define __in_bcount(x)
#define __in_bcount_opt(x)
#define __in_ecount(x)
#define __in_xcount(x)

#define __inout
#define __inout_opt
#define __inout_bcount_full(x)
#define __inout_bcount_opt(x)

#define __out
#define __out_bcount(x)
#define __out_bcount_full(x)
#define __out_bcount_opt(x)
#define __out_bcount_part_opt(x,y)
#define __out_ecount(x)
#define __out_xcount(x)
#define __out_opt

#define __deref
#define __deref_inout_opt
#define __deref_out
#define __deref_opt_inout_bcount_part_opt(x,y)
#define __deref_out_bcount(x)
#define __deref_out_bcount_opt(x)

#define __nullterminated

//#define __out_ecount_part(x)
//#define __out_ecount_part(x,y)
#define __struct_bcount(x)
#define __field_ecount_opt(x)

#define __success(x)
#ifndef OPTIONAL
#define OPTIONAL
#endif
#endif


#endif
