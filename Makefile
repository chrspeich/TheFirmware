# the above include may override LKROOT and LKINC to allow external
# directories to be included in the build
-include lk_inc.mk

LKMAKEROOT ?= .
LKROOT ?= .
LKINC ?=
BUILDROOT ?= .
DEFAULT_PROJECT ?=
TOOLCHAIN_PREFIX ?=

LKINC := $(LKROOT) $(LKINC)

export LKMAKEROOT
export LKROOT
export LKINC
export BUILDROOT
export DEFAULT_PROJECT
export TOOLCHAIN_PREFIX

# vaneer makefile that calls into the engine with lk as the build root
# if we're the top level invocation, call ourselves with additional args
$(MAKECMDGOALS) _top:
	@$(MAKE) -C $(LKMAKEROOT) -rR -f $(LKROOT)/engine.mk $(addprefix -I,$(LKINC)) $(MAKECMDGOALS)

.PHONY: _top