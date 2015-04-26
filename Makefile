#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

all: libs plugins gen

# --------------------------------------------------------------

libs:
	$(MAKE) -C dpf/dgl

plugins: libs
	$(MAKE) all -C plugins/Info
	$(MAKE) all -C plugins/Meters
	$(MAKE) all -C plugins/Parameters
	$(MAKE) all -C plugins/States

gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/Info
	$(MAKE) clean -C plugins/Meters
	$(MAKE) clean -C plugins/Parameters
	$(MAKE) clean -C plugins/States

# --------------------------------------------------------------

.PHONY: plugins
