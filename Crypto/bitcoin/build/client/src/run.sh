#!/bin/bash

./electrum daemon start
python3 app.py

#sleep 3
#chromium-browser --no-sandbox --headless --disable-gpu --remote-debugging-port=9222 http://192.168.2.137:8080
#tail -f /dev/null
