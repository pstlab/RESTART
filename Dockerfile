# Use the CoCo base image
FROM pstlab/coco_base

# Expose the API port
EXPOSE 8080

# Set the environment variables
ARG MONGODB_HOST=restart-mongo
ARG MONGODB_PORT=27017
ARG CLIENT_FOLDER=extern/client

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
RUN mkdir build && cd build && cmake .. && make

# Build RESTART Frontend
WORKDIR /home/RESTART/client
RUN source ~/.nvm/nvm.sh && npm install && npm run build

RUN mkdir /coco

WORKDIR /home/RESTART
CMD /home/RESTART/build/restart