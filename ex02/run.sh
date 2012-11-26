#!/bin/sh

SCRIPT_DIR=$(cd $(dirname $0); pwd)
TARGET_DIR="${SCRIPT_DIR}/target"
RESOURCES_DIR="${SCRIPT_DIR}/src/main/resources"
EXECUTABLE="${TARGET_DIR}/cg1_ex02"

cd "${SCRIPT_DIR}"

if [ ! -f "${EXECUTABLE}" ]; then
	./build.sh
fi

cd "${TARGET_DIR}"

if [ ! -e data ]; then
	ln -s ../src/data data
fi

"${EXECUTABLE}"

