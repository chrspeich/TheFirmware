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

#include <log.h>
#include <string.h>

#if 0
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN "\033[0;32m"
#define COLOR_GREEN "\033[0;36m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GRAY "\033[1;30m"
#else
#define COLOR_RESET ""
#define COLOR_CYAN ""
#define COLOR_GREEN ""
#define COLOR_YELLOW ""
#define COLOR_RED ""
#define COLOR_GRAY ""
#endif

static const log_level_t min_log_level = LOG_LEVEL_DEBUG;
static file_t log_file = NULL;

void log_set_file(file_t f)
{
	log_file = f;
}

void _log(const char* file, int line, log_level_t log_level, const char* message, ...) {
	if (log_level >= min_log_level) {
		va_list args;
		va_start(args, message);
		_logv(file, line, log_level, message, args);
		va_end(args);
	}
}

void _logv(const char* file, int line, log_level_t log_level, const char* message, va_list args)
{
	if (!log_file) {
		return;
	}

	const char* level_str;

	switch(log_level) {
		case LOG_LEVEL_DEBUG:
			level_str = COLOR_CYAN"[D]"COLOR_RESET;
			break;
		case LOG_LEVEL_VERBOSE:
			level_str = COLOR_CYAN"[V]"COLOR_RESET;
			break;
		case LOG_LEVEL_INFO:
			level_str = COLOR_GREEN"[I]"COLOR_RESET;
			break;
		case LOG_LEVEL_WARN:
			level_str = COLOR_YELLOW"[W]"COLOR_RESET;
			break;
		case LOG_LEVEL_ERROR:
			level_str = COLOR_RED"[E]"COLOR_RESET;
			break;
	}

	if (file) {
		fprintf(log_file, COLOR_GRAY"%s:%d %s ", file, line, level_str);
	}
	else {
		fprintf(log_file, "%s ", level_str);
	}

	vfprintf(log_file, message, args);
	write(log_file, "\r\n", 2);
}
