# Use the CoCo base image
FROM pstlab/coco_base

# Expose the API port
EXPOSE 8080

# Set the environment variables
ARG MONGODB_HOST=restart-mongo
ARG MONGODB_PORT=27017
ARG CLIENT_FOLDER=extern/client

# Install NVM and Node.js
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.0/install.sh | bash
RUN bash -c ". ~/.nvm/nvm.sh && nvm install node"
