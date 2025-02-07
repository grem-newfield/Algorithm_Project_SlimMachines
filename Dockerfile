FROM alpine:latest

RUN apk update && apk add --no-cache \
    g++ \
    make \
    just \
    git \
    cmake \
    libstdc++ \
    linux-headers \
    boost-dev \
    zlib-dev \
    openssl-dev \
    wget \
    asio-dev \
    sqlite-dev

# Clone and build Crow
RUN git clone https://github.com/CrowCpp/Crow.git /tmp/crow && \
    mkdir /tmp/crow/build && \
    cd /tmp/crow/build && \
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF -DCROW_ENABLE_SSL=ON -DCROW_ENABLE_COMPRESSION=ON && \
    make && \
    make install

WORKDIR /app

# Download lodepng files
RUN wget https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.cpp -O /usr/local/include/lodepng.cpp && \
    wget https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.h -O /usr/local/include/lodepng.h


# Copy the current directory contents into the container at /app
COPY main.cpp .
COPY justfile .

# Compile the app C++ code
RUN just

# Expose the required ports for networking (e.g., 8080)
EXPOSE 8080

# Run the compiled program
CMD ["./lean_machines_app"]
