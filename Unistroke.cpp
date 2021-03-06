// Unistroke.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "math.h"
#include <vector>
#define PI 3.14159265
using namespace std;

class point
{
public:
    int xCoordinate, yCoordinate;

    point()
    {
        xCoordinate = 0;
        yCoordinate = 0;
    }
    point(int x, int y)
    {
        xCoordinate = x;
        yCoordinate = y;
    }
};

class dataPath
{
public:
    vector <point> pointVector;

    dataPath()
    {

    }
    void insertPoint(point p)
    {
        pointVector.push_back(p);
    }

    vector<point> getDataPath()
    {
        return pointVector;
    }
};

struct result
{
    int b = INT_MAX;

};

int main()
{
    std::cout << "Hello World!\n";
}

int getDistance(point a, point b) //calculates didstance between two points
{
    int xDiff = abs(a.xCoordinate - abs(b.xCoordinate));
    int yDiff = abs(a.yCoordinate - abs(b.yCoordinate));
    int distance = sqrt(xDiff * xDiff + yDiff * yDiff);
    return distance;
}

int calculateLength(vector<point> pointVector) // calculates length of the entire path
{
    int length = 0;
    for (int i = 1; i < pointVector.size(); i++)
    {
        length += getDistance(pointVector[i - 1], pointVector[i]);
    }
    return length;
}
//start of step 1
vector<point> resampling(vector<point> candidate, int rate)//I regard it as a step to "normalize" our graph
{
    int pathLength = calculateLength(candidate);
    int interval = pathLength / rate; //preferred distance between data points composing our dataset.
    int D = 0; //I personally regard it as a tolerance rate (offset).
    vector<point> resampled;
    resampled.push_back(candidate[0]);
    for (int i = 1; i < candidate.size(); i++)
    {
        int relativeDistance = getDistance(candidate[i - 1], candidate[i]);
        if (D + relativeDistance > interval)//if tolerance offset is larger than our preset interval distance, try drag it closer
        {
            int tempX = candidate[i - 1].xCoordinate + ((interval - D) / relativeDistance) * (candidate[i].xCoordinate - candidate[i - 1].xCoordinate);
            int tempY = candidate[i - 1].yCoordinate + ((interval - D) / relativeDistance) * (candidate[i].yCoordinate - candidate[i - 1].yCoordinate);
            point temp(tempX, tempY);
            resampled.push_back(temp);
            D = 0;
        }
        else //if tolerance is smaller than our preset interval distance, skip it then try sampling the next
        {
            D = D + relativeDistance;
        }
    }
    return resampled;
}
//end of step 1

//start of step 2
point centroid(vector<point> graph)
{
    int x = 0;
    int y = 0;
    for (int i = 0; i < graph.size(); i++)
    {
        x += graph[i].xCoordinate;
        y += graph[i].yCoordinate;
    }
    x = x / graph.size();
    y = y / graph.size();
    point temp(x, y);
    return temp;
}
vector<point> rotateBy(vector<point> original, double theta)
{
    vector<point> newVector;
    point c = centroid(original);
    for (int i = 0; i < original.size(); i++)
    {
        int tempX = (original[i].xCoordinate - c.xCoordinate) * cos(theta) - (original[i].yCoordinate - c.yCoordinate) * sin(theta) + c.xCoordinate;
        int tempY = (original[i].xCoordinate - c.xCoordinate) * sin(theta) - (original[i].yCoordinate - c.yCoordinate) * cos(theta) + c.yCoordinate;
        point temp(tempX, tempY);
        newVector.push_back(temp);
    }
    return newVector;
}
vector<point> rotateToZero(vector<point> original)
{
    point c = centroid(original);
    int tempX = c.xCoordinate - original[0].xCoordinate;
    int tempY = c.yCoordinate = original[0].yCoordinate;
    double theta = atan2(tempY, tempX);
    return rotateBy(original, theta);
}
//end of step 2

//start of step 3
vector<point> boundToSquare(vector<point> original, int size)
{
    int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
    for (int i = 0; i < original.size(); i++)
    {
        minX = min(minX, original[i].xCoordinate);
        minY = min(minY, original[i].yCoordinate);
        maxX = max(maxX, original[i].xCoordinate);
        maxY = max(maxY, original[i].yCoordinate);
    }
    int width = maxX - minX;
    int height = maxY - minY;
    for (int i = 0; i < original.size(); i++)
    {
        original[i].xCoordinate = original[i].xCoordinate * (size / width);
        original[i].yCoordinate = original[i].yCoordinate * (size / height);
    }
    return original;
}
vector<point> translateToOrigin(vector<point> original)
{
    point c = centroid(original);
    for (int i = 0; i < original.size(); i++)
    {
        original[i].xCoordinate = original[i].xCoordinate - c.xCoordinate;
        original[i].yCoordinate = original[i].yCoordinate - c.yCoordinate;
    }
    return original;
}
//end of step 3

//start of step 4
int pathDistance(vector<point> candidate, vector<point> graphTemplate)
{
    //assuming template has the same amount of points as template does.
    int d = 0;
    for(int i = 0; i < candidate.size(); i++)
    {
        d += getDistance(candidate[i], graphTemplate[i]);
    }
    return d / candidate.size();
}
int distanceAtAngle(vector<point> candidate, vector<point> graphTemplate, double theta)
{
    vector<point> temp = rotateBy(candidate, theta);
    int d = pathDistance(temp, graphTemplate);
    return d;
}
int distanceAtBestAngle(vector<point> candidate, vector<point> graphTemplate, double thetaA, double thetaB, double thetaDelta)
{
    double PHI = 0.5 * (-1 + sqrt(5));

    double x1 = PHI * thetaA + (1 - PHI) * thetaB;
    int f1 = distanceAtAngle(candidate, graphTemplate, x1);

    double x2 = (1 - PHI) * thetaA + PHI * thetaB;
    int f2 = distanceAtAngle(candidate, graphTemplate, x2);

    while(abs(thetaB - thetaA) > thetaDelta)
    {
        if(f1 < f2)
        {
            thetaB = x2;
            x2 = x1;
            f2 = f1;
            x1 = PHI * thetaA + (1 - PHI) * thetaB;
            f1= distanceAtAngle(candidate, graphTemplate, x1);
        }
        else
        {
            thetaA = x1;
            x1 = x2;
            f1 = f2;
            x2 = (1 - PHI) * thetaA + PHI * thetaB;
            f2 = distanceAtAngle(candidate, graphTemplate, x2);
        }
    }
    return min(f1, f2);
}

string regognition(vector<point> candidate, vector<vector<point>> graphTemplates)
{
    double theta = PI / 4; //45 degrees;
    double negativeTheta = -1 * theta;
    double thetaDelta = 1 / 90 * PI;
    double b = INT_MAX;
    double d = 0;
    double score = 0;
    int size = 0; //size of square, dummy variable now.
    string result; // dummy variable now.
    vector<point> currentTemplate;
    for(int i = 0; i < graphTemplates.size(); i++)
    {
        d = distanceAtBestAngle(candidate, graphTemplates[i], negativeTheta, theta, thetaDelta);
        if(d < b)
        {
            d = b;
            currentTemplate = graphTemplates[i];
        }
    }
    score = 1 - b / 0.5 * sqrt(size * size + size * size);
    return result;
}