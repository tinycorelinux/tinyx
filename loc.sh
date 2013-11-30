#!/bin/sh

find $1 -name "*.c" -or -name "*.h" | xargs wc -l
