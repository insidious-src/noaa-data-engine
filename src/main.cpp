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
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <memory>

int main(int argc, char* argv[])
{
    QCoreApplication gApp(argc, argv);

    std::shared_ptr<DataEngine> gData[23];
    CSVParser                   gCSV;

    RedNodeJson gJsonRad(gCSV, "db/grib2.json", [&](std::size_t idx)
    {
        return gCSV.get<6>(idx).toDouble();
    });

    RedNodeJson gJsonPower(gCSV, "db/grib2.power.json", [&](std::size_t idx)
    {
        return (gCSV.get<6>(idx).toDouble() * 2.5 / 1000.) * 0.86;
    });

    for(auto i = 0; i < 23; ++i)
    {
        auto date = QDate::currentDate();
        date.setDate(date.year(), date.month(), date.day());

        gData[i].reset(new DataEngine(date, i, { 24.5f, 24.5f, 43.25f, 43.25f }));

        QObject::connect(&(*gData[i]), &DataEngine::downloadFinished, [&](DataEngine::string_type const& file)
        {
            static auto count = 0U;

            // append csv file to the database
            gCSV += file.toStdString() + ".csv";
            ++count;

            if (count >= 22)
            {
                gJsonRad  .save();
                gJsonPower.save();
                QCoreApplication::quit();
            }
        });
    }

    return gApp.exec();
}
