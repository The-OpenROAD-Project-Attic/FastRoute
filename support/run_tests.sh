#!/usr/bin/env bash

bin_path=./src/FastRoute
log_folder=./logs
gold_folder=./logs_fr41

# bin_path=./best3/FastRoute
# log_folder=./logs_best3
# gold_folder=./logs_best3

data_folder=./best3

input_files=(a1 a2 a3 a5 b1 b2 b3 b4 n1 n2 n3 n4 n5 n6)

cd ..

cp "$bin_path" FastRoute

mkdir -p "$log_folder"
for f in "${input_files[@]}"
do
        ./FastRoute "$data_folder/$f" -o "$log_folder/$f.output" | grep -vi time &> "$log_folder/$f.log"
done

failed=0
suceeded=0
for f in "${input_files[@]}"
do
        out=$(diff -q "$log_folder/$f.log" "$gold_folder/$f.log")
        if [[ "$out" != "" ]]; then
                echo "Test $f failed"
                failed=$((failed+1))
        else
                echo "Test $f suceeded"
                suceeded=$((suceeded+1))
        fi
done

total=$((failed+suceeded))

cat << HEREDOC

###############################################################################
#                                 TEST REPORT                                 #
###############################################################################

Total tests $total
Failed = $failed ($((100*failed/total))%)
Suceeded = $suceeded ($((100*suceeded/total))%)

HEREDOC
