# Deployment Instructions for PerfectClear

## Local Deployment with Docker

### Prerequisites
- Docker installed on your system
- Docker Compose (optional, for easier management)

### Steps to Deploy

1. Clone the repository:
   ```
   git clone https://github.com/stevenhao/perfectclear.git
   cd perfectclear
   ```

2. Build and run the Docker container:
   ```
   # Using Docker directly
   docker build -t perfectclear .
   docker run -p 4444:4444 perfectclear

   # OR using Docker Compose
   docker-compose up
   ```

3. Access the application:
   Open your browser and navigate to `http://localhost:4444`

## Cloud Deployment Options

### Deploy to a VPS (DigitalOcean, AWS EC2, etc.)
1. SSH into your server
2. Clone the repository and navigate to it
3. Build and run the Docker container as described above
4. Configure your firewall to allow traffic on port 4444
5. Access the application via your server's IP address: `http://your-server-ip:4444`

### Deploy to Kubernetes
1. Build and push the Docker image to a container registry
   ```
   docker build -t your-registry/perfectclear:latest .
   docker push your-registry/perfectclear:latest
   ```

2. Create a Kubernetes deployment and service
   ```yaml
   # deployment.yaml
   apiVersion: apps/v1
   kind: Deployment
   metadata:
     name: perfectclear
   spec:
     replicas: 1
     selector:
       matchLabels:
         app: perfectclear
     template:
       metadata:
         labels:
           app: perfectclear
       spec:
         containers:
         - name: perfectclear
           image: your-registry/perfectclear:latest
           ports:
           - containerPort: 4444
   ---
   apiVersion: v1
   kind: Service
   metadata:
     name: perfectclear
   spec:
     type: LoadBalancer
     ports:
     - port: 80
       targetPort: 4444
     selector:
       app: perfectclear
   ```

3. Apply the configuration
   ```
   kubectl apply -f deployment.yaml
   ```

4. Access the application via the LoadBalancer IP or domain name

## Deploy to Fly.io

To deploy this application to Fly.io:

1. Install the Fly.io CLI:
   ```
   curl -L https://fly.io/install.sh | sh
   ```

2. Set up environment variables:
   ```
   export FLYCTL_INSTALL="/home/ubuntu/.fly"
   export PATH="$FLYCTL_INSTALL/bin:$PATH"
   ```

3. Authenticate with Fly.io:
   ```
   flyctl auth login
   ```

4. Launch the application:
   ```
   flyctl launch --name perfectclear-app
   ```

5. Deploy the application:
   ```
   flyctl deploy
   ```

6. Access the application:
   ```
   flyctl open
   ```

The application will be available at https://perfectclear-app.fly.dev

## Notes
- The application runs on port 4444 by default
- The Docker container runs both the C++ backend and the CoffeeScript web server
- For production deployments, consider setting up HTTPS using a reverse proxy like Nginx
