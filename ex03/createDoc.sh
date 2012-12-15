#!/bin/sh

SCRIPT_DIR=$(cd $(dirname $0); pwd)
TARGET_DIR="${SCRIPT_DIR}/target"
DOC_DIR="${SCRIPT_DIR}/doc"

cd "${SCRIPT_DIR}"

cd "${DOC_DIR}"
pdflatex \
		-interaction=nonstopmode \
		-output-format=pdf \
		-output-directory="${TARGET_DIR}" \
		doc.tex

cd "${TARGET_DIR}"
mv doc.pdf CG1_WS1213_Gruppe6_Ex03.pdf

