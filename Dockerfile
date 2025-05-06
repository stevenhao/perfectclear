FROM ubuntu:22.04 as builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++-9 \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the server
RUN make clean && make server

FROM ubuntu:22.04

# Runtime dependencies (minimal)
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the compiled binary and required data files
COPY --from=builder /app/server.o /app/
COPY pieces centers kicks /app/
COPY book_100k.txt /app/

# Expose the port the server listens on
EXPOSE 4445

# Command to run the server
CMD ["./server.o", "4445"]
