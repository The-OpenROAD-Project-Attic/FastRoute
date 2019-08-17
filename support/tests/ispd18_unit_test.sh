#!/bin/bash

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


cd "$(dirname "$0")"/../../ || exit

bin_path=./FRlefdef
test_name="ispd18_test1"

base_dir="$PWD"
support_dir="$base_dir/support"
test_dir="$support_dir/ispd18/${test_name}"

output_file="${test_dir}/${test_name}.guide"
gold_dir="${support_dir}/gold"
gold_file="${gold_dir}/${test_name}.guide"

if [[ ! -f "${test_dir}/${test_name}.input.lef" ]] || [[ ! -f "${test_dir}/${test_name}.input.def" ]]; then
        mkdir -p "${test_dir}"
        cd "$test_dir" || exit
        wget "http://www.ispd.cc/contests/18/${test_name}.tgz"
        tar zxvf "${test_name}.tgz"
        cd - || exit
fi

cd "${base_dir}" || exit

fail()
{
        echo
        echo
        echo "Unit test failed, please check end of log file"
        echo
        echo
        diff  "${output_file}" "${gold_file}" >> "$base_dir/unit_test.log"
        exit 1
}

"$bin_path" --no-gui --script "${support_dir}/rsyn/${test_name}.rsyn" > "$base_dir/unit_test.log"

if [[ $? != 0 ]]; then
        echo "Runtime error"
        fail
fi

cmp "${output_file}" "${gold_file}"

if [[ $? != 0 ]]; then
        echo "Output missmatch"
        fail
fi

echo
echo
echo "Passed unit test"
echo
echo
