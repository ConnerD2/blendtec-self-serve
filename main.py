from flask import Flask
from flask import render_template
import serial

#ser = serial.Serial('/dev/ttyUSB0')

app = Flask(__name__)

@app.route("/")
def main():
        #ser.write('I')
        #x = ser.readline()
        x = "Ready for service"
        if "Ready for service" in x:
          	return render_template('index.html')
        else:
                return "Error Encountered, printing Arduino Response: " + x # Improve this so it shows a beautiful page and reports to blendtec

@app.route("/flavor")
def sel_flavor():
    return "Set flavor array to: "

@app.route("/initialize")
def init_arduino():
	return "{ 'status': 'init' }"

@app.route("/make")
def make():
        ser.write('M')
        # Parse Flavor Array here
        ser.write()
        return "{ 'status' : ''}"
