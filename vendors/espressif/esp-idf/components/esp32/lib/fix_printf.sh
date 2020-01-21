#!/bin/bash
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=phy_printf libphy.a 
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=rtc_printf librtc.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=wps_printf libwps.a 
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=wpa_printf libwpa.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=wpa2_printf libwpa2.a 
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=wpa2_printf libwpa2.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=sc_printf libsmartconfig.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=pp_printf libpp.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=net80211_printf libnet80211.a 
xtensa-esp32-elf-objcopy --redefine-sym printf=core_printf libcore.a 
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=core_printf libcore.a 
xtensa-esp32-elf-objcopy --redefine-sym ets_printf=coexist_printf libcoexist.a
xtensa-esp32-elf-objcopy --redefine-sym printf=coexist_printf libcoexist.a
