ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

INSTALLDIR=usr/bin

define PINFO
PINFO DESCRIPTION=Evaluate arguments as an expression
endef

USEFILE=$(PROJECT_ROOT)/$(NAME).use

include $(MKFILES_ROOT)/qtargets.mk

