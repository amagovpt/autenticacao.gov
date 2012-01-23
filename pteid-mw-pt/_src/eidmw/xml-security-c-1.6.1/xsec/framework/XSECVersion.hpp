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
 * XSECVersion.hpp
 * 
 * Version information for the package
 */

/*
 * These are the high level numerics that need to be changed to bump the
 * version number. They are used to create version strings.
 */

#define XSEC_VERSION_MAJOR   1
#define XSEC_VERSION_MEDIUM  6
#define XSEC_VERSION_MINOR   1

// --------------------------------------------------------------------------------
//           Version Handling
// --------------------------------------------------------------------------------

/*
 * The following code makes use of the Xerces version handling macros to define
 * some constants that can be used during conditional compilation.
 */

/* This can be used for conditional compilation and for testing during
 * autoconfigures.
 *
 * It will create a string of the form 10000 * MAJOR + 100 * MEDIUM + MINOR
 * E.g. 10301 for version 1.3.1
 */

#define _XSEC_VERSION_FULL CALC_EXPANDED_FORM (XSEC_VERSION_MAJOR,XSEC_VERSION_MEDIUM,XSEC_VERSION_MINOR)

/* Some useful strings for versioning - based on the same strings from Xerces */

#define XSEC_FULLVERSIONSTR INVK_CAT3_SEP_UNDERSCORE(XSEC_VERSION_MAJOR,XSEC_VERSION_MEDIUM,XSEC_VERSION_MINOR)
#define XSEC_FULLVERSIONDOT INVK_CAT3_SEP_PERIOD(XSEC_VERSION_MAJOR,XSEC_VERSION_MEDIUM,XSEC_VERSION_MINOR)
#define XSEC_FULLVERSIONNUM INVK_CAT3_SEP_NIL(XSEC_VERSION_MAJOR,XSEC_VERSION_MEDIUM,XSEC_VERSION_MINOR)
#define XSEC_VERSIONSTR     INVK_CAT2_SEP_UNDERSCORE(XSEC_VERSION_MAJOR,XSEC_VERSION_MEDIUM)

/* The following is used for backwards compatibility with previous version handling */

#define XSEC_VERSION         "XSEC_FULLVERSIONDOT"
