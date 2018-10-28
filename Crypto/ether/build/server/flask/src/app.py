import os
import random
import string
import re
import json
import requests
from flask import Flask, render_template, request, session, redirect, Response
from tinydb import TinyDB, Query
import time
import shutil
import tempfile
import subprocess
from web3 import Web3,HTTPProvider,IPCProvider
import rlp
from sha3 import keccak_256
import gevent

IP_ADDR = os.environ['IP_ADDR']
flag = {}
flag["1"] = "SECCON{CreateRandomInBlockchainIsDifficult}"
flag["2"] = "SECCON{EthereumUseHashInManyPlaces}"

app = Flask(__name__)
app.config["SECRET_KEY"] = "insecure"
db = TinyDB('db.json')
User = Query()

def dig(domain):
    result = subprocess.run(['nslookup', domain], stdout=subprocess.PIPE)
    ip = str(result.stdout).split("\\n")[-3].split(" ")[1]
    return ip

etherverIP = dig("etherver")
url = 'http://' + etherverIP + ':8545'
web3 = Web3(HTTPProvider(url))

enode = web3.admin.nodeInfo["enode"].replace("[::]", IP_ADDR)

adminAddr = "0x0D7134fe1Fff95780f05c6751dc4514cDf0134E7"
adminPass = "kLPm23z6HjnXKD99"

factory1Addr = "0x277344b0e0daf45a79a0e78508ad09598c59d474"
factory1ABI = [{"constant": False,"inputs": [{"name": "_password","type": "bytes32"},{"name": "_seed","type": "uint256"},{"name": "_player","type": "address"}],"name": "generateContract","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [{"name": "_contract","type": "address"}],"name": "validateContract","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [{"name": "_player","type": "address"}],"name": "referContractAddress","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"inputs": [],"payable": False,"stateMutability": "nonpayable","type": "constructor"},{"anonymous": False,"inputs": [{"indexed": False,"name": "_player","type": "address"},{"indexed": False,"name": "_contract","type": "address"}],"name": "Generate","type": "event"}]
factory1Ins = web3.eth.contract(abi=factory1ABI, address=web3.toChecksumAddress(factory1Addr))

gacha1ABI = [{"constant": False,"inputs": [{"name": "_seed","type": "uint256"}],"name": "initSeed","outputs": [],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [],"name": "played","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": False,"inputs": [],"name": "pickUp","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [],"name": "lastHash","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "player","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "seed","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "owner","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "getItem","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": False,"inputs": [{"name": "_password","type": "bytes32"}],"name": "changeOwner","outputs": [],"payable": False,"stateMutability": "nonpayable","type": "function"},{"inputs": [{"name": "_password","type": "bytes32"},{"name": "_seed","type": "uint256"},{"name": "_player","type": "address"}],"payable": False,"stateMutability": "nonpayable","type": "constructor"}]


factory2Addr = "0x7ee79357baa4b3d5b5ca6b1b734be7cc4c2102e5"
factory2ABI = [{"constant": False,"inputs": [{"name": "_password","type": "bytes32"},{"name": "_seed","type": "uint256"},{"name": "_player","type": "address"}],"name": "generateContract","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [{"name": "_contract","type": "address"}],"name": "validateContract","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [{"name": "_player","type": "address"}],"name": "referContractAddress","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"inputs": [],"payable": False,"stateMutability": "nonpayable","type": "constructor"},{"anonymous": False,"inputs": [{"indexed": False,"name": "_player","type": "address"},{"indexed": False,"name": "_contract","type": "address"}],"name": "Generate","type": "event"}]
factory2Ins = web3.eth.contract(abi=factory2ABI, address=web3.toChecksumAddress(factory2Addr))

gacha2ABI = [{"constant": False,"inputs": [{"name": "_seed","type": "uint256"}],"name": "initSeed","outputs": [],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [],"name": "played","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": False,"inputs": [],"name": "pickUp","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "nonpayable","type": "function"},{"constant": True,"inputs": [],"name": "lastHash","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "player","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [{"name": "_password","type": "bytes"}],"name": "checkPassword","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "seed","outputs": [{"name": "","type": "uint256"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "owner","outputs": [{"name": "","type": "address"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": True,"inputs": [],"name": "getItem","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "view","type": "function"},{"constant": False,"inputs": [{"name": "_password","type": "bytes"}],"name": "changeOwner","outputs": [{"name": "","type": "bool"}],"payable": False,"stateMutability": "nonpayable","type": "function"},{"inputs": [{"name": "_password","type": "bytes32"},{"name": "_seed","type": "uint256"},{"name": "_player","type": "address"}],"payable": False,"stateMutability": "nonpayable","type": "constructor"}]

gachaCode = {}
with open ("Gacha.sol", "r") as fd:
    gachaCode["1"] = fd.read()
with open ("Gacha2.sol", "r") as fd:
    gachaCode["2"] = fd.read()

# ---

def rpc_signUp(password):
    accountAddr = web3.personal.newAccount(password)
    return accountAddr

def rpc_validatePassword(accountAddr, password):
    if web3.personal.unlockAccount(accountAddr, password):
        web3.personal.lockAccount(accountAddr)
        return True
    return False

def rpc_giveInitialEther(toAddr, value):
    web3.personal.unlockAccount(adminAddr, adminPass)
    return web3.eth.sendTransaction({"from": adminAddr, "to": toAddr, "value": web3.toWei(value,"ether")})
 
def rpc_checkTX(addr):
    if not web3.eth.getTransactionReceipt(addr) == None:
        return True
    else:
        return False

def rpc_getBalance(accountAddr):
    return web3.fromWei(web3.eth.getBalance(accountAddr), "ether")

def rpc_issueNewContract(addr, level):
    web3.personal.unlockAccount(adminAddr, adminPass)
    if level == 1:
        contractPass = [random.randint(0, 256) for e in range(3)]
        #txAddr = factory1Ins.transact({"from": adminAddr, "gas": 2500000}).generateContract(bytes(contractPass), 11, addr)
        txAddr = factory1Ins.transact({"from": adminAddr}).generateContract(bytes(contractPass), 11, addr)
    else:
        nonce = web3.eth.getTransactionCount(web3.toChecksumAddress(factory2Addr))
        contractPass = "0x" + keccak_256(rlp.encode([bytes.fromhex(web3.toChecksumAddress(factory2Addr)[2:]), nonce])).hexdigest()
        #txAddr = factory2Ins.transact({"from": adminAddr, "gas": 2500000}).generateContract(contractPass, 11, addr)
        txAddr = factory2Ins.transact({"from": adminAddr}).generateContract(contractPass, 11, addr)
    res = rpc_checkTX(txAddr)
    wait = 1
    while not res and wait < 10:
        gevent.sleep(wait)
        res = rpc_checkTX(txAddr)
        wait += 1
    if wait == 10:
        return "Timeout"
    receipt = web3.eth.getTransactionReceipt(txAddr)
    contractAddr = web3.toChecksumAddress(receipt["logs"][0]["data"][-40:])
    return contractAddr

def rpc_checkFlag(contractAddr, level):
    if level == 1:
        flag = factory1Ins.call().validateContract(contractAddr)
    else:
        flag = factory2Ins.call().validateContract(contractAddr)
    return flag

def rpc_testLuck(accountAddr, contractAddr, level):
    password = session.get("password")
    web3.personal.unlockAccount(accountAddr, password)
    if level == 1:
        ins = web3.eth.contract(abi=gacha1ABI, address=contractAddr)
        #txAddr = ins.transact({"from": accountAddr, "gas": 2500000}).pickUp()
        txAddr = ins.transact({"from": accountAddr}).pickUp()
    else:
        ins = web3.eth.contract(abi=gacha2ABI, address=contractAddr)
        #txAddr = ins.transact({"from": accountAddr, "gas": 2500000}).pickUp()
        txAddr = ins.transact({"from": accountAddr}).pickUp()
    res = rpc_checkTX(txAddr)
    wait = 1
    while not res and wait < 10:
        gevent.sleep(wait)
        res = rpc_checkTX(txAddr)
        wait += 1
    if wait == 10:
        return "Timeout"
    receipt = web3.eth.getTransactionReceipt(txAddr)
    return True

def rpc_getPlayed(contractAddr, level):
    if level == 1:
        ins = web3.eth.contract(abi=gacha1ABI, address=contractAddr)
    else:
        ins = web3.eth.contract(abi=gacha2ABI, address=contractAddr)
    return str(ins.call().played())

# ---

@app.route("/kLPm23z6HjnXKD99/admin/show")
def admin():
    return "adminAddr:" + adminAddr +"<br>" + \
           "balance  :" + str(web3.eth.getBalance(adminAddr)) + "<br>" + \
           "blockNum :" + str(web3.eth.blockNumber)

@app.route("/kLPm23z6HjnXKD99/admin/init")
def init():
    web3.miner.setEtherbase(adminAddr)
    miner.start(2)
    return adminAddr

@app.route("/kLPm23z6HjnXKD99/miner/<string:status>")
def miner(status):
    if status == "start":
        web3.miner.start(2)
    else:
        web3.miner.stop()
    return status

# ---

@app.route("/")
def index():
    accountAddr = session.get("accountAddr")
    if not accountAddr == None:
        return redirect("/home", code=302)
    return render_template("index.html")


@app.route("/signup", methods = ["POST"])
def signup():
    password = request.form["pass"]
    try:
        accountAddr = rpc_signUp(password)
    except:
        return "User Creation Error"

    session["password"] = password
    session["accountAddr"] = accountAddr
    db.insert({"accountAddr": accountAddr, "gaveETH": False, "lv1Addr": None, "lv2Addr": None})

    return redirect("/home", code=302)


@app.route("/signin", methods = ["POST"])
def signin():
    accountAddr = request.form["accountAddr"]
    password = request.form["pass"]
    if not rpc_validatePassword(accountAddr, password):
        return redirect("/", code=302)
    res = db.search(User.accountAddr == accountAddr)
    session["password"] = password
    session["accountAddr"] = accountAddr
    return redirect("/home", code=302)


@app.route("/connection")
def connection():
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return redirect("/", code=302)
    tmpdir = tempfile.mkdtemp()
    try:
        archivedir = os.path.join(tmpdir, 'connection')
        os.mkdir(archivedir)
        url = "http://etherver/api/getKey/"  + accountAddr
        res = requests.get(url)
        with open(os.path.join(archivedir, "prv.key"), "w") as fd:
            fd.write(res.text)
        url = "http://etherver/api/getGenesis"
        res = requests.get(url)
        with open(os.path.join(archivedir, "genesis.json"), "w") as fd:
            fd.write(res.text)
        with open(os.path.join(archivedir, "static-nodes.json"), "w") as fd:
            fd.write("[" + enode + "]")
        data = open(shutil.make_archive(os.path.join(tmpdir,"connection.zip"), 'zip', archivedir), 'rb').read()
        ret = Response(data,
                    mimetype='application/zip',
                    headers={'Content-Disposition':'attachment;filename=connection.zip'})
    except:
        ret = "Zip File Error.. Please Try Again."
    finally:
        shutil.rmtree(tmpdir)
    return ret


@app.route("/home")
def home():
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return redirect("/", code=302)
    res = db.search(User.accountAddr == accountAddr)
    gaveETH = res[0]["gaveETH"]
    lv1Addr = res[0]["lv1Addr"]
    lv2Addr = res[0]["lv2Addr"]
    return render_template("home.html", accountAddr=accountAddr, gaveETH=gaveETH, lv1Addr=lv1Addr, lv2Addr=lv2Addr)

@app.route("/getCode/<int:level>")
def getCode(level):
    if not(level == 1 or level == 2):
        return "error"
    return gachaCode[str(level)]


@app.route("/api/getBalance")
def getBalance():
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return "error"
    return str(rpc_getBalance(accountAddr))


@app.route("/api/claimETH")
def claimETH():
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return "error"
    res = db.search(User.accountAddr == accountAddr)
    if res[0]["gaveETH"] == True:
        return "done"
    try:
        txAddr = rpc_giveInitialEther(accountAddr, 10)
        res = rpc_checkTX(txAddr)
        wait = 1
        while not res and wait < 10:
            gevent.sleep(wait)
            res = rpc_checkTX(txAddr)
            wait += 1
    except:
        return "Giving Initial Ether Error"
    if wait == 10:
        return "Timeout"
    db.upsert({'gaveETH': True}, User.accountAddr == accountAddr)
    return "done"


@app.route("/api/deploy/<int:level>")
def deploy(level):
    if not(level == 1 or level == 2):
        return "error"
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return "error"
    key = "lv" + str(level) + "Addr"
    res = db.search(User.accountAddr == accountAddr)
    if res[0][key] != None:
        return "done"
    try:
        contractAddr = rpc_issueNewContract(accountAddr, level)
        db.upsert({key: contractAddr}, User.accountAddr == accountAddr)
    except:
        return "Contract Creation Error"
    return "done"


@app.route("/api/getData/<int:level>")
def getData(level):
    if not(level == 1 or level == 2):
        return "error"
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return "error"
    ret = {}
    key = "lv" + str(level) + "Addr"
    res = db.search(User.accountAddr == accountAddr)
    ret["contractAddr"] = res[0][key]
    if ret["contractAddr"] ==None:
        return json.dumps(ret)
    ret["played"] = rpc_getPlayed(ret["contractAddr"], level)
    if rpc_checkFlag(ret["contractAddr"], level):
        ret["flag"] = flag[str(level)]
    else:
        ret["flag"] = "---"
    return json.dumps(ret)


@app.route("/api/testLuck/<int:level>")
def testLuck(level):
    if not(level == 1 or level == 2):
        return "error"
    accountAddr = session.get("accountAddr")
    if accountAddr == None:
        return "error"
    key = "lv" + str(level) + "Addr"
    res = db.search(User.accountAddr == accountAddr)
    contractAddr = res[0][key]
    if rpc_testLuck(accountAddr, contractAddr, level):
        return "done"


if __name__ == "__main__":
    #app.run(debug = True)
    app.run(debug=False, host="0.0.0.0", port=8000)


