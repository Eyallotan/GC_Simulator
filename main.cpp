#include <iostream>
#include <fstream>
#include <cstdlib>
#include "AlgoRunner.h"
using namespace std;

/* get parameters from command line
 * #1:PHYSICAL_BLOCK_NUMBER
 * #2:LOGICAL_BLOCK_NUMBER
 * #3:PAGES_PER_BLOCK
 * #4:PAGE_SIZE
 * #5:NUMBER_OF_PAGES
 * #6:DATA_DISTRIBUTION
 * #7:ALGORITHM
 * #8:optional parameter - filename to redirect output to
 */

/**
 * Help function for CLI.
 */
void printHelp()
{
    cout << "Garbage collection (GC) manual: " << endl;
    cout << "These are the parameters you must set for each simulation:\n"
            "1. Number of physical blocks (T).\n"
            "2. Number of logical blocks (U).\n"
            "3. Pages per block (Z).\n"
            "4. Page size (in bytes).\n"
            "5. Number of pages (N).\n"
            "6. Data distribution.\n"
            "7. GC algorithm.\n"
            "8. Optional parameter: Filename to redirect output to." << endl;
    cout << "For data distribution parameter choose between uniform or hot_cold. If you choose hot/cold distribution, " << endl
         << "you will be asked to choose the hot page percentage and the probability for a hot page." << endl;
    cout << "For GC algorithm choose between the following:\n"
            "1. greedy.\n"
            "2. greedy_lookahead.\n"
            "3. writing_assignment (initial version).\n"
            "4. generational. If you choose this option you will be prompt to choose the number of generations. " << endl
            << "Make sure that the number of generations is always larger then T-U (this will be enforced by the simulator)." << endl
            << "If you choose number of generations to be 0, the simulator will choose the optimized parameter based on the OP (over provisioning)." << endl;
}

int main(int argc, char** argv) {
	if (argc < 8) {
	    if (argc == 2 && strcmp("--help", argv[1]) == 0){
	        printHelp();
	        return 0;
	    }

		cerr << "Invalid number of arguments!" << endl;
	    printHelp();
		return -1;
	}

	if (argc == 9) {
		output_file = argv[8];
		freopen(output_file, "a", stdout);
	}

	PHYSICAL_BLOCK_NUMBER = atoi(argv[1]);
	LOGICAL_BLOCK_NUMBER = atoi(argv[2]);
	PAGES_PER_BLOCK = atoi(argv[3]);
	PAGE_SIZE = atoi(argv[4]);
	NUMBER_OF_PAGES = atoll(argv[5]);
	PageDistribution page_dist = distributionStringToEnum(argv[6]);
	if (page_dist == INVALID_DIST){
        cerr << "Invalid Distribution Parameter!" << endl;
        printHelp();
        return -1;
	}
	Algorithm algo = algoStringToEnum(argv[7]);
	if (algo == INVALID_ALGO){
        cerr << "Invalid Algorithm Parameter!" << endl;
        printHelp();
        return -1;
	}

	float ALPHA = (float) LOGICAL_BLOCK_NUMBER / PHYSICAL_BLOCK_NUMBER;
    cout << "Starting GC Simulator!" << endl;
	cout << "Physical Blocks:\t" << PHYSICAL_BLOCK_NUMBER << endl;
	cout << "Logical Blocks:\t\t" << LOGICAL_BLOCK_NUMBER << endl;
	cout << "Pages/Block:\t\t" << PAGES_PER_BLOCK << endl;
	cout << "Page Size:\t\t" << PAGE_SIZE << endl;
	cout << "Alpha:\t\t\t" << ALPHA << endl;
	cout << "Over Provisioning:\t"<< (float)(PHYSICAL_BLOCK_NUMBER-LOGICAL_BLOCK_NUMBER)/LOGICAL_BLOCK_NUMBER<<endl;
    cout << "Number of Pages:\t" << NUMBER_OF_PAGES << endl;
    cout << "Page Distribution:\t" << argv[6] << endl;
    cout << "GC Algorithm:\t\t" << argv[7] << endl;


    /* activate random number generator seed */
	seed();

	/* generate scheduledGC object */
    AlgoRunner* scg = new AlgoRunner(NUMBER_OF_PAGES, page_dist, algo);

    /* if you wish to activate print mode remove comment */
    //scg->setPrintMode(true);

    /* if you wish to deactivate steady state mode remove comment */
    //scg->setSteadyState(false);

    /* run simulation and print results */
    scg->runSimulation(algo);
    scg->printSimulationResults();

    /* cleanup */
    delete scg;
    output_file = nullptr;
	if (argc == 9){
		fclose(stdout);
	}


	return 0;
}

