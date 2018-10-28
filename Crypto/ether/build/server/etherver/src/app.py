import os
from flask import Flask

app = Flask(__name__)
app.config["SECRET_KEY"] = "insecure"

keyDir = "/usr/src/etherver/keystore"

@app.route("/api/getKey/<string:accountAddr>")
def getKey(accountAddr):
    files = os.listdir(keyDir)
    f = [fname for fname in files if accountAddr[2:].lower() in fname]
    with open(os.path.join(keyDir, f[0]), "r") as fd:
        key = fd.read()
    return key

@app.route("/api/getGenesis")
def getGenesis():
    with open("/usr/src/etherver/genesis.json", "r") as fd:
        genesis = fd.read()
    return genesis

if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0", port=80)

