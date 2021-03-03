#!/bin/sh

setup_idt()
{
    echo "Setup Python venv for IDT test "
    pushd ..
    python3 -m venv env
    source env/bin/activate
    pip3 --no-cache-dir install --upgrade pip
    pip3 install cysecuretools==2.0.0
    pip3 install pyocd==0.27.0
    pyocd --version
    pip3 install pyserial
    
    popd
}

