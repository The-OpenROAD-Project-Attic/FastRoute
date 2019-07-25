#!/bin/bash

support_folder=$1
export support_folder
export bin_path=./FRlefdef

cd "$(dirname "$0")/../" || exit

par_run()
{
        "$bin_path" --no-gui --script "$support_folder/ispd18_test${1}/ispd18_test${1}_generic_reader.rsyn"
}
export -f par_run

if [[ ! "$(command -v parallel >/dev/null)" ]]
then
        parallel par_run ::: "$(seq 1 10)"
else
        for num in $(seq 1 10)
        do
                par_run "$num"
        done
fi
