CSRCS += src/gui_home.c
CSRCS += src/gui_keyboard.c

DEPPATH += --dep-path gui/src
VPATH += :gui/src

CFLAGS += -Igui/inc