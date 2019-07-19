#!/usr/bin/env bash

################################################################################
## Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora Oliveira
##          (Advisor: Ricardo Reis)
##
## BSD 3-Clause License
##
## Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and#or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
################################################################################

bin_path=./src/FastRoute
log_folder=./logs
gold_folder=./logs_gold

data_folder=./data

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
                diff "$log_folder/$f.log" "$gold_folder/$f.log" > "$log_folder/$f.diff"
                echo "Test $f failed"
                failed=$((failed+1))
        else
                echo "Test $f suceeded"
                suceeded=$((suceeded+1))
        fi
        rm "$log_folder/$f.output"
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
