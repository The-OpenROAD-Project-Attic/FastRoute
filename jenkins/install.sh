mkdir -p /fastroute/build
cd /fastroute/build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/opt/rh/rh-mongodb32/root/usr ..  && \
    make