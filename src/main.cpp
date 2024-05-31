//  main.cpp

#include <iostream>
#include <fstream>
#include <vector>
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
    do{
    repeat = false;
    cout << '\n' << "Saved data found." << '\n'
    << "Proceed with loading the previous vessel? (Y to load / N to delete saved data / E to exit)"
    << '\n' << "> ";
    char response;
    cin >> response;
    if (response == 'Y' || response == 'y'){
        cout << '\n' << "Loading previous vessel..." << '\n' << endl;
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
        cout << "List of available shapes:" << '\n'
        << " |" << '\t' << "tube" << '\n'
        << " |" << '\t' << "ring" << '\n'
        << " |" << '\t' << "spheroid" << '\n';
        for (int i = 0; i < 2; i++)
        {
            cout << '\n' << "Configuring the ";
            if (i == 0) cout << "first, clockwise ";
            else cout << "second, counter-clockwise ";
            cout << "rotating assembly of the vessel." << '\n'
            << "Enter the number of component subassemblies (geometric shapes):"
            << '\n' << "> ";
            int subassemblies;
            cin >> subassemblies;
            save << subassemblies << endl;
            for (int k = 0; k < subassemblies; k++)
            {
                cout << '\t' << "Enter shape of subassembly " << k+1 << ": (from list of shapes)" << '\n' << "> ";
                string shape;
                cin >> shape;
                if (shape == "tube")
                {
                    save << "tube" << endl;
                }else if (shape == "ring")
                {
                    save << "ring" << endl;
                }else if (shape == "spheroid")
                {
                    save << "spheroid" << endl;
                }else{
                    cout << "Unidentified shape name." << '\n';
                    k--;
                }
            }
        }
        save.close();
        return 'y';
    }
    else if (run_case == 1)
    {
        assembly half[2];
        highlighter tbd;
        map<int, double> density = getMaterials();
        wall exterior_wall(run_case);
        ifstream save("data.txt");
        string dump;
        getline(save, dump);
        for (int i = 0; i < exterior_wall.number_of_layers; i++)
        {
            getline(save, dump);
            getline(save, dump);
        }
        cout << "TBD is short for To-Be-Determined. This program solves for a singular specified TBD size." << '\n';
        for (int i = 0; i < 2; i++)
        {
            cout << '\n' << "Configuring the ";
            if (i == 0) cout << "first, clockwise ";
            else cout << "second, counter-clockwise ";
            cout << "rotating assembly of the vessel." << '\n';
            int subassemblies;
            save >> subassemblies;
            for (int k = 0; k < subassemblies; k++)
            {
                string shape;
                save >> shape;
                if (shape == "tube")
                {
                    half[i].part.push_back(new tube);
                }else if (shape == "ring")
                {
                    half[i].part.push_back(new ring);
                }else if (shape == "spheroid")
                {
                    half[i].part.push_back(new spheroid);
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
