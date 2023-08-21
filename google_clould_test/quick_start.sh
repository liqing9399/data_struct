#######s############################################
#filename      : quick_start.sh
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2022-10-06 16:39:51
#last modified : 2022-10-06 16:39:51
####################################################
#!/bin/bash

# 1. 创建抢占式-tpu 虚拟机
gcloud compute tpus tpu-vm create tpu-lt --zone=us-central1-b --accelerator-type=v3-8 --version=tpu-vm-tf-2.10.0  --preemptible

# 1.1 查看tpu是否被抢占
gcloud compute tpus tpu-vm list --zone=us-central1-b

# 2. 连接虚拟机
gcloud compute tpus tpu-vm ssh tpu-lt --zone us-central1-b

# 3. 设置环境变量
export TPU_NAME=local

# 4. 执行相关训练、推理代码文件。
python3 test-tpu.py

# 5. 退出 tpu
exit

# 6. 删除tpu 虚拟机
gcloud compute tpus tpu-vm delete tpu-lt --zone=us-central1-b

# 7. 验证是否删除
gcloud compute tpus tpu-vm list --zone=us-central1-b






# 8. 查看有关 cloud-tpu 的信息
gcloud alpha compute tpus tpu-vm describe tpu-lt  --zone=us-central1-b

# 9. 停用cloud-tpu 资源
gcloud alpha compute tpus tpu-vm stop tpu-lt --zone=us-central1-b
gcloud alpha compute tpus tpu-vm stop tpu-lt --zone=us-central1-b

# 10. 启动 cloud-tpu 资源
gcloud alpha compute tpus tpu-vm start tpu-lt --zone=us-central1-b

# 11. 使用特定端口映射连接tpu
gcloud compute tpus tpu-vm ssh tpu-lt --zone=us-central1-b --ssh-flag="-4 -L 9001:localhost:9001"




# google cloud storage
# 1. 创建存储桶
gsutil mb -c standard -l us-central1 gs://lt-tpu-bucket
# 2. 删除存储桶
gsutil rm -r gs://lt-tpu-bucket


# 下载 tpu-profiler 工具，下载后使用tensorboard才能解析profiler文件
pip install -U tensorboard-plugin-profile
TPU_LOAD_LIBRARY=0 tensorboard --logdir logs/ --port 9001
