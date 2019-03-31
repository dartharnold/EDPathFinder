//
//  Copyright (C) 2016  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "System.h"
#include "AStarRouter.h"

PresetEntry System::s_presetEntry;

QString System::formatDistance(int64 dist, bool trunc) {
    if(dist > 0) {
        return trunc
               ? QString("%1").arg(static_cast<int64>(dist / 10.0))
               : QString("%1.%2").arg(dist / 10).arg(dist % 10);
    } else {
        return "0.0";
    }
}

System::System(AStarSystemNode *system) : _name(system->name()), _position(system->position()) {}

void System::addSettlement(const QString &planetName, const Settlement &settlement, int distance) {
    for(auto &planet: _planets) {
        if(planet.name() == planetName) {
            planet.addSettlement(settlement);
            return;
        }
    }
    _planets.push_back(Planet(planetName, distance, settlement));
}

System::~System() {
    delete _presetEntry;
}

System::System(const QJsonObject &jsonObject)
        : _name(jsonObject["name"].toString())  {
    auto coords = jsonObject["coords"].toObject();
    _position.setX((float) coords["x"].toDouble());
    _position.setY((float) coords["y"].toDouble());
    _position.setZ((float) coords["z"].toDouble());
}

const QString System::formatPlanets() const {
    QStringList planets;
    addSystemString(planets, ValuableBodyFlagsEW, "Earth-like world");
    addSystemString(planets, ValuableBodyFlagsWT, "Water world (TF)");
    addSystemString(planets, ValuableBodyFlagsWW, "Water world");
    addSystemString(planets, ValuableBodyFlagsAW, "Ammonia world");
    addSystemString(planets, ValuableBodyFlagsTF, "Other TF");
    return planets.join("\n");
}


int32 System::estimatedValue() const {
    return _estimatedValue;
}

void System::setEstimatedValue(int32 estimatedValue) {
    _estimatedValue = estimatedValue / 1000;
}

void System::addSystemString(QStringList &list, ValuableBodyFlags type, QString name) const {
    uint8_t numPlanets = _numPlanets[type];
    if(type == ValuableBodyFlagsWW) {
        numPlanets -= _numPlanets[ValuableBodyFlagsWT];
    }
    switch(numPlanets) {
    case 0:
        break;
    case 1:
        list.append(name);
        break;
    default:
        list.append(QString("%1 x%2").arg(name).arg(numPlanets));
        break;
    }
}

const std::string & System::key() const {
    if(_key.empty()) {
        _key = System::makeKey(_name);
    }
    return _key;
}

const std::string System::makeKey(const QString &name) {
    std::string key(name.toStdString());
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

void System::setPresetEntry(const PresetEntry &presetEntry) {
    if(!_presetEntry) {
        _presetEntry = new PresetEntry;
    }
    *_presetEntry = presetEntry;
}

const PresetEntry &System::presetEntry() const {
    return _presetEntry ? *_presetEntry : s_presetEntry;
}
