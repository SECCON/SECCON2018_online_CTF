import os
import random
import string
import re
from flask import Flask, render_template, request, make_response
from subprocess import Popen

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/post', methods = ['POST'])
def messagePost():
    def generateRandomString(n):
        return ''.join(random.choices(string.ascii_lowercase + string.ascii_uppercase + string.digits, k=n))
    def filterString(message):
        # xss filtering~~
        return message
    key = generateRandomString(8)
    keyPath = os.path.join("./", "dest", key)
    message = filterString(request.form["pirate-forms-contact-message"])
    with open(keyPath, "w") as fd:
        fd.write(message)

    cmd = ["curl http://client:8000/api/"+key]
    proc = Popen(cmd, shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)

    return render_template("message.html", message=message)

@app.route('/show/<string:key>', methods = ['GET'])
def messageShow(key):
    matched = re.match(u"^\w{8}$", key)
    if matched == None:
        return "key format error"
    keyPath = os.path.join("./", "dest", key)
    try:
        with open(keyPath, "r") as fd:
            message = fd.read()
    except:
        return "key is invalid"
    resp = make_response(render_template("message2.html", message=message))
    resp.set_cookie('hint', 'Pz8/ID8/PyA/Pz8gPz8/ID8/PyA/Pz8gPz8/ID8/PyA/Pz8gPz8/ID8/PyA/Pz8gPz8/ID8/PyA/Pz8gPz8/ID8/PyA/Pz8gPz8/ID8/PyBwaWVjZSBjcmltZSBtaXh0dXJlIGFydHdvcms=')
    return resp

if __name__ == '__main__':
    app.run(debug = True)
    #app.run(debug=False, host='0.0.0.0', port=8000)
