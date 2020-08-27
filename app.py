# import SQLAlchemy as SQLAlchemy
import random
import datetime
import sqlite3
import time
import serial as serial
import serial
from serial import Serial
from time import sleep
from flask_sqlalchemy import SQLAlchemy
from flask import Flask, render_template, request, redirect, url_for
from sqlalchemy import Column, Integer, Text
__all__ = [serial, SQLAlchemy, Flask, render_template, request, redirect, url_for]
app = Flask(__name__)

app.config['SECRET_KEY'] = 'hello'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///site.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)
report_values = [0, 0, 0, 0, 0]


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(20), unique=True, nullable=False)
    temp_air = db.Column(db.Integer, default=0)
    temp_soil = db.Column(db.Integer, default=0)
    hum_air = db.Column(db.Integer, default=0)
    hum_soil = db.Column(db.Integer, default=0)

    def __repr__(self):
        return f"Plant('{self.name}', '{self.temp_air}', '{self.temp_soil}', '{self.hum_air}', '{self.hum_soil}')"

    def __init__(self, id, name, temp_air, temp_soil, hum_air, hum_soil):
        self.id = id
        self.name = name
        self.temp_air = temp_air
        self.temp_soil = temp_soil
        self.hum_air = hum_air
        self.hum_soil = hum_soil


@app.route('/')
@app.route('/index')
def index():
    return render_template('Menu.html', title='Home')


@app.route('/Humidity', methods=['POST', 'GET'])
def humidity():
    if request.method == "POST":
        plant_soil_hum = request.form['formControlRange']
        plant_air_hum = request.form['formControlRange2']
        found_plant = User.query.filter_by(id=1).first()
        if found_plant:
            found_plant.hum_soil = plant_soil_hum
            found_plant.hum_air = plant_air_hum
            db.session.commit()
            humidity_write(found_plant.hum_soil, found_plant.hum_air)
        return redirect(url_for("index"))
    else:
        return render_template('Humidity.html', title='Humidity')


@app.route('/Temperature', methods=['POST', 'GET'])
def temperature():
    if request.method == "POST":
        #plant_soil_temp = request.form['formControlRange']
        plant_air_temp = request.form['formControlRange2']
        found_plant = User.query.filter_by(id=1).first()
        if found_plant:
            #found_plant.temp_soil = plant_soil_temp
            found_plant.temp_air = plant_air_temp
            db.session.commit()
            temperature_write(found_plant.temp_air)
        return redirect(url_for("index"))
    else:
        return render_template('Temperature.html', title='Temperature')


@app.route('/Report')
def report():
    report_read()

    temp_air = report_values[1]
    heath_index = report_values[2]
    hum_air = report_values[0]
    hum_soil = report_values[3]
    h_per = report_values[4]

    return render_template('GenReport.html', title='Report', t_air=temp_air, t_soil=heath_index, hum_air=hum_air, hum_soil=hum_soil, hum_percent=h_per)


@app.route('/Manual', methods=['POST', 'GET'])
def manual():
    if request.method == "POST":
        if request.form['status'] == 'buttonOn':
            f = open("C:\\Users\\theo\\Desktop\\arduino\\onoff.txt", "w") #schimb fisier
            f.write("1")
            f.close()
            return redirect(url_for("index"))
        elif request.form['status'] == 'buttonOff':
            f = open("C:\\Users\\theo\\Desktop\\arduino\\onoff.txt", "w") #schimb fisier
            f.write("0")
            f.close()
            return redirect(url_for("index"))
    elif request.method == "GET":
        return render_template('Manual.html', title='Manual On/Off')


@app.route('/AddPlant', methods=['POST', 'GET'])
def addplant():
    if request.method == "POST":
        plant_name = request.form['inputName']
        new_plant = User(id=random.randint(1, 1000), name=plant_name, temp_air=0, temp_soil=0, hum_air=0, hum_soil=0)
        db.session.add(new_plant)
        db.session.commit()
        return redirect(url_for("index"))
    else:
        return render_template('AddPlant.html', title='Add a new plant')


def read_arduino():
    arduinoData = serial.Serial('COM9', 9600, timeout=0.1)
    while arduinoData.inWaiting:
        val = arduinoData.readline().decode('ascii')
        if len(val) == 13:
            return val


def humidity_write(value_soil, value_air):
    total_air = 'Air Humidity: {}'.format(value_air)
    total_soil = 'Soil Humidity: {}'.format(value_soil)

    with open("C:\\Users\\theo\\Desktop\\arduino\\config.txt", 'r+') as f:
        contents_air = f.read().replace('Air Humidity: ', total_air)
        f.seek(0)
        f.truncate()
        f.write(contents_air)
    with open("C:\\Users\\theo\\Desktop\\arduino\\config.txt", 'r+') as f:
        contents_soil = f.read().replace('Soil Humidity: ', total_soil)
        f.seek(0)
        f.truncate()
        f.write(contents_soil)


def temperature_write(value_air):
    total_air = 'Air Temperature: {}'.format(value_air)

    with open("C:\\Users\\theo\\Desktop\\arduino\\config.txt", 'r+') as f:
        contents_air = f.read().replace('Air Temperature: ', total_air)
        f.seek(0)
        f.truncate()
        f.write(contents_air)


def report_read():
    i = 0

    with open("C:\\Users\\theo\\Desktop\\arduino\\Measurement_1.txt", "r") as a_file:
        for line in a_file:
            if ':' in line:
                mock, value = line.rstrip().split(':')
                report_values[i] = value
                i = i+1
            if i == 5:
                break


if __name__ == '__main__':
    db.create_all()
    app.run(debug=True)

