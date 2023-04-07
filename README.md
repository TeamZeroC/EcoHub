# EcoHub
(Almost) Every Team-0C tool in one application
> **INDEX:**
> 1. [Setup](https://github.com/TeamZeroC/EcoHub#setup)
> 2. [Omg Errors!](https://github.com/TeamZeroC/EcoHub#omg-errors)
> 3. [Project Workflow](https://github.com/TeamZeroC/EcoHub#project-workflow)
> 4. [Applications](https://github.com/TeamZeroC/EcoHub#applications)

# Setup
- Install the [vcpck](https://vcpkg.io/en/) package manager.
- Install the following packages:
  - Boost ➜ ```vcpkg install boost-property-tree:x64-windows``` && ```vcpkg install boost-optional:x64-windows```
  - ImGui ➜ ```vcpkg install imgui[core,opengl3-binding,glfw-binding]:x64-windows```
  - ImPlot ➜ ```vcpkg install implot:x64-windows```
- Restart Visual Studio (if open).

# Omg Errors!
- Make sure the project is set to use c++17;
- Make sure the packages installation was successful;
- Make sure the packages are all x64, **not** x86.

# Project Workflow
Every application has its own folder, plus a folder for simple libraries includes.
Every application heart is a class. That same class must then be passed to the *IGH* in the *main.cpp*.
Please refer to the *TelemetryAnalyzer* app as an example of how the code should be structured (also feel free to improve the code structure).
> Do not even look at *PaperUI* as it can be considered "legacy" code adapted from an older version of ImGui.

# Applications
## PaperUI
Small application meant to be used to create the UI for the car's dash.
Use the drag-and-drop interface to create the layout. Generate the code, and paste it into the *DashTeensyEPaper* project where needed.

*(The generated code is ment to be used in congunction with the ToroPiniPaint library)*
## TelemetryAnalyzer
Allows for fast log analysis by automatically plotting the most common information. Also allows plotting of any column from a generic *.csv* file <br /> (as of now, the generic slot only supports vertical *.csv* tables)
### Ready to use graphs
- Instant Speed;
- Average Speed;
- Steer Angle;
- Battery Voltage;
- Motor Power.
### Analysis tools
- **Laps Focus:** Automatically plots vertical markers at the start and end of the specified lap interval.
- **Miss Activations:** Automatic detection of sudden drops in the motor power, most likely indicating a pilot miss-release.
- **Find Bad:** Allows to quickly compare between two logs or between a log file and the simulation data. Most useful to compare the pilot and car performance with the simulation. Also useful to easily check the consistency between attempts.
The comparison consists in highlighting the graph section where the delta between the two speeds is greater than a customizable max-delta.
