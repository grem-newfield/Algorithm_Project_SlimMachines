# Define the compiler and flags as variables
CXX := "g++"
CXXFLAGS := "-std=c++17 -Wall -Wextra -I/usr/local/include"
LDFLAGS := "-L/usr/local/lib -lpthread -lz -lssl -lsqlite3"

# Define the target executable and source files
TARGET := "lean_machines_app"
SRCS := "main.cpp lodepng.cpp"
OBJS := "main.o lodepng.o"

# Define the Docker image name
IMAGE := TARGET + "_image"
CONTAINER := TARGET + "_container"

# Define the build rule for the C++ code
build:
    {{CXX}} {{CXXFLAGS}} -c main.cpp -o main.o
    {{CXX}} {{CXXFLAGS}} -c lodepng.cpp -o lodepng.o
    {{CXX}} {{CXXFLAGS}} -o {{TARGET}} {{OBJS}} {{LDFLAGS}}

# Define the clean rule
clean:
    rm -f {{TARGET}} {{OBJS}}

# Set the default recipe to run the 'all' recipe
default: build

# Build the Docker image
docker-build:
    docker build -t {{IMAGE}} --progress=plain .

# Run the Docker container
docker-run:
    docker run -d --name {{CONTAINER}} --network host {{IMAGE}}

# Stop the Docker container
docker-stop:
    docker stop {{CONTAINER}}
    docker rm {{CONTAINER}}
