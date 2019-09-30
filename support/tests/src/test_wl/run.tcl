#!/usr/bin/env tclsh

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

proc checkWirelength {goldFile outFile} {
        _puts "--Verify QoR: global routed wire lenght..."

        set base_dir [pwd]
        set grep_pattern "Final routing length"
        set length_report [catch {exec grep -i "${grep_pattern}" $outFile} result]

        set status [catch {exec grep -q $result $goldFile} rslt]
        if {$status == 0} {
                _puts "--Verify QoR: Success!"
        } else {
                _puts stderr "Wirelengths are different"
                _puts stderr "********************************************************************************"
                _puts stderr $rslt
                _puts stderr "********************************************************************************"
                _err "Current routing have wirelength different from gold wl"
        }
}

set curr_test "${tests_dir}/test_wl"

set gold_wl "${curr_test}/golden.wl"

set script_file "${curr_test}/routeDesign.tcl"
set output_file "${curr_test}/${test_name}.guide"
set output_log "${curr_test}/${test_name}.log"
set bin_file "$base_dir/FRlefdef"

downloadBenchmark $test_name $curr_test
if {[file exists "${curr_test}/${test_name}.tgz"]} {
        exec rm "${curr_test}/${test_name}.tgz"
}

runFastRoute $test_name $curr_test $bin_file $output_log

checkWirelength $gold_wl $output_log



















