TARGET := boot.dol
SOURCES := src/main.cpp
INCLUDES := include

# Wii / GameCube geliştirme ortamını otomatik tanı
include $(DEVKITPPC)/base_rules

CXXFLAGS := -O2 -Wall -mrvl -mcpu=750 -meabi -mhard-float -I$(LIBOGC_INC) -I$(INCLUDES)
LDFLAGS := -g -L$(LIBOGC_LIB) -logc -lm

all: $(TARGET)

$(TARGET): $(SOURCES:.cpp=.elf)
	$(OBJCOPY) -O binary $< $@

%.elf: %.o
	$(CXX) $(LDFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.o *.dol
