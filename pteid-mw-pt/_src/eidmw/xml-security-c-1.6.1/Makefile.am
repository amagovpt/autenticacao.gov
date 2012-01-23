#
# Copyright 2006-2010 The Apache Software Foundation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = xsec

EXTRA_DIST = \
	CHANGELOG.txt \
	INSTALL.txt \
	LICENSE.txt \
	NOTICE.txt \
	Projects \
	xml-security-c.spec \
	m4/acx_pthread.m4 \
	xsec/framework/resource.h \
	xsec/framework/version.rc \
	xsec/tools/threadTest/threadtest.cpp

dist-hook:
	rm -rf `find $(distdir)/Projects -name .svn`
