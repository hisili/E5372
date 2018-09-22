##########################################################################
# ptable module compiler process.
# Output:ptable.obj
##########################################################################
BSP_CUR_DIR = $(BSP_DIR)/services/ptable
INCLUDE_DIR  = $(BSP_DIR)/bootloader/code/bootloader
#		-I$(CURRENT_DIR)/../../common  \
#		-I$(CURRENT_DIR)/../../include \
#		-I$(CURRENT_DIR)/../../drivers/nandc  \
#		-I$(CURRENT_DIR)/../../drivers/timer  \
#		-I$(CURRENT_DIR)/../../om/dump
#		-I$(CURRENT_DIR)/../../../external/bsp  \


ifeq ($(BSP_IMAGE_TYPE),BSP_IMAGE_BOOTROM)
	OBJFILE_PTABLE = ptable_common.o ptable_native.o ptable_product.o
else
	OBJFILE_PTABLE = ptable_common.o ptable_native.o ptable_product.o
endif


ifeq ($(BUILD_TYPE_PARALLEL),TRUE)

ifeq ($(BSP_IMAGE_TYPE),BSP_IMAGE_BOOTROM)
	OBJFILE_PTABLE2 = $(TEMP_PATH)/ptable_common.o $(TEMP_PATH)/ptable_native.o $(TEMP_PATH)/ptable_product.o
else
	OBJFILE_PTABLE2 = $(TEMP_PATH)/ptable_common.o $(TEMP_PATH)/ptable_native.o $(TEMP_PATH)/ptable_product.o
endif


ptable.obj: $(OBJFILE_PTABLE) 
	$(LD) $(LD_PARTIAL_FLAGS) $(OBJFILE_PTABLE2) -o $(TEMP_PATH)/$@
	$(RM) $(OBJFILE_PTABLE2)
	
$(OBJFILE_PTABLE):%.o:$(addprefix $(BSP_CUR_DIR)/,%.c)
	$(RM) $(TEMP_PATH)/$@
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $< -o $(TEMP_PATH)/$@

else

ptable.obj: $(OBJFILE_PTABLE) 
	$(LD) $(LD_PARTIAL_FLAGS) $(OBJFILE_PTABLE) -o $@
	$(RM) $(OBJFILE_PTABLE)

$(OBJFILE_PTABLE):%.o:$(addprefix $(BSP_CUR_DIR)/,%.c)
	$(RM) $@
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(CFLAGS_OPTIONS_ALL) -c  $<


endif
