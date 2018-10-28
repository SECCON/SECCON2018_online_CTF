FROM python
RUN pip install flask gunicorn requests tinydb web3 pysha3 gevent

RUN apt-get update
RUN apt-get install -y dnsutils

RUN useradd webapp
USER webapp
ADD ./src/ /usr/src/webapp
ENV HOME /usr/src/webapp
WORKDIR /usr/src/webapp
