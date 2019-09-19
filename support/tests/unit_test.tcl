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

rename puts _puts

proc _err {s {n 1}} {
        _puts stderr "ERRO: $s"
        exit $n
}

proc _warn {s} {
        _puts "WARNING: $s"
}

proc _debug {s {n 1}} {
        global debugLevel
        if {$n <= $debugLevel} {
                _puts "DEBUG: $s"
        }
}

proc _info {s {n 0}} {
        global infoLevel
        if {$n <= $infoLevel} {
                _puts "INFO: $s"
        }
}

proc _infoN {s} {
        _puts -nonewline "INFO: $s"
}

proc downloadBenchmark {testName} {
        set base_dir [pwd]
        set support_dir "$base_dir/support"
        set test_dir "$support_dir/ispd19/"

        if {![file exists "${test_dir}/${testName}/${testName}.input.lef"] || \
            ![file exists "${test_dir}/${testName}/${testName}.input.def"]} {
                _puts "Downloading test files..."
                exec mkdir -p "${test_dir}"
                cd $test_dir
                exec wget -q "http://www.ispd.cc/contests/19/benchmarks/${testName}.tgz"
                set status [catch {exec tar zxvf "${testName}.tgz"} result]
                cd "$base_dir"
                if {$status == 0} {
                        _puts "Benchmark successfully downloaded"
                } else {
                        _puts stderr "Error in download benchmark"
                        _puts stderr "********************************************************************************"
                        _err "Benchmakr could not be downloaded"
                }
        }
}

proc checkGuidesFile {goldFile outFile} {
        _puts "--Compare guides..."
        
        if {![file exists $goldFile]} {
                _err "Gold file $goldFile not found!"
        }
        if {![file exists $outFile]} {
                _err "Out file $outFile not found!"
        }
        set status [catch {exec diff $goldFile $outFile} result]
        if {$status == 0} {
                _puts "--Compare guides: Success!"
        } else {
                _puts stderr "Files are different"
                _puts stderr "********************************************************************************"
                _puts stderr $result
                _puts stderr "********************************************************************************"
                _err "files are different: $goldFile and $outFile... "
        }
}

proc checkPinsCoverage {goldCheckGuides testName} {
        _puts "--Check pin coverage..."

        set base_dir [pwd]
        set support_dir "$base_dir/support"
        set gr_checker_file "${support_dir}/FlexRoute"

        set guides_validation "guides_validation.log"
        exec $gr_checker_file "${support_dir}/param/run_checker_${testName}.param" > "${support_dir}/${guides_validation}"

        set status [catch {exec grep -q $goldCheckGuides ${support_dir}/${guides_validation}} result]
        if {$status == 0} {
                _puts "--Check pin coverage: Success!"
        } else {
                _puts stderr "Pins are not covered"
                _puts stderr "********************************************************************************"
                _puts stderr "${support_dir}/${guides_validation}"
                _puts stderr "********************************************************************************"
                _err "Generated guides are not valid"
        }
}

proc checkWirelength {goldFile testName} {
        _puts "--Verify QoR: global routed wire lenght..."

        set base_dir [pwd]
        set support_dir "$base_dir/support"
        set gr_checker_file "${support_dir}/FlexRoute"
        set log_file "${testName}.log"

        set grep_pattern "Final routing length"
        set length_report [catch {exec grep -i "${grep_pattern}" ${base_dir}/${log_file}} result]
        
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

proc runFR {testName} {
        _puts "--Run FastRoute4-lefdef..."

        set base_dir [pwd]
        set support_dir "$base_dir/support"
        set test_dir "$support_dir/ispd19/"
        set scripts_dir "${support_dir}/scripts"
        set log_file "${testName}.log"

        set bin_file "$base_dir/FRlefdef"
        set run_FR_file "${scripts_dir}/run_FRlefdef.tcl"
        set status [catch {exec tclsh "${run_FR_file}" "${testName}" "${test_dir}/${testName}/${testName}.input.lef" \
        "${test_dir}/${testName}/${testName}.input.def" "${bin_file}"} rslt]

        if {$status == 0} {
                _puts "--Run FastRoute4-lefdef: Success!"
        } else {
                set log [open $base_dir/${testName}.log r]
                _puts stderr "FastRoute4-lefdef fails in run"
                _puts stderr "********************************************************************************"
                _puts stderr $rslt
                while { [gets $log data] >= 0 } {
                        _puts $data
                }
                _puts stderr "********************************************************************************"
                close $log
                _err "Runtime error"
        }
}

# proc Main {} {

_puts "Start unit tests..."

set test_name "ispd19_test7"

set base_dir [pwd]
set support_dir "$base_dir/support"
set test_dir "$support_dir/ispd19/"
set scripts_dir "${support_dir}/scripts"

set bin_file "$base_dir/FRlefdef"
set run_FR_file "${scripts_dir}/run_FRlefdef.tcl"

set output_file "${test_dir}${test_name}/${test_name}.guide"
set gold_dir "${support_dir}/gold"
set gold_guides "${gold_dir}/${test_name}.guide"

set gold_check_guides "complete post process guides ..."

set gold_wl "${gold_dir}/${test_name}.wl"

downloadBenchmark $test_name
runFR $test_name
exec mv ${base_dir}/${test_name}.guide $test_dir/$test_name
checkGuidesFile $gold_guides $output_file
checkPinsCoverage $gold_check_guides $test_name
checkWirelength $gold_wl $test_name

# }