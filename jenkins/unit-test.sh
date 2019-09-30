ln -s /fastroute/build/third_party/rsyn/bin/rsyn ./FRlefdef
cp /fastroute/support/*.dat /fastroute/
cd fastroute/
tclsh support/tests/run_all.tcl
