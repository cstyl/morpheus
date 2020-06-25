#!/bin/bash

SCRIPT_PATH="$1"
MATRIX_DIR="$2"
RESULTS_PATH="$3"
PROGRESS_FILE="$4"
VERSION="$5"
BINARY="$6"
REPS="$7"
SPMV_ITER="$8"

for matdir in "$MATRIX_DIR"/*/
do
  BASE=$(basename $matdir)
  DIR=$(dirname $matdir)
  MATRIX="$DIR/$BASE/$BASE.mtx"

  echo -e "\t$BASE" 2>&1 | tee -a "$PROGRESS_FILE"

  for rep in `seq -w 1 $REPS`
    do
      echo -e "\t\t$rep" 2>&1 | tee -a "$PROGRESS_FILE"
      outdir="$RESULTS_PATH/$BASE/$VERSION/$rep"
      mkdir -p "$outdir"
      "$BINARY" "$MATRIX" "$outdir" "$SPMV_ITER" 2> >(tee -a "$PROGRESS_FILE") 1> >(tee "$outdir/output.txt")
    done

done