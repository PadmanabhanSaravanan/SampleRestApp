# Table of contents 

1. [Install Docker Desktop](#install-docker-desktop)
2. [Install Minikube](#install-minikube)
3. [Install Kubectl](#install-kubectl) 
4. [Start Minikube](#start-minikube) 
5. [Verify Minikube Installation](#verify-minikube-installation) 
6. [Additional Information](#additional-information---managing-minikube-cluster)

# Install Docker Desktop 

[Install Docker on your ubuntu machine](Install using the Apt repository - https://docs.docker.com/engine/install/ubuntu/)

# Install Minikube 


**Step 1: Apply updates**

Apply all updates of existing packages of your system by executing the following apt commands,

```bash +@Output
$ sudo apt update -y
$ sudo apt upgrade -y
```
**Step 2: Once all the updates are installed then reboot your system once**

```bash +@Output
$ sudo reboot
```

**Step 3: Install Minikube dependencies**
Install the following minikube dependencies by running beneath command,

```bash +@Output
$ sudo apt install -y curl wget apt-transport-https
```

**Step 4: Download Minikube Binary**
Use the following curl command to download latest minikube binary,

```bash +@Output
$ curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64

```

**Step 5: Once the binary is downloaded, copy it to the path /usr/local/bin and set the executable permissions on it**

```bash +@Output
$ sudo install minikube-linux-amd64 /usr/local/bin/minikube

```

**Step 6: Verify the minikube version**

```bash +@Output
$ minikube version
```

# Install Kubectl 

Kubectl is a command line utility which is used to interact with Kubernetes cluster. It is used for managing deployments, service and pods etc. 

**Step 1: curl command to download latest version of kubectl**

```bash +@Output
$ curl -LO https://storage.googleapis.com/kubernetes-release/release/`curl -s https://storage.googleapis.com/kubernetes-release/release/stable.txt`/bin/linux/amd64/kubectl

```

**Step 2: Set the executable permissions on kubectl binary and move it to the path /usr/local/bin**

```bash +@Output
$ chmod +x kubectl
$ sudo mv kubectl /usr/local/bin/

```

**Step 3: verify the kubectl version**

```bash +@Output
$ kubectl version -o yaml
$ kubectl version --client

```

# Start Minikube 

Minikube generally advises against running with root privileges, especially with the "docker" driver, for security reasons.

**Step 1: Create a dedicated user for Minikube**

```bash +@Output
$ sudo adduser minikube-user

```

**Step 2: Add User to the Docker Group**

```bash +@Output
$ sudo usermod -aG docker minikube-user
```

**Step 3: Log Out and Back In**

After adding the user to the "docker" group, the changes will take effect after the user logs out and logs back in. Alternatively, you can use the newgrp command to apply the group changes without logging out

```bash +@Output
$ newgrp docker
```

**Step 4: Switch to the new user**

```bash +@Output
$ sudo su - minikube-user
```

**Step 5: Start minikube**

We would be using docker as base for minikube, so start the minikube with the docker driver, run,

```bash +@Output
$ minikube start --driver=docker
```

**Step 6: Run below minikube command to check status**

```bash +@Output
$ minikube status
```

**Step 7: verify the Kubernetes version, node status and cluster info**

```bash +@Output
$ kubectl cluster-info
$ kubectl get nodes
```

# Verify Minikube Installation

To verify the minikube installation, let’s try to deploy nginx based deployment.

**Step 1: Install nginx based deployment**

```bash +@Output
$ kubectl create deployment my-nginx --image=nginx
```

**Step 2: Verify deployment status**

```bash +@Output
$ kubectl get deployments.apps my-nginx
$ kubectl get pods
```

**Step 3: Expose the deployment**

```bash +@Output
$ kubectl expose deployment my-nginx --name=my-nginx-svc --type=NodePort --port=80
$ kubectl get svc my-nginx-svc

```

**Step 4: Get your service url**

```bash +@Output
$ minikube service my-nginx-svc --url

```

**Step 5: Now try to access your nginx based deployment using above url**

```bash +@Output
$ curl http://url:port
```

# Additional Information - Managing Minikube Cluster

**To stop the minikube** 

```bash +@Output
$ minikube stop
```

**To delete the minikube**

```bash +@Output
$ minikube delete
```

**To Start the minikube**

```bash +@Output
$ minikube start
```

**To configure minikube**

In case you want to start the minikube with higher resource like 8 GB RM and 4 CPU then execute following commands one after the another.

```bash +@Output
$ minikube config set cpus 4
$ minikube config set memory 8192
$ minikube delete
$ minikube start
```