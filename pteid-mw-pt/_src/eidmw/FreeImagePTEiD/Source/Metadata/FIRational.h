// ==========================================================
// Helper class for rational numbers
//
// Design and implementation by
// - Hervé Drolon <drolon@infonie.fr>
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#ifndef FIRATIONAL_H
#define FIRATIONAL_H

/**
Helper class to deal with rational numbers. 
NB: LONG_FREEIMAGE data type is assumed to be a signed 32-bit number. 
*/
class FIRational {
private:
	/// numerator
	LONG_FREEIMAGE _numerator;
	/// denominator
	LONG_FREEIMAGE _denominator;

public:
	/// Default constructor
	FIRational();

	/// Constructor with longs
	FIRational(LONG_FREEIMAGE n, LONG_FREEIMAGE d = 1);

	/// Constructor with FITAG
	FIRational(const FITAG *tag);

	/// Constructor with a float
	FIRational(float value);

	/// Copy constructor
	FIRational (const FIRational& r);

	/// Destructor
	~FIRational();

	/// Assignement operator
	FIRational& operator=(FIRational& r);

	/// Get the numerator
	LONG_FREEIMAGE getNumerator();

	/// Get the denominator
	LONG_FREEIMAGE getDenominator();

	/// Converts rational value by truncating towards zero
	LONG_FREEIMAGE truncate() {
		// Return truncated rational
		return _denominator ? (LONG_FREEIMAGE) (_numerator / _denominator) : 0;
	}

	/**@name Implicit conversions */
	//@{	
	short shortValue() {
		return (short)truncate();
	}
	int intValue() {
		return (int)truncate();
	}
	LONG_FREEIMAGE longValue() {
		return (LONG_FREEIMAGE)truncate();
	}
	float floatValue() {
		return _denominator ? ((float)_numerator)/((float)_denominator) : 0;
	}
	double doubleValue() {
		return _denominator ? ((double)_numerator)/((double)_denominator) : 0;
	}
	//@}

	/// Checks if this rational number is an integer, either positive or negative
	BOOL_FREEIMAGE isInteger();

	/// Convert as "numerator/denominator"
	std::string toString();

private:
	/// Initialize and normalize a rational number
	void initialize(LONG_FREEIMAGE n, LONG_FREEIMAGE d);

	/// Calculate GCD
	LONG_FREEIMAGE gcd(LONG_FREEIMAGE a, LONG_FREEIMAGE b);
	
	/// Normalize numerator / denominator 
	void normalize();

};

#endif // FIRATIONAL_H

