#!/bin/sh

st-util &
PID=$!

${CROSS_COMPILE}gdb -x gdbrc build/image.elf

echo "Killing st-util"
kill $PID
