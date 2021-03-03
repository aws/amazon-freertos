parse_yaml() {
   local prefix=$2
   local s='[[:space:]]*' w='[a-zA-Z0-9_]*' fs=$(echo @|tr @ '\034')
   sed -ne "s|^\($s\)\($w\)$s:$s\"\(.*\)\"$s\$|\1$fs\2$fs\3|p" \
        -e "s|^\($s\)\($w\)$s:$s\(.*\)$s\$|\1$fs\2$fs\3|p"  $1 |
   awk -F$fs '{
      indent = length($1)/2;
      vname[indent] = $2;
      for (i in vname) {if (i > indent) {delete vname[i]}}
      if (length($3) > 0) {
         vn=""; for (i=0; i<indent; i++) {vn=(vn)(vname[i])("_")}
         printf("%s%s%s=\"%s\"\n", "'$prefix'",vn, $2, $3);
      }
   }'
}

if [[ $# -lt 1 ]]
then
    echo "Usage: coverity_post_results.sh <configuration_file>"
    exit 0
fi
echo "+ $0 $1"
eval $(parse_yaml $1 "THIS_COVERITY_")
# This is the list of parameters from .coverity_analyze.config
# THIS_COVERITY_DATA
# THIS_COVERITY_PROJECT
# THIS_COVERITY_STREAM
# THIS_COVERITY_CODING_STANDARD (optional)
# THIS_COVERITY_ENABLE_CHECKERS (optional)
# THIS_COVERITY_DISABLE_CHECKERS (optional)
# THIS_COVERITY_SOURCE_ROOT (optional)
# THIS_COVERITY_SOURCE_FILTER (optional)

if [ "$OSTYPE" != "msys" ]
then
    # Ubuntu or MacOS
    THIS_PWD=${0%/*}
    THIS_ROOT=${THIS_PWD%/*}
    COVERITY_AUTHKEY=$THIS_PWD/$COVERITY_AUTHKEY
else
    # Windows
    THIS_PWD=${0%\\*}
    THIS_ROOT=${THIS_PWD%\\*}
    COVERITY_AUTHKEY=$THIS_PWD\\$COVERITY_AUTHKEY
    COVERITY_ROOT=${COVERITY_PATH%\\*}
    COVERITY_ROOT=${COVERITY_ROOT/\"}
fi

# Create the log file
THIS_LOGFILE=cov-commit-defects-log-$THIS_COVERITY_STREAM.txt
touch $THIS_LOGFILE

# Commit the analysis results to the coverity server
# Check if $THIS_COVERITY_PROJECT exists
COV_MANAGE_IM_START=$(date +%s)
set +e
project_query=$(cov-manage-im --host $COVERITY_HOST --port $COVERITY_PORT \
    --auth-key-file $COVERITY_AUTHKEY --mode projects --show --name $THIS_COVERITY_PROJECT \
    --fields Project 2>/dev/null)
set -e
if [ "$project_query" == "Project" ]
then
    # Create $THIS_COVERITY_PROJECT if it does not exist
    echo "THIS_COVERITY_PROJECT=$THIS_COVERITY_PROJECT [Create New]"
    (echo; (set -x; cov-manage-im --host $COVERITY_HOST --port $COVERITY_PORT --auth-key-file $COVERITY_AUTHKEY \
    --mode projects --add --set name:$THIS_COVERITY_PROJECT) 2>&1 | tee -a $THIS_LOGFILE)
else echo "THIS_COVERITY_PROJECT=$THIS_COVERITY_PROJECT [Use Existing]" 2>&1 | tee -a $THIS_LOGFILE
fi

# Check if $THIS_COVERITY_STREAM exists
set +e
stream_query=$(cov-manage-im --host $COVERITY_HOST --port $COVERITY_PORT \
    --auth-key-file $COVERITY_AUTHKEY --mode streams --show --name $THIS_COVERITY_STREAM \
    --fields Stream 2>/dev/null)
set -e
if [ "$stream_query" == "Stream" ]
then
    # Create $THIS_COVERITY_STREAM if it does not exist
    echo "THIS_COVERITY_STREAM=$THIS_COVERITY_STREAM [Create New]"
    (echo; (set -x; cov-manage-im --host $COVERITY_HOST --port $COVERITY_PORT --auth-key-file $COVERITY_AUTHKEY \
    --mode streams --add --set lang:cpp \
    --set name:$THIS_COVERITY_STREAM) 2>&1 | tee -a $THIS_LOGFILE)
else echo "THIS_COVERITY_STREAM=$THIS_COVERITY_STREAM [Use Existing]" 2>&1 | tee -a $THIS_LOGFILE
fi

# Attach $THIS_COVERITY_STREAM to $THIS_COVERITY_PROJECT
(echo; (set -x; cov-manage-im --host $COVERITY_HOST --port $COVERITY_PORT --auth-key-file $COVERITY_AUTHKEY \
    --mode projects --name $THIS_COVERITY_PROJECT --update \
    --insert stream:$THIS_COVERITY_STREAM) 2>&1 | tee -a $THIS_LOGFILE)
COV_MANAGE_IM_FINISH=$(date +%s)

# Commit the defects to the $COVERITY_HOST
COV_COMMIT_START=$(date +%s)
(echo; (set -x; cov-commit-defects --verbose 0 --dir $THIS_COVERITY_DATA \
    --host $COVERITY_HOST --port $COVERITY_PORT --auth-key-file $COVERITY_AUTHKEY \
    --stream $THIS_COVERITY_STREAM ) 2>&1 | tee -a $THIS_LOGFILE)
COV_COMMIT_FINISH=$(date +%s)

echo "=TIME= cov-manage-im:       $(($COV_MANAGE_IM_FINISH - $COV_MANAGE_IM_START)) sec"
echo "=TIME= cov-commit-defects:  $(($COV_COMMIT_FINISH - $COV_COMMIT_START)) sec"
