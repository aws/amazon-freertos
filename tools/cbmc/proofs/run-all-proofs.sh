#!/bin/sh

PROOFS_DIR=$(cd "$(dirname "$0")" || exit 1 ; pwd)
LITANI="${PROOFS_DIR}/../litani/litani"

"${PROOFS_DIR}"/prepare.py
if [ "$?" -ne 0 ]; then exit 1; fi

"${LITANI}" init --project "Amazon FreeRTOS"
if [ "$?" -ne 0 ]; then exit 1; fi

N_PROOFS=$(find "${PROOFS_DIR}" -name cbmc-batch.yaml | wc -l)
CNT=0

echo "Configuring..."
find "${PROOFS_DIR}" -name cbmc-batch.yaml | while read -r yaml; do
  PROOF_PATH=$(dirname "${yaml}")

  HARNESS=$(ls "${PROOF_PATH}"/*_harness.c)
  PROOF_NAME=$(
    basename "${HARNESS}" |
    rev                   |
    cut -c 11-            | # Trim off "_harness.c"
    rev
  )

  CNT=$(echo "${CNT} + 1" | bc)
  printf "\r                                                           "
  printf "\r%d/%d %s" "${CNT}" "${N_PROOFS}" "${PROOF_NAME}"

  N_HARNESSES=$(find "${PROOF_PATH}" -name "*_harness.c" | wc -l)
  if [ "${N_HARNESSES}" -ne 1 ]; then
    (>&2 echo "Found ${N_HARNESSES} harnesses in proof directory ${PROOF_PATH}")
    exit 1
  fi

  # Build goto binary

  GOTO_BINARY="${PROOF_PATH}/${PROOF_NAME}_harness.goto"
  "${LITANI}" add-job \
    --command "make -B goto" \
    --outputs "${GOTO_BINARY}" \
    --pipeline-name "${PROOF_NAME}" \
    --ci-stage "build" \
    --cwd "${PROOF_PATH}" \
    --description "${PROOF_NAME}: building goto-binary"
  if [ "$?" -ne 0 ]; then exit 1; fi


  # Run 3 CBMC jobs

  SAFETY="${PROOF_PATH}/cbmc.txt"
  PROPERTY="${PROOF_PATH}/property.xml"
  COVERAGE="${PROOF_PATH}/coverage.xml"

  "${LITANI}" add-job \
    --command "make cbmc" \
    --inputs "${GOTO_BINARY}" \
    --outputs "${SAFETY}" \
    --pipeline-name "${PROOF_NAME}" \
    --ci-stage "test" \
    --cwd "${PROOF_PATH}" \
    --description "${PROOF_NAME}: running safety checks"
  if [ "$?" -ne 0 ]; then exit 1; fi

  "${LITANI}" add-job \
    --command "make property" \
    --inputs "${GOTO_BINARY}" \
    --outputs "${PROPERTY}" \
    --pipeline-name "${PROOF_NAME}" \
    --ci-stage "test" \
    --cwd "${PROOF_PATH}" \
    --description "${PROOF_NAME}: printing properties"
  if [ "$?" -ne 0 ]; then exit 1; fi

  "${LITANI}" add-job \
    --command "make coverage" \
    --inputs "${GOTO_BINARY}" \
    --outputs "${COVERAGE}" \
    --pipeline-name "${PROOF_NAME}" \
    --ci-stage "test" \
    --cwd "${PROOF_PATH}" \
    --description "${PROOF_NAME}: computing coverage"
  if [ "$?" -ne 0 ]; then exit 1; fi


  # Generate report

  "${LITANI}" add-job \
    --command "make report" \
    --inputs "${SAFETY}" "${COVERAGE}" "${PROPERTY}" \
    --outputs "${PROOF_PATH}/html" \
    --pipeline-name "${PROOF_NAME}" \
    --ci-stage "report" \
    --cwd "${PROOF_PATH}" \
    --description "${PROOF_NAME}: generating report"
  if [ "$?" -ne 0 ]; then exit 1; fi

done
if [ "$?" -ne 0 ]; then exit 1; fi

echo ""

"${LITANI}" run-build
if [ "$?" -ne 0 ]; then exit 1; fi
