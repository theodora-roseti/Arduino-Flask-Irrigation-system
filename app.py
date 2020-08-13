# import SQLAlchemy as SQLAlchemy
import random

from flask_sqlalchemy import SQLAlchemy
from flask import Flask, render_template, request, redirect, url_for
from sqlalchemy import Column, Integer, Text

app = Flask(__name__)

app.config['SECRET_KEY'] = 'hello'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///site.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)


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
        return redirect(url_for("index"))
    else:
        return render_template('Humidity.html', title='Humidity')


@app.route('/Temperature', methods=['POST', 'GET'])
def temperature():
    if request.method == "POST":
        plant_soil_temp = request.form['formControlRange']
        plant_air_temp = request.form['formControlRange2']
        found_plant = User.query.filter_by(id=1).first()
        if found_plant:
            found_plant.temp_soil = plant_soil_temp
            found_plant.temp_air = plant_air_temp
            db.session.commit()
        return redirect(url_for("index"))
    else:
        return render_template('Temperature.html', title='Temperature')


@app.route('/Report')
def report():
    found_plant = User.query.filter_by(id=1).first()
    if found_plant:
        temp_air = found_plant.temp_soil
        temp_soil = found_plant.temp_air
        hum_soil = found_plant.hum_soil
        hum_air = found_plant.hum_air

    return render_template('GenReport.html', title='Report', t_air=temp_air, t_soil=temp_soil, h_air=hum_air, hum_soil=hum_soil)


@app.route('/Manual', methods=['POST', 'GET'])
def manual():
    if request.method == "POST":
        if request.form['status'] == 'buttonOn':
            return redirect(url_for("index"))
        elif request.form['status'] == 'buttonOff':
            pass
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


if __name__ == '__main__':
    db.create_all()
    app.run(debug=True)
