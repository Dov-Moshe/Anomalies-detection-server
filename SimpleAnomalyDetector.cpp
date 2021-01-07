#include "SimpleAnomalyDetector.h"
#include <iterator>


SimpleAnomalyDetector::SimpleAnomalyDetector() {}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {}


void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {

    std::vector<std::string> featuresNames = ts.getFeaturesNames();
    int numOfFeature = featuresNames.size();
    int numberOfRow = ts.getFeature(featuresNames[0]).size();
    
    float* features[numOfFeature];
    for(int i = 0; i < numOfFeature; i++) {
        features[i] = new float[numberOfRow];
        std::vector<float> temp = ts.getFeature(featuresNames[i]);
        for(int j = 0; j < numberOfRow; j++) {
            features[i][j] = temp[j];
        }
    }

    for(int i = 0; i < numOfFeature - 1; i++) {
        for (int j = i + 1; j < numOfFeature; j++) {
            correlatedFeatures correlationFeatures;
            // checking the correlation
            float correlation = pearson(features[i], features[j], numberOfRow);
            correlationFeatures.corrlation = correlation;
            // if it's a good correlation
            if(abs(correlation) > 0.5) {
                correlationFeatures.feature1 = ts.getFeaturesNames()[i];
                correlationFeatures.feature2 = ts.getFeaturesNames()[j];

                // creating the 'Regression Line'
                Point* ps[numberOfRow];
                for(int k = 0; k < numberOfRow; k++) {
                    ps[k]=new Point(features[i][k],features[j][k]);
                }
                correlationFeatures.lin_reg = linear_reg(ps,numberOfRow);
                // free the allocation of the points
                for(int i=0;i<numberOfRow;i++)
                    delete ps[i];

                // calculate the threshold
                float thresholdTemp = 0;
                for(int k = 0; k < numberOfRow; k++) {
                    float d = dev(Point(features[i][k], features[j][k]),correlationFeatures.lin_reg);
                    if(d > thresholdTemp) {
                        thresholdTemp = d;
                    }
                }
                correlationFeatures.threshold = thresholdTemp * 1.8;
                // adding the new 'correlationFeatures' struct
                cf.push_back(correlationFeatures);
            }
        }
    }
    // free the allocation of the array
    for(int i = 0; i < numOfFeature; i++)
        delete [] features[i];
}


vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts) {
    // getting number of row
    int numberOfRow = ts.getFeature(ts.getFeaturesNames()[0]).size();
    vector<AnomalyReport> v;
    // check for every struct of correlatedFeatures
    for (auto it = cf.begin(); it != cf.end(); it++) {
        vector<float> v1 = ts.getFeature(it->feature1);
        vector<float> v2 = ts.getFeature(it->feature2);
        // check every row for two specific features
        for (int i = 0; i < numberOfRow; i++) {
            float y = it->lin_reg.f(v1[i]);
            float distance = abs(y - v2[i]);
            // if there is a anomaly, then creating a new object of 'AnomalyReport'
            if(it->corrlation >= threshold && distance > it->threshold) {
                v.push_back(AnomalyReport(it->feature1 + "-" + it->feature2, i+1));
            }
        }
    }
    return v;

}