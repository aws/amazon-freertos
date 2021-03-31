#!/bin/sh

setup_idt()
{
    echo "Setup Python venv for IDT test "
    pushd ..
    python3 -m venv env
    source env/bin/activate
    pip3 --no-cache-dir install --upgrade pip
    pip3 install --upgrade --force-reinstall git+http://git-ore.aus.cypress.com/repo/cysecuretools.git@CYSECURETOOLS_3.1.0_CYBOOTLOADER_SWAP_SCRATCH_TC3
    pyocd --version
    pip3 install pyserial
    
    popd
}

