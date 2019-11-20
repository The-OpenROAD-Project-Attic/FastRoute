mkdir -p /fastroute/build
rm -f /fastroute/src/TclInterfaceWrap.cpp
cd /fastroute/build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/opt/rh/rh-mongodb32/root/usr ..  && \
    make