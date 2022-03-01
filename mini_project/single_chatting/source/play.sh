#!/bin/bash

str1="make"
str2="clean"

if [ "$1" == "$str1" ];
then
	gcc -o client client.c
	gcc -o server server.c
fi

if [ "$1" == "$str2" ];
then
	rm client server
fi
