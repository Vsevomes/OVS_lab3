# -----------------------------------
# Путь к SystemC
# -----------------------------------
SYSTEMC_HOME    = /usr/local/systemc-2.3.3
TARGET_ARCH     = linux64

SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib-$(TARGET_ARCH)

# -----------------------------------
# Флаги компиляции и линковки
# -----------------------------------
FLAGS           = -g -Wall -pedantic -Wno-long-long \
                  -DSC_INCLUDE_DYNAMIC_PROCESSES -fpermissive \
                  -I$(SYSTEMC_INC_DIR) -Iinclude
LDFLAGS         = -L$(SYSTEMC_LIB_DIR) -lsystemc -lm

# -----------------------------------
# Файлы исходного кода
# -----------------------------------
SRCS = src/accelerator.cpp \
       src/control_unit.cpp \
       src/io_controller.cpp \
	   src/matrix_bus.cpp \
       src/pe_mac.cpp \
       src/pe_activation.cpp \
       src/shared_memory.cpp \
       src/main.cpp

OBJS = $(SRCS:.cpp=.o)

# -----------------------------------
# Цель по умолчанию
# -----------------------------------
all: accelerator_sim

# -----------------------------------
# Компиляция исполняемого файла
# -----------------------------------
accelerator_sim: $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS) $(FLAGS)

# -----------------------------------
# Компиляция cpp -> o
# -----------------------------------
%.o: %.cpp
	g++ -c $< -o $@ $(FLAGS)

# -----------------------------------
# Очистка
# -----------------------------------
clean:
	rm -f $(OBJS) accelerator_sim *.vcd

.PHONY: all clean
