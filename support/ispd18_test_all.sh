#!/bin/bash

if [[ $# -lt 1 ]]; then
        echo "usage $0: benchmark_folder [max_parallel]"
        exit 1
fi

support_folder=$1
max_par=${2:-4}
export support_folder
export bin_path=./FRlefdef

cd "$(dirname "$0")/../" || exit

par_run()
{
        cur_test="$support_folder/ispd18_test${1}/ispd18_test${1}_generic_reader"
        "$bin_path" --no-gui --script "${cur_test}.rsyn" | tee "${cur_test}.log"
}
export -f par_run

command -v parallel >/dev/null
if [[ $? ]]
then
        for num in $(seq 1 10)
        do
                par_run "$num"
        done
else
        parallel -j"$max_par" par_run ::: "$(seq 1 10)"
fi
