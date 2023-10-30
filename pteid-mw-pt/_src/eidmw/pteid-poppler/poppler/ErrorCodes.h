
//========================================================================
//
// ErrorCodes.h
//
// Copyright 2002-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef ERRORCODES_H
#define ERRORCODES_H

#define errNone             0	// no error

#define errOpenFile         1	// couldn't open the PDF file

#define errBadCatalog       2	// couldn't read the page catalog

#define errDamaged          3	// PDF file was damaged and couldn't be
				// repaired

#define errEncrypted        4	// file was encrypted and password was
				// incorrect or not supplied

#define errHighlightFile    5	// nonexistent or invalid highlight file

#define errBadPrinter       6   // invalid printer

#define errPrinting         7   // error during printing

#define errPermission       8	// PDF file doesn't allow that operation

#define errBadPageNum       9	// invalid page number

#define errFileIO          10   // file I/O error

#define errFileINTR		   11 // process interrupted

#define errFileNAMETOOLONG 12 // too long of a file name

#define errFileOPFLSYSTEM  13 // too many files opened in the SYSTEM

#define errFileOPFLPROCES  14 // too many files opened in the PROCESS

#define errFileNOSPC 	   15 // Disk is Full

#define errFilePERM		   16 // operation is not permited

#define errFileREADONLY    17 // file is read only

#define errFileDEV		   18 // trying to move a file by renaming it

#endif
