#!/bin/bash

support_dir="support/ispd18"
bin_path=./FRlefdef
test_num=1

cd "$(dirname "$0")/../$support_dir" || exit

fname="ispd18_test${test_num}"

if [[ ! -f "${fname}/${fname}.input.lef" ]] || [[ ! -f "${fname}/${fname}.input.def" ]]; then
        wget "http://www.ispd.cc/contests/18/${fname}.tgz"
        mkdir -p "${fname}"
        tar zxvf "${fname}.tgz" -C "./$fname/"
fi

"$bin_path" --no-gui --script "$support_dir/ispd18_test${test_num}/ispd18_test${test_num}_generic_reader.rsyn"
