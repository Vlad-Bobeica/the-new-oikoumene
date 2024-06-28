# The New Oikoumene

The New Oikoumene is a terminal C++ application designed to accurately size a space settlement using physics principles. It computes the dimensions of various components necessary for the construction of a space settlement, ensuring structural integrity and optimal performance in space environments.

## Features

- **Physics-Based Sizing**: Utilizes physics formulas to determine the dimensions of components such as tubes, rings, and spheroids.
- **Material Customization**: Allows users to input material properties for precise calculations.
- **Configurable Wall Layers**: Enables users to specify the number and properties of wall layers for structural integrity.
- **Dynamic Angular Momentum Calculation**: Computes the angular momentum of rotating assemblies to ensure stability.
- **Interactive User Interface**: Provides an intuitive command-line interface for easy configuration and operation.

## Installation

1. Clone the repository to your local machine:

    ```
    git clone https://github.com/Vlad-Bobeica/the-new-oikoumene.git
    ```

2. Navigate to the project directory:

    ```
    cd the-new-oikoumene
    ```

3. Create the `Makefile` or solution using **CMake**:

    ```
    cmake .
    ```

4. Compile the source code:

  	- On MacOS / Linux :
		```
		make
		```

  	- On Windows :
		```
		msbuild the-new-oikoumene.sln /p:OutDir=$(SolutionDir)
		```
5. Run the program:

  	- On MacOS / Linux :
		```
		./tno
		```

  	- On Windows :
		```
		tno.exe
		``` 

## Usage

Upon running the application, follow the prompts to configure a new vessel or load a previously configured one.
Specify the number and types of component subassemblies (geometric shapes) for the rotating assembly of the vessel.
Input dimensions for each component, and choose to have a To-Be-Determined (TBD) size.
The application will compute the optimal dimensions for TBD components based on specified constraints.
Review the results and choose to run the application again for multiple iterations or exit.

## Example
### Application in terminal
```

No previous data found.
Configuring a new vessel.

Table of materials:
 0 |      water |      997 (average interior density)
 1 |   titanium |     4506
 2 |      water |      997
 3 |        air |    1.293

Initialising wall:
    Enter number of wall layers:
> 3
    Enter thickness of Layer 1 in meters: (interior to exterior)
> 2
    Enter material of Layer 1: (number corresponding to a material)
> 1
    Enter thickness of Layer 2 in meters: (interior to exterior)
> 5.5
    Enter material of Layer 2: (number corresponding to a material)
> 2
    Enter thickness of Layer 3 in meters: (interior to exterior)
> 2.5
    Enter material of Layer 3: (number corresponding to a material)
> 1
Wall initialised successfully.

List of available shapes:
 |    tube
 |    ring
 |    spheroid

Configuring the first, clockwise rotating assembly of the vessel.
Enter the number of component subassemblies (geometric shapes):
> 2
    Enter shape of subassembly 1: (from list of shapes)
> spheroid
    Enter shape of subassembly 2: (from list of shapes)
> tube

Configuring the second, counter-clockwise rotating assembly of the vessel.
Enter the number of component subassemblies (geometric shapes):
> 1
    Enter shape of subassembly 1: (from list of shapes)
> ring


Vessel initialised. Opening configuration file...

Press ENTER to continue after filling in the configuration file.

Table of materials:
 0 |      water |      997 (average interior density)
 1 |   titanium |     4506
 2 |      water |      997
 3 |        air |    1.293

Wall loaded successfully.

TBD is short for To-Be-Determined. This program solves for a singular specified TBD size.

Configuring the first, clockwise rotating assembly of the vessel.

Creating spheroid component.
Spheroid (internal, equatorial) radius: 1800 m
Spheroid ratio: 2
120% of g will be generated at radius.

Creating tube component.
Tube (internal) radius: 290 m
Tube length: 2500 m
This tube is coupled to the previous component of the assembly.


Configuring the second, counter-clockwise rotating assembly of the vessel.

Creating ring component.
Ring (internal, outer) radius: 1800 m
Ring thickness: 400 m
Ring length: TBD
Has Interlink: yes
Ring is connected to the tube of (internal) radius: 290 m
120% of g will be generated at radius.


Your To-Be-Determined value is: 421.617 m

Run again? (y / n)
> n


Exiting program...
```

### Configuration file
```
This is the configuration file. Fill in, after each '>' character, dimensions in meters (input '0' for only one TBD size) and the percentage of g generated at the radius of each component using decimal numbers (input '1' for Earth gravity, '0' if the tube is coupled to the previous component of the assembly).


First, clockwise rotating assembly of the vessel:

Spheroid component:
    (internal, equatorial) radius: > 1800
    ratio: (equatorial radius over polar radius | input 1 for a sphere) > 2
    percentage of g to be generated at radius: > 1.2


Tube component:
    (internal) radius: > 290
    length: > 2500
    percentage of g to be generated at radius: > 0


Second, counter-clockwise rotating assembly of the vessel:

Ring component:
    (internal, outer) radius: > 1800
    thickness: > 400
    length: > 0
    Has interlink? (0 if welded to the tube it spins with, else 1) > 1
    (internal) radius of the tube it is connected to: > 290
    percentage of g to be generated at radius: > 1.2
```

## License
This project is licensed under the MIT License.
