
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
CSRC_DIR = $(BASEDIR)/component/bds_light_sdk
vpath %.c $(shell find -L $(CSRC_DIR) -type d)

#add your include path here
MODULE_IFLAGS += -I$(CSRC_DIR)/client/
MODULE_IFLAGS += -I$(CSRC_DIR)/client_adapter/
MODULE_IFLAGS += -I$(CSRC_DIR)/connection/
MODULE_IFLAGS += -I$(CSRC_DIR)/connection/io_watch/
MODULE_IFLAGS += -I$(CSRC_DIR)/connection/state/
MODULE_IFLAGS += -I$(CSRC_DIR)/connection/strategy/
MODULE_IFLAGS += -I$(CSRC_DIR)/dsp/
MODULE_IFLAGS += -I$(CSRC_DIR)/hardware/dsp_feature/
MODULE_IFLAGS += -I$(CSRC_DIR)/include/
MODULE_IFLAGS += -I$(CSRC_DIR)/log/
MODULE_IFLAGS += -I$(CSRC_DIR)/online_engine/
MODULE_IFLAGS += -I$(CSRC_DIR)/online_engine/asr/
MODULE_IFLAGS += -I$(CSRC_DIR)/online_engine/message_push/
MODULE_IFLAGS += -I$(CSRC_DIR)/online_engine/post_event/
MODULE_IFLAGS += -I$(CSRC_DIR)/protocol/
MODULE_IFLAGS += -I$(CSRC_DIR)/recorder/
MODULE_IFLAGS += -I$(CSRC_DIR)/tool/
MODULE_IFLAGS += -I$(CSRC_DIR)/wakeup/
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/swlib/
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/libopus/include/
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/api/
# MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/fwlib/include/

#set your source code path here
# CUSTOMER_DIR = $(ROOTDIR)/make/project/bdspeech

TARGET_LIB = lib_bdspeech
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#add your source code here
CSRC +=	\
	$(wildcard $(CSRC_DIR)/client/*.c) \
	$(wildcard $(CSRC_DIR)/client_adapter/*.c) \
	$(wildcard $(CSRC_DIR)/connection/*.c) \
	$(wildcard $(CSRC_DIR)/connection/io_watch/*.c) \
	$(wildcard $(CSRC_DIR)/connection/state/*.c) \
	$(wildcard $(CSRC_DIR)/connection/strategy/*.c) \
	$(wildcard $(CSRC_DIR)/dsp/*.c) \
	$(wildcard $(CSRC_DIR)/hardware/dsp_feature/*.c) \
	$(wildcard $(CSRC_DIR)/log/*.c) \
	$(wildcard $(CSRC_DIR)/online_engine/*.c) \
	$(wildcard $(CSRC_DIR)/online_engine/asr/*.c) \
	$(wildcard $(CSRC_DIR)/online_engine/message_push/*.c) \
	$(wildcard $(CSRC_DIR)/online_engine/post_event/*.c) \
	$(wildcard $(CSRC_DIR)/protocol/*.c) \
	$(wildcard $(CSRC_DIR)/recorder/*.c) \
	$(wildcard $(CSRC_DIR)/tool/*.c) \
	$(wildcard $(CSRC_DIR)/wakeup/*.c) \

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
