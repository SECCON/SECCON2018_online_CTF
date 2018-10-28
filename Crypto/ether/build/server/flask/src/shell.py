import subprocess
from web3 import Web3,HTTPProvider,IPCProvider
def dig(domain):
    result = subprocess.run(['nslookup', domain], stdout=subprocess.PIPE)
    ip = str(result.stdout).split("\\n")[-3].split(" ")[1]
    return ip

etherverIP = dig("etherver")
url = 'http://' + etherverIP + ':8545'
web3 = Web3(HTTPProvider(url))

