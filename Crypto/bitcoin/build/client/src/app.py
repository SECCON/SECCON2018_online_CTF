import os
import random
import string
import re
from flask import Flask, render_template, request
from subprocess import Popen

app = Flask(__name__)

@app.route('/')
def index():
    return "alive"

@app.route('/api/<string:key>', methods = ['GET', 'POST'])
def checkMessage(key):
    matched = re.match(u"^\w{8}$", key)
    if matched == None:
        return "key format error"

    URL = "http://flask:8000/show/" + key
    #URL = "http://192.168.2.137:8080/" + key
    cmd = ["timeout -sKill 15  chromium-browser --args --disable-xss-auditor --no-sandbox --headless --disable-gpu --remote-debugging-port=9222 "+URL]
    proc = Popen(cmd, shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)
    return "done"

if __name__ == '__main__':
    #app.run(debug = True)
    app.run(debug=False, host='0.0.0.0', port=8000)
