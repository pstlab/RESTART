# Use a base image with Ubuntu
FROM ubuntu:latest

# Expose the COCO application port
EXPOSE 8080

# Set environment variables for MongoDB connection
ARG MONGODB_HOST=coco-db
ARG MONGODB_PORT=27017
ARG CLIENT_DIR=/gui

# Install the necessary dependencies
RUN apt update && apt install -y build-essential cmake libssl-dev unzip wget curl git

# Compile and install CLIPS
RUN wget -O /tmp/clips.zip https://sourceforge.net/projects/clipsrules/files/CLIPS/6.4.2/clips_core_source_642.zip/download \
    && unzip /tmp/clips.zip -d /tmp \
    && cd /tmp/clips_core_source_642/core \
    && make release_cpp \
    && mkdir -p /usr/local/include/clips \
    && cp *.h /usr/local/include/clips \
    && cp libclips.a /usr/local/lib \
    && rm -rf /tmp/clips.zip /tmp/clips_core_source_642

# Compile and install the mongo-cxx driver
RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.1.1/mongo-cxx-driver-r4.1.1.tar.gz \
    && tar -xzf mongo-cxx-driver-r4.1.1.tar.gz \
    && cd mongo-cxx-driver-r4.1.1/build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 \
    && cmake --build . \
    && cmake --build . --target install \
    && cd /tmp && rm -rf mongo-cxx-driver-r4.1.1* \
    && ldconfig

# Install Node.js (version 22.x)
RUN curl -fsSL https://deb.nodesource.com/setup_22.x | bash - \
    && apt-get install -y nodejs

# Clone and build RESTART
RUN git clone --recursive -b uncertainty https://github.com/pstlab/RESTART \
    && cd RESTART \
    && mkdir build && cd build \
    && cmake -DLOGGING_LEVEL=DEBUG -DMONGODB_HOST=${MONGODB_HOST} -DMONGODB_PORT=${MONGODB_PORT} -DCLIENT_DIR=${CLIENT_DIR} -DCMAKE_BUILD_TYPE=Release .. \
    && make -j$(nproc)

# Build the GUI application
RUN npm --prefix /RESTART/gui install && npm --prefix /RESTART/gui run build

# Move the built COCO files to the /app directory
RUN mv /RESTART/build/RESTART /restart \
    && mkdir -p /gui && mv /RESTART/gui/dist /gui \
    && rm -rf /RESTART

# Start the RESTART application
CMD ["./restart"]