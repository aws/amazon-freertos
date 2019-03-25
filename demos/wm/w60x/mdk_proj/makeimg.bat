@rem 参数0: exe
@rem 参数1: 输入bin文件 ,原始文件或者压缩档文件
@rem 参数2 :输出文件 
@rem 参数3:  输入文件类型,0是image文件，1是FLASHBOOT文件 2是secboot文件
@rem 参数4: 是否压缩文件：0：plain文件，1：压缩类型文件
@rem 参数5: 版本号文件，执行完exe之后版本号会自动累加
@rem 参数6：存放位置
@rem 参数7：原始bin文件
@rem 参数8：压缩文件的解压位置

@echo off

copy helloworld.map ..\Bin\WM_W600.map
copy objs\helloworld.bin ..\Bin\WM_W600.bin
cd ..\Bin

copy ..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\version.txt ..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\version_bk.txt
..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Tools\wm_gzip.exe "WM_W600.bin"
..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Tools\makeimg.exe "WM_W600.bin" "WM_W600.img" 0 0 "..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\version.txt" 90000 10100
..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Tools\makeimg.exe "WM_W600.bin.gz" "WM_W600_GZ.img" 0 1 "..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\version.txt" 90000 10100 "WM_W600.bin" 
..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Tools\makeimg.exe "WM_W600.bin" "WM_W600_SEC.img" 0 0 "..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\version.txt" 90000 10100
..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Tools\makeimg_all.exe "..\..\..\..\lib\third_party\mcu_vendor\wm\w60x\Bin\secboot.img" "WM_W600.img" "WM_W600.FLS"
@del "WM_W600.img"
