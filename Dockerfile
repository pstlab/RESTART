# Use the CoCo base image
FROM pstlab/coco_base

# Expose the API port
EXPOSE 8080

# Set the environment variables
ARG GITHUB_TOKEN=no_token
ARG MONGODB_HOST=restart-db
ARG MONGODB_PORT=27017
ARG TRANSFORMER_HOST=restart-transformer
ARG CLIENT_FOLDER=extern/client
ARG LOGGING_LEVEL=TRACE

# Configure Git to use the GitHub token
RUN if [ "${GITHUB_TOKEN}" != "no_token" ]; then git config --global credential.helper '!f() { echo "username=oauth2" ; echo "password=${GITHUB_TOKEN}" ; }; f'; fi

# Install NVM and Node.js
WORKDIR /home
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash
SHELL ["/bin/bash", "-c"]
RUN source ~/.nvm/nvm.sh && nvm install node && nvm alias default node

# Install RESTART
WORKDIR /home
RUN git clone --recursive https://github.com/pstlab/RESTART

# Build RESTART Backend
WORKDIR /home/RESTART
RUN mkdir build && cd build && cmake -DLOGGING_LEVEL=${LOGGING_LEVEL} -DMONGODB_HOST=${MONGODB_HOST} -DMONGODB_PORT=${MONGODB_PORT} -DTRANSFORMER_HOST=${TRANSFORMER_HOST} -DCLIENT_FOLDER=${CLIENT_FOLDER} .. && make

# Build RESTART Frontend
WORKDIR /home/RESTART/client
RUN source ~/.nvm/nvm.sh && npm install && npm run build

RUN mkdir /coco

WORKDIR /home/RESTART
CMD /home/RESTART/build/restart