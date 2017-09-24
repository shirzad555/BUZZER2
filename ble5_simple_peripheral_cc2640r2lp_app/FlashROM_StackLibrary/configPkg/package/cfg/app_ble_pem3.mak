#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.arm.elf.M3{1,0,16.9,3
#
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/app_ble_pem3.oem3.dep
package/cfg/app_ble_pem3.oem3.dep: ;
endif

package/cfg/app_ble_pem3.oem3: | .interfaces
package/cfg/app_ble_pem3.oem3: package/cfg/app_ble_pem3.c package/cfg/app_ble_pem3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem3 $< ...
	$(ti.targets.arm.elf.M3.rootDir)/bin/armcl -c  -mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/knl" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/boards/SABLEXR2_DEV_BOARD" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/family/arm/m3" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Hardware" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=NPI_USE_UART --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=SABLEXR2_DEV_BOARD --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=TBM_ACTIVE_ITEMS_ONLY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_9_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem3 $< -C   -mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/knl" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/boards/SABLEXR2_DEV_BOARD" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/family/arm/m3" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Hardware" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=NPI_USE_UART --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=SABLEXR2_DEV_BOARD --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=TBM_ACTIVE_ITEMS_ONLY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_9_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/app_ble_pem3.oem3: export C_DIR=
package/cfg/app_ble_pem3.oem3: PATH:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)
package/cfg/app_ble_pem3.oem3: Path:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)

package/cfg/app_ble_pem3.sem3: | .interfaces
package/cfg/app_ble_pem3.sem3: package/cfg/app_ble_pem3.c package/cfg/app_ble_pem3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem3 -n $< ...
	$(ti.targets.arm.elf.M3.rootDir)/bin/armcl -c -n -s --symdebug:none -mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/knl" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/boards/SABLEXR2_DEV_BOARD" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/family/arm/m3" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Hardware" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=NPI_USE_UART --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=SABLEXR2_DEV_BOARD --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=TBM_ACTIVE_ITEMS_ONLY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_9_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem3 $< -C  -n -s --symdebug:none -mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/knl" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/boards/SABLEXR2_DEV_BOARD" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/family/arm/m3" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Application" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Hardware" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Startup" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/PROFILES" --include_path="C:/Users/Shirzad/Documents/Shirzad/Work/PC_BUZZER/FW/SaBLE_x_R2/X1/ble5_simple_peripheral_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=NPI_USE_UART --define=DeviceFamily_CC26X0R2 --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=1 --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=SABLEXR2_DEV_BOARD --define=CC26XX --define=CC26XX_R2 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=TBM_ACTIVE_ITEMS_ONLY --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_9_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/app_ble_pem3.sem3: export C_DIR=
package/cfg/app_ble_pem3.sem3: PATH:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)
package/cfg/app_ble_pem3.sem3: Path:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)

clean,em3 ::
	-$(RM) package/cfg/app_ble_pem3.oem3
	-$(RM) package/cfg/app_ble_pem3.sem3

app_ble.pem3: package/cfg/app_ble_pem3.oem3 package/cfg/app_ble_pem3.mak

clean::
	-$(RM) package/cfg/app_ble_pem3.mak
