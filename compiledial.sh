#!/bin/bash
set -xe
KEYBOARD=sparrowdial
KEYMAP=default
qmk compile -kb $KEYBOARD -km $KEYMAP
scp .build/${KEYBOARD}_${KEYMAP}.uf2 nnyn@192.168.1.196:/Users/nnyn/Desktop/
