FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install -y wget chromium-browser python3 python3-pip pyqt5-dev-tools python3-setuptools unzip 

RUN pip3 install flask

RUN useradd -ms /bin/bash wallet

WORKDIR /home/wallet
RUN wget https://github.com/spesmilo/electrum/archive/3.0.0.zip && unzip 3.0.0.zip

WORKDIR electrum-3.0.0
RUN pyrcc5 icons.qrc -o gui/qt/icons_rc.py && python3 setup.py install

ADD ./src/daemon.py /home/wallet/electrum-3.0.0/lib/
ADD ./src/servers.json /home/wallet/electrum-3.0.0/lib/
ADD ./src/servers_testnet.json /home/wallet/electrum-3.0.0/lib/

ADD ./src/wallet.tgz /home/wallet/

ADD ./src/run.sh /home/wallet/electrum-3.0.0/
RUN chmod +x /home/wallet/electrum-3.0.0/run.sh

ADD ./src/app.py /home/wallet/electrum-3.0.0/

USER wallet

#CMD timeout -sKILL 15 /home/wallet/electrum-3.0.0/run.sh
#RUN ./electrum daemon --testnet &
# chromium-browser --no-sandbox --headless --disable-gpu