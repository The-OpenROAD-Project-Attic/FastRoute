ln -s /fastroute/build/third_party/rsyn/bin/rsyn ./FRlefdef
cp /fastroute/support/*.dat /fastroute/
sh /fastroute/support/ispd18_download.sh 
sh /fastroute/support/tests/ispd18_unit_test.sh /FRlefdef