FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install -y wget python3 python3-pip
RUN pip3 install flask

WORKDIR /opt/
RUN wget https://gethstore.blob.core.windows.net/builds/geth-linux-amd64-1.8.15-89451f7c.tar.gz &&\
    tar xzvf geth-linux-amd64-1.8.15-89451f7c.tar.gz
ENV PATH $PATH:/opt/geth-linux-amd64-1.8.15-89451f7c



