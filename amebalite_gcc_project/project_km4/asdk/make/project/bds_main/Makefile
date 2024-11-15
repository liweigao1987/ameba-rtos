
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
CSRC_DIR = $(BASEDIR)/component/bds_main
vpath %.c $(shell find -L $(CSRC_DIR) -type d)

#add your include path here
MODULE_IFLAGS += -I$(CSRC_DIR)/
MODULE_IFLAGS += -I$(CSRC_DIR)/audio/
MODULE_IFLAGS += -I$(CSRC_DIR)/bdvs/
MODULE_IFLAGS += -I$(CSRC_DIR)/context/
MODULE_IFLAGS += -I$(CSRC_DIR)/file/
MODULE_IFLAGS += -I$(CSRC_DIR)/net/
MODULE_IFLAGS += -I$(CSRC_DIR)/player/
MODULE_IFLAGS += -I$(CSRC_DIR)/rpc/
MODULE_IFLAGS += -I$(CSRC_DIR)/session/
MODULE_IFLAGS += -I$(CSRC_DIR)/speech/
MODULE_IFLAGS += -I$(CSRC_DIR)/tool/
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/swlib/
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces/
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/libopus/include/
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/api/
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/rpc/include/
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/rpc/src/aidl/
MODULE_IFLAGS += -I$(BASEDIR)/component/example/bdspeech/
MODULE_IFLAGS += -I$(BASEDIR)/component/bds_light_sdk/include/
MODULE_IFLAGS += -I$(BASEDIR)/component/bds_light_sdk/log/
MODULE_IFLAGS += -I$(BASEDIR)/component/bds_light_sdk/tool/
# MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/fwlib/include/

#set your source code path here
# CUSTOMER_DIR = $(ROOTDIR)/make/project/bdspeech

TARGET_LIB = lib_bds_main
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#add your source code here
CSRC +=	\
	$(wildcard $(CSRC_DIR)/*.c) \
	$(wildcard $(CSRC_DIR)/bdvs/*.c) \
	$(wildcard $(CSRC_DIR)/context/*.c) \
	$(wildcard $(CSRC_DIR)/file/*.c) \
	$(wildcard $(CSRC_DIR)/net/*.c) \
	$(wildcard $(CSRC_DIR)/player/*.c) \
	$(wildcard $(CSRC_DIR)/rpc/*.c) \
	$(wildcard $(CSRC_DIR)/session/*.c) \
	$(wildcard $(CSRC_DIR)/speech/*.c) \
	$(wildcard $(CSRC_DIR)/tool/*.c) \

# vpath %.c $(CSRC_DIR)/bdspeech
# CSRC +=	$(CSRC_DIR)/bdspeech/test_lwg.c

#*****************************************************************************#
#                               OBJECT FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Dependency	                              #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	@echo "-------------------lwg---$(BASEDIR)-----------------"
	@echo "dir=$(CSRC_DIR)"
	@echo "----------------------------------------------------"
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))


#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)
