#!/bin/sh
MAX_S3_INSTANCES_NUM=20

if [ $# -gt 1 ];
then
  echo "Invalid number of arguments passed to the script"
  echo "Usage: sudo ./iss3up.sh [<instance number>]"
  exit 1
fi

instance=1
if [ $# -eq 1 ]; then
  if [[ $1 =~ ^[0-9]+$ ]] && [ $1 -le $MAX_S3_INSTANCES_NUM ]; then
    instance=$1
  else
    echo "Invalid argument [$1], needs to be an integer <= $MAX_S3_INSTANCES_NUM"
    exit 1
  fi
fi

if [[ $instance -ne 0 ]]; then
  s3port=$((8080 + $instance))
  s3_pid="$(ps aux | grep "/var/run/s3server.$s3port.pid" | grep -v grep)"
  if [[ "$s3_pid" != "" ]]; then
    s3_instance=$(netstat -an | grep $s3port | grep LISTEN)
    if [[ "$s3_instance" != "" ]]; then
      echo "s3 server is running and listening on $s3port"
    else
      echo "s3 server is stopped for port: $s3port"
      exit 1
    fi
  else
    echo "s3 server is stopped for port: $s3port"
    exit 1
  fi
fi