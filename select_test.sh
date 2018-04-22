#!/bin/bash

for i in {0..1024..1}
  do 
     make -f client || exit
 done
