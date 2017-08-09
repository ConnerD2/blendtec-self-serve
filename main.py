from flask import Flask
from flask import render_template, request
from flask_sockets import Sockets
from wifi import Cell, Scheme
import json
import serial

#ser = serial.Serial('/dev/ttyUSB0')
#ser.write('I')
#x = ser.readline()
x = "Ready for service"
if "Ready for service" in x:
    ready = True
else:
    ready = False

smflavor = {}
smhealth = None

app = Flask(__name__)
sockets = Sockets(app)

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

@sockets.route('/status')
def echo_socket(ws):
    while not ws.closed:
        #Wait until serial sends complete message
        #s = ser.readline()
        import time
        time.sleep(5)
        s = 'Done'
        if 'Done' in s:
            ws.send("OK")
        else:
            ws.send("FAIL")

@app.route("/complete")
def complete():
    return render_template('complete.html')

@app.route("/flavor",methods=['GET', 'POST'])
def sel_flavor():
    if request.method == 'POST':
        flavor = request.get_json(force=True)
        global smflavor
        smflavor = flavor
        return "Set Flavor to: " + str(flavor)
    else:
        return str(flavor)

@app.route("/sethealth", methods=['GET','POST'])
def set_health():
    if request.method == 'POST':
        localhealth = request.get_json(force=True)
        global smhealth
        if localhealth['health'] == "yes":
            smhealth = True
        if localhealth['health'] == "no":
            smhealth = False
        return "Set Health to: " + str(smhealth)
    else:
        return health

@app.route("/make")
def make():
    #ser.write('M')
    global smflavor
    global smhealth
    print json.dumps(smflavor)
    print smhealth
    #ser.write()
    return "OK"

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
    flavor = {}
    health = None
    ser.write('X')
    return "{ 'status' : '' }"

@app.route("/reset")
def reset():
    flavor = {}
    health = None
    ser.write('R')
    return "{ 'status' : ''}"

@app.route("/admin")
def admin():
    return render_template('admin.html')

@app.route("/admin/wifi")
@app.route("/admin/wifi/<network>")
def wifi(network=None):
    if network:
        ssid = network
        pw = request.data
        #Connect to network here
    else:
        #Get list of WiFi networks, print here
        #iwlist = Cell.all('wlan0')
        #wirelesslist = []
        #for i in iwlist:
        #    wirelesslist.append(i.ssid)
        return json.dumps(["BlendTec1","CustomerWifi","BillWiTheScienceFi"])

if __name__ == "__main__":
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler
    server = pywsgi.WSGIServer(('', 5000), app, handler_class=WebSocketHandler)
    server.serve_forever()
