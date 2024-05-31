//  component.cpp

#include "component.h"
#include <cmath>

const double pi = 2 * acos(0.0);
const double g = 9.8067;


wall::wall(int run_case)
{
    if (run_case == 0)
    {
        std::ofstream save("data.txt", std::ios::app);
        std::cout << "Initialising wall:" << '\n'
            << '\t' << "Enter number of wall layers:" << '\n' << "> ";
        std::cin >> number_of_layers;
        save << number_of_layers << std::endl;
        total_thickness = 0;
        for (int i = 1; i <= number_of_layers; i++)
        {
            layer current;
            std::cout << '\t' << "Enter thickness of Layer " << i << " in meters: (interior to exterior)" << '\n' << "> ";
            std::cin >> current.thickness;
            save << current.thickness << std::endl;
            total_thickness += current.thickness;
            std::cout << '\t' << "Enter material of Layer " << i << ": (number corresponding to a material)" << '\n' << "> ";
            std::cin >> current.material;
            save << current.material << std::endl;
            wall_layers.push_back(current);
        }
        save.close();
        std::cout << "Wall initialised successfully." << '\n' << '\n';
    }
    else if (run_case == 1)
    {
        std::ifstream save("data.txt");
        save >> number_of_layers;
        total_thickness = 0;
        for (int i = 1; i <= number_of_layers; i++)
        {
            layer current;
            save >> current.thickness;
            total_thickness += current.thickness;
            save >> current.material;
            wall_layers.push_back(current);
        }
        save.close();
        std::cout << "Wall loaded successfully." << '\n' << '\n';
    }
}


void component::compute(wall exterior, std::map<int, double> density)
{
    std::cout << "Error occurred : component type was not identified." << std::endl;
}
void component::calculate_angular_velocity(double percentage)
{
    angular_velocity = sqrt(g * percentage / dimensions[0]);
}
double component::get_angular_momentum()
{
    return angular_velocity * moment_of_inertia;
}

tube::tube()
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating tube component. Input dimensions in meters, input '0' for only one TBD size." << '\n'
        << "Enter the tube (internal) radius:" << '\n' << '\t' << "> ";
    double input;
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter the tube length:" << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter percentage of g to be generated at radius:" << '\n'
        << "(input 1 for Earth gravity, 0 if the tube is coupled to the previous component of the assembly)"
        << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input)
    {
        calculate_angular_velocity(input);
    }else{
        isCoupled = true;
    }
}
void tube::compute(wall exterior, std::map<int, double> density)
{
    double radius = dimensions[0];
    double length = dimensions[1];
    double volume = pi * square(radius) * length;
    setVolume(volume);
    double mass = density[0] * volume;
    for (int i = 0; i < exterior.number_of_layers; i++)
    {
        mass += density[exterior.wall_layers[i].material] * pi
            * (square(radius + exterior.wall_layers[i].thickness) - square(radius));
        radius += exterior.wall_layers[i].thickness;
    }
    setMass(mass);
    double moment_of_inertia = 0.5 * mass * square(radius + exterior.total_thickness);
    setMomentOfInertia(moment_of_inertia);
}

ring::ring()
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating ring component. Input dimensions in meters, input '0' for only one TBD size." << '\n'
        << "Enter the ring (internal, outer) radius:" << '\n' << '\t' << "> ";
    double input;
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter the ring thickness:" << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter the ring length:" << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter percentage of g to be generated at radius:" << '\n'
        << "(input 1 for Earth gravity, 0 if the ring is coupled to the previous component of the assembly)"
        << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input)
    {
        calculate_angular_velocity(input);
    }else{
        isCoupled = true;
    }
}
void ring::compute(wall exterior, std::map<int, double> density)
{
    double outer_radius = dimensions[0];
    double thickness = dimensions[1];
    double length = dimensions[2];
    double volume = pi * (square(outer_radius) - square(outer_radius - thickness)) * length;
    setVolume(volume);
    double mass = density[0] * volume;
    double disc_inner = outer_radius - thickness - exterior.wall_layers[0].thickness;
    double disc_outer = outer_radius + exterior.wall_layers[0].thickness;
    mass += 2 * density[exterior.wall_layers[0].material] * pi * (square(disc_outer) - square(disc_inner))
        * exterior.wall_layers[0].thickness;
    for (int i = 1; i < exterior.number_of_layers; i++)
    {
        disc_inner -= exterior.wall_layers[i].thickness;
        disc_outer += exterior.wall_layers[i].thickness;
        mass += 2 * density[exterior.wall_layers[i].material] * pi * (square(disc_outer) - square(disc_inner))
            * exterior.wall_layers[i].thickness;
    }
    for (int i = 0; i < exterior.number_of_layers; i++)
    {
        mass += density[exterior.wall_layers[i].material] * pi
            * (square(outer_radius + exterior.wall_layers[i].thickness) - square(outer_radius)) * length;
        outer_radius += exterior.wall_layers[i].thickness;
    }
    outer_radius -= exterior.total_thickness - thickness;
    for (int i = 0; i < exterior.number_of_layers; i++)
    {
        mass += density[exterior.wall_layers[i].material] * pi
            * (square(outer_radius) - square(outer_radius - exterior.wall_layers[i].thickness)) * length;
        outer_radius -= exterior.wall_layers[i].thickness;
    }
    setMass(mass);
    double moment_of_inertia = 0.5 * mass *
        (square(outer_radius) + square(outer_radius + thickness + 2 * exterior.total_thickness));
    setMomentOfInertia(moment_of_inertia);
}

spheroid::spheroid()
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating spheroid component. Input dimensions in meters, input '0' for only one TBD size." << '\n'
        << "Enter the spheroid (internal, equatorial) radius:" << '\n' << '\t' << "> ";
    double input;
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter the spheroid ratio: (equatorial radius over polar radius | input 1 for a sphere)" << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input == 0) isTBD = true;
    dimensions.push_back(input);
    std::cout << "Enter percentage of g to be generated at radius:" << '\n'
        << "(input 1 for Earth gravity, 0 if the tube is coupled to the previous component of the assembly)"
        << '\n' << '\t' << "> ";
    std::cin >> input;
    if (input)
    {
        calculate_angular_velocity(input);
    }else{
        isCoupled = true;
    }
}
void spheroid::compute(wall exterior, std::map<int, double> density)
{
    double eq_radius = dimensions[0];
    double ratio = dimensions[1];
    double polar_radius = eq_radius / ratio;
    double volume = 4 / 3 * pi * square(eq_radius) * polar_radius;
    setVolume(volume);
    double mass = density[0] * volume;
    for (int i = 0; i < exterior.number_of_layers; i++)
    {
        mass += density[exterior.wall_layers[i].material] * 4 / 3 * pi
            * ((square(eq_radius + exterior.wall_layers[i].thickness) * (polar_radius + exterior.wall_layers[i].thickness))
            - (square(eq_radius) * polar_radius));
        eq_radius += exterior.wall_layers[i].thickness;
        polar_radius += exterior.wall_layers[i].thickness;
    }
    setMass(mass);
    double moment_of_inertia = 0.4 * mass * square(eq_radius);
    setMomentOfInertia(moment_of_inertia);
}