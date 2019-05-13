#/bin/sh
gcc `echo $@ | sed -e 's/\/cygdrive\/\([a-zA-Z]\)\//\1:\//g'`
