from flask import Flask
from flask import render_template
import serial

#ser = serial.Serial('/dev/ttyUSB0')
#ser.write('I')
#x = ser.readline()
x = "Ready for service"
if "Ready for service" in x:
    ready = True
else:
    ready = False

flavor = {}
health = None

app = Flask(__name__)

@app.route("/")
def main():
    if ready == True:
        return render_template('index.html')
    else:
        return "Error: " + x

@app.route("/selection")
def selector():
    return render_template('select.html')

@app.route("/health")
def health():
    return render_template('health.html')

@app.route("/blending")
def blending():
    return render_template('blending.html')

@app.route("/complete")
def complete():
    return render_template('complete.html')

@app.route("/flavor")
def sel_flavor():
    return "Set flavor array to: " + flavor

@app.route("/sethealth")
def set_health():
    return " Set health to: " + health

@app.route("/make")
def make():
    ser.write('M')
    # Parse Flavor Array here (flavor)
    ser.write()
    return "{ 'status' : ''}"

@app.route("/take")
def take():
    ser.write('T')
    return "{ 'status' : ''}"

@app.route("/rinse")
def rinse():
    ser.write('D')
    return "{ 'status' : ''}"

@app.route("/clean")
def clean():
    ser.write('C')
    return "{ 'status' : '' }"

@app.route("/cancel")
def cancel():
    ser.write('X')
    return "{ 'status' : '' }"

@app.route("/reset")
def reset():
    ser.write('R')
    return "{ 'status' : ''}"

@app.route("/admin")
def admin():
    return render_template('admin.html')
