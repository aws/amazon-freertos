if [[ $# -lt 1 ]]
then
    echo "Usage: coverity_analyze.sh <configuration_file>"
    exit 0
fi
echo "+ $0 $1"
toolpath=${0%/*}
eval $(python3 ${toolpath}/parse_config_file.py $1 "THIS_COVERITY_")
# This is the list of parameters from configuration_file
# THIS_COVERITY_DATA
# THIS_COVERITY_PROJECT
# THIS_COVERITY_STREAM
# THIS_COVERITY_CODING_STANDARD (optional)
# THIS_COVERITY_ENABLE_CHECKERS (optional)
# THIS_COVERITY_DISABLE_CHECKERS (optional)
# THIS_COVERITY_SOURCE_ROOT (optional)
# THIS_COVERITY_SOURCE_FILTER (optional)
# THIS_COVERITY_ANALYZE_OPTS (optional)
echo "THIS_COVERITY_DATA: ${THIS_COVERITY_DATA}"
echo "THIS_COVERITY_PROJECT: ${THIS_COVERITY_PROJECT}"
echo "THIS_COVERITY_STREAM: ${THIS_COVERITY_STREAM}"
echo "THIS_COVERITY_CODING_STANDARD: ${THIS_COVERITY_CODING_STANDARD}"
echo "THIS_COVERITY_ENABLE_CHECKERS: ${THIS_COVERITY_ENABLE_CHECKERS}"
echo "THIS_COVERITY_DISABLE_CHECKERS: ${THIS_COVERITY_DISABLE_CHECKERS}"
echo "THIS_COVERITY_SOURCE_ROOT: ${THIS_COVERITY_SOURCE_ROOT}"
echo "THIS_COVERITY_SOURCE_FILTER: ${THIS_COVERITY_SOURCE_FILTER}"
echo "THIS_COVERITY_ANALYZE_OPTS: ${THIS_COVERITY_ANALYZE_OPTS}"

#if [ "$OSTYPE" != "msys" ]
#then
    # Ubuntu or MacOS
    THIS_PWD=${0%/*}
    THIS_ROOT=${THIS_PWD%/*}
    COVERITY_AUTHKEY=$THIS_PWD/$COVERITY_AUTHKEY
#else
    # Windows
#    THIS_PWD=${0%\\*}
#    THIS_ROOT=${THIS_PWD%\\*}
#    COVERITY_AUTHKEY=$THIS_PWD\\$COVERITY_AUTHKEY
#    COVERITY_ROOT=${COVERITY_PATH%\\*}
#    COVERITY_ROOT=${COVERITY_ROOT/\"}
#fi

SOURCE_FILTERING=0
if [ "$THIS_COVERITY_SOURCE_FILTER" != "" ]
then
   SOURCE_FILTERING=1
fi

COV_STRIP_PATH_OPT=""
if [ "$THIS_COVERITY_SOURCE_ROOT" != "" ]
then
    COV_STRIP_PATH_OPT="--strip-path $THIS_COVERITY_SOURCE_ROOT"
fi
(set -x; chmod 600 $COVERITY_AUTHKEY)

# Analyze the code
COV_CODING_STD=""
if [ "$THIS_COVERITY_CODING_STANDARD" != "" ]
then
    if [ "$OSTYPE" == "msys" ]
    then
        COV_CODING_STD="--coding-standard-config \"$THIS_COVERITY_CODING_STANDARD\""
    else
        COV_CODING_STD="--coding-standard-config $THIS_COVERITY_CODING_STANDARD"
    fi
fi

COV_CUSTOM_CHECKERS=""
if [ "$THIS_COVERITY_CUSTOM_CHECKER" != "" ]
then
    for checker in $THIS_COVERITY_CUSTOM_CHECKER
    do
        COV_CUSTOM_CHECKERS+=" --codexm ${checker}"
    done
fi

COV_MANAGE_EMIT="cov-manage-emit --dir $THIS_COVERITY_DATA"
echo; echo "Translation Units Analyzed:"
if [ $SOURCE_FILTERING -eq 1 ]
then
    echo "+ $COV_MANAGE_EMIT --tu-pattern \"$THIS_COVERITY_SOURCE_FILTER\" list"
    ($COV_MANAGE_EMIT \
    --tu-pattern "$THIS_COVERITY_SOURCE_FILTER" \
    list | grep -v "Translation unit:" | awk '{$1=$2=""}1' | sed "s|$THIS_COVERITY_SOURCE_ROOT/||g")
else
    echo "+ $COV_MANAGE_EMIT list"
    ($COV_MANAGE_EMIT \
    list | grep -v "Translation unit:" | awk '{$1=$2=""}1' | sed "s|$THIS_COVERITY_SOURCE_ROOT/||g")
fi
COV_ANALYZE_START=$(date +%s)
if  [ $SOURCE_FILTERING -eq 1 ]
then
    (echo; (set -x; cov-analyze $COV_STRIP_PATH_OPT --dir $THIS_COVERITY_DATA \
            --tu-pattern "$THIS_COVERITY_SOURCE_FILTER" \
            $THIS_COVERITY_ANALYZE_OPTS $COV_CUSTOM_CHECKERS \
            $THIS_COVERITY_ENABLE_CHECKERS $THIS_COVERITY_DISABLE_CHECKERS \
            $COV_CODING_STD) 2>&1 | tee cov-analyze-log-$THIS_COVERITY_STREAM.txt)
else
    (echo; (set -x; cov-analyze $COV_STRIP_PATH_OPT --dir $THIS_COVERITY_DATA \
            $THIS_COVERITY_ANALYZE_OPTS $COV_CUSTOM_CHECKERS \
            $THIS_COVERITY_ENABLE_CHECKERS $THIS_COVERITY_DISABLE_CHECKERS \
            $COV_CODING_STD) 2>&1 | tee cov-analyze-log-$THIS_COVERITY_STREAM.txt)
fi
COV_ANALYZE_FINISH=$(date +%s)
# Get a list of the defects

ERRORLIST_FILE=cov-analyze-errorlist-$THIS_COVERITY_STREAM.txt
(echo; set -x; (cov-format-errors --dir $THIS_COVERITY_DATA --emacs-style \
                &> $ERRORLIST_FILE))
echo " output in $ERRORLIST_FILE"
(set -x; grep -c "Type:" $ERRORLIST_FILE)
echo "=TIME= cov-analyze:         $(($COV_ANALYZE_FINISH - $COV_ANALYZE_START)) sec"
