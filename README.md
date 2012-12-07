# CityDrain3

# Installation
The installer requires the following components to be installed:

* [Visual C 2008 Runtime x86](http://download.microsoft.com/download/9/7/7/977B481A-7BA6-4E30-AC40-ED51EB2028F2/vcredist_x86.exe)
* [Python 2.7 x86](http://python.org/ftp/python/2.7.2/python-2.7.2.msi)

# Compilation
For compilation of CityDrain3 the following tools/libraries are needed:

*   cmake
*  python 2.7 (including the developement packages)
*  swig
*  a recent compiler able to link against the python dlls (Visual Studio in 
Windows)
*  OpenMP capable compiler if you want the parallelization features
*  boost with:
    * filesystem
    * system
    * thread
    * signals
    * program_options
    * date_time
*  and Qt.


# Usage
*CityDrain3* includes a terminal application and a GUI application.

## Terminal Application
The terminal application is called *cd3*. On first call of *cd3* this usage is 
prompted:

    $ ./cd3
    $ provide a model file
    $ CityDrain3 command line options:
    $   -h [ --help ]          produce help message
    $   -d [ --state-dir ] arg used to store and locate restart data
    $   -r [ --restart ] arg   specifiy the time where to restart
    $   -m [ --model ] arg     the model to run the simulation

## GUI Application
The GUI application is called *cd3-gui* and should be availabe from your 
Windows menu if the installer was used on windows.

If you Start the *CityDrain3* GUI you are greeted with the main window. The main 
window has three parts with which you work the most:

- Node List: The node list contains the nodes which are available for your urban 
drainage system. 
- Model Area: The model area is used to connect and arrange your sewer system 
and urban drainage components.
- Log View: The log view is used to give detailed feedback to the user.

![](https://github.com/gregorburger/CityDrain3/raw/master/doc/manual/imgs/gui/mainwindow.png)

In the red highlighted area the left most button is used for creating a new and 
empty simulation. If you hit this button a dialog shows up where you can enter 
the details of the simulation. Everything except the flow definition can be 
changed afterwards. 

![New Simulation](https://github.com/gregorburger/CityDrain3/raw/master/doc/manual/imgs/gui/newsim.png)

Once you have accepted the properties of your new simulation you get back to the
 main window. The **Node List** is now filled with a list of predefined nodes. 
You can drag and drop the nodes from the list into the **Model Area**.

![](https://github.com/gregorburger/CityDrain3/raw/master/doc/manual/imgs/gui/emptysim.png)

Once you have droped a node on the **Model Area** a dialog pops up which prompts
 you to edit the properties of the node. This dialog is dynamically generated 
based on the nodes defined properties. If you cancel this dialog the node won't 
be 
placed on the **Model Area**. If you click **Ok** the properties are then set 
and the nodes *init* function is called. 
The node then verifies the properties. If a property is not accepted the dialog 
pops up again and a message shows up 
in the **Log View**. Only if all properties are verified to work the dialog
 disappears after hitting **Ok**.

![](https://github.com/gregorburger/CityDrain3/raw/master/doc/manual/imgs/gui/catchmentproperties.png)

To connect two nodes, i.e. make water flow from one component to the other, 
you begin by clicking a **source port** of a
node, drag the mouse to a **sink port** and release the mouse. Its important 
to always begin nodes at **source ports** they
are on the right of a node and end them at **sink ports**, which are located 
on the left of a node. After you are finished
with modelling your system you can run the simulation. 

![](https://github.com/gregorburger/CityDrain3/raw/master/doc/manual/imgs/gui/connection.png)
 
