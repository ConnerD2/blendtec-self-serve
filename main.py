from flask import Flask
from flask import render_template, request
from flask_sockets import Sockets
from wifi import Cell, Scheme
from math import floor
import json
import serial
from datetime import datetime

from flask_peewee.db import Database
from peewee import *

# configure our database
DATABASE = {
    'name': 'smoothie.db',
    'engine': 'peewee.SqliteDatabase',
}
DEBUG = True
SECRET_KEY = 'asdfasdfasdfasdfasdf'

ser = serial.Serial('/dev/ttyACM0',115200)
#ser = serial.Serial('/dev/pts/5')
ser.write('I')

smflavor = {}
smhealth = None

class JSONField(TextField):
    def db_value(self, value):
        return json.dumps(value)

    def python_value(self, value):
        if value is not None:
            return json.loads(value)

cupmultiplier = 4.5
productpercent = 20
icepercent = 65
waterpercent = 0

app = Flask(__name__)
app.config.from_object(__name__)
sockets = Sockets(app)
db = Database(app)

class Flavor(db.Model):
    name = TextField()
    slot = IntegerField()
    amount = DecimalField()
    healthproperties = JSONField()
    touchimage = TextField()

class Wifi(db.Model):
    ssid = TextField()
    password = TextField()

class Order(db.Model):
    timestamp = DateTimeField()
    flavor = JSONField()
    # Place anything else we may want to report about an order into this database section

def create_tables():
    Order.create_table()
    Wifi.create_table()
    Flavor.create_table()

#create_tables()

healthproperties = {
    'antioxidants' : 0,
    'detox' : 0,
    'electrolytes': 0,
    'energy': 0,
    'immunity': 0,
    'oatfiber': 0,
    'power': 0,
}


@sockets.route('/status')
def status(ws):
    ser.write('I')
    while not ws.closed:
        x = ser.readline()
        ws.send(x)

@app.route("/")
def main():
    return render_template('index.html')

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
        if localhealth['health'] == "mix":
            smhealth = None
        if localhealth['health'] == "no":
            smhealth = False
        return "Set Health to: " + str(smhealth)
    else:
        return health

@app.route("/make")
def make():
    global smflavor
    #This creates a new record of this order in the database
    Order.create(timestamp = datetime.utcnow(), flavor = smflavor)
    f1 = smflavor['flavor0'][1]
    f2 = smflavor['flavor1'][1]
    f3 = smflavor['flavor2'][1]
    f4 = smflavor['flavor3'][1]
    f5 = smflavor['flavor4'][1]
    f6 = smflavor['flavor5'][1]
    f7 = smflavor['flavor6'][1]
    f8 = smflavor['flavor7'][1]
    smtot = (f1 + f2 + f3 + f4 + f5 + f6 + f7 + f8) * 1.0
    f1 = int(floor((f1/smtot) * productpercent * cupmultiplier))
    f2 = int(floor((f2/smtot) * productpercent * cupmultiplier))
    f3 = int(floor((f3/smtot) * productpercent * cupmultiplier))
    f4 = int(floor((f4/smtot) * productpercent * cupmultiplier))
    f5 = int(floor((f5/smtot) * productpercent * cupmultiplier))
    f6 = int(floor((f6/smtot) * productpercent * cupmultiplier))
    f7 = int(floor((f7/smtot) * productpercent * cupmultiplier))
    f8 = int(floor((f8/smtot) * productpercent * cupmultiplier))
    ice = int(floor(smflavor['ice'] * icepercent * cupmultiplier))
    water = int(floor(smflavor['water'] * waterpercent * cupmultiplier))
    blend = smflavor['blend']
    ser.write('M ' + str(f1) + ' ' + str(f2) + ' ' + str(f3) + ' ' + str(f4) + ' ' + str(f5) + ' ' + str(f6) + ' ' + str(f7) + ' ' + str(f8) + ' ' + str(water) + ' ' + str(ice) + ' ' + str(blend) + '\n')
    return "OK"

@app.route("/take")
def take():
    ser.write('T')
    return "OK"

@app.route("/rinse")
def rinse():
    ser.write('D')
    return "OK"

@app.route("/clean")
def clean():
    ser.write('C')
    return "OK"

@app.route("/cancel")
def cancel():
    flavor = {}
    health = None
    ser.write('X')
    return "OK"

@app.route("/reset")
def reset():
    flavor = {}
    health = None
    ser.write('R')
    return "OK"

@app.route("/admin")
def admin():
    order_list = Order.select().order_by(Order.timestamp.desc()).limit(5)
    return render_template('admin.html',order_list = order_list)

@app.route("/admin/wifi")
@app.route("/admin/wifi/<network>")
def wifi(network=None):
    if network:
        ssid = network
        pw = request.data
        #Connect to network here
    else:
        #Get list of WiFi networks, print here
        iwlist = Cell.all('wlan0')
        wirelesslist = []
        for i in iwlist:
            wirelesslist.append(i.ssid)
            return json.dumps(wirelesslist)
        #Demo version
        #return json.dumps(["BlendTec1","CustomerWifi","BillWiTheScienceFi"])

if __name__ == "__main__":
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler
    server = pywsgi.WSGIServer(('', 5000), app, handler_class=WebSocketHandler)
    server.serve_forever()
