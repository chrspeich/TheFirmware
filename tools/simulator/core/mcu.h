//
// Copyright (c) 2014, Christian Speich
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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ev.h>

typedef struct mcu* mcu_t;
typedef struct mcu_callbacks* mcu_callbacks_t;
typedef struct mcu_instr16* mcu_instr16_t;
typedef struct mcu_instr32* mcu_instr32_t;
typedef struct mem_dev* mem_dev_t;

typedef enum {
	mcu_halted,
	mcu_running,
} mcu_state_t;

struct mcu {
	mcu_instr16_t instrs16;
	mcu_instr32_t instrs32;

	mem_dev_t mem_devs;

	mcu_state_t state;
	halt_reason_t halt_reason;

	mcu_callbacks_t callbacks;

	bool unlocked;

	struct ev_loop *loop;
	ev_idle idle;
};

bool mcu_init(mcu_t mcu, struct ev_loop* loop);
bool mcu_is_halted(mcu_t mcu);
halt_reason_t mcu_halt_reason(mcu_t mcu);

/// Halts the cpu and disables processing instructions
///
/// @param reason specifies the reason the mcu was halted
///		if the reason is less than 0, it is considered a silent halt
///		which is used for cpu sleeping states
///
bool mcu_halt(mcu_t mcu, halt_reason_t reason);
bool mcu_resume(mcu_t mcu);

/// Returns true when the mcu is unlocket.
/// Unlocked means that additional access (for the debugger) is granted.
/// This allows write/read access to regions previously disabled.
///
bool mcu_is_unlocked(mcu_t mcu);

/// Unlocks the mcu
bool mcu_unlock(mcu_t mcu);

/// Locks the mcu
bool mcu_lock(mcu_t mcu);

/// Posts an exception to the mcu
bool mcu_do_exception(mcu_t mcu, exception_t exception);

/// Posts an fault to the mcu
bool mcu_do_fault(mcu_t mcu, fault_t fault);

/// Posts an irq to the mcu
bool mcu_do_irq(mcu_t mcu, irq_t fault);

// Call repeatly to do on workpackage
// Does not need to be called when mcu is halted
bool mcu_runloop(mcu_t mcu);

bool mcu_step(mcu_t mcu);

bool mcu_add_mem_dev(mcu_t mcu, uint32_t offset, mem_dev_t dev);

bool mcu_reset(mcu_t mcu);
uint32_t mcu_read_reg(mcu_t _mcu, reg_t reg);
void mcu_write_reg(mcu_t _mcu, reg_t reg, uint32_t val);

bool mcu_instr_step(mcu_t mcu);

struct mcu_instr16 {
	uint16_t mask;
	uint16_t instr;
	bool (^impl)(mcu_t mcu, uint16_t instr);
};

struct mcu_instr32 {
	uint32_t mask;
	uint32_t instr;
	bool (^impl)(mcu_t mcu, uint32_t instr);
};

#if 0
#define trace_instr16(fmt, ...) printf("[pc=0x%04x]     %04x: "fmt, mcu_read_reg(mcu, REG_PC) - 4, instr, ##__VA_ARGS__)
#define trace_instr32(fmt, ...) printf("[pc=0x%04x] %08x: "fmt, mcu_read_reg(mcu, REG_PC) - 6, instr, ##__VA_ARGS__)
#define trace_print(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define trace_instr16(fmt, ...)
#define trace_instr32(fmt, ...)
#define trace_print(fmt, ...) 
#endif

void mcu_add_callbacks(mcu_t mcu, mcu_callbacks_t callbacks);

struct mcu_callbacks {
	mcu_callbacks_t next;

	void (*mcu_did_halt)(mcu_t mcu, halt_reason_t reason, void* context);

	void* context;
};

typedef enum {
	mem_class_ram,
	mem_class_flash,

	mem_class_io,

	mem_class_other,
} mem_class_t;

struct mem_dev {
	mem_class_t class;
	uint16_t    type;

	uint32_t offset;
	uint32_t length;
	mem_dev_t next;

	bool (*fetch16)(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint16_t* valueOut);
	bool (*fetch32)(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint32_t* valueOut);

	bool (*write16)(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint16_t value);
	bool (*write32)(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint32_t value);
};

bool mcu_fetch16(mcu_t mcu, uint32_t addr, uint16_t* value);
bool mcu_fetch32(mcu_t mcu, uint32_t addr, uint32_t* value);
bool mcu_write16(mcu_t mcu, uint32_t addr, uint16_t value);
bool mcu_write32(mcu_t mcu, uint32_t addr, uint32_t value);

bool mcu_util_fetch8(mcu_t mcu, uint32_t addr, uint8_t* value);
bool mcu_util_write8(mcu_t mcu, uint32_t addr, uint8_t value);

bool mcu_emu_fetch16(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint16_t* valueOut);
bool mcu_emu_write16(mcu_t mcu, mem_dev_t mem_dev, uint32_t addr, uint16_t valueIn);
