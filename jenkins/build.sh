docker build -f jenkins/Dockerfile.dev -t fastroute .
docker run -v $(pwd):/fastroute fastroute bash -c "./fastroute/jenkins/install.sh"