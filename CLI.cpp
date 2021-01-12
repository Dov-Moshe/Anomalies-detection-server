#include "CLI.h"

CLI::CLI(DefaultIO* dio) {
    CLI::dio = dio;
    mapping.insert(pair<float, Command*>(1, new UploadCsv(dio)));
    mapping.insert(pair<float, Command*>(2, new Settings(dio, &data)));
    mapping.insert(pair<float, Command*>(3, new Detect(dio, &data)));
    mapping.insert(pair<float, Command*>(4, new Display(dio, &data)));
    mapping.insert(pair<float, Command*>(5, new UploadAnomalies(dio, &data)));
    mapping.insert(pair<float, Command*>(6, new Exit(dio)));
}

void CLI::start() {
    while (true) {
        dio->write("Welcome to the Anomaly Detection Server.\n");
        dio->write("Please choose an option:\n");
        for (map<float, Command*>::iterator it = mapping.begin(); it != mapping.end(); ++it) {
            string s = it->second->getDescription() + "\n";
            dio->write(s);
        }
        float f;
        dio->read(&f);
        if(f == 6)
            break;
        mapping.find(f)->second->execute();
    }
}

CLI::~CLI() {
    for (map<float, Command*>::iterator it = mapping.begin(); it != mapping.end(); ++it)
        delete it->second;
    if(data.train != nullptr)
        delete data.train;
    if(data.test != nullptr)
        delete data.test;
}

