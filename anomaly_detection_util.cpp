/*
 * anomaly_detection_util.cpp
 *
 * Author: ID: 205694441, Name: Dov Moshe
 */

#include <math.h>
#include "anomaly_detection_util.h"


// returns the average of X
float avg(float* x, int size){
	// sum is the sum of x's
    float sum = 0;
    // the sigma
    for (int i = 0; i < size; i++) {
        sum += x[i];
    }

    return sum / size;
}

// returns the variance of X and Y
float var(float* x, int size){
	// the E(X)
    float e = avg(x, size);

    float sum = 0;
    // the sigma for: VAR(X)
    for (int i = 0; i < size; i++) {
        sum = sum + pow(x[i], 2);
    }
    // divides 'sum' by 'size'
    sum = sum / size;

    // return: sigma - (expected_value^2)
    return sum - pow(e, 2);
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size){
	// the E(X)
    float eX = avg(x, size);
    // the E(Y)
    float eY = avg(y, size);

    float sum = 0;
    // the sigma for: COV(X,Y) = E((X-E(X))(Y-E(Y)))
    for (int i = 0; i < size; i++) {
        sum = sum + (x[i] - eX) * (y[i] - eY);
    }

    // return: COV(X,Y)
    return sum / size;
}


// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size) {
	// the COV(Y)
    float covariance = cov(x, y, size);
    
    // the VAR(X) and VAR(Y)
    float sqrtVarianceX = sqrt(var(x, size));
    float sqrtVarianceY = sqrt(var(y, size));

    return covariance / (sqrtVarianceX * sqrtVarianceY);
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size) {
    // arrays for x and y
	float x[size];
    float y[size];

    for (int i = 0; i < size; i++) {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }

    float covariance = cov(x, y, size);
    float variance = var(x, size);

    float a = covariance / variance;
    float x_average = avg(x, size);
    float y_average = avg(y, size);
    float b = y_average - (a * x_average);

    return Line(a,b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
	Line line = linear_reg(points, size);
    float f_x = line.f(p.x);

    if(p.y > f_x) {
        return p.y - f_x;
    }  
    else {
        return f_x - p.y;
    }
}

// returns the deviation between point p and the line
float dev(Point p,Line l){
	float f_x = l.f(p.x);

    if(p.y > f_x) {
        return p.y - f_x;
    }  
    else {
        return f_x - p.y;
    }
}