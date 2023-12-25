FROM ubuntu:22.04
WORKDIR /home
RUN apt-get update && apt-get install build-essential libboost-all-dev cmake libssl-dev git wget curl python3 -y

# Define environment variables
ARG GITHUB_TOKEN=no_token
ARG MONGODB_HOST=restart-db
ARG MONGODB_PORT=27017

# Configure Git to use the GitHub token
RUN if [ "${GITHUB_TOKEN}" != "no_token" ]; then git config --global credential.helper '!f() { echo "username=oauth2" ; echo "password=${GITHUB_TOKEN}" ; }; f'; fi

# Install CLIPS
RUN wget -O clips_core_source_641.tar.gz https://sourceforge.net/projects/clipsrules/files/CLIPS/6.4.1/clips_core_source_641.tar.gz/download
RUN tar -xvf clips_core_source_641.tar.gz
WORKDIR /home/clips_core_source_641/core
RUN make release_cpp
RUN mkdir /usr/local/include/clips
RUN cp *.h /usr/local/include/clips
RUN cp libclips.a /usr/local/lib

# Install MongoDB cxx drivers
WORKDIR /home
RUN git clone https://github.com/mongodb/mongo-c-driver.git
WORKDIR /home/mongo-c-driver
RUN python3 build/calc_release_version.py > VERSION_CURRENT
RUN mkdir cmake-build
WORKDIR /home/mongo-c-driver/cmake-build
RUN cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
RUN cmake --build .
RUN cmake --build . --target install

WORKDIR /home
RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.9.0/mongo-cxx-driver-r3.9.0.tar.gz
RUN tar -xzf mongo-cxx-driver-r3.9.0.tar.gz
WORKDIR /home/mongo-cxx-driver-r3.9.0/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF -DBUILD_VERSION=3.9.0
RUN cmake --build .
RUN cmake --build . --target install

# Install NVM and Node.js
WORKDIR /home
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
SHELL ["/bin/bash", "-c"]
RUN source ~/.nvm/nvm.sh && nvm install node && nvm alias default node

# Install RESTART
WORKDIR /home
RUN git clone --recursive https://github.com/pstlab/RESTART
WORKDIR /home/RESTART
RUN mkdir build && cd build && cmake -DVERBOSE_LOG=ON -DMONGODB_HOST=${MONGODB_HOST} -DMONGODB_PORT=${MONGODB_PORT} .. && make
COPY ./rules /home/rules

# Install the front-end
WORKDIR /home/RESTART/client
RUN source ~/.nvm/nvm.sh && npm install && npm run build

WORKDIR /home/RESTART
CMD /home/RESTART/build/bin/RESTART