#!/bin/bash

targets=("component" "entity" "entityManager" "transform" "renderable" "camera" "selectable" "primitives")

for t in "${targets[@]}"; do
    echo "--------------------------------------"
    echo "▶️  Running test: $t"
    echo "--------------------------------------"
    make -f Makefile.tests "$t"
    echo "⏳ Waiting 5 seconds before next..."
    sleep 1
done
