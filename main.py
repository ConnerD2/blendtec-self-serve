from flask import Flask
from flask import render_template
app = Flask(__name__)

@app.route("/")
def main():
	return render_template('index.html')

@app.route("/flavor")
def sel_flavor(flavor=flavor):
	return flavor

@app.route("/initialize")
def init_arduino():
	return "{ 'status': 'init' }"
