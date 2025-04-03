FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++-9 \
    make \
    curl \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

# Install yarn
RUN npm install -g yarn && \
    yarn global add coffeescript

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the C++ components
RUN make server

# Expose port
EXPOSE 4444

# Start the application
CMD ./server.o 4445 & coffee index.coffee 4445
