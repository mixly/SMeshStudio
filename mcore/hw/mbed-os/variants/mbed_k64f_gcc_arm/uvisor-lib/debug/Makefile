###########################################################################
#
#  Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License"); you may
#  not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

PROJECT=uvisor-lib
PREFIX:=arm-none-eabi-

ifeq ("$(ARCH)","MK64F")
	CPU:=$(ARCH)N1M0XXX12
	YT_TARGET:=frdm-k64f-gcc
	GDB_TARGET:=k64f
else
	ARCH:=STM32F
	CPU:=$(ARCH)429ZI
	YT_TARGET:=stm32f429i-disco-gcc
	GDB_TARGET:=stm32f4
endif

#
# flash firmware image using segger JLink
# https://www.segger.com/jlink-software.html
#
JLINK:=JLinkExe
JLINK_VIEWER_MASK:=0x7
JLINK_PARAM:=-Device $(CPU) -if SWD
AUXHFRCO_FREQ:=0

#
# GDB configuration
#
JLINK_SERVER:=JLinkGDBServer
GDB:=$(PREFIX)gdb
DEBUG_HOST:=localhost:2331
OBJDUMP:=$(PREFIX)objdump

#
# derived variables
#
ifneq ("$(T)","")
	TEST:=$(PROJECT)-test-$(T)
	TARGET_ELF:=../build/$(YT_TARGET)/test/$(TEST)
	TARGET_BIN:=$(TARGET_ELF).bin
endif

# tty
OS := $(shell uname -s)
ifeq ($(OS),Linux)
	TTY:=/dev/ttyACM*
endif
ifeq ($(OS),Darwin)
	TTY:=/dev/tty.usbmodemfa*
endif

#
# add inline files for debugging / flashing
#
include Makefile.scripts

# check if uvisor symbol file can be read - add to symbols GDB config if present
GDB_DEBUG_SYMBOLS_FILE1:=$(wildcard ../../uvisor/k64f/uvisor/k64f_uvisor.elf)
ifneq ("$(GDB_DEBUG_SYMBOLS_FILE1)","")
	GDB_DEBUG_UVISOR:=add-symbol-file $(GDB_DEBUG_SYMBOLS_FILE1) uvisor_init
endif
GDB_DEBUG_SYMBOLS_FILE2:=$(wildcard ../../k64f/uvisor/k64f_uvisor.elf)
ifneq ("$(GDB_DEBUG_SYMBOLS_FILE2)","")
	GDB_DEBUG_UVISOR:=add-symbol-file $(GDB_DEBUG_SYMBOLS_FILE2) uvisor_init
endif

#
# build targets
#

.PHONY: all setup test single debug erase reset gdb.script gdbserver clean

all: test

setup:
	cd .. && yotta target frdm-k64f-gcc

single:
ifndef T
	$(error Specify single test to debug: make T=testX single)
endif
	cd .. && mbedgt -c cp --target=$(YT_TARGET) -n $(TEST)

test:
	stty -F $(TTY) sane
	cd .. && mbedgt -c cp --target=$(YT_TARGET)

debug: gdb.script
ifndef T
	$(error Specify single test to debug: make T=testX debug)
endif
	cd .. && mbedgt --debug -O -c cp --target=$(YT_TARGET) -n $(TEST)
	$(OBJDUMP) -d $(TARGET_ELF) > $(TARGET_ELF).asm
	@echo "$$__SCRIPT_FLASH" | $(JLINK) $(JLINK_PARAM)
	@$(PREFIX)size $(TARGET_ELF)
	$(GDB) -x $<
	rm -f ../gdb.script

erase:
	@echo "$$__SCRIPT_ERASE" | $(JLINK) $(JLINK_PARAM)

reset:
	@echo "$$__SCRIPT_RESET" | $(JLINK) $(JLINK_PARAM)

gdb.script:
	@echo "$$__SCRIPT_GDB" > $@

gdbserver:
	$(JLINK_SERVER) $(JLINK_PARAM) $(APP_JLINK_PARAM)

clean: setup
	rm -f ../gdb.script
	yotta clean
