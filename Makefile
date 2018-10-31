# ------------------------------------------------
# Generic Makefile
#
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
#   2017-04-24 - changed order of linker params
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = siguel


# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = build
BINDIR   = bin
OUTDIR   = out
INCDIR   = $(SRCDIR)

# debug mode: Coloque o valor para 1 para habilitar o modo debug
DEBUG = 1

PFLAGS   = -pedantic -std=c99 -fstack-protector-all

CC       = gcc
# compiling flags here
CFLAGS   = -lm -I $(INCDIR) $(PFLAGS)

LINKER   = gcc
# linking flags here
LFLAGS   = -I $(INCDIR) -lm

ifeq ($(DEBUG), 1)
	CFLAGS += -g -Wall -D DEBUG
	LFLAGS += -g -Wall
endif

SOURCES  := $(shell find $(SRCDIR) -type f -name "*.c")
INCLUDES := $(shell find $(SRCDIR) -type f -name "*.h")
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -rf

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c src/control/controlador.r
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled \"$<\" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

.PHONY: clean-test
clean-test:
	@$(rm) $(OUTDIR)/***/*.svg $(OUTDIR)/***/*.txt
	@echo "Tests removed!"

remake: remove $(BINDIR)/$(TARGET)
	@echo "Remaked!"