# Deploying perfectclear to Vercel

This document explains how to deploy the perfectclear frontend to Vercel.

## Prerequisites

1. A Vercel account
2. Access to the GitHub repository (stevenhao/perfectclear)
3. A deployed backend server (optional, for full functionality)

## Setup Steps

### 1. Connect to Vercel

1. Go to [Vercel Dashboard](https://vercel.com/dashboard)
2. Click "Add New" → "Project"
3. Import the GitHub repository (stevenhao/perfectclear)
4. Configure the project:
   - Build Command: `yarn build`
   - Output Directory: `dist`
   - Install Command: `yarn`

### 2. Configure Environment Variables

For a fully functional deployment, you need to set up the following environment variable:

- `BACKEND_HOST` - URL of your deployed backend server (without the protocol)

Example: If your backend is deployed at `https://perfectclear-backend.example.com`, set `BACKEND_HOST` to `perfectclear-backend.example.com`

**Note:** If you don't set up a backend, the frontend will still deploy, but game functionality requiring the backend won't work.

### 3. Deploy

Click "Deploy" to start the deployment process. Vercel will automatically:
1. Clone the repository
2. Install dependencies
3. Build the application
4. Deploy the frontend

## Automatic Deployments

With the repository connected to Vercel, any push to the main branch will automatically trigger a new deployment. You can also set up preview deployments for pull requests in the Vercel project settings.
