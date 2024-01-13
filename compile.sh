#!/bin/bash
set -xe
KEYBOARD=sparrow60c
KEYMAP=74th
qmk compile -kb $KEYBOARD -km $KEYMAP
scp .build/${KEYBOARD}_${KEYMAP}.uf2 nnyn@192.168.1.196:/Users/nnyn/Desktop/
