# Flash GC Simulator

This simulator enables to run various garbage collection (GC) algorithms on an SSD memory layout. we implement a fully customizable infastructure that allows you to run your algorithm of choice on many different memory settings and record statistics such as number of erases and write amplification.

For full details about the theoretical model and the algorithms implemented in this simulator please read the [project report](https://github.com/Eyallotan/GC_Simulator/blob/d9eda9a190eb17bbe6059b055a68dc30678620fb/Garbage%20Collection%20Algorithms%20for%20Flash%20Memories.pdf). In the report you will find all the needed prerequisites, along with a full breakdown of each algorithm, experiments results and more. 

## Installation

Follow these steps:

```bash
$ git clone https://github.com/Eyallotan/GC_Simulator.git
$ cd GC_Simulator
$ make
$ ./Simulator <enter command line parameters>
```

## Usage

### Command Line Parameters

These are the parameters you must set for each simulation:
1. Number of physical blocks (T)
2. Number of logical blocks (U)
3. Pages per block (Z)
4. Page size (in bytes)
5. Number of pages (N) 
6. Data distribution
7. GC algorithm
8. Optional parameter: Filename to redirect output to 

* For data distribution parameter choose between ```uniform``` or ```hot_cold```. If you choose hot/cold distribution, you will be asked to choose the hot page percentage and the probability for a hot page.
* For GC algorithm choose between the following:
1. ```greedy```
2. ```greedy_lookahead```
3. ```writing_assingment``` (This algorithm is still a work in progress..)
4. ```generational```. If you choose this option you will be prompt to choose the number of generations. In our initial [project report](https://github.com/Eyallotan/GC_Simulator/blob/d9eda9a190eb17bbe6059b055a68dc30678620fb/Garbage%20Collection%20Algorithms%20for%20Flash%20Memories.pdf) we analyze the case for 2 generations. In any case, you should make sure that number of generations is always larger then T-U (this will also be enforced by the simulator).

For example: 
```bash
$ ./Simulator 64 50 32 4096 100000 uniform greedy
Starting GC Simulator!
Physical Blocks:        64
Logical Blocks:         50
Pages/Block:            32
Page Size:              4096
Alpha:                  0.78125
Over Provisioning:      0.28
Number of Pages:        100000
Page Distribution:      uniform
GC Algorithm:           greedy
Starting Greedy Algorithm simulation...
Reaching Steady State...
Steady State Reached...
Simulation Results:
Number of erases: 7395. Write Amplification: 2.36627

$ ./Simulator 64 50 32 4096 100000 uniform greedy_lookahead
Starting GC Simulator!
Physical Blocks:        64
Logical Blocks:         50
Pages/Block:            32
Page Size:              4096
Alpha:                  0.78125
Over Provisioning:      0.28
Number of Pages:        100000
Page Distribution:      uniform
GC Algorithm:           greedy_lookahead
Starting Greedy LookAhead Algorithm simulation...
Reaching Steady State...
Steady State Reached...
Simulation Results:
Number of erases: 7333. Write Amplification: 2.34665

$ ./Simulator 64 50 32 4096 100000 uniform generational
Starting GC Simulator!
Physical Blocks:        64
Logical Blocks:         50
Pages/Block:            32
Page Size:              4096
Alpha:                  0.78125
Over Provisioning:      0.28
Number of Pages:        100000
Page Distribution:      uniform
GC Algorithm:           generational
Starting Generational Algorithm simulation...
Enter number of generations for Generational GC:
2
Reaching Steady State...
Steady State Reached...
Simulation Results:
Number of erases: 7053. Write Amplification: 2.25694
```

### Steady State 
We use steady state assumption (as stated in the [project report](https://github.com/Eyallotan/GC_Simulator/blob/d9eda9a190eb17bbe6059b055a68dc30678620fb/Garbage%20Collection%20Algorithms%20for%20Flash%20Memories.pdf)). Therefore the steady state flag is automatically turned on. if you wish to turn it off you can comment out the following line in [```main.cpp```](main.cpp):
```cpp
67.  /* if you wish to deactivate steady state mode remove comment */
68.    //scg->setSteadyState(false);
``` 
You can also adjust the number of writes to use to achieve steady state (this can be better integrated in the future to be part of the adjustable parameters..). 

### Print Mode
We have implemented a print mode option that reflects block and page statistics as the simulator runs, along with information about the number of logical writes and more useful information. The print mode option is turned off by default and should not be used unless you redirect your output to a file (otherwise print time will probably make the simulation run for a very long time). if you wish to turn on the print mode you can comment out the following line in [```main.cpp```](main.cpp):
```cpp
64.  /* if you wish to activate print mode remove comment */
65.    //scg->setPrintMode(true);
``` 

## Contributing

Pull requests are welcomed. 
There are many future TODO's we hope to get to in the near (or far) future. 
Some of our ideas are written in the [project report](https://github.com/Eyallotan/GC_Simulator/blob/d9eda9a190eb17bbe6059b055a68dc30678620fb/Garbage%20Collection%20Algorithms%20for%20Flash%20Memories.pdf).

## License

The Simulator FTL interface is based on the work of Alex Yucovich. Thank you for letting us base our project on his work.
All rights are reserved to Eyal Lotan and Dor Sura.
