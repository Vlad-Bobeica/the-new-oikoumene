//  component.h

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

class wall
{
    struct layer
    {
        double thickness;
        int material;
    };
    
public:
    int number_of_layers;
    double total_thickness;
    std::vector<layer> wall_layers;
    wall(int run_case);
};

class component
{
    double volume;
    double mass;
    double moment_of_inertia;
    double angular_velocity;
    double rpm;
    
public:
    std::vector<double> dimensions;
    bool isTBD;
    bool isCoupled;
    static double square(double x){return x * x;}
    void calculate_angular_velocity(double percentage);
    void calculate_rpm();
    double get_angular_momentum();
    void setAngularVelocity(double x){angular_velocity = x;}
    double getAngularVelocity(){return angular_velocity;}
    void setVolume(double x){volume = x;}
    void setMass(double x){mass = x;}
    void setMomentOfInertia(double x){moment_of_inertia = x;}
    virtual void compute(wall exterior, std::map<int, double> density);
};

class tube : public component
{
public:
    tube(std::ifstream& userFile);
    void compute(wall exterior, std::map<int, double> density);
    static void print_config();
};

class ring : public component
{
public:
    ring(std::ifstream& userFile);
    void compute(wall exterior, std::map<int, double> density);
    static void print_config();
};

class spheroid : public component
{
public:
    spheroid(std::ifstream& userFile);
    void compute(wall exterior, std::map<int, double> density);
    static void print_config();
};
