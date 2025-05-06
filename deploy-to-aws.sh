set -e

AWS_REGION="us-east-1"  # Change to your preferred region
AWS_ACCOUNT_ID=$(aws sts get-caller-identity --query Account --output text)
ECR_REPOSITORY="perfectclear-backend"
IMAGE_TAG="latest"

aws ecr describe-repositories --repository-names $ECR_REPOSITORY --region $AWS_REGION || \
  aws ecr create-repository --repository-name $ECR_REPOSITORY --region $AWS_REGION

docker build -t $ECR_REPOSITORY:$IMAGE_TAG .

aws ecr get-login-password --region $AWS_REGION | docker login --username AWS --password-stdin $AWS_ACCOUNT_ID.dkr.ecr.$AWS_REGION.amazonaws.com

docker tag $ECR_REPOSITORY:$IMAGE_TAG $AWS_ACCOUNT_ID.dkr.ecr.$AWS_REGION.amazonaws.com/$ECR_REPOSITORY:$IMAGE_TAG
docker push $AWS_ACCOUNT_ID.dkr.ecr.$AWS_REGION.amazonaws.com/$ECR_REPOSITORY:$IMAGE_TAG

sed -e "s/\${AWS_ACCOUNT_ID}/$AWS_ACCOUNT_ID/" -e "s/\${AWS_REGION}/$AWS_REGION/" aws-task-definition.json > /tmp/task-definition.json
aws ecs register-task-definition --cli-input-json file:///tmp/task-definition.json --region $AWS_REGION

CLUSTER_NAME="perfectclear-cluster"
SERVICE_NAME="perfectclear-backend-service"

aws ecs describe-clusters --clusters $CLUSTER_NAME --region $AWS_REGION | grep $CLUSTER_NAME || \
  aws ecs create-cluster --cluster-name $CLUSTER_NAME --region $AWS_REGION

TASK_DEFINITION_ARN=$(aws ecs describe-task-definition --task-definition perfectclear-backend --region $AWS_REGION --query taskDefinition.taskDefinitionArn --output text)

if aws ecs describe-services --cluster $CLUSTER_NAME --services $SERVICE_NAME --region $AWS_REGION | grep $SERVICE_NAME; then
  aws ecs update-service --cluster $CLUSTER_NAME --service $SERVICE_NAME --task-definition $TASK_DEFINITION_ARN --region $AWS_REGION
else
  aws ecs create-service \
    --cluster $CLUSTER_NAME \
    --service-name $SERVICE_NAME \
    --task-definition $TASK_DEFINITION_ARN \
    --desired-count 1 \
    --launch-type FARGATE \
    --network-configuration "awsvpcConfiguration={subnets=[your-subnet-id],securityGroups=[your-security-group-id],assignPublicIp=ENABLED}" \
    --region $AWS_REGION
fi

echo "Service deployed. You'll need to configure an Application Load Balancer to expose the service publicly."
