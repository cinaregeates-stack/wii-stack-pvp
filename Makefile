TARGET = boot
BUILD_DIR = build
SRC = src/main.cpp
OBJ = $(SRC:.cpp=.o)

# Devkitpro toolchain prefix (change only if you use a custom toolchain)
PREFIX ?= powerpc-eabi-
CXX = $(PREFIX)g++
CC = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy

# Paths - let user rely on DEVKITPPC environment if set
DEVKITPPC ?= /opt/devkitpro/devkitPPC
INCLUDES = -Iinclude -I$(DEVKITPPC)/libogc/include
LIBS = -L$(DEVKITPPC)/libogc/lib -logc -lwiiuse -lm

CXXFLAGS = -O2 -mno-fused-madd -ffast-math -fno-exceptions -fno-rtti -Wall -std=gnu++11 $(INCLUDES)
LDFLAGS = $(LIBS)

all: $(TARGET).dol

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET).dol: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).dol

.PHONY: all clean
