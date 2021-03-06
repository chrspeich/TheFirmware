LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/can_node.c

MODULE_DEPS := \
	lib/sensor \
	lib/config

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)/include



CAN_NODE_NUM_OUTPUT ?= 0
CAN_NODE_NUM_OUTPUT_SOURCE ?= 5

GLOBAL_DEFINES += \
	HAVE_CAN_NODE=1 \
	CAN_NODE_NUM_OUTPUT=$(CAN_NODE_NUM_OUTPUT) \
	CAN_NODE_NUM_OUTPUT_SOURCE=$(CAN_NODE_NUM_OUTPUT_SOURCE) 

include make/module.mk
