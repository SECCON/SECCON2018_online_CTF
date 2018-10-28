python
ls
gunicorn app:app -b :8000 --name app --worker-connections 1000
gunicorn app:app -b :8000 --name app --worker-connections 1000
gunicorn app:app -b :8000 --name app -w 4
python 
ls
gunicorn app:app -b :8000 --name app -k gevent --worker-connections 1000
ls
gunicorn app:app -b :8000 --name app -k gevent --worker-connections 1000
