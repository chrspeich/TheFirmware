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

#pragma once

#include <file.h>

#ifndef HAVE_CONSOLE
#define CONSOLE_UNAVAIABLE __attribute__((unavailable("requires compiling with console support")))
#else
#define CONSOLE_UNAVAIABLE
#endif

typedef struct console* console_t;

// Spawns a new console on the given console
//
// This starts a new background thread for servicing this console
//
// @return the newly created console, can left unused
console_t console_spawn(file_t file) CONSOLE_UNAVAIABLE;

size_t printf(const char* format, ...) CONSOLE_UNAVAIABLE;

struct console_cmd {
	const char* name;
	int (*func)(int argc, const char** argv);
};

#ifdef HAVE_CONSOLE
#define CONSOLE_CMD(_name, _func) \
	const struct console_cmd __init_struct_##_name __attribute__ ((section (".console_cmds." #_name))) = { \
		.name = #_name, \
		.func = _func, \
	}
#else
#define CONSOLE_CMD(_name, _func)
#endif
