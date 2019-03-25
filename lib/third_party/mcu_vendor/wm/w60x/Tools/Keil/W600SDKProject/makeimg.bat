@rem 参数0: exe
@rem 参数1: 输入bin文件 ,原始文件或者压缩档文件
@rem 参数2: 输出文件(目标生成文件）
@rem 参数3: 输入文件类型,0是image文件，1是FLASHBOOT文件 2是secboot文件
@rem 参数4: 是否压缩文件：0：plain文件，1：压缩类型文件
@rem 参数5: 版本号文件
@rem 参数6：升级文件再FLASH里的存放位置（相对位置）
@rem 参数7：升级后的文件启动位置（相对位置）
@rem 参数8：原始bin文件

@echo off

copy WM_W600.map ..\..\..\Bin
copy objs\WM_W600.bin ..\..\..\Bin
cd ..\..\..\Tools

copy ..\Bin\version.txt ..\Bin\version_bk.txt

wm_gzip.exe "..\Bin\WM_W600.bin"
makeimg.exe "..\Bin\WM_W600.bin" "..\Bin\WM_W600.img" 0 0 "..\Bin\version.txt" 90000 10100
makeimg.exe "..\Bin\WM_W600.bin.gz" "..\Bin\WM_W600_GZ.img" 0 1 "..\Bin\version.txt" 90000 10100 "..\Bin\WM_W600.bin" 
makeimg.exe "..\Bin\WM_W600.bin" "..\Bin\WM_W600_SEC.img" 0 0 "..\Bin\version.txt" 90000 10100
makeimg_all.exe "..\Bin\secboot.img" "..\Bin\WM_W600.img" "..\Bin\WM_W600.FLS"
@del "..\Bin\WM_W600.img"


