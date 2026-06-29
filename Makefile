# Proje ismi
TARGET := boot.dol
SOURCES := src/main.cpp

# DevkitPro değişkenlerini otomatik al
include $(DEVKITPPC)/base_rules

# Derleme ayarları
CXX := powerpc-eabi-g++
CXXFLAGS := -g -O2 -Wall -mrvl -mcpu=750 -meabi -mhard-float
LDFLAGS := -g -L$(LIBOGC_LIB) -L$(LIBOGC_INC) -logc -lm

all: $(TARGET)

$(TARGET): $(SOURCES:.cpp=.elf)
	$(OBJCOPY) -O binary $< $@

%.elf: %.o
	$(CXX) $(LDFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.o *.dol
