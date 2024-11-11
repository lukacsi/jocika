# Stage 1: Builder Stage
FROM ubuntu:22.04 AS builder

# Set environment variables to non-interactive to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update package lists and install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libmpg123-dev \
    libssl-dev \
    zlib1g-dev \
    libopus-dev \
    pkg-config \
    wget \
    ca-certificates \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Download and install D++ via .deb package
RUN wget -O dpp.deb https://dl.dpp.dev/ \
    && dpkg -i dpp.deb \
    && rm dpp.deb \
    && apt-get install -f -y \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory for your project
WORKDIR /app

# Copy only the necessary files to leverage Docker caching
COPY CMakeLists.txt /app/
COPY src/ /app/src/
COPY cmake/ /app/cmake/

# Build the project
RUN mkdir build && cd build \
    && cmake .. \
    && cd .. \
    && cmake --build ./build

# Stage 2: Runtime Stage
FROM ubuntu:22.04

# Set environment variables to non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libmpg123-0 \
    libssl3 \
    zlib1g \
    libopus0 \
    libboost-system1.74.0 \
    libstdc++6 \
    wget \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Download and install D++ via .deb package in runtime
RUN wget -O dpp.deb https://dl.dpp.dev/ \
    && dpkg -i dpp.deb \
    && rm dpp.deb \
    && apt-get install -f -y \
    && rm -rf /var/lib/apt/lists/*

# Add a non-root user for better security
RUN useradd -m jocika

# Set the working directory
WORKDIR /app

# Copy the built executable from the builder stage
COPY --from=builder /app/build/jocika ./jocika
COPY ./media ./media

# Ensure the executable has the correct permissions
RUN chmod +x ./jocika

# Switch to the non-root user
USER jocika

# Define the default command to run your executable
ENTRYPOINT ["./jocika"]

