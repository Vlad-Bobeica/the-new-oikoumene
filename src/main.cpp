//  main.cpp

#include <string>
#include <iomanip>

#include "component.h"

using namespace std;

// Defining a structure that holds components and a total angular momentum.
struct assembly
{
    vector<component*> part;
    double angular_momentum;
};

// Defining a structure that holds indices to a specific half->subassembly->dimension;
// Is used to store the indices to the TBD value.
struct highlighter
{
    int half;
    int sub;
    int dimension;
};

// Function to initialise the material densities used when calculating masses.
// Pulls data from the accessible "materials.txt" file and displays a table, returns a map.
map<int, double> getMaterials()
{
    map<int, double> density;
    ifstream densities("materials.txt");
    int i;
    string str;
    double dens;
    cout << "Table of materials:" << endl;
    while (densities >> i >> str >> dens)
    {
        cout << setw(2) << i << " | ";
        cout << setw(10) << str << " | ";
        cout << setw(8) << dens;
        if (i == 0) cout << " (average interior density)";
        cout << '\n';
        density[i] = dens;
    }
    densities.close();
    cout << endl;
    return density;
}

// TO-DO COMMENTS
int checkFile()
{
    bool repeat;
    ifstream save("data.txt", std::ios::ate);
    if (save.tellg() != 0)
    {
        do{
            repeat = false;
            cout << '\n' << "Saved data found." << '\n'
            << "Proceed with loading the previous vessel? (Y to load / N to delete saved data / E to exit)"
            << '\n' << "> ";
            char response;
            cin >> response;
            if (response == 'Y' || response == 'y'){
                cout << '\n' << "Loading previous vessel..." << '\n' << endl;
                cout << '\n' << '\n' << "Opening configuration file..." 
                    << '\n' << '\n' << "Press ENTER to continue after filling in the configuration file." << '\n';
                #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
                std::system("config.txt");
                system("pause");
                #else
                std::system("open config.txt");
                system("read");
                #endif
                return 1;
            }
            else if (response == 'N' || response == 'n')
            {
                ofstream save("data.txt");
                save.close();
                cout << "Saved data deleted." << '\n' << '\n';
                return 0;
            }
            else if (response == 'E' || response == 'e')
            {
                return -1;
            }
            else{
                cout << "Undefined input. Restarting..." << '\n' << endl;
                repeat = true;
            }
        }while(repeat);
    }
    return 0;
}

//TO-DO COMMENTS
char run(int run_case)
{
    if (run_case == 0)
    {
        cout << '\n' << "No previous data found." << '\n' << "Configuring a new vessel." << '\n' << endl;
        map<int, double> density = getMaterials();
        wall exterior_wall(run_case);
        ofstream save("data.txt", std::ios::app);
        ofstream userFile("config.txt");
        userFile << "This is the configuration file. Fill in, after each '>' character, dimensions in meters (input '0' for only one TBD size) "
            << "and the percentage of g generated at the radius of each component using decimal numbers "
            << "(input '1' for Earth gravity, '0' if the tube is coupled to the previous component of the assembly)."
            << '\n' << '\n' << '\n';
        userFile.close();
        cout << "List of available shapes:" << '\n'
        << " |" << '\t' << "tube" << '\n'
        << " |" << '\t' << "ring" << '\n'
        << " |" << '\t' << "spheroid" << '\n';
        for (int i = 0; i < 2; i++)
        {
            ofstream userFile("config.txt", std::ios::app);
            cout << '\n' << "Configuring the ";
            if (i == 0)
            {
                cout << "first, clockwise ";
                userFile << "First, clockwise ";
            }else{ 
                cout << "second, counter-clockwise ";
                userFile << "Second, counter-clockwise ";
            }
            userFile << "rotating assembly of the vessel:" << '\n' << '\n';
            cout << "rotating assembly of the vessel." << '\n'
            << "Enter the number of component subassemblies (geometric shapes):"
            << '\n' << "> ";
            int subassemblies;
            cin >> subassemblies;
            save << subassemblies << endl;
            userFile.close();
            for (int k = 0; k < subassemblies; k++)
            {
                cout << '\t' << "Enter shape of subassembly " << k+1 << ": (from list of shapes)" << '\n' << "> ";
                string shape;
                cin >> shape;
                if (shape == "tube")
                {
                    save << "tube" << endl;
                    tube::print_config();
                }else if (shape == "ring")
                {
                    save << "ring" << endl;
                    ring::print_config();
                }else if (shape == "spheroid")
                {
                    save << "spheroid" << endl;
                    spheroid::print_config();
                }else{
                    cout << "Unidentified shape name." << '\n';
                    k--;
                    continue;
                }
                ofstream userFile("config.txt", std::ios::app);
                userFile << '\n';
                userFile.close();
            }
        }
        save.close();
        cout << '\n' << '\n' << "Vessel initialised. Opening configuration file..." 
            << '\n' << '\n' << "Press ENTER to continue after filling in the configuration file." << '\n';
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        std::system("config.txt");
        system("pause");
        #else
        std::system("open config.txt");
        system("read");
        #endif
        return 'y';
    }
    else if (run_case == 1)
    {
        assembly half[2];
        highlighter tbd;
        map<int, double> density = getMaterials();
        wall exterior_wall(run_case);
        ifstream save("data.txt");
        save.ignore(100, '\n');
        for (int i = 0; i < exterior_wall.number_of_layers; i++)
        {
            save.ignore(100, '\n');
            save.ignore(100, '\n');
        }
        ifstream userFile("config.txt");
        userFile.ignore(std::numeric_limits<std::streamsize>::max(), '>');
        cout << "TBD is short for To-Be-Determined. This program solves for a singular specified TBD size." << '\n';
        for (int i = 0; i < 2; i++)
        {
            cout << '\n' << "Configuring the ";
            if (i == 0) cout << "first, clockwise ";
            else cout << "second, counter-clockwise ";
            cout << "rotating assembly of the vessel." << '\n' << '\n';
            int subassemblies;
            save >> subassemblies;
            for (int k = 0; k < subassemblies; k++)
            {
                string shape;
                save >> shape;
                if (shape == "tube")
                {
                    half[i].part.push_back(new tube(userFile));
                }else if (shape == "ring")
                {
                    half[i].part.push_back(new ring(userFile));
                }else if (shape == "spheroid")
                {
                    half[i].part.push_back(new spheroid(userFile));
                }
                if (half[i].part[k]->isTBD)
                {
                    tbd.half = i;
                    tbd.sub = k;
                }
                if (half[i].part[k]->isCoupled)
                {
                    half[i].part[k]->setAngularVelocity(half[i].part[k-1]->getAngularVelocity());
                }
            }
        }
        save.close();
        userFile.close();
        double trial_size = 0;
        double lower_bound = 10;
        double upper_bound = 10000;
        double added_momentum = 0;
        half[abs(tbd.half - 1)].angular_momentum = 0;
        for (int k = 0; k < half[abs(tbd.half - 1)].part.size(); k++)
        {
            half[abs(tbd.half - 1)].part[k]->compute(exterior_wall, density);
            half[abs(tbd.half - 1)].angular_momentum += half[abs(tbd.half - 1)].part[k]->get_angular_momentum();
        }
        half[tbd.half].angular_momentum = 0;
        for (int k = 0; k < half[tbd.half].part.size(); k++)
        {
            if (k != tbd.sub)
            {
                half[tbd.half].part[k]->compute(exterior_wall, density);
                half[tbd.half].angular_momentum += half[tbd.half].part[k]->get_angular_momentum();
            }
        }
        for (int i = 0; i < half[tbd.half].part[tbd.sub]->dimensions.size(); i++)
            if (half[tbd.half].part[tbd.sub]->dimensions[i] == 0)
            {
                tbd.dimension = i;
                break;
            }
        half[tbd.half].part[tbd.sub]->dimensions[tbd.dimension] = lower_bound;
        half[tbd.half].part[tbd.sub]->compute(exterior_wall, density);
        added_momentum = half[tbd.half].part[tbd.sub]->get_angular_momentum();
        if (half[abs(tbd.half - 1)].angular_momentum - (half[tbd.half].angular_momentum + added_momentum) < 1000)
        {
            cout << '\n' <<"Invalid data set (the angular momentum of the TBD assembly is too large)." << '\n' << endl;
            cout << "Press ENTER to continue after modifying the configuration file." << '\n';
            #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
            system("pause");
            #else
            system("read");
            #endif
            return 'y';
        }
        added_momentum = 0;
        while (lower_bound <= upper_bound &&
                (fabs(half[abs(tbd.half - 1)].angular_momentum - half[tbd.half].angular_momentum) > 1000))
        {
            half[tbd.half].angular_momentum -= added_momentum;
            trial_size = (lower_bound + upper_bound) * 0.5;
            half[tbd.half].part[tbd.sub]->dimensions[tbd.dimension] = trial_size;
            half[tbd.half].part[tbd.sub]->compute(exterior_wall, density);
            added_momentum = half[tbd.half].part[tbd.sub]->get_angular_momentum();
            half[tbd.half].angular_momentum += added_momentum;
            if (half[tbd.half].angular_momentum > half[abs(tbd.half - 1)].angular_momentum)
                upper_bound = trial_size;
            else if (half[tbd.half].angular_momentum < half[abs(tbd.half - 1)].angular_momentum)
                lower_bound = trial_size;
            if (trial_size > 9999)
            {
                cout << '\n' <<"Invalid data set (the angular momentum of the TBD assembly is too small)." << '\n' << endl;
                cout << "Press ENTER to continue after modifying the configuration file." << '\n';
                #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
                system("pause");
                #else
                system("read");
                #endif
                return 'y';
            }
        }
        cout << '\n' << "Your To-Be-Determined value is: " << trial_size << " m" << endl;
        cout << '\n' << "Run again? (y / n)" << '\n' << "> ";
        char repeat;
        cin >> repeat;
        cout << '\n' << endl;
        return repeat;
    }
    else return 'n';
}


int main()
{
    if (run(checkFile()) == 'y')
    {
        while(run(1) == 'y');
    }
    cout << "Exiting program..." << endl;
    return 0;
}
