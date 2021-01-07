
#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <vector>
#include <iostream>
#include <map>

//using namespace std;

class TimeSeries {
    std::string fileName;
    // map of data
    std::map<std::string, std::vector<float>> mapping;

public:
    TimeSeries(const char* CSVfileName);
    virtual ~TimeSeries();
    const std::vector<std::string> getFeaturesNames() const;
    const std::vector<float> getFeature(std::string featureName) const;

private:
    void read_from_file();
};



#endif /* TIMESERIES_H_ */



