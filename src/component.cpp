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
std::string component::get_component_name()
{
    std::cout << "Error occurred : component type was not identified." << std::endl;
    return "";
}
void component::calculate_angular_velocity(double percentage)
{
    angular_velocity = sqrt(g * percentage / dimensions[0]);
}
double component::get_angular_momentum()
{
    return angular_velocity * moment_of_inertia;
}

tube::tube(std::ifstream& userFile)
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating tube component." << '\n';
    double input;
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Tube (internal) radius: TBD" << '\n';
    }else{
        std::cout << "Tube (internal) radius: " << dimensions[0] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    { 
        isTBD = true;
        std::cout << "Tube length: TBD" << '\n';
    }else{
        std::cout << "Tube length: " << dimensions[1] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    if (input)
    {
        std::cout << input * 100 << "% of g will be generated at radius." << '\n';
        calculate_angular_velocity(input);
    }else{
        std::cout << "This tube is coupled to the previous component of the assembly." << '\n';
        isCoupled = true;
    }
    std::cout << '\n';
}
void tube::print_config()
{
    std::ofstream userFile("config.txt", std::ios::app);
    userFile << "Tube component:" << '\n' << '\t'
        << "(internal) radius: > " << '\n' << '\t'
        << "length: > " << '\n' << '\t'
        << "percentage of g to be generated at radius: > "
        << '\n' << '\n';
    userFile.close();
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
            * (square(radius + exterior.wall_layers[i].thickness) - square(radius))
            * length;
        radius += exterior.wall_layers[i].thickness;
    }
    setMass(mass);
    double moment_of_inertia = 0.5 * mass * square(radius);
    setMomentOfInertia(moment_of_inertia);
}

ring::ring(std::ifstream& userFile)
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating ring component." << '\n';
    double input;
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Ring (internal, outer) radius: TBD" << '\n';
    }else{
        std::cout << "Ring (internal, outer) radius: " << dimensions[0] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Ring thickness: TBD" << '\n';
    }else{
        std::cout << "Ring thickness: " << dimensions[1] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Ring length: TBD" << '\n';
    }else{
        std::cout << "Ring length: " << dimensions[2] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        std::cout << "Has Interlink: no, it is welded to the connected tube" << '\n';
    }else{
        std::cout << "Has Interlink: yes" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    std::cout << "Ring is connected to the tube of (internal) radius: " << dimensions[4] << " m" << '\n';
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    if (input)
    {
        std::cout << input * 100 << "% of g will be generated at radius." << '\n';
        calculate_angular_velocity(input);
    }else{
        std::cout << "This ring is coupled to the previous component of the assembly." << '\n';
        isCoupled = true;
    }
    std::cout << '\n';
}
void ring::print_config()
{
    std::ofstream userFile("config.txt", std::ios::app);
    userFile << "Ring component:" << '\n' << '\t'
        << "(internal, outer) radius: > " << '\n' << '\t'
        << "thickness: > " << '\n' << '\t'
        << "length: > " << '\n' << '\t'
        << "Has interlink? (0 if welded to the tube it spins with, else 1) > " << '\n' << '\t'
        << "(internal) radius of the tube it is connected to: > " << '\n' << '\t'
        << "percentage of g to be generated at radius: > "
        << '\n' << '\n';
    userFile.close();
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
    double ring_moment_of_inertia = 0.5 * mass
        * (square(outer_radius) + square(outer_radius + thickness + 2 * exterior.total_thickness));
    double moment_of_inertia;

    std::ifstream settings("settings.txt");
    std::string textLine;
    for (int i = 1; i <= 3; i++)
    {
        getline(settings, textLine);
    }
    double spoke_radius = stoi(textLine.substr(0, textLine.find(" ")));
    getline(settings, textLine);
    int spoke_number = 6 * (floor(length / stoi(textLine.substr(0, textLine.find(" ")))) + 1);
    bool hasInterlink = dimensions[3];
    double tube_radius = dimensions[4] + exterior.total_thickness;
    if (hasInterlink == true)
    {
        getline(settings, textLine);
        double interlink_thickness = stoi(textLine.substr(0, textLine.find(" ")));
        getline(settings, textLine);
        double interlink_clearance = stoi(textLine.substr(0, textLine.find(" ")));
        double interlink_inner_radius = tube_radius + interlink_clearance + exterior.total_thickness;
        double interlink_volume = pi 
            * (square(interlink_inner_radius + interlink_thickness) - square(interlink_inner_radius)) * length;
        double interlink_mass = density[0] * interlink_volume;
        double interlink_disc_inner = interlink_inner_radius - exterior.wall_layers[0].thickness;
        double interlink_disc_outer = interlink_inner_radius + interlink_thickness + exterior.wall_layers[0].thickness;
        interlink_mass += 2 * density[exterior.wall_layers[0].material] * pi
            * (square(interlink_disc_outer) - square(interlink_disc_inner)) * exterior.wall_layers[0].thickness;
        for (int i = 1; i < exterior.number_of_layers; i++)
        {
            interlink_disc_inner -= exterior.wall_layers[i].thickness;
            interlink_disc_outer += exterior.wall_layers[i].thickness;
            interlink_mass += 2 * density[exterior.wall_layers[i].material] * pi
                * (square(interlink_disc_outer) - square(interlink_disc_inner)) * exterior.wall_layers[i].thickness;
        }
        for (int i = 0; i < exterior.number_of_layers; i++)
        {
            interlink_mass += density[exterior.wall_layers[i].material] * pi
                * (square(interlink_inner_radius) - square(interlink_inner_radius - exterior.wall_layers[i].thickness)) * length;
            interlink_inner_radius -= exterior.wall_layers[i].thickness;
        }
        interlink_inner_radius += exterior.total_thickness + interlink_thickness;
        for (int i = 0; i < exterior.number_of_layers; i++)
        {
            interlink_mass += density[exterior.wall_layers[i].material] * pi
                * (square(interlink_inner_radius + exterior.wall_layers[i].thickness) - square(interlink_inner_radius)) * length;
            interlink_inner_radius += exterior.wall_layers[i].thickness;
        }
        double interlink_moment_of_inertia = 0.5 * interlink_mass
            * (square(interlink_inner_radius) + square(interlink_inner_radius - interlink_thickness - 2 * exterior.total_thickness));

        double spoke_volume = pi * square(spoke_radius) * (outer_radius - interlink_inner_radius);
        double spoke_mass = density[0] * spoke_volume;
        for (int i = 0; i < exterior.number_of_layers; i++)
        {
            spoke_mass += density[exterior.wall_layers[i].material] * pi
                * (square(spoke_radius + exterior.wall_layers[i].thickness) - square(spoke_radius))
                * (outer_radius - interlink_inner_radius);
            spoke_radius += exterior.wall_layers[i].thickness;
        }
        double spokes_moment_of_inertia = (1.0 / 3)
            * (spoke_number * spoke_mass) * square(outer_radius);

        moment_of_inertia = ring_moment_of_inertia + interlink_moment_of_inertia
            + spokes_moment_of_inertia;
    }else{
        double spoke_volume = pi * square(spoke_radius) * (outer_radius - tube_radius);
        double spoke_mass = density[0] * spoke_volume;
        for (int i = 0; i < exterior.number_of_layers; i++)
        {
            spoke_mass += density[exterior.wall_layers[i].material] * pi
                * (square(spoke_radius + exterior.wall_layers[i].thickness) - square(spoke_radius))
                * (outer_radius - tube_radius);
            spoke_radius += exterior.wall_layers[i].thickness;
        }
        double spokes_moment_of_inertia = (1.0 / 3)
            * (spoke_number * spoke_mass) * square(outer_radius);

        moment_of_inertia = ring_moment_of_inertia + spokes_moment_of_inertia;
    }
    settings.close();
    setMomentOfInertia(moment_of_inertia);
}

spheroid::spheroid(std::ifstream& userFile)
{
    isTBD = false;
    isCoupled = false;
    std::cout << "Creating spheroid component." << '\n';
    double input;
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Spheroid (internal, equatorial) radius: TBD" << '\n';
    }else{
        std::cout << "Spheroid (internal, equatorial) radius: " << dimensions[0] << " m" << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    dimensions.push_back(input);
    if (input == 0)
    {
        isTBD = true;
        std::cout << "Spheroid ratio: TBD" << '\n';
    }else{
        std::cout << "Spheroid ratio: " << dimensions[1] << '\n';
    }
    userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    userFile >> input;
    if (input)
    {
        std::cout << input * 100 << "% of g will be generated at radius." << '\n';
        calculate_angular_velocity(input);
    }else{
        std::cout << "This spheroid is coupled to the previous component of the assembly." << '\n';
        isCoupled = true;
    }
    std::cout << '\n';
}
void spheroid::print_config()
{
    std::ofstream userFile("config.txt", std::ios::app);
    userFile << "Spheroid component:" << '\n' << '\t'
        << "(internal, equatorial) radius: > " << '\n' << '\t'
        << "ratio: (equatorial radius over polar radius | input 1 for a sphere) > " << '\n' << '\t'
        << "percentage of g to be generated at radius: > "
        << '\n' << '\n';
    userFile.close();
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