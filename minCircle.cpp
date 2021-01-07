#include "minCircle.h"

Circle findCircle(vector<Point>::iterator start, vector<Point>::iterator end, vector<Point> boundPoints);

Circle circleTwoPoints(Point *p1, Point *p2);

Circle circleThreePoints(Point *p1, Point *p2, Point *p3);

Circle minCircle(vector<Point>::iterator start, vector<Point>::iterator end, vector<Point> boundPoints);

bool isInside(Point p, Circle circle);

// implement
Circle findMinCircle(Point **points, size_t size) {
    // copy the array to vector
    vector<Point> newPoints;
    for (int i = 0; i < size; i++) {
        newPoints.push_back(*points[i]);
    }
    vector<Point> boundPoints;
    vector<Point>::iterator start = newPoints.begin();
    vector<Point>::iterator end = newPoints.end();
    return minCircle(start, end, boundPoints);
}


// you may add helper functions here

Circle minCircle(vector<Point>::iterator start, vector<Point>::iterator end, vector<Point> boundPoints) {
    if (end - start == 1 || boundPoints.size() == 3) {
        return findCircle(start, end, boundPoints);
    } else {
        Point p(start->x, start->y);
        Circle c = minCircle(start + 1, end, boundPoints);
        if (isInside(p, c)) {
            return c;
        }
        boundPoints.push_back(p);
        return minCircle(start + 1, end, boundPoints);
    }
}

bool isInside(Point p, Circle circle) {
    // distance = sqrt((p1.x - p2.x)^2 + (p1.y - p2.y)^2)
    float x = (p.x - circle.center.x) * (p.x - circle.center.x);
    float y = (p.y - circle.center.y) * (p.y - circle.center.y);
    float distance = sqrt(x + y);
    if (distance <= circle.radius) {
        return true;
    }
    return false;
}

/**
 * The function finds the minimum circle by given three points or there is no points.
 * If there is no points then the func returns point with radius -1.
 * Else, checking if the minimum circle define by two points, then the func returns the circle.
 * Else, the func returns the minimum circle define by three points.
 */
Circle findCircle(vector<Point>::iterator start, vector<Point>::iterator end, vector<Point> boundPoints) {
    Point p(start->x, start->y);
    Circle c(p, 0);
    if (boundPoints.size() == 3) {
        return circleThreePoints(&boundPoints[0], &boundPoints[1], &boundPoints[2]);
    }
    // here we know from the call to the function that end-start == 1
    if (boundPoints.empty()) {
        return c;
    }
    if (boundPoints.size() == 1) {
        return circleTwoPoints(&boundPoints[0], &p);
    }
    if (boundPoints.size() == 2) {
        if (isInside(p, c = circleTwoPoints(&boundPoints[0], &boundPoints[1]))) {
            return c;
        }
        return circleThreePoints(&boundPoints[0], &boundPoints[1], &p);
    }
    // will never come to here
    return c;
}

Circle circleTwoPoints(Point *p1, Point *p2) {
    // the middle point
    float xCenter = (p1->x + p2->x) / 2;
    float yCenter = (p1->y + p2->y) / 2;
    // distance = sqrt((p1.x - p2.x)^2 + (p1.y - p2.y)^2)
    float x = (p1->x - p2->x) * (p1->x - p2->x);
    float y = (p1->y - p2->y) * (p1->y - p2->y);
    float distance = sqrt(x + y);
    float radius = distance / 2;
    return Circle(Point(xCenter, yCenter), radius);
};

Circle circleThreePoints(Point *p1, Point *p2, Point *p3) {
    float xCenter;
    float yCenter;
    // 1st line
    float m1 = (p1->y - p2->y) / (p1->x - p2->x);
    float x1 = (p1->x + p2->x) / 2;
    float y1 = (p1->y + p2->y) / 2;
    m1 = -1 / m1;
    // 2nd line
    float m2 = (p2->y - p3->y) / (p2->x - p3->x);
    float x2 = (p2->x + p3->x) / 2;
    float y2 = (p2->y + p3->y) / 2;
    m2 = -1 / m2;
    // find points
    xCenter = (m1 * x1 - m2 * x2 - y1 + y2) / (m1 - m2);
    yCenter = m1 * xCenter - m1 * x1 + y1;

    // distance = sqrt((p1.x - p2.x)^2 + (p1.y - p2.y)^2)
    float x = (p1->x - xCenter) * (p1->x - xCenter);
    float y = (p1->y - yCenter) * (p1->y - yCenter);

    return Circle(Point(xCenter, yCenter), sqrt(x + y));
}