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


int main(int argc, char** argv) {
	if (argc < 8) {
		cerr << "Invalid number of arguments!" << endl;
		return 0;
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
        return 0;
	}
	Algorithm algo = algoStringToEnum(argv[7]);
	if (algo == INVALID_ALGO){
        cerr << "Invalid Algorithm Parameter!" << endl;
        return 0;
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

