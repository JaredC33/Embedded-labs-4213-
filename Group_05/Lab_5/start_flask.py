from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
    return "Hello World"

if __name__ == '__main__':
    IP = "10.227.115.47" # fill in your IP.
    PORT = "8080"
    DEBUG = True
    app.run(host = IP, port = PORT, debug = DEBUG)