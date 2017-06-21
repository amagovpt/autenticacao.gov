/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * XSECCryptoProvider := Base virtual class to define a crpyto module
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */

#include <xsec/enc/XSECCryptoProvider.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#include <xercesc/util/Janitor.hpp>

XSECCryptoKeyEC* XSECCryptoProvider::keyEC() const {
    throw XSECCryptoException(XSECCryptoException::UnsupportedError,
		"XSECCryptoProvider - EC keys not supported");
}

XSECCryptoKey* XSECCryptoProvider::keyDER(const char* buf, unsigned long buflen, bool base64) const {
    throw XSECCryptoException(XSECCryptoException::UnsupportedError,
		"XSECCryptoProvider - DER-encoded keys not supported");
}
