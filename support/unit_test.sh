#!/bin/bash

cd "$(dirname "$0")/../"


par_run()
{
        fname=""
        ./FRlefdef --no-gui --script "./benchmarks/ispd18_test$1/ispd18_test$1_generic_reader.rsyn"
}
export -f par_run

if [[ ! "$(command -v parallel >/dev/null)" ]]
then
        parallel par_run ::: "$(seq 1 10)"
else
        for num in $(seq 1 10)
        do
                fname="ispd18_test${num}"
                ./FRlefdef --no-gui --script "./benchmarks/${fname}/${fname}_generic_reader.rsyn"
        done
fi
