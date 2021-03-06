#!/bin/sh

SCRIPT_DIR=$(cd $(dirname $0); pwd)
TARGET_DIR="${SCRIPT_DIR}/target"
RESOURCES_DIR="${SCRIPT_DIR}/src/main/resources"
EXECUTABLE="${TARGET_DIR}/cg1_ex04"

cd "${SCRIPT_DIR}"

if [ ! -f "${EXECUTABLE}" ]; then
	./build.sh
fi

cd "${TARGET_DIR}"

if [ ! -e data ]; then
	ln -s ../src/data data
fi

if [ ! -e meshes ]; then
	if [ ! -e ../src/meshes ]; then
		ln -s ../../ex03/src/meshes ../src/meshes
	fi
	ln -s ../src/meshes meshes
fi

ARGUMENTS=${2} ${3} ${4}

if [ "${1}" = "debug" ]; then
	echo "ARGUMENTS: ${ARGUMENTS}"
	ddd "${EXECUTABLE}" &
else
	"${EXECUTABLE}" ${ARGUMENTS}
fi

