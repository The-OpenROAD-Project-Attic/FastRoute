cp /fastroute/etc/*.dat /fastroute/
cd fastroute/
rm -f FastRoute
ln -s /fastroute/build/FastRoute ./FastRoute
tclsh tests/run_all.tcl
