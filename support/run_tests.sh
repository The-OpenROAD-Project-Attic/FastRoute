#!/usr/bin/env bash

bin_path=./fr41/FastRoute
# bin_path=./best3/FastRoute

log_folder=./logs
data_folder=./best3

input_files=(a1 a2 a3 a5 b1 b2 b3 b4 n1 n2 n3 n4 n5 n6)

cd ..

cp "$bin_path" FastRoute

mkdir -p "$log_folder"
for f in "${input_files[@]}"
do
        ./FastRoute "$data_folder/$f" -o "$log_folder/$f.output" | grep -vi time &> "$log_folder/$f.log"
done
