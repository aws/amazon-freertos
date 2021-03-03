export ASSET_TEMPLATE_ROOT=$1
export COVERITY_TOOLS=$ASSET_TEMPLATE_ROOT/coverity
[ "$OSTYPE" == "msys" ] && export COVERITY_ROOT="C:\Program Files\Coverity\Coverity Static Analysis"
[ "$OSTYPE" != "msys" ] && export COVERITY_ROOT=/gpfs/iot/projects/hnd_tools/linux/Coverity/cov-analysis-linux64-latest
export COVERITY_PATH=$COVERITY_ROOT/bin
export PATH=$COVERITY_PATH:$PATH
export COVERITY_HOST=iot-coverity.aus.cypress.com
export COVERITY_PORT=8080
export COVERITY_AUTHKEY=coverity-authkey
export COVERITY_DATADIR=cov-analyze-data
export COVERITY_CONFIG=./cov-config/coverity_config.xml
export RUN_COV_BUILD="cov-build --dir $COVERITY_DATADIR --config $COVERITY_CONFIG --return-emit-failures --emit-complementary-info"
export RUN_COV_ANALYZE="bash -e $COVERITY_TOOLS/coverity_analyze.sh"
export RUN_COV_POST_RESULTS="bash -e $COVERITY_TOOLS/coverity_post_results.sh"
echo "DATA: ${COVERITY_DATADIR}"