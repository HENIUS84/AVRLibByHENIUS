message("cmake toolchain for AVR platform")

################################
# Compiler settings
################################

find_program(AVR_CC avr-gcc)
find_program(AVR_CXX avr-g++)
find_program(AVR_OBJCOPY avr-objcopy)
find_program(AVR_SIZE_TOOL avr-size)
find_program(AVR_OBJDUMP avr-objdump)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})

# Pass defines to compiler
add_definitions(
    -DF_CPU=${F_CPU}
    -DBAUD=${BAUD}
)

# MMCU MUST be passed to both the compiler and linker
add_compile_options(
	-mmcu=${MCU}        # MCU
	-std=gnu99          # C99 standard
	-Os                 # optimize
	-Wall               # enable warnings
	-Wno-main
	-Wundef
	#-pedantic
	-Wstrict-prototypes
	#-Werror
	#-Wfatal-errors
	-Wno-unused-variable
	-Wno-unused-function
	-Wl,--relax,--gc-sections
	-g
	-gdwarf-2
	-funsigned-char     # a few optimizations
	-funsigned-bitfields
	-fpack-struct
	-fshort-enums
	-ffunction-sections
	-fdata-sections
	-fno-split-wide-types
	-fno-tree-scev-cprop
)

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_FILES})

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy
                  -R .eeprom
				  -O ihex ${PRODUCT_NAME}.elf
				          ${PRODUCT_NAME}.hex DEPENDS strip)
# Transform binary into hex file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom avr-objcopy
                  -j .eeprom
				  --set-section-flags=.eeprom="alloc,load"
				  --change-section-lma .eeprom=0
				  -O ihex ${PRODUCT_NAME}.elf
				          ${PRODUCT_NAME}.eep DEPENDS strip)
						  
if (AVR_UPLOAD_ENABLED)
	# Upload the firmware with avrdude
	add_custom_target(upload avrdude 
	                  -c ${PROG_TYPE}
					  -p ${MCU}
					  -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

	# Upload the eeprom with avrdude
	add_custom_target(upload_eeprom avrdude
	                  -c ${PROG_TYPE}
					  -p ${MCU}
					  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

	# Burn fuses
	add_custom_target(fuses avrdude
	                  -c ${PROG_TYPE}
					  -p ${MCU}
					  -U lfuse:w:${L_FUSE}:m
					  -U hfuse:w:${H_FUSE}:m
					  -U efuse:w:${E_FUSE}:m
					  -U lock:w:${LOCK_BIT}:m )
endif()

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
                         "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")