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

typedef enum {
	TEST_AFTER_ARCH_EARLY_INIT,
	TEST_AFTER_ARCH_LATE_INIT,
	TEST_IN_MAIN_TASK,
} test_type;

struct test {
	const char* desc;
	test_type type;
	void (*func)();
};

#define DECLARE_TEST(_desc, _type, _func) \
struct test test_##_func __attribute__ ((section (".tests." # _func))) = { \
	.desc = _desc, \
	.type = _type, \
	.func = _func \
};

#ifdef TESTS_SUPPORTED
void test_do(test_type type);
void test_fail(const char* reason);

#define test_assert(expr, desc) if (!(expr)) { test_fail(desc); }

#else

static void test_do(test_type type) {};
static void test_fail(const char* reason) {};

#define test_assert(expr, desc) 

#endif
