#!/bin/sh

geth --datadir /usr/src/etherver/ --port 30303 --networkid=2994 --rpc --rpccorsdomain "*" --rpcaddr "0.0.0.0" --rpcapi="admin,debug,eth,miner,net,personal,rpc,txpool,web3,utils" --nodiscover --maxpeers 10000 &
python3 /usr/src/etherver/app.py
