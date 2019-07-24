#!/bin/bash
cd "$(dirname "$0")/../"
BENCHMAKR_DIR="${1:-benchmarks}"
mkdir -p "$BENCHMAKR_DIR"
cd "$BENCHMAKR_DIR" || exit

par_download()
{
        ls
        pwd
        # num=$1
        # fname="ispd18_test${num}"
        # wget "http://www.ispd.cc/contests/18/${fname}.tgz"
        # mkdir -p "${fname}"
        # tar zxvf "${fname}.tgz" -C "./$fname/"
}
export -f par_download

if [[ $(command -v parallel >/dev/null) ]]
then
        parallel par_download ::: "$(seq 1 10)"
else
        for num in $(seq 1 10)
        do
                fname="ispd18_test${num}"
                wget "http://www.ispd.cc/contests/18/${fname}.tgz"
                mkdir -p "${fname}"
                tar zxvf "${fname}.tgz" -C "./$fname/"
        done
fi

wait

mkdir tar
mv ./*.tar tar/
