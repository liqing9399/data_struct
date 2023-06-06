#######s############################################
#filename      : run.sh
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-05 14:46:07
#last modified : 2023-06-05 20:33:32
####################################################
#!/bin/bash

use_gdb=0

while getopts "d" arg #选项后面的冒号表示该选项需要参数
do
        case $arg in
             d)
                echo "use gdb "
                use_gdb=1
                ;;
             ?)  #当有不认识的选项的时候arg为?
            echo "unkonw argument"
        exit 1
        ;;
        esac
done

#echo "$use_gdb"

if (( $use_gdb ))
then
  echo "use gdb"
  gdb ./build/evalt
else
  echo "normal run"
  ./build/evalt | tee log.date
fi
