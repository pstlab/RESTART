FROM pstlab/coco_base

# Expose the port that RESTART uses to run
EXPOSE 8080

# Define environment variables
ARG GITHUB_TOKEN=no_token
ARG MONGODB_HOST=restart-db
ARG MONGODB_PORT=27017
ARG RASA_HOST=restart-rasa
ARG RASA_PORT=5005

# Configure Git to use the GitHub token
RUN if [ "${GITHUB_TOKEN}" != "no_token" ]; then git config --global credential.helper '!f() { echo "username=oauth2" ; echo "password=${GITHUB_TOKEN}" ; }; f'; fi

# Install NVM and Node.js
WORKDIR /home
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
SHELL ["/bin/bash", "-c"]
RUN source ~/.nvm/nvm.sh && nvm install node && nvm alias default node

# Install RESTART
WORKDIR /home
RUN git clone --recursive https://github.com/pstlab/RESTART
WORKDIR /home/RESTART
RUN mkdir build && cd build && cmake -DVERBOSE_LOG=ON -DMONGODB_HOST=${MONGODB_HOST} -DMONGODB_PORT=${MONGODB_PORT} -DRASA_HOST=${RASA_HOST} -DRASA_PORT=${RASA_PORT} .. && make
COPY ./rules /home/rules

# Install the front-end
WORKDIR /home/RESTART/client
RUN source ~/.nvm/nvm.sh && npm install && npm run build

WORKDIR /home/RESTART
CMD /home/RESTART/build/bin/RESTART