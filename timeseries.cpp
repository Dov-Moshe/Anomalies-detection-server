#include "timeseries.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


TimeSeries::TimeSeries(const char* CSVfileName):fileName(CSVfileName){
    read_from_file();
}

void TimeSeries::read_from_file() {
    std::ifstream file(fileName);
    std::string line, word;

    // set the feature names
    std::getline(file, line, '\n');
    std::stringstream str(line);
    std::vector<std::string> featureNames;
    while(std::getline(str,  word, ',')) {
        featureNames.push_back(word);
    }

    int numOfFeature = featureNames.size();
    // set vectors for the features
    for (int i = 0; i < numOfFeature; i++) {
        std::vector<float> feature;
        mapping.insert(std::pair<std::string , std::vector<float>>(featureNames[i], feature));
    }

    // set the value of the features
    while(file) {
        // get row from file
        std::getline(file, line, '\n');
        // represent string as stream
        std::stringstream str(line);
        if (str.str() != "") {
            // split the data of the row
            for (int i = 0; i < numOfFeature; i++) {
                std::getline(str, word, ',');
                mapping[featureNames[i]].push_back(std::stof(word));
            }
        }
    }
    file.close();
}

const std::vector<std::string> TimeSeries::getFeaturesNames() const {
    std::vector<std::string> keys;
    for (auto const& imap: mapping) {
        keys.push_back(imap.first);
    }
    return keys;
}

const std::vector<float> TimeSeries::getFeature(std::string featureName) const {
    return mapping.find(featureName)->second;
}

TimeSeries::~TimeSeries() {}