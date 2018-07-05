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
#include <QtCore/QFile>
#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char* argv[])
{
    constexpr const auto factor = 0.785;
    constexpr const auto hours  = 23;

    QCoreApplication gApp(argc, argv);

    std::shared_ptr<DataEngine> gData[hours];
    CSVParser                   gCSV;

    auto fn = [&]
    {
        std::cout << "requesting " << hours << " downloads...\n";

        for (auto i = 0; i < hours; ++i)
        {
            auto date = QDate::currentDate();
            date.setDate(date.year(), date.month(), date.day());

            gData[i].reset(new DataEngine(date, i, { 24.5f, 24.5f, 43.25f, 43.25f }));

            QObject::connect(gData[i].get(), &DataEngine::downloadFailed, [&]
            {
                std::cout << "\nRecords FAILED to download at "
                          << QTime::currentTime().toString("HH:mm").toStdString()
                          << "\n\n";
            });

            QObject::connect(gData[i].get(), &DataEngine::downloadFinished, [&](DataEngine::string_type const& file)
            {
                // append csv file to the database
                gCSV += file.toStdString() + ".csv";

                QFile(file).remove();

                if (gCSV.lineCount() >= 22)
                {
                    RedNodeJson gJsonRad(gCSV, [&](std::size_t idx)
                    {
                        return gCSV.get<6>(idx).toDouble();
                    });

                    RedNodeJson gJsonPower(gCSV, [&](std::size_t idx)
                    {
                        return (gCSV.get<6>(idx).toDouble() * 2.4 / 1000.) * factor;
                    });

                    auto jsonRadDoc   = gJsonRad  .parse();
                    auto jsonPowerDoc = gJsonPower.parse();

                    gJsonRad.save(jsonRadDoc, "db/grib2.rad.json");
                    gJsonRad.save(jsonRadDoc, "db/grib2." + gJsonRad.csv().dateToString() + ".rad.json");

                    gJsonPower.save(jsonPowerDoc, "db/grib2.power.json");
                    gJsonPower.save(jsonPowerDoc, "db/grib2." + gJsonRad.csv().dateToString() + ".power.json");

                    gCSV.clear();

                    std::cout << "\nRecords downloaded at "
                              << QTime::currentTime().toString("HH:mm").toStdString()
                              << "\n\n";
                }
            });
        }
    };


    std::function<void()> scheduler;

    scheduler = [&]{
        auto time = QTime::currentTime();

        for (auto h = time.hour(); h < 24; ++h)
        {
            for (auto m = time.minute(); m < 60; m += 24)
            {
                time.setHMS(h, m, 0);
                runAt(time, fn);
            }
        }
    };

    fn       ();
    scheduler();
    return gApp.exec();
}
