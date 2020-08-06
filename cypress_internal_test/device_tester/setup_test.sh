idt_ver=$1
pushd "$(dirname "${BASH_SOURCE[0]}")/.."

source setenv.sh
run_setenv $idt_ver

popd


AFR_DEVICE_TESTER_DIR_NAME=$(basename $AFR_DEVICE_TESTER_DIR)
# The device tester directory cannot be contained in the afr directroy.
# Device tester create a copy of amazon-freertos and put in the device tester directory.
# If device tester is in the amazon-freertos directory, it will cause a recursive infinite copy.
# Use the user home directory.
rm -rf ~/device_tester
mkdir ~/device_tester
cp -r $AFR_DEVICE_TESTER_DIR ~/device_tester/$AFR_DEVICE_TESTER_DIR_NAME
