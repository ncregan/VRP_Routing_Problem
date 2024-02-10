# VRP_Routing_Problem - Nick Cregan

# Deploying
This C++ program was compiled using g++ on windows x86-64, so any windows machine with the necessary dependencies should be able to run the program with "./a.exe FILE_PATH" or "a.exe FILE_PATH", where FILE_PATH corresponds to the input file. Otherwise, this program will need to be compiled using a c++ compiler to create an executable file, which will then work using the same comand as above (or similar, depending on the conventions of the operating system). Using the available python script evaluateShared.py (not my work), the entire problem set in the directory TrainingProblems can be run, using "python3 .\evaluateShared.py --cmd "COMMAND_TO_RUN" --problemDir .\TrainingProblems".

**IMPORTANT:** If running the evaluation scrip results in the error: "load # was not assigned to a driver", then you may need to recompile the program on your machine. Alternatively, this error can be fixed by adding "loadID = str.rstrip(loadID)" after line 81 in the script (it is commented out in this repo). I'm sorry for any inconvenience this may cause, more information regarding this issue in the **Complications** section.

# Used Resources
This program uses no outside algorithms or studies of the VRP problem. I did, however, breifly examine the Clarke and Wright savings algorithm and the "Branch and Bound" method so I may have taken some inspiration from these proven solutions.

# Approach
This program solves the VRP routing problem with pickup and delivery by finding combinations of optimized routes. The steps are as follows:
1. Create a priority queue of routes that are the closest to the starting location of all drivers (0,0).
2. By popping from this queue, the route that starts the closest to the starting locations will be the first route that driver # takes
3. Then, create a priority queue of the routes that are closest to the ending location of the route that driver # is given. Then pop from this queue.
4. If adding the second route to the schedule of the driver does not exceed his drive time limit (ensuring he can return to base as well), then add this route to his schedule, and go back to **step 3** using the new route's ending location as the basis for the priority queue.
5. Else if adding the second route to the schedule exceeds the drivers time limit, then discard this route, pop another route off the queue and return to **step 4**.
6. Else if there are no more routes in the queue, then this driver cannot take any other routes within his drive time limit so his schedule is complete and move back to **step 2**.
7. Once all routes are accounted for, the program prints the schedules, and exits.

# Design Choices
I decided to use structs as opposed to classes to represent each route and driver. For the route struct, it stores all of the data given by the input file, as well as the length of the route, and distance to the starting location from both the pickup and dropoff locations. This is so that none of this needs to be calculated by the program later. A vector of routes is stored as a global variable, which I would not normally do in an application, but since this program has limited scope, I decided that the ease of access would be enough to justify this decision.

# Complications
I ran into an issue attempting to run the evaluateShared.py script on my program. The script was failing to remove the carriage returns after the newline on the program output and would simply fail the error checks and break the other methods of the script. After some research, I learned that it is nearly impossible to remove these carriage returns when outputing newlines in c++. By adding the line "loadID = str.rstrip(loadID)" after line 81 in the script, it was able to successfully run and evaluate my algorithm. After converting my program to python, I found that I was still running into this issue. I used a VM to run the program in another environment, and found that it must be something native to my system, which I still have yet to figure out. 