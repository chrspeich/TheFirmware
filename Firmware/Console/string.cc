//
// Copyright (c) 2013, Christian Speich
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "Firmware/Console/string.h"

#include "Firmware/Runtime.h"

extern "C" {

bool charInStr(char c, const char* str)
{
	assert(str, "str can't be NULL");

	for (; *str != '\0'; str++)
		if (*str == c)
			return true;

	return false;
}

size_t strlen(const char* str)
{
	assert(str, "str can't be NULL");

	size_t size = 0;
	for (; *str != '\0'; size++, str++)
		;

	return size;
}

char *strsep(char **stringp, const char *delim)
{
	assert(stringp, "stringp can't be NULL");
	assert(delim, "delim can't be NULL");

	char* found = *stringp;
	char* remaining = found;

	if (!stringp)
		return NULL;

	for (; *remaining != '\0'; remaining++) {
		if (charInStr(*remaining, delim)) {
			*remaining = '\0';
			remaining++;
			break;
		}
	}

	if (*remaining == '\0')
		*stringp = NULL;
	else 
		*stringp = remaining;

	return found;
}

char* strsep_ext(char** stringp, const char* delim) {
	char* value;

	do {
		value = strsep(stringp, delim);
	} while (value && strlen(value) == 0);

	return value;
}

int strcmp(const char* str1, const char* str2)
{
	assert(str1, "str1 can't be NULL");
	assert(str2, "str2 can't be NULL");

	for (; *str1 != '\0' && *str2 != '\0' && *str1 == *str2; str1++, str2++)
		;

	if (*str1 < *str2)
		return -1;
	else if (*str1 > *str2)
		return 1;
	else
		return 0;
}

} // extern "C"
