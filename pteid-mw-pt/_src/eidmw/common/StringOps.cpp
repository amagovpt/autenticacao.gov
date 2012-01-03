/*
 * StringOps.cpp
 *
 *  Created on: Dec 26, 2011
 *      Author: ruim
 */

#include <string>

using namespace std;
namespace eIDMW {

const char *replace(const char* in, const char* search, const char* replace){

		string str(in);
		string searchString(search);
		string replaceString(replace);

		string::size_type pos = 0;
		while ( (pos = str.find(searchString, pos)) != string::npos ) {
			str.replace( pos, searchString.size(), replaceString );
			pos++;
		}
		return str.c_str();
}

} /* namespace eIDMW */
