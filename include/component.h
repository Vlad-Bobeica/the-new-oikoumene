//  component.h

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

class wall
{
    struct layer
    {
        double thickness; //TO-DO: Make load function compatible with non-integer values
        int material;
    };
    
public:
    int number_of_layers;
    double total_thickness;
    std::vector<layer> wall_layers;
    wall()
    {
        std::ofstream save("data.txt", std::ios::app);
        std::cout << "Initialising wall:" << '\n'
            << '\t' << "Enter number of wall layers:" << '\n' << "> ";
        std::cin >> number_of_layers;
        save << number_of_layers;
        total_thickness = 0;
        for (int i = 1; i <= number_of_layers; i++)
        {
            layer current;
            std::cout << '\t' << "Enter thickness of Layer " << i << " in meters: (interior to exterior)" << '\n' << "> ";
            std::cin >> current.thickness;
            save << current.thickness;
            total_thickness += current.thickness;
            std::cout << '\t' << "Enter material of Layer " << i << ": (number corresponding to a material)" << '\n' << "> ";
            std::cin >> current.material;
            save << current.material;
            wall_layers.push_back(current);
        }
        save << '\n';
        save.close();
        std::cout << "Wall initialised successfully." << '\n' << '\n';
    }
    wall(long long code)
    {
        std::ifstream save("data.txt");
        char digit;
        save >> digit;
        number_of_layers = digit - '0';
        total_thickness = 0;
        for (int i = 1; i <= number_of_layers; i++)
        {
            layer current;
            save >> digit;
            current.thickness = digit - '0';
            total_thickness += current.thickness;
            save >> digit;
            current.material = digit - '0';
            wall_layers.push_back(current);
        }
        save.close();
        std::cout << "Wall loaded successfully." << '\n' << '\n';
    }
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
    tube();
    void compute(wall exterior, std::map<int, double> density);
    
};

class ring : public component
{
public:
    ring();
    void compute(wall exterior, std::map<int, double> density);
};

class spheroid : public component
{
public:
    spheroid();
    void compute(wall exterior, std::map<int, double> density);
};
