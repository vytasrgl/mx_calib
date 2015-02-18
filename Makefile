
TARGET = mx_calib

LIBS = m pthread
LIB_PATHS = ./src/ode/lib64/
INCLUDES = src/ src/CommonOptions/include src/usb2dynamixel

# Flags
CPPFLAGS = -c -O0 -ggdb -Wall -Wextra -fmessage-length=0 -std=c++11
LINKERFLAGS = 



CPP_SUFFIX = .cpp
OBJ_SUFFIX = .o
DEP_SUFFIX = .d
OBJ_DIR = obj/


CPP_FILES =	$(sort $(shell find src -name "*$(CPP_SUFFIX)"))
OBJ_FILES = $(sort $(addprefix $(OBJ_DIR), $(CPP_FILES:%$(CPP_SUFFIX)=%$(OBJ_SUFFIX))))
DEP_FILES = $(sort $(addprefix $(OBJ_DIR), $(CPP_FILES:%$(CPP_SUFFIX)=%$(DEP_SUFFIX))))

INCLUDE_CMD = $(addprefix -I, $(INCLUDES))
LIB_CMD = $(addprefix -l, $(LIBS))
LIB_PATH_CMD = $(addprefix -L, $(LIB_PATHS))

all: $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

depend: $(DEP_FILES)
	
$(TARGET): $(OBJ_FILES)
	@echo linking $(target)
	$(CXX) -o $@ $^ $(LINKERFLAGS) $(LIB_PATH_CMD) $(LIB_CMD)


$(OBJ_DIR)%$(OBJ_SUFFIX): %$(CPP_SUFFIX) $(OBJ_DIR)%$(DEP_SUFFIX)
	@echo building $<
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(INCLUDE_CMD) -o $@ $<


$(OBJ_DIR)%$(DEP_SUFFIX): %$(CPP_SUFFIX)
	rm -f $@
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(INCLUDE_CMD) -MM -MF $@ $<

include $(DEP_FILES)
