#!/bin/bash
make
./server 127.0.0.1 2342 & 
./client 127.0.0.1 2342 hello
./client 127.0.0.1 2342 goodbye
./client 127.0.0.1 2342 exit