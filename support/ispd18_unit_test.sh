#!/bin/bash

bin_path=./FRlefdef
test_num=1

repo_base_dir=$(realpath "$(dirname "$0")"/../)
support_dir="$repo_base_dir/support/ispd18"

cd "$support_dir" || exit

fname="ispd18_test${test_num}"
test_path="$support_dir/${fname}"

if [[ ! -f "${fname}/${fname}.input.lef" ]] || [[ ! -f "${fname}/${fname}.input.def" ]]; then
        wget "http://www.ispd.cc/contests/18/${fname}.tgz"
        mkdir -p "${fname}"
        tar zxvf "${fname}.tgz" -C "./$fname/"
fi

cd "${repo_base_dir}" || exit

fail()
{
        echo
        echo
        echo "Unit test failed, please check end of log file"
        echo
        echo
        diff "${test_path}/${fname}.guide" "$support_dir/ispd18_unit_test_golden.guide" >> "$repo_base_dir/unit_test.log"
        exit 1
}

"$bin_path" --no-gui --script "${test_path}/${fname}_generic_reader.rsyn" > "$repo_base_dir/unit_test.log"

if [[ $? != 0 ]]; then
        echo "Runtime error"
        fail
fi

cmp "${test_path}/${fname}.guide" "$support_dir/ispd18_unit_test_golden.guide"

if [[ $? != 0 ]]; then
        echo "Output missmatch"
        fail
fi

echo
echo
echo "Passed unit test"
echo
echo
