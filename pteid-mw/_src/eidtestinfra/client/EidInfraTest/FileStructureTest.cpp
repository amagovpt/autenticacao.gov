/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include <algorithm>
#include <cstring>
#include "FileStructure.h"
#include "SoftReaderManager.h"
#include "CommandTestHeader.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include "Control.h"
#include "virtualCardAccess.h"
#include "UnitTest++.h"
#include "xml.h"
#include "utils2.h"
using namespace EidInfra;
using namespace std;
using namespace eidmw::pcscproxy;

#pragma pack(1)

void dumphex4(const char *msg, const unsigned char *buf, size_t len);
extern char * createRelativePath(const char * fileName);

//test paths
static const BYTE p0[2] = { 0x3F, 0x00 };                      //Root ID
static const BYTE p1[2] = { 0x2F, 0x00 };
static const BYTE p2[4] = { 0xDF, 0x01, 0x40, 0x38 };
static const BYTE p3[2] = { 0x2F, 0x01 };                          //does not exist
static const BYTE p4[1] = { 0x4F };                                //does not exist
static const BYTE p5[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };  //does not exist
static BYTE       pathBuf1[MAX_FILE_PATH];
static BYTE       pathBuf2[MAX_FILE_PATH];
static size_t     ArrayLength;
static const BYTE hardNumber[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };
static const File *selected1;
static const File *selected2;

#ifdef _WIN32
static const char *PTEID_CTRL    = "_DocsInternal\\PTeidTestCtrl.xml";
static const char *VIRT_STEPHEN = "_DocsInternal\\virtual_stephen.xml";
#else
static const char *PTEID_CTRL    = "_DocsInternal/PTeidTestCtrl.xml";
static const char *VIRT_STEPHEN = "_DocsInternal/virtual_stephen.xml";
#endif

TEST(addAll)
{
	std::vector<std::string> files;
	FileStructure            fs;
	fs.addAll(files);  //empty files, should be ok
	files.push_back("Dummy file entry 1");
	files.push_back("Dummy file entry 2");
	CHECK(!fs.addAll(files));                //this should return 0 for no files added = pass test
	CHECK(!fs.selectByPath(p1, sizeof(p1))); //Should return 0 = fail
}

TEST(selectByPath)
{
	std::vector<std::string> files;
	const File               * f;
	const BYTE               * content;

	// Create a card first
	eidmw::pcscproxy::Control ctl(createRelativePath(PTEID_CTRL));
	VirtualCard               vc(ctl.getSoftCardFileLocation(hardNumber).c_str());

	FileStructure             fs;
	files = vc.GetAllFiles2(createRelativePath(VIRT_STEPHEN));
	CHECK(files.size());

	CHECK(fs.addAll(files));

	CHECK(fs.selectByPath(p1, sizeof(p1)));
	f = fs.getSelected();

	CHECK(!f->isDir());
	content = f->getContent();
	CHECK_EQUAL(0x61, content[0]);
	CHECK_EQUAL(0x02, content[f->size() - 1]);

	CHECK(fs.selectByPath(p2, sizeof(p2)));

	f = fs.getSelected();
	CHECK(!f->isDir());
	content = f->getContent();
	CHECK_EQUAL(0xa7, content[0]);
	CHECK_EQUAL(0xc8, content[f->size() - 1]);

	CHECK(fs.selectByPath(p2, sizeof(p2) - 2));
	selected1 = fs.getSelected();
	CHECK(selected1->isDir());  // this is a directory

	//this path is wrong, but it will not change previously selected path or cause a crash
	CHECK(!fs.selectByPath(p3, sizeof(p3) - 2));
	selected2   = fs.getSelected();
	ArrayLength = sizeof(p3);
	selected2->getFID((BYTE *) pathBuf2, &ArrayLength);

	ArrayLength = sizeof(p2);

	selected1->getPath(pathBuf1, &ArrayLength);
	selected2->getPath(pathBuf2, &ArrayLength);
	CHECK_ARRAY_EQUAL((BYTE *) pathBuf1, (BYTE *) pathBuf2, (int) ArrayLength); //check that wrong FID did not change selected

	CHECK(!fs.selectByFID(p4));                                                 //single byte array make sure no progem upsets
	CHECK(!fs.selectByFID(p5));                                                 //mode than expeced bytes make sure no program upsets
}

TEST(selectByFID)
{
	std::vector<std::string> files;

	// Create a card first
	VirtualCard   vc(createRelativePath(VIRT_STEPHEN));

	FileStructure fs;
	files = vc.GetAllFiles2(createRelativePath(VIRT_STEPHEN));
	CHECK(files.size());

	CHECK(fs.addAll(files));

	CHECK(fs.selectByFID(p1));
	selected1   = fs.getSelected();
	ArrayLength = sizeof(p1);
	selected1->getFID((BYTE *) pathBuf1, &ArrayLength);

	CHECK(!fs.selectByFID(p3)); //this FID is wrong so it will not change selected
	selected2   = fs.getSelected();
	ArrayLength = sizeof(p3);
	selected2->getFID((BYTE *) pathBuf2, &ArrayLength);

	CHECK_ARRAY_EQUAL((BYTE *) pathBuf1, (BYTE *) pathBuf2, 2); //check that wrong FID did not change selected and no crash!

	CHECK(!fs.selectByPath(p4, sizeof(p4)));                    //single byte array should fail but make sure no crashes
	CHECK(!fs.selectByPath(p5, sizeof(p5)));                    //more than expected bytes should fail but make sure no crashes
}

TEST(selectByAID)
{
	std::vector<std::string> files;
	const BYTE               * content;
	// Create a card first
	VirtualCard              vc(createRelativePath(VIRT_STEPHEN));
	FileStructure            fs;
	files = vc.GetAllFiles2(createRelativePath(VIRT_STEPHEN));
	CHECK(files.size());

	CHECK(fs.addAll(files));

	CHECK(fs.selectByAID(BELPIC_AID, sizeof(BELPIC_AID)));
	selected1 = fs.getSelected();
	content   = selected1->getContent();

	ArrayLength = sizeof(BELPIC_AID);
	selected1->getAID((BYTE *) pathBuf1, &ArrayLength);

	CHECK(!fs.selectByAID(p3, sizeof(BELPIC_AID))); //this AID is wrong and should not change selected
	selected2   = fs.getSelected();
	ArrayLength = sizeof(BELPIC_AID);
	selected2->getAID((BYTE *) pathBuf2, &ArrayLength);

	CHECK_ARRAY_EQUAL((BYTE *) pathBuf1, (BYTE *) pathBuf2, 2); //check that wrong AID did not change selected
}
