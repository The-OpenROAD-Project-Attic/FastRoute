cp /fastroute/etc/*.dat /fastroute/
cd fastroute/
ln -s /fastroute/build/FRlefdef ./FRlefdef
tclsh tests/run_all.tcl
