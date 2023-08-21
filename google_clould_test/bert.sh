#######s############################################
#filename      : bert.sh
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2022-10-20 11:24:26
#last modified : 2022-10-20 11:24:26
####################################################
#!/bin/bash

export TPU_NAME=local
export STORAGE_BUCKET=gs://tpu-lt-bucket
export INIT_CHECKPOINT=${STORAGE_BUCKET}/uncased_L-12_H-768_A-12/bert_model.ckpt
export TFDS_DIR=${STORAGE_BUCKET}/tfds
export VOCAB_FILE=${STORAGE_BUCKET}/uncased_L-12_H-768_A-12/vocab.txt
export MODEL_DIR=${STORAGE_BUCKET}/bert-output
export TASK=mnli
export PYTHONPATH=~/tf-models

cd ~/tf-models
python3 official/nlp/train.py \
  --tpu=${TPU_NAME} \
  --experiment=bert/sentence_prediction_text \
  --mode=eval \
  --model_dir=${MODEL_DIR} \
  --config_file=official/nlp/configs/experiments/glue_mnli_text.yaml \
  --params_override="runtime.distribution_strategy=tpu, task.init_checkpoint=${INIT_CHECKPOINT}, task.train_data.tfds_data_dir=${TFDS_DIR}, task.train_data.vocab_file=${VOCAB_FILE}, task.validation_data.tfds_data_dir=${TFDS_DIR}, task.validation_data.vocab_file=${VOCAB_FILE}, trainer.train_steps=2000"
