#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {}

void HybridAnomalyDetector::learnNormal(const TimeSeries& ts) {
    SimpleAnomalyDetector::learnNormal(ts);
    learnNormalCircle(ts);
}

vector<AnomalyReport> HybridAnomalyDetector::detect(const TimeSeries& ts) {
    vector<AnomalyReport> d = SimpleAnomalyDetector::detect(ts);
    detectCircle(ts, d);
    return d;
}

HybridAnomalyDetector::~HybridAnomalyDetector() {}

void HybridAnomalyDetector::learnNormalCircle(const TimeSeries& ts) {
    for (auto it = cf.begin(); it != cf.end(); it++) {
        if (it->corrlation < SimpleAnomalyDetector::threshold) {
            vector<float> f1= ts.getFeature(it->feature1);
            vector<float> f2= ts.getFeature(it->feature2);
            size_t size = f1.size();
            // create array of points
            Point** ps_copy=new Point*[size];
            for(size_t j=0;j<size;j++)
                ps_copy[j]=new Point(f1[j],f2[j]);
            // find the minimum circle
            Circle c = findMinCircle(ps_copy, size);
            it->threshold = c.radius * 1.1;
            it->x = c.center.x;
            it->y = c.center.y;
            // delete array of points
            for(size_t j=0;j<size;j++)
                delete ps_copy[j] ;
            delete [] ps_copy;
        }
    }
}

void HybridAnomalyDetector::detectCircle(const TimeSeries& ts, vector<AnomalyReport>& d) {
    int numberOfRow = ts.getFeature(ts.getFeaturesNames()[0]).size();
    for (auto it = cf.begin(); it != cf.end(); it++) {
        vector<float> v1 = ts.getFeature(it->feature1);
        vector<float> v2 = ts.getFeature(it->feature2);
        for (int i = 0; i < numberOfRow && it->corrlation < threshold; i++) {
            float distance = sqrt(pow(it->x - v1[i], 2) + pow(it->y - v2[i], 2));
            if(distance > it->threshold) {
                d.push_back(AnomalyReport(it->feature1 + "-" + it->feature2, i+1));
            }
        }
    }
}