#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <math.h>
#include <sstream>
#include <bits/stdc++.h>
#include <set>

    //CONSTANTS
const double DRIVE_TIME_LIMIT = 720;

    //STRUCTS
struct route
{
    int index;
    std::pair<double, double> pickup;
    std::pair<double, double> dropoff;
    double length;
    double startDistanceFromBase;
    double endDistanceFromBase;
    bool isUsed = false;

    void printRoute()
    {
        std::cout << index << " : (" << pickup.first << ", " << pickup.second << "), (" << dropoff.first << ", " << dropoff.second << "), length: " << length << ", " << startDistanceFromBase << ", " << endDistanceFromBase << std::endl;
    }
};

struct driver
{
    std::vector<int> routes;
    double driveTimeRemaining = DRIVE_TIME_LIMIT;
};

    //GLOBALS
typedef std::pair<double, int> pi;

std::vector<route> routes; //made global since program is just one algorithm, makes refactoring easier
std::vector<driver> drivers;
std::priority_queue<pi, std::vector<pi>, std::greater<pi>> priorityQ;
std::priority_queue<pi, std::vector<pi>, std::greater<pi>> beginQ;
std::set<int> availableRoutes;

    //METHODS
double calcDistance(double x1, double y1, double x2, double y2)
{
    double x = x2 - x1;
    double y = y2 - y1;
    x *= x;
    y *= y;
    return sqrt(x + y);
}

void parseInput(std::string fileName)
{
    std::ifstream fin(fileName);
    std::string line;
    std::getline(fin, line);
    for (line; std::getline(fin, line);)
    {
        route currRoute;
        double x1, x2, y1, y2;
        int index;

        std::istringstream ss(line);
        std::string pointString1, pointString2;
        ss >> index >> pointString1 >> pointString2;
        pointString1 = pointString1.substr(1, pointString1.length() - 2);
        pointString2 = pointString2.substr(1, pointString2.length() - 2);

        std::string tmp1, tmp2;
        int comma = pointString1.find(',');
        tmp1 = pointString1.substr(0, comma);
        tmp2 = pointString1.substr(comma + 1, pointString1.length() - 1);

        x1 = std::stod(tmp1);
        y1 = std::stod(tmp2);

        comma = pointString2.find(',');
        tmp1 = pointString2.substr(0, comma);
        tmp2 = pointString2.substr(comma + 1, pointString2.length() - 1);

        x2 = std::stod(tmp1);
        y2 = std::stod(tmp2);

        double l = calcDistance(x1, y1, x2, y2);
        double startToBase = calcDistance(x1, y1, 0, 0);
        double endToBase = calcDistance(x2, y2, 0, 0);

        currRoute.index = index - 1; // want to 0 index, will revert to normal during output
        currRoute.pickup = std::make_pair(x1, y1);
        currRoute.dropoff = std::make_pair(x2, y2);
        currRoute.length = l;
        currRoute.endDistanceFromBase = endToBase;
        currRoute.startDistanceFromBase = startToBase;

        routes.push_back(currRoute);
        availableRoutes.insert(index - 1);
    }
}

void printOutput() //outputs the schedule
{
    for (int i = 0; i < drivers.size(); ++i)
    {
        std::cout << "[";
        for (int j = 0; j < drivers.at(i).routes.size() - 1; ++ j)
        {
            std::cout << drivers.at(i).routes.at(j) + 1 << ", "; //ensure to add 1 to revert from 0 indexing
        }
        std::cout << drivers.at(i).routes.at(drivers.at(i).routes.size() - 1) + 1;
        std::cout << "]\n" << std::flush;
    }
}

void createQueue(double x, double y) //this method should modify the priority queue with routes whose start location is closest to (x,y)
{
    for (auto i: availableRoutes)
    {
        double distance = calcDistance(routes.at(i).pickup.first, routes.at(i).pickup.second, x, y);   // Get total distance
        priorityQ.push(std::make_pair(distance, i));
    }

}

void constructSchedule(driver &d) //given a driver, create a schedule for him using the closest possible routes to his position
{
    int index = 0;
    bool driverFinished = false;
    while (1)
    {
        priorityQ = std::priority_queue<pi, std::vector<pi>, std::greater<pi>>(); //clear the queue
        createQueue(routes.at(d.routes.at(index)).dropoff.first, routes.at(d.routes.at(index)).dropoff.second);

        while (1)
        {
            if (priorityQ.empty()) 
            {
                driverFinished = true;
                break;
            }

            std::pair<double, int> nextRoute = priorityQ.top();
            priorityQ.pop();
            double totalAddedTime = nextRoute.first + routes.at(nextRoute.second).length + routes.at(nextRoute.second).endDistanceFromBase; 

            if (d.driveTimeRemaining - totalAddedTime > 0) //route should be added
            {
                d.routes.push_back(nextRoute.second);
                d.driveTimeRemaining -= nextRoute.first; //do not want to add back to base yet
                d.driveTimeRemaining -= routes.at(nextRoute.second).length;
                availableRoutes.erase(nextRoute.second);
                break;
            }
        }

        if (driverFinished) { break; }
        index++;
    }
}

    //MAIN LOGIC
int main(int argc, const char * argv[])
{
        parseInput(argv[1]); //parses the input file



        for (int i = 0; i < routes.size(); ++i) //initialize begin queue (how close each route is to the starting point)
        {
            beginQ.push(std::make_pair(routes.at(i).startDistanceFromBase, i));
        }

        while (!beginQ.empty()) //While we still have routes to cover
        {

                // make driver
            driver currDriver;
                // give driver next optimal route
            std::pair<double, int> tmp = beginQ.top(); //every route must be possible, so no reason to do any drive time checks yet
            beginQ.pop();

            if (availableRoutes.find(tmp.second) == availableRoutes.end()) { continue; } //This route is already in a schedule so skip it
            availableRoutes.erase(tmp.second);

            if (availableRoutes.empty()) //This is the last route, so this driver will only do this one
            {
                currDriver.routes.push_back(tmp.second);
                drivers.push_back(currDriver);
                break;
            }

            currDriver.routes.push_back(tmp.second);
            currDriver.driveTimeRemaining -= tmp.first; 
            currDriver.driveTimeRemaining -= routes.at(tmp.second).length; 
            constructSchedule(currDriver);
            drivers.push_back(currDriver);
        }

        printOutput(); //prints the schedule of each driver
        return 0;
}
