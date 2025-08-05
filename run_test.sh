#!/bin/bash

EXEC=./eval_test
TESTDIR=./test/VettoriTest

for file in "$TESTDIR"/*.txt; do
  echo "#####################################################################"
  echo "Testing $file ..."
  $EXEC "$file"
  if [ $? -eq 0 ]; then
    echo "PASS: $file"
  else
    echo "FAIL: $file"
  fi
done
echo "#####################################################################"