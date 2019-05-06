//
//  Copyright (C) 2017  David Hedbor <neotron@gmail.com>
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

#include <QListView>
#include "SystemEntryCoordinateResolver.h"
#include "EDSMQueryExecutor.h"


SystemEntryCoordinateResolver::SystemEntryCoordinateResolver(QObject *parent, AStarRouter *router, QLineEdit *lineEdit,
                                                             QLabel *x, QLabel *y, QLabel *z)
        : QObject(parent), _router(router), _lineEdit(lineEdit), _pendingLookups(),_x(x), _y(y), _z(z) {
    auto completer = new QCompleter(_router, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    auto popup = new QListView();
    popup->setBatchSize(10);
    popup->setLayoutMode(QListView::Batched);
    popup->setUniformItemSizes(true);
    completer->setPopup(popup);
    connect(completer, SIGNAL(activated(const QString &)), this, SLOT(onCompletion(const QString &)));
    _lineEdit->setCompleter(completer);
    connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(onEntry()));
}



void SystemEntryCoordinateResolver::downloadSystemCoordinates(const QString &systemName, int retryCount) {
    if(_pendingLookups.contains(systemName)) {
        return;
    }
    _retries = retryCount;
    _pendingLookups << systemName.toLower();
    auto executor = EDSMQueryExecutor::systemCoordinateRequest(systemName);
    connect(executor, &QThread::finished, executor, &QObject::deleteLater);
    connect(executor, &EDSMQueryExecutor::coordinatesReceived, this, &SystemEntryCoordinateResolver::systemCoordinatesReceived);
    connect(executor, &EDSMQueryExecutor::coordinateRequestFailed, this,
            &SystemEntryCoordinateResolver::systemCoordinatesRequestFailed);
    executor->start();
    emit systemLookupInitiated(systemName);

}


void SystemEntryCoordinateResolver::systemCoordinatesRequestFailed(const QString &systemName) {
    _retries++;
    if(_retries < 2) {
        downloadSystemCoordinates(systemName, _retries);
        return;
    }
    _pendingLookups.remove(systemName.toLower());
    emit systemLookupFailed(systemName);
}

void SystemEntryCoordinateResolver::systemCoordinatesReceived(const System &system) {
    auto systemName = QString(system.name().toLower());
    if(0.0 == system.x() && 0.0 == system.y() && 0.0 == system.z()) {
        // If coords are all zero, this is not a valid lookup.  - that means it's Sol. 
        systemCoordinatesRequestFailed(systemName);
    } else {
        _pendingLookups.remove(systemName);
        _router->addSystem(system);
        sendSystemLookupCompleted(system);
    }
}


void SystemEntryCoordinateResolver::onCompletion(const QString &systemName) {
    resolve(systemName);
}

void SystemEntryCoordinateResolver::onEntry() {
    resolve(_lineEdit->text());
}

void SystemEntryCoordinateResolver::resolve(const QString &systemName) {
    if(systemName.isEmpty()) {
        if(_x) { _x->setText("-"); }
        if(_y) { _y->setText("-"); }
        if(_z) { _z->setText("-"); }
        return;
    }
    auto system = _router->findSystemByName(systemName);
    if(system) {
        sendSystemLookupCompleted(*system);
    } else {
        downloadSystemCoordinates(systemName);
    }
}

bool SystemEntryCoordinateResolver::isComplete() const {
    return _pendingLookups.empty();
}

void SystemEntryCoordinateResolver::sendSystemLookupCompleted(const System &system) {
    if(_x) { _x->setText(QString::number(system.x())); }
    if(_y) { _y->setText(QString::number(system.y())); }
    if(_z) { _z->setText(QString::number(system.z())); }
    _lineEdit->setText(system.name());

    emit systemLookupCompleted(system);
}







