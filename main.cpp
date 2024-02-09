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

    void printRoute() //print details about route
    {
        std::cout << index << " : (" << pickup.first << ", " << pickup.second << "), (" << dropoff.first << ", " << dropoff.second << "), length: " << length << ", start distance from base: " 
        << startDistanceFromBase << ", end distance from base: " << endDistanceFromBase << std::endl;
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
std::priority_queue<pi, std::vector<pi>, std::greater<pi>> priorityQ; //Priority queues need to sort from min to max
std::priority_queue<pi, std::vector<pi>, std::greater<pi>> beginQ;
std::set<int> availableRoutes;

    //METHODS
    //Takes two points and calculates the euclidean distance between them
double calcDistance(double x1, double y1, double x2, double y2) 
{
    double x = x2 - x1;
    double y = y2 - y1;
    x *= x;
    y *= y;
    return sqrt(x + y);
}

    //Parses input file and populates routes vector
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

    //Prints the output of the schedule for each driver
void printOutput()
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
    //Creates a priority queue with the available routes whose start location is closest to the parameters
void createQueue(double x, double y) 
{
    for (auto i: availableRoutes)
    {
        double distance = calcDistance(routes.at(i).pickup.first, routes.at(i).pickup.second, x, y);  
        priorityQ.push(std::make_pair(distance, i));
    }

}

    //Given a driver, creates a schedule for him using the closest possible routes to his current position
void constructSchedule(driver &d) 
{
    int index = 0;
    bool driverFinished = false;
    while (!driverFinished)
    {
        priorityQ = std::priority_queue<pi, std::vector<pi>, std::greater<pi>>(); //clear the queue
        createQueue(routes.at(d.routes.at(index)).dropoff.first, routes.at(d.routes.at(index)).dropoff.second);

        while (!priorityQ.empty())
        {
            std::pair<double, int> nextRoute = priorityQ.top();
                //The time is would take to do this route and return to base
            double totalAddedTime = nextRoute.first + routes.at(nextRoute.second).length + routes.at(nextRoute.second).endDistanceFromBase; 

            if (d.driveTimeRemaining - totalAddedTime > 0) //route should be added
            {
                d.routes.push_back(nextRoute.second);
                d.driveTimeRemaining -= nextRoute.first; 
                d.driveTimeRemaining -= routes.at(nextRoute.second).length;
                availableRoutes.erase(nextRoute.second);
                break;
            }

            priorityQ.pop(); //Want to pop after in case driver picks up last availble route, he may be able to do more 
        }
        if (priorityQ.empty()) { driverFinished = true; } //There are no more routes that he can given his time constraints

        index++;
    }
}

    //MAIN LOGIC
int main(int argc, const char * argv[])
{
        parseInput(argv[1]);

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

        printOutput();
        return 0;
}
