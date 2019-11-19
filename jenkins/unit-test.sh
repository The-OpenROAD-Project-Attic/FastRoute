cp /fastroute/etc/*.dat /fastroute/
cd fastroute/
rm -f FRlefdef
ln -s /fastroute/build/FRlefdef ./FRlefdef
tclsh tests/run_all.tcl
