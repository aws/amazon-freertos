#!/bin/sh

setup_idt()
{
    echo "Setup Python venv for IDT test "
    pushd ..
    python3 -m venv env
    source env/bin/activate
    pip --no-cache-dir install --upgrade pip
    pip install cysecuretools==2.0.0 
    popd
}

