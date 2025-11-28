#pragma once

#include "ByteArray.h"
#include "CardLayerConst.h"
#include "MWException.h"
#include "P15Objects.h"
#include "Thread.h"
#include "eidErrors.h"

namespace eIDMW {

/**
 * Base interface class for pinpad implementations.
 * Provides the common interface that all pinpad implementations must follow.
 */
class PinpadInterface {
public:
	virtual CByteArray PinCmd(tPinOperation operation, const tPin &pin, unsigned char ucPinType,
							  const CByteArray &oAPDU, unsigned long &ulRemaining, void *wndGeometry = 0) {
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	};
};
} // namespace eIDMW