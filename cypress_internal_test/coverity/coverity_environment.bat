@set COVERITY_TOOLS=%~dp0#
@set COVERITY_TOOLS=%COVERITY_TOOLS:\#=%
@set COVERITY_ROOT=C:\Program Files\Coverity\Coverity Static Analysis
@set COVERITY_PATH=%COVERITY_ROOT%\bin
@set PATH=%PATH%;%COVERITY_PATH%
@set COVERITY_HOST=iot-coverity.aus.cypress.com
@set COVERITY_PORT=8080
@set COVERITY_AUTHKEY=coverity-authkey
@set COVERITY_DATADIR=cov-analyze-data
@set COVERITY_CONFIG=".\cov-config\coverity_config.xml"
@echo DATA: %COVERITY_DATADIR%