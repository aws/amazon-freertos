#!/bin/bash

rm -rf ../../../.git/hooks/*
ln src/pre_commit.py ../../../.git/hooks/pre-commit
ln src/commit_msg.py ../../../.git/hooks/commit-msg
