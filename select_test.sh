#!/bin/bash

for i in `seq 1 10`
  do
    ./Project4Client -p 30600 -s 127.0.0.1 -d ./testClientDir > /dev/null 2>&1 &
  done
