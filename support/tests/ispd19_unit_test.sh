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

echo
echo "Start unit tests..."
echo

FRlefdef_bin=$1
bin_path=${FRlefdef_bin:-./FRlefdef}
test_name="ispd19_test7"

base_dir="$PWD"
support_dir="$base_dir/support"
test_dir="$support_dir/ispd19/"

output_file="${test_dir}/${test_name}/${test_name}.guide"
gold_dir="${support_dir}/gold"
gold_file="${gold_dir}/${test_name}.guide"

guides_validation="guides_validation.log"
gold_guides="complete post process guides ..."

gold_wl="${gold_dir}/${test_name}.wl"
grep_pattern="Final routing length"
log_file="unit_test.log"

if [[ ! -f "${test_dir}/${test_name}/${test_name}.input.lef" ]] || [[ ! -f "${test_dir}/${test_name}/${test_name}.input.def" ]]; then
        echo "Downloading test files... from ${base_dir}"
        mkdir -p "${test_dir}"
        cd "$test_dir" || exit
        wget "http://www.ispd.cc/contests/19/benchmarks/${test_name}.tgz"
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
        diff  "${output_file}" "${gold_file}" >> "$base_dir/${log_file}"
        exit 1
}

"$bin_path" --no-gui --script "${support_dir}/rsyn/${test_name}.rsyn" > "$base_dir/${log_file}"

if [[ $? != 0 ]]; then
        echo "Runtime error"
        fail
fi


# Test 1: compare output guides
echo
echo "--Compare guides..."
echo

cmp "${output_file}" "${gold_file}"

if [[ $? != 0 ]]; then
        echo "--ERROR: Compare guides: Output guides missmatch"
        fail
fi

echo
echo "--Compare guides: Success!"
echo

#Test 2: check pins coverage
echo
echo "--Check pin coverage..."
echo

cd "${support_dir}"

if [[ ! -f "${support_dir}/FlexRoute" ]];
then
        # TODO: change download link to avoid permission issues with binary
        wget "https://vlsicad.ucsd.edu/~bangqixu/toDaeyeon/FlexRoute"
fi

cd "${base_dir}"
./"support"/FlexRoute "${support_dir}/param/run_checker_ispd19_test7.param" > "${support_dir}/${guides_validation}"


check_guides_report=$(grep -i "${gold_guides}" ${support_dir}/${guides_validation})

if [[ "${check_guides_report}" != "${gold_guides}" ]];
then
        echo "--ERROR: guides are not valid"
        echo "--Check ${guides_validation} at ${support_dir}"
        fail
fi

echo
echo "--Check pin coverage... Success!"
echo

#Test 3: check wire length
echo
echo "--Verify QoR: global routed wire lenght..."
echo

length_report=$(grep -i "${grep_pattern}" ${base_dir}/${log_file})
gold_report=$(grep -i "${grep_pattern}" ${gold_wl})

if [[ "$length_report" != "$gold_report" ]];
then
        echo "--ERROR: Verify QoR: wirelength missmatch"
        fail
fi

echo
echo "--Vefiry QoR: Success!"
echo

echo
echo "Unit tests... Done!"
echo