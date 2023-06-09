#######s############################################
#filename      : run.sh
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-05 14:46:07
#last modified : 2023-06-07 15:34:24
####################################################
#!/bin/bash

use_gdb=0

while getopts "v:d" arg #选项后面的冒号表示该选项需要参数
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
  echo "use gdb: $2 $3"
  gdb --args ./build/evalt $2 $3
else
  echo "normal run: $1 $2"
  ./build/evalt $1 $2 | tee log.date.$1.$2
fi
