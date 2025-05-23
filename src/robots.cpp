// 12/13/2024

#include <iostream>
#include <array>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

// code representation
constexpr int EMPTY {0};
constexpr int WALL {1};
constexpr int BATTERY {2};


enum Direction
{
    North, // 0
    South, // 1
    East, // 2
    West, // 3

    RandomDir
};

// each gene has four sensor states for each direction
// also has the action code that tells the robot what to do in the 
// event the current sensor state matches the four states on the gene
struct Gene
{
    std::array<int, 5 > sensorStates {};
};

// coordinates to show location on 10x10 grid
struct Coordinates
{
    int x {};
    int y {};
};

class Map
{
    private:
        // 2D array for the 10x10 grid
        // putting 12 since the walls do not count as part of the 10x10 dimension
        std::array<std::array<char, 12>, 12> m_map {};
        int m_batteries {};
    public:
        Map()
        // 40 percent of the map = 40 batteries 
        : m_batteries {40}
        {
            for (std::size_t i {0}; i < 12; ++i)
            {
                for (std::size_t j {0}; j < 12; ++j)
                {
                    // place the walls at the edges of the map
                    if (i == 0 || i == 11 || j == 0 || j == 11)
                    {
                        m_map[i][j] = 'W';
                    }
                    // put empty spots everywhere else
                    else
                    {
                        m_map[i][j] = '-';
                    }
                }
            }
            // place the batteries
            while(m_batteries != 0)
            {
                // generate random coordinates for the batteries
                int randomX {1 + (rand() % (10 - 1 + 1))};
                int randomY {1 + (rand() % (10 - 1 + 1))};

                // check to make sure that a battery is not already placed in that coordinate
                // im static casting to size_t to get rid of compiler warnings
                if (m_map[static_cast<std::size_t>(randomX)][static_cast<std::size_t>(randomY)] != 'B')
                {
                    m_map[static_cast<std::size_t>(randomX)][static_cast<std::size_t>(randomY)] = 'B';
                    --m_batteries;  
                }
                else
                {
                    continue;
                }
            }
        }

        // position is empty if it contains a '-'
        bool isPositionEmpty(int x, int y)
        {
            return m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] == '-';
        }

        // place the robot onto the map
        void placeRobot(int x, int y)
        {
            m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] = 'R';
        }

        // get the coordinate above the current robot position
        char getNorthCoordinate(int x, int y)
        {
            return m_map[static_cast<std::size_t>(x - 1)][static_cast<std::size_t>(y)];
        }

        // get the coordinate below the current robot position
        char getSouthCoordinate(int x, int y)
        {
            return m_map[static_cast<std::size_t>(x + 1)][static_cast<std::size_t>(y)];
        }

        // get the coordinate to the right of the current robot position
        char getEastCoordinate(int x, int y)
        {
            return m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y + 1)];
        }

        // get the coordinate to the left of the current robot position
        char getWestCoordinate(int x, int y)
        {
            return m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y - 1)];
        }

        void moveNorth(int& x, int& y, int& power, int& turnsSurvived, int& powerHarvested)
        {
            // keep the robot in the same place if it is trying to move into a wall
            // will still consume energy
            if (m_map[static_cast<std::size_t>(x - 1)][static_cast<std::size_t>(y)] == 'W')
            {
                --power;
                ++turnsSurvived;
                return;
            }
            else if (m_map[static_cast<std::size_t>(x - 1)][static_cast<std::size_t>(y)] == 'B')
            {
                x = x - 1;
                placeRobot(x, y);

                // robot gains 5 power when consuming battery
                power+=5;
                --power; // am confused if I am still to decrement power when robot consumes battery or not
                ++turnsSurvived;
                powerHarvested+=5;
            }
            else
            {
                --power;
                ++turnsSurvived;

                x = x - 1;
                placeRobot(x, y);
            }

            // Reset the position when the robot leaves it
            m_map[static_cast<std::size_t>(x + 1)][static_cast<std::size_t>(y)] = '-';
        }

        void moveSouth(int& x, int& y, int& power, int& turnsSurvived, int& powerHarvested)
        {
            if (m_map[static_cast<std::size_t>(x + 1)][static_cast<std::size_t>(y)] == 'W')
            {
                --power;
                ++turnsSurvived;
                return;
            }
            else if (m_map[static_cast<std::size_t>(x + 1)][static_cast<std::size_t>(y)] == 'B')
            {
                x = x + 1;
                placeRobot(x, y);

                power+=5;
                --power; // am confused if I am still to decrement power when robot consumes battery or not
                ++turnsSurvived;
                powerHarvested+=5;
            }
            else
            {
                --power;
                ++turnsSurvived;

                x = x + 1;
                placeRobot(x, y);
            }

            // Reset the position when the robot leaves it
            m_map[static_cast<std::size_t>(x - 1)][static_cast<std::size_t>(y)] = '-';
        }

        void moveEast(int& x, int& y, int& power, int& turnsSurvived, int& powerHarvested)
        {
            if (m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y + 1)] == 'W')
            {
                --power;
                ++turnsSurvived;
                return;
            }
            else if (m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y + 1)] == 'B')
            {
                y = y + 1;
                placeRobot(x, y);

                power+=5;
                --power; // am confused if I am still to decrement power when robot consumes battery or not
                ++turnsSurvived;
                powerHarvested+=5;
            }
            else
            {
                --power;
                ++turnsSurvived;

                y = y + 1;
                placeRobot(x, y);
            }

            // Reset the position when the robot leaves it
            m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y - 1)] = '-';
        }

        void moveWest(int& x, int& y, int& power, int& turnsSurvived, int& powerHarvested)
        {
            if (m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y - 1)] == 'W')
            {
                --power;
                ++turnsSurvived;
                return;
            }
            else if (m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y - 1)] == 'B')
            {
                y = y - 1;
                placeRobot(x, y);

                power+=5;
                --power; // am confused if I am still to decrement power when robot consumes battery or not
                ++turnsSurvived;
                powerHarvested+=5;
            }
            else
            {
                --power;
                ++turnsSurvived;

                y = y - 1;
                placeRobot(x, y);
            }

            // Reset the position when the robot leaves it
            m_map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y + 1)] = '-';
        }

        void moveRandom(int& x, int& y, int& power, int& turnsSurvived, int& powerHarvested)
        {
            // get a random number used to choose the direction
            int randomNum {rand() % 4};

            switch (randomNum)
            {
            case 0:
                moveNorth(x, y, power, turnsSurvived, powerHarvested);
                break;
            case 1:
                moveSouth(x, y, power, turnsSurvived, powerHarvested);
                break;
            case 2:
                moveEast(x, y, power, turnsSurvived, powerHarvested);
                break;
            case 3:
                moveWest(x, y, power, turnsSurvived, powerHarvested);
                break;
            }
        }

        void displayMap()
        {
            // row
            for (std::size_t i {0}; i < 12; ++i)
            {
                // column
                for (std::size_t j {0}; j < 12; ++j)
                {
                    std::cout << m_map[i][j] << " ";

                    // start a new line at the final column number
                    if (j == 11)
                    {
                        std::cout << '\n';
                    }
                }
            }
        }
};

class Robot
{
    private:
       std::array<Gene, 16> m_genes {};
       int m_turnsSurvived {}; 
       int m_power {};
       Coordinates m_coordinates {};
       Map m_map {};
       std::array<int, 4> m_sensor {};
       int m_powerHarvested {};
    public:
        Robot()
        {
            // robots start with power of 5 when they spawn on the map
            m_power = 5;
            // start 0 turns survived and power harvested = 0
            m_turnsSurvived = 0;
            m_powerHarvested = 0;


            // will show if the position is empty or not
            bool validPosition {false};

            // keep generating random coordinates until it is a valid coordinate
            while (!validPosition)
            {
                // robots will spawn on a random 10x10 grid
                m_coordinates.x = 1 + (rand() % (10 - 1 + 1));
                m_coordinates.y = 1 + (rand() % (10 - 1 + 1));

                if (m_map.isPositionEmpty(m_coordinates.x, m_coordinates.y))
                    validPosition = true;
            }

            // fill the genes with random codes
            // 0 = empty, 1 = wall, 2 = battery
            // for the action code: 0 = north, 1 = south, 2 = east, 3 = west, 4 = random direction
            for (std::size_t i {0}; i < m_genes.size(); ++i)
            {
                for (std::size_t j {0}; j < m_genes[i].sensorStates.size(); ++j)
                {
                    if (j < m_genes[i].sensorStates.size() - 1)
                    {
                        m_genes[i].sensorStates[j] = rand() % 3;
                        continue;
                    }
                    if (j == m_genes[i].sensorStates.size() - 1)
                    {
                        m_genes[i].sensorStates[j] = rand() % 5;
                        continue;
                    }
                }
            }
            // place the robot on the map with its randomly generated coordinates
            m_map.placeRobot(m_coordinates.x, m_coordinates.y);
        }

        friend std::ostream& operator<<(std::ostream& out, Robot robot);

        // getter functions
        int getPower() const {return m_power;}
        int getTurnsSurvived() const {return m_turnsSurvived;}
        int getPowerHarvested() const {return m_powerHarvested;}

        // get the top half of parent genes
        std::array<Gene, 8> getGenesTopHalf() const 
        {
            std::array<Gene, 8> topHalf {};

            for (std::size_t i {0}; i < m_genes.size() / 2; ++i)
            {
                topHalf[i] = m_genes[i];
            }
            return topHalf;
        }

        // get the bottom half of parent genes
        std::array<Gene, 8> getGenesBottomHalf() const
        {
            std::array<Gene, 8> bottomHalf {};

            for (std::size_t i {0}; i < m_genes.size() / 2; ++i)
            {
                bottomHalf[i] = m_genes[i + (m_genes.size() / 2)];
            }
            return bottomHalf;
        }

        void setPowerHarvested(int powerHarvested) {m_powerHarvested = powerHarvested;}

        // set the child genes using the top half and bottom half of the parent genes
        void setChildGenes(std::array<Gene, 8> topHalf, std::array<Gene, 8> bottomHalf)
        {
            for (std::size_t i {0}; i < m_genes.size(); ++i)
            {
                if (i < m_genes.size() / 2)
                {
                    m_genes[i] = topHalf[i];
                }
                else
                {
                    m_genes[i] = bottomHalf[i - (m_genes.size() / 2)];
                }
            }
            int mutationProbability {rand() % 100};
            int geneToMutateIndex {rand() % 16};
            int sensorStateToMutateIndex {rand() % 4};
            int mutationValue {rand() % 3};

            if (mutationProbability < 5) {
                // 5% chance
                m_genes[static_cast<std::size_t>(geneToMutateIndex)].sensorStates[static_cast<std::size_t>(sensorStateToMutateIndex)] = mutationValue;
            }
        }

        // * The display functions are just used for testing
        // display the map for a specific robot
        void displayMap()
        {
            m_map.displayMap();
        }

        // display the genes for a specific robot
        void displayGenes()
        {
            for (std::size_t i {0}; i < m_genes.size(); ++i)
            {
                for (std::size_t j {0}; j < m_genes[i].sensorStates.size(); ++j)
                {
                    std::cout << m_genes[i].sensorStates[j] << " ";
                }
                std::cout << '\n';
            }
        }

        // display the sensor for a specific robot
        // need to call updateSensor() first to get accurate reading
        void displaySensor()
        {
            for (std::size_t i {0}; i < m_sensor.size(); ++i)
            {
                std::cout << m_sensor[i] << " ";
            }
        }

        // get the sensor's readings in each direction
        void updateSensor()
        {
            // North direction
            switch (m_map.getNorthCoordinate(m_coordinates.x, m_coordinates.y))
            {
            case '-':
                m_sensor[North] = EMPTY;
                break;
            case 'W':
                m_sensor[North] = WALL;
                break;
            case 'B':
                m_sensor[North] = BATTERY;
                break;
            }

            // South direction
            switch (m_map.getSouthCoordinate(m_coordinates.x, m_coordinates.y))
            {
            case '-':
                m_sensor[South] = EMPTY;
                break;
            case 'W':
                m_sensor[South] = WALL;
                break;
            case 'B':
                m_sensor[South] = BATTERY;
                break;
            }

            // East direction
            switch (m_map.getEastCoordinate(m_coordinates.x, m_coordinates.y))
            {
            case '-':
                m_sensor[East] = EMPTY;
                break;
            case 'W':
                m_sensor[East] = WALL;
                break;
            case 'B':
                m_sensor[East] = BATTERY;
                break;
            }

            // West direction
            switch (m_map.getWestCoordinate(m_coordinates.x, m_coordinates.y))
            {
            case '-':
                m_sensor[West] = EMPTY;
                break;
            case 'W':
                m_sensor[West] = WALL;
                break;
            case 'B':
                m_sensor[West] = BATTERY;
                break;
            }
        }

        // move the robot based on what the action code tells it to do
        void moveRobot(int actionCode)
        {
            switch (actionCode)
            {
                // 0
            case North:
                m_map.moveNorth(m_coordinates.x, m_coordinates.y, m_power, m_turnsSurvived, m_powerHarvested);
                break;
                // 1
            case South:
                m_map.moveSouth(m_coordinates.x, m_coordinates.y, m_power, m_turnsSurvived, m_powerHarvested);
                break;
                // 2
            case East:
                m_map.moveEast(m_coordinates.x, m_coordinates.y, m_power, m_turnsSurvived, m_powerHarvested);
                break;
                // 3
            case West:
                m_map.moveWest(m_coordinates.x, m_coordinates.y, m_power, m_turnsSurvived, m_powerHarvested);
                break;
                // Random direction
            case 4:
                m_map.moveRandom(m_coordinates.x, m_coordinates.y, m_power, m_turnsSurvived, m_powerHarvested);
                break;
            }
        }

        // see if there is a match with the sensor and one of the 16 genes
        // if there is not a match, follow the instruction from the 16th gene
        void update()
        {
            int actionCode {};

            // loop through the genes
            for (std::size_t i {0}; i < m_genes.size(); ++i)
            {
                bool match {true};

                // loop through the genomes
                for (std::size_t j {0}; j < m_genes[i].sensorStates.size() - 1; ++j)
                {
                    // if an element in the sensor is not equal to the genome,
                    // break and go to the next gene
                    if (m_sensor[j] != m_genes[i].sensorStates[j])
                    {
                        match = false;
                        break;  
                    }
                }
                // successful match
                if (match)
                {
                    // set actionCode equal to the last element in the gene
                    actionCode = m_genes[i].sensorStates[4];

                    moveRobot(actionCode);

                    return;
                }   
            }
            // if there were no matches, use the instruction from the very last gene
            actionCode = m_genes[15].sensorStates[4];

            moveRobot(actionCode);  
        }
};

// Function Prototypes
void sortVector(std::vector<Robot>& robots);
void destroyBottom50Percent(std::vector<Robot>& robots);
void breedRobots(std::vector<Robot>& robots);

int main()
{
    // seed the randomizer using current time
    srand(time(NULL));

    // determines if robot is still alive or not
    bool alive {true};

    // create the population of 200 robots
    std::vector<Robot> robots(200);

    // keep track of number of generations
    int generation {};

    // run through 100 generations 
    // print the fitness score for each generation
    while (generation < 100)
    {
        int totalPowerHarvested {};

        for (std::size_t i {0}; i < robots.size(); ++i)
        {
            // set each new robot to alive
            alive = true;
            while (alive)
            {
                if (robots[i].getPower() == 0)
                {
                    // calculate the total power
                    totalPowerHarvested += robots[i].getPowerHarvested();
                    // kill the robot when power is 0
                    alive = false;
                }
                else
                {
                    // update everything for it to move across the map
                    robots[i].updateSensor();
                    robots[i].update();
                }
            }
        }

        std::cout << "The Average Fitness Score for Generation #" << generation << ": " << (totalPowerHarvested / static_cast<int>(robots.size())) << '\n';

        sortVector(robots);
        destroyBottom50Percent(robots); 
        breedRobots(robots);

        // increment generation
        ++generation;

    }
    
    return 0;
}

// prints relevant information for a robot
// ***Mainly for testing***
std::ostream& operator<<(std::ostream& out, Robot robot)
{
    out << "***Genes***\n";
    robot.displayGenes();

    out << "***Map***\n";
    robot.displayMap();

    out << "***Sensor***\n";
    robot.updateSensor();
    robot.displaySensor();
    
    std::cout << '\n';

    out << "***Turns Survived***\n" << robot.getTurnsSurvived() << '\n';
    out << "***Power***\n" << robot.getPower() << '\n';
    out << "***Power Harvested***\n" << robot.getPowerHarvested() << '\n';

    return out;
}

// sorts the robot's power harvested from greatest to least
void sortVector(std::vector<Robot>& robots) 
{
    std::sort(robots.begin(), robots.end(), [](const Robot& a, const Robot& b) {
        return a.getPowerHarvested() > b.getPowerHarvested();
    });
}

// deletes the robots that are in the bottom 50 percent in power harvested.
void destroyBottom50Percent(std::vector<Robot>& robots) 
{
    // compute the index to erase from
    int size = static_cast<int>(robots.size() / 2);  // Floor result by default

    // destroy robots from the halfway point to the end
    robots.erase(robots.begin() + size, robots.end());
}

void breedRobots(std::vector<Robot>& robots)
{
    std::vector<Robot> newRobots; // Container for new robots

    // Iterate through the robots in pairs
    for (std::size_t i = 0; i + 1 < robots.size(); i += 2)
    {
        const Robot& parentRobot1 = robots[i];
        const Robot& parentRobot2 = robots[i + 1];

        // Create child robots with combined genes
        Robot childRobot1 {};
        childRobot1.setChildGenes(parentRobot1.getGenesTopHalf(), parentRobot2.getGenesTopHalf());

        Robot childRobot2 {};
        childRobot2.setChildGenes(parentRobot1.getGenesBottomHalf(), parentRobot2.getGenesBottomHalf());

        // Store new robots in the temporary vector
        newRobots.push_back(childRobot1);
        newRobots.push_back(childRobot2);
    }

    // Add all new robots to the original vector
    robots.insert(robots.end(), newRobots.begin(), newRobots.end());
}
