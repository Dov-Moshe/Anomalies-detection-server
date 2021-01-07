

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
    virtual void learnNormal(const TimeSeries& ts) override;
    virtual vector<AnomalyReport> detect(const TimeSeries& ts) override;
    virtual void learnNormalCircle(const TimeSeries& ts);
    virtual void detectCircle(const TimeSeries& ts, vector<AnomalyReport>& d);
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
