/*
 * Copyright 2018 K. Petrov <fymfifa@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <noaa/csv.h>
#include <noaa/json.h>
#include <noaa/dataengine.h>
//#include <noaa/ipcinterface.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <memory>

int main(int argc, char* argv[])
{
    QCoreApplication gApp(argc, argv);

    std::shared_ptr<DataEngine> gData[16];
    CSVParser                   gCSV;
    JsonParser                  gJson(gCSV, "grib2.json");

    for(auto i = 0, n = 4; n < 20; ++n, ++i)
    {
        gData[i].reset(new DataEngine(QDate(2018, 6, 26), n, { 24.5f, 24.5f, 43.25f, 43.25f }, gJson));
    }

    //QDBusConnection connection = QDBusConnection::sessionBus();
    //connection.registerService("org.SolarPower");

    //IPCInterface iface(connection);

    return gApp.exec();
}
