#!/bin/bash

function check_exist()
{
    file_name=$1
    log_file=$2
    
    grep "$file_name" $log_file
    if [ "$?" -ne 0 ]
    then
        echo "$file_name not found"
        exit 1
    fi
}

function check_not_exist()
{
    file_name=$1
    log_file=$2
    
    grep "$file_name" $log_file
    if [ "$?" -eq 0 ]
    then
        echo "$file_name found when it should not be"
        exit 1
    fi
}