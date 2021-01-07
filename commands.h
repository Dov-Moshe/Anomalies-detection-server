

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
};

// you may add here helper classes

class StandardIO: public DefaultIO {
    ifstream in;
public:
    virtual string read() {
        string s;
        in>>s;
        if(s.compare("done") == 0)
            in.close();
        return s;
    }
    virtual void write(string text) {
        cout << text;
        //out << text;
        if(text.compare("Please upload your local train CSV file.\n") == 0 ||
        text.compare("Please upload your local test CSV file.\n") == 0 ||
        text.compare("Please upload your local anomalies file.\n") == 0) {
            string s;
            cin>>s;
            in.open(s);
        }
    }
    virtual void write(float f) {
        cout << f << endl;
    }
    virtual void read(float* f) {
        cin >> *f;
    }
    ~StandardIO() {
        if(in.is_open())
            in.close();
    }
};

struct Data{
    HybridAnomalyDetector hybrid;
    TimeSeries* train = nullptr;
    TimeSeries* test = nullptr;
    vector<AnomalyReport> ar;
};

// you may edit this class
class Command{
    string description;
protected:
    DefaultIO* dio;
public:
	Command(DefaultIO* dio, string description):dio(dio),description(description){}
	virtual void execute()=0;
	virtual ~Command(){}
	virtual string getDescription () {
        return description;
	}
};

// implement here your command classes

class UploadCsv: public Command {
public:
    UploadCsv(DefaultIO* dio) : Command(dio, "1.upload a time series csv file") {}
    void execute() override {
        string line;
        // reading and writing to file the "Anomaly Train"
        Command::dio->write("Please upload your local train CSV file.\n");
        ofstream outTrain("anomalyTrain.csv");
        while(true) {
            line = Command::dio->read();
            if(line.compare("done") == 0) {
                break;
            }
            outTrain << line << endl;
        }
        outTrain.close();
        Command::dio->write("Upload complete.\n");

        // reading and writing to file the "Anomaly Test"
        Command::dio->write("Please upload your local test CSV file.\n");
        ofstream outTest("anomalyTest.csv");
        while(true) {
            line = Command::dio->read();
            if(line.compare("done") == 0) {
                break;
            }
            outTest << line << endl;
        }
        outTest.close();
        Command::dio->write("Upload complete.\n");
    }

};

class Settings: public Command {
    Data* data;
public:
    Settings(DefaultIO* dio, Data* data) : Command(dio, "2.algorithm settings") {
        Settings::data = data;
    }
    void execute() {
        dio->write("The current correlation threshold is ");
        dio->write(floor(data->hybrid.getCorrelation()*100)/100);
        dio->write("\n");
        int goBack = true;
        float f;
        do {
            dio->read(&f);
            if(f < 0 || f > 1) {
                dio->write("please choose a value between 0 and 1\n");
            } else {
                goBack = false;
            }
        } while (goBack);
        data->hybrid.changeCorrelation(f);
        dio->write("Type a new threshold\n");
    }
};

class Detect: public Command {
    Data* data;
public:
    Detect(DefaultIO* dio, Data* data) : Command(dio, "3.detect anomalies") {
        Detect::data = data;
    }
    void execute() {
        TimeSeries* tsTrain = new TimeSeries("anomalyTrain.csv");
        data->train = tsTrain;
        data->hybrid.learnNormal(*tsTrain);
        TimeSeries* tsTest = new TimeSeries("anomalyTest.csv");
        data->test = tsTest;
        data->ar = data->hybrid.detect(*tsTest);
        dio->write("anomaly detection complete.\n");
    }
};

class Display: public Command {
    Data* data;
public:
    Display(DefaultIO* dio, Data* data) : Command(dio, "4.display results") {
        Display::data = data;
    }
    void execute() {

        for(auto it = data->ar.begin(); it != data->ar.end(); it++) {
            dio->write(to_string(it->timeStep) + "\t" + it->description + "\n");
        }
        dio->write("Done.\n");
    }
};

class UploadAnomalies: public Command {
    Data* data;
public:
    UploadAnomalies(DefaultIO* dio, Data* data) : Command(dio, "5.upload anomalies and analyze results") {
        UploadAnomalies::data = data;
    }
    void execute() {
        dio->write("Please upload your local anomalies file.\n");
        string line;
        vector<pair<float, float>> anomalies;
        while(true) {
            line = Command::dio->read();
            if(line.compare("done") == 0) {
                break;
            }
            findStartEnd(anomalies, line);
        }
        dio->write("Upload complete.\n");
        vector<float> result = findDifference(anomalies);
        //dio->write("True Positive Rate: " + to_string(result[0]) + "\n");
        //dio->write("False Positive Rate: " + to_string(result[1]) + "\n");

        float TP = result[0];
        float FP = result[1];
        dio->write("True Positive Rate: ");
        dio->write(floor(TP*1000)/1000);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        dio->write(floor(FP*1000)/1000);
        dio->write("\n");

        /*for(auto & anomalie : anomalies) {
            cout << anomalie.first  << "\t"  << anomalie.second  <<  "\n";
        }*/

    }

    void findStartEnd(vector<pair<float, float>>& anomalies, string line) {
        pair<float, float> anomaly;
        stringstream str(line);
        string timeStep;
        getline(str, timeStep, ',');
        anomaly.first = stof(timeStep);
        getline(str, timeStep, ',');
        anomaly.second = stof(timeStep);
        anomalies.push_back(anomaly);
    }

    vector<float> findDifference(vector<pair<float, float>>& anomalies) {
        vector<pair<pair<float, float>, bool>> rangeReports = getRangeReports();
        vector<float> difference;
        int TP = 0;
        int FP = 0;
        float P = anomalies.size();
        float numOfRow = data->test->getFeature(data->test->getFeaturesNames()[0]).size();
        float N;
        for(auto it = anomalies.begin(); it != anomalies.end(); it++) {
            N = numOfRow - (it->second - it->first + 1);
            for(auto itReport = rangeReports.begin(); itReport != rangeReports.end(); itReport++) {
                if((itReport->first.second >= it->first && itReport->first.first <= it->first) ||
                (itReport->first.first >= it->first && itReport->first.first <= it->second) ||
                (itReport->first.first >= it->first && itReport->first.second <= it->second)) {
                    TP++;
                    itReport->second = true;
                }
            }
        }
        for(auto it = rangeReports.begin(); it != rangeReports.end(); it++) {
            if (!it->second)
                FP++;
        }

        difference.push_back(TP/P);
        difference.push_back(FP/N);
        return difference;
    }

    vector<pair<pair<float, float>, bool>> getRangeReports() {
        vector<pair<pair<float, float>, bool>> rangeReports;
        auto it = data->ar.begin();
        while (it != data->ar.end()) {
            pair<pair<float, float>, bool> timeStep;
            timeStep.first.first = it->timeStep;
            timeStep.first.second = it->timeStep;
            string description = it->description;
            timeStep.second = false;
            while (true) {
                it++;
                if (it == data->ar.end()) {
                    rangeReports.push_back(timeStep);
                    break;
                } else if (it->description.compare(description) == 0 && it->timeStep == timeStep.first.second + 1) {
                    timeStep.first.second = it->timeStep;
                } else {
                    rangeReports.push_back(timeStep);
                    break;
                }
            }
        }
        return rangeReports;
    }

};

class Exit: public Command {
public:
    Exit(DefaultIO* dio) : Command(dio, "6.exit") {}
    void execute() {}
};




#endif /* COMMANDS_H_ */
