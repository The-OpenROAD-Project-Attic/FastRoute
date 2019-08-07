#!/bin/bash

export support_dir="support/ispd18"

max_par=${1:-4}

cd "$(dirname "$0")/../"

cd "$support_dir" || exit

par_download()
{
        num=$1
        fname="ispd18_test${num}"
        if [[ ! -f "${fname}/${fname}.input.lef" ]]; then
                wget "http://www.ispd.cc/contests/18/${fname}.tgz"
                mkdir -p "${fname}"
                tar zxvf "${fname}.tgz" -C "./$fname/"
        fi
}
export -f par_download

command -v parallel >/dev/null
if [[ $? ]]
then
        for num in $(seq 1 10)
        do
                par_download "$num"
        done
else
        parallel par_download ::: "$(seq 1 10)"
fi

wait
