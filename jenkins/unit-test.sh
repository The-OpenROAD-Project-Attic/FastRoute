cp /fastroute/support/*.dat /fastroute/
cd fastroute/
ln -s /fastroute/build/third_party/rsyn/bin/rsyn ./FRlefdef
tclsh support/tests/run_all.tcl
