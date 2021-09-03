/*
 *	Created by Eyal Lotan and Dor Sura.
 */

#ifndef FLASHGC_ALGORUNNER_H
#define FLASHGC_ALGORUNNER_H

#include "MyRand.h"
#include "FTL.hpp"
#include "ListItem.h"
#include "Auxilaries.h"
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unistd.h>

#define TBD -11

using std::map;
using std::vector;

class UserParameters{
public:
    long long window_size;
    int num_of_gen;
    /* parameters for Hot/Cold memory simulation */
    int hot_pages_percentage;
    double hot_pages_probability;
    UserParameters() : window_size(NUMBER_OF_PAGES), num_of_gen(0), hot_pages_percentage(0), hot_pages_probability(0){};
};

class AlgoRunner{
public:

    ////// member elements:  //////
    /* Algorithm's type which user would like to simulate */
    Algorithm algo;

    /* the writing sequence that is given as an input to all Look Ahead algorithms in this class.
     * the writing sequence is an array of integers, where writing_sequence[i] is the logical page
     * number that will be written in the ith place (i.e the i+1 write since we start from 0).
     */
    unsigned int* writing_sequence;

    /* number of pages in writing sequence. This parameter can be adjusted to be a window of known writes, but
     * this feature may require some more adjustments
     * In the current implementation we use the global NUMBER_OF_PAGES macro, but this is bad practice for sure
     * if we wish to scale up in any way.. in that case we should switch and use this member element */
    unsigned long long number_of_pages;

    /* location list is a hash table that maps a logical page number to a ListItem that contains a list of indexes
     * in the writing sequence such that for page i and each index j in the location list matching that page i it
     * holds that writing_sequence[j] == i
     */
    map<unsigned int,ListItem>* logical_pages_location_map;

    /* writing page_dist represents the data distribution type - uniform distribution or Hot/Cold distribution */
    PageDistribution page_dist;

    UserParameters user_parameters;

    /* FTL memory layout object */
    FTL* ftl;

    /* data to write in each page. As mentioned below, this data is generated randomly and is the same across all
     * pages. for the sake if this simulator this is fine, but of course you can change this to contain some
     * meaningful data
     */
    char* data;

    bool reach_steady_state;
    bool print_mode;

    ////// C'tors & D'tor:  //////

    /* C'tor for scheduledGC object.
     * NOTE: throughout the whole class implementation we use the global macros NUMBER_OF_PAGES, PAGES_PER_BLOCK, etc.
     * I guess that best practice is not to use the global macros, and instead pass relevant parameters to the class c'tor (as we do with number_of_pages).
     * but for now this is fine. If you were to use this class file and copy it to their your personal use,
     * you should note that some changes may be needed to use only the parameters passed to the class
     * c'tor (and this is better coding practice).
     */
    AlgoRunner(long long number_of_pages, PageDistribution page_dist, Algorithm algo) : number_of_pages(number_of_pages), page_dist(page_dist),
                                                                        algo(algo), ftl(nullptr), data(nullptr), reach_steady_state(true), print_mode(false){

        /* generate a writing sequecne according to the desired writing page_dist */
        if (page_dist == UNIFORM){
            writing_sequence = generateUniformlyDistributedWriteSequence();
        }
        else {
            //double hot_pages_percentage, p;
            if(output_file){
                dup2(fd_stdout, 1);
            }
            cout<<"Please enter parameters for Hot/Cold memory simulation."<<endl<<"Enter the hot page percentage out of all logical pages in memory (0-100): "<<endl;
            cin >> user_parameters.hot_pages_percentage;
            if(user_parameters.hot_pages_percentage < 0 or user_parameters.hot_pages_percentage > 100){
                cerr<<"Error! Hot pages percentage must be in 0-100 range. crashing.."<<endl;
                exit(1);
            }
            cout<<"Enter the probability for hot pages (0-1): "<<endl;
            cin >> user_parameters.hot_pages_probability;
            if(user_parameters.hot_pages_probability < 0 or user_parameters.hot_pages_probability > 1){
                cerr<<"Error! Hot pages probability must be in 0-1 range. crashing.."<<endl;
                exit(1);
            }
            if(output_file)
                freopen(output_file, "a", stdout);
            writing_sequence = generateHotColdWriteSequence(user_parameters.hot_pages_percentage, user_parameters.hot_pages_probability);
        }

        /* construct a mapping between a logical page number to a ListItem corresponding the logical page.
        * each ListItem for logical page i contains the list of locations in the writing sequence where
        * the page i is written. each ListItem contains a list of locations sorted in an ascending order.
        */
        logical_pages_location_map = createLocationsMap(0,NUMBER_OF_PAGES);
        initializeFTL();

        if(algo != GREEDY)
            getWindowSizeFromUser();
        if(algo == GENERATIONAL)
            getNumOfGenerationsFromUser();
    }

    ~AlgoRunner() {
        delete [] writing_sequence;
        delete [] logical_pages_location_map;
        delete [] data;
        delete ftl;
    }


    ////// class functions:  //////


    /* initialize FTL simulator. We construct an FTL object that represents the memory layout.
     * If steady state flag is turned on, we bring the simulator to a steady state by writing random pages
     * (we choose the pages uniformly in random).
     * NOTE: the data is generated by random. for the purpose of this simulator there is no meaning to the
     * data itself. for this reason we populate all pages with the same value.
     */
    void initializeFTL(){
        ftl = new FTL();

        /* initialize data page. will remain the same */
        data = new char[PAGE_SIZE];

       /* fill pages with random data */
        for (int j = 0; j < PAGE_SIZE; j++) {
            data[j] = KISS() % 256;
        }
    }

    void setSteadyState(bool state){
        reach_steady_state = state;
    }

    void setPrintMode(bool mode){
        print_mode = mode;
        ftl->print_mode = mode;
    }

    void reachSteadyState(){
        unsigned int logical_page_to_write;
        data = new char[PAGE_SIZE];

        /* fill pages with random data */
        for (int j = 0; j < PAGE_SIZE; j++) {
            data[j] = KISS() % 256;
        }

        /* reach steady state */
        cout<<"Reaching Steady State..."<<endl;
        if (print_mode){
            ftl->printHeader();
        }
        /* you can adjust this */
        for (int i = 0; i < 1000000; i++) {
            logical_page_to_write = KISS() % (LOGICAL_BLOCK_NUMBER * PAGES_PER_BLOCK);
            ftl->write(data,logical_page_to_write,GREEDY);
        }
        ftl->erases_steady = ftl->erases;
        ftl->logicalPageWritesSteady = ftl->logicalPageWrites;
        ftl->physicalPageWritesSteady = ftl->physicalPageWrites;
        cout<<"Steady State Reached..."<<endl;
    }

    /* get the number of unique logical pages in writing_sequence */
    unsigned int getLocationListSize(unsigned long long base_index, unsigned int window_size) const{
        set<int> logical_pages_in_window;
        for (unsigned long long i = base_index; i < base_index + window_size && i < NUMBER_OF_PAGES ; ++i) {
            logical_pages_in_window.insert(writing_sequence[i]); // will not add duplicates
        }
        return logical_pages_in_window.size();
    }


    map<unsigned int,ListItem>* createLocationsMap(unsigned long long base_index, unsigned int window_size) const{
        unsigned int location_list_size = getLocationListSize(base_index,window_size);
        map<unsigned int,ListItem>* locations_list = new map<unsigned int,ListItem>[location_list_size];
        for (unsigned long long i = base_index; i < base_index + window_size && i < NUMBER_OF_PAGES; ++i) {
            auto iterator = locations_list->find(writing_sequence[i]);
            if (iterator == locations_list->end()){
                locations_list->insert({writing_sequence[i],ListItem(writing_sequence[i],i)});
            }
            else {
                iterator->second.addLocation(i);
            }
        }
        return locations_list;
    }

    void runGreedySimulation(Algorithm algo, long long window_size = 0) {
        if (reach_steady_state){
            reachSteadyState();
        }
        for (unsigned long long i = 0; i < window_size; i++) {
            ftl->write(data,writing_sequence[i],GREEDY_LOOKAHEAD, writing_sequence, i);
        }
        for (unsigned long long i = window_size; i < NUMBER_OF_PAGES; i++) {
            ftl->write(data,writing_sequence[i],GREEDY, writing_sequence, i);
        }
    }

    void runSimulation(Algorithm algorithm){
        switch (algorithm) {
            case GREEDY:
                cout<<"Starting Greedy Algorithm simulation..."<<endl;
                runGreedySimulation(GREEDY);
                break;
            case GREEDY_LOOKAHEAD:
                cout<<"Starting Greedy LookAhead Algorithm simulation..."<<endl;
                runGreedySimulation(GREEDY_LOOKAHEAD, this->user_parameters.window_size);
                break;
            case GENERATIONAL:
                cout<<"Starting Generational Algorithm simulation..."<<endl;
                runGenerationalSimulation(this->user_parameters.num_of_gen, this->user_parameters.window_size);
                break;
            case WRITING_ASSIGNMENT:
                cout<<"Starting Writing Assignment Algorithm simulation..."<<endl;
                runWritingAssignmentSimulation();
                break;
            default:
                cerr<<"Error in runSimulation"<<endl;
                exit(1);
        }
    }

    void printSimulationResults() const{
        int erases = ftl->erases-ftl->erases_steady;
        int logical_page_writes = ftl->logicalPageWrites-ftl->logicalPageWritesSteady;
        int physical_page_writes = ftl->physicalPageWrites-ftl->physicalPageWritesSteady;
        double wa = (double)physical_page_writes/logical_page_writes;
        //double erasure_factor = erases/(NUMBER_OF_PAGES /(double)PAGES_PER_BLOCK);
        cout<<"Simulation Results:"<<endl<<"Number of erases: "<<erases<<". Write Amplification: "<<wa<<endl;
    }

    void runWritingAssignmentSimulation(){
        if (reach_steady_state){
            reachSteadyState();
        }
        unsigned long long base_index = 0;
        unsigned int window_size = getWindowSize();
        while (base_index < NUMBER_OF_PAGES){
            vector<pair<unsigned int,int>> writing_assignment = getWritingAssignment(base_index,window_size);
            for (unsigned long long i = 0; i < writing_assignment.size() && i < NUMBER_OF_PAGES; i++) {
                ftl->writeToBlock(data, writing_assignment[i].first, writing_assignment[i].second);
            }
            base_index += window_size;
            window_size = getWindowSize();
        }
    }

    unsigned int getWindowSize() const{
        if (page_dist == UNIFORM){
            return (PHYSICAL_BLOCK_NUMBER * PAGES_PER_BLOCK) - ftl->windowSizeAux();
        }
        return (PHYSICAL_BLOCK_NUMBER * PAGES_PER_BLOCK) - ftl->getNumberOfValidPages();
    }

    void getWindowSizeFromUser(){
        char use_window_size;
        cout<<"Would you like to use window size parameter? y/n"<<endl;
        cin>>use_window_size;
        if(use_window_size != 'y' and use_window_size != 'n'){
            cerr<<"Error! Answer must be 'y' or 'n'. crashing.."<<endl;
            exit(1);
        }
        if(use_window_size == 'y'){
            cout<<"Enter Window Size..."<<endl;
            cin >> user_parameters.window_size;
            if(user_parameters.window_size < 0){
                cerr<<"Error! Window size is a negative number. crashing.."<<endl;
                exit(1);
            }
            if(user_parameters.window_size > NUMBER_OF_PAGES){
                cerr<<"Error! Window size is bigger than Number Of Pages. crashing.."<<endl;
                exit(1);
            }
        }
    }

    void getNumOfGenerationsFromUser(){
        if(output_file){
            dup2(fd_stdout, 1);
        }
        cout<<"Enter number of generations for Generational GC: (Enter 0 for best number of generations argument)"<<endl;
        cin >> user_parameters.num_of_gen;
        if(output_file)
            freopen(output_file, "a", stdout);
        if (user_parameters.num_of_gen > PHYSICAL_BLOCK_NUMBER - LOGICAL_BLOCK_NUMBER){
            cerr << "Error! number of generations must be at least T-U. crashing.." <<endl;
            exit(1);
        }
        if (user_parameters.num_of_gen == 1){
            cerr << "Error! genarations number must be bigger than 1. crashing.." <<endl;
            exit(1);
        }
        if (user_parameters.num_of_gen < 0){
            cerr<<"Error! Window size is a negative number. crashing.."<<endl;
            exit(1);
        }
        if(user_parameters.num_of_gen == 0)
            user_parameters.num_of_gen = ftl->optimized_params.second;
    }

    void getHotColdParametersFromUser(){

    }

    /* this is the writing assignment algorithm with printing operations
     * that print out memory layout, writing assignments on the fly and window sizes. this
     * should be used for debug purposes only and with small numbers in order to not mess up
     * the printing functions
     */
    void runWritingAssignmentSimulationDEBUG(){
        if (reach_steady_state){
            reachSteadyState();
        }
        ftl->printMemoryLayout();
        unsigned long long base_index = 0;
        unsigned int window_size = getWindowSize();
        while (base_index < NUMBER_OF_PAGES){
            cout<<"memory before window writes:"<<endl;
            ftl->printMemoryLayout();
            cout<<"window size: "<<window_size<<endl;
            vector<pair<unsigned int,int>> writing_assignment = getWritingAssignment(base_index,window_size);
            printAssignment(writing_assignment);
            for (unsigned long long i = 0; i < writing_assignment.size() && i < NUMBER_OF_PAGES; i++) {
                ftl->writeToBlock(data, writing_assignment[i].first, writing_assignment[i].second);
            }
            cout<<"memory after window writes:"<<endl;
            ftl->printMemoryLayout();
            base_index += window_size;
            window_size = getWindowSize();
        }
    }

    static void printAssignment(const vector<pair<unsigned int,int>>& writing_assignment){
        for (auto pair : writing_assignment){
            cout<<"page number: "<<pair.first<<" assignment: "<<pair.second<<endl;
        }
    }

    vector<pair<unsigned int,int>> getWritingAssignment(unsigned long long base_index, unsigned int window_size){

        /* construct a result vector, containing pairs of (logical_page_to_write,physical_block_to_write_to) */
        vector<pair<unsigned int,int>> res(window_size);
        int j = 0;
        for (unsigned long long i = base_index; i < base_index + window_size && i < NUMBER_OF_PAGES ; i++){
            res[j].first = writing_sequence[i];
            res[j].second = TBD;
            j++;
        }

        /* construct a mapping between a logical page number to a ListItem corresponding the logical page.
         * each ListItem for logical page i contains the list of locations in the writing sequence where
         * the page i is written. each ListItem should contain a list of locations sorted in an ascending order.
         */
        map<unsigned int,ListItem>* locations_list = createLocationsMap(base_index, window_size);

        /* get an ordered list of block numbers to assign writes to. Blocks are ordered by block score function
         * in ascending order.
         */
        vector<int> blocks = getBlockOrdering(base_index);

        /* find the assignment for each page in the window. we do this by populating each
         * block at a time according to the blocks vector.
         * for blocks[0], we assign the pages that are the FIRST ONES to be overwritten,
         * i.e marked as INVALID. for blocks[1] we assign pages that are the SECOND ONES
         * to be overwritten, etc.
         */
        assignWritesToBlocks(locations_list, &res, blocks, base_index);

        /* the final output contains the block number for each page in the window.
         * this is the physical page that we will write the page to.
         */

        delete [] locations_list;
        return res;

    }

    vector<int> getBlockOrdering(unsigned long long base_index) const {
        vector<int> block_list;
        vector<pair<int,double>> block_scores;

        for (auto block : ftl->freeList){
            double score = ftl->getBlockScore(block->blockNo, base_index, writing_sequence);
            block_scores.emplace_back((pair<int,double>{block->blockNo,score}));
        }

        //TODO: adjust k
        ftl->updateMinValid();
        for (int k = ftl->Y ; k <= (page_dist == UNIFORM ? ftl->Y + 1 : PAGES_PER_BLOCK-1) ; k++) {
            for (int block_num : ftl->V[k]) {
                double score = ftl->getBlockScore(block_num, base_index, writing_sequence);
                block_scores.emplace_back(pair<int, double>{block_num, score});
            }
        }

        std::sort(block_scores.begin(),block_scores.end(),[] (const pair<int,double>& l_val, const pair<int,double>& r_val) {
            return l_val.second < r_val.second;
        });

        for (auto pair : block_scores){
            block_list.emplace_back(pair.first);
        }

        return block_list;
    }

    void updateBlockNumAndWritesCount(int* i, int* writes_in_block, vector<int> blocks) const{
        while (*writes_in_block == PAGES_PER_BLOCK){
            (*i)++;
            *writes_in_block = ftl->getValidWritesInBlock(blocks[*i]);
        }
    }

    void assignWritesToBlocks(map<unsigned int,ListItem>* locations_list, vector<pair<unsigned int,int>>* res, vector<int> blocks, unsigned long long base_index){

        int i = 0;
        int writes_in_block = ftl->getValidWritesInBlock(blocks[i]);
        updateBlockNumAndWritesCount(&i,&writes_in_block,blocks);

        /* assign all invalid pages. i.e pages that will be overwritten within this window */
        auto next_block_indexes = getNextBlockIndexes(locations_list,PAGES_PER_BLOCK-writes_in_block);
        while(!next_block_indexes.empty()){
            for (unsigned int j = 0 ; j < next_block_indexes.size() ; j++){
                /* NOTE: loc represents the absolute location in the writing_sequence, but we want to access
                 * res in the location relative to the base index
                 */
                unsigned long long loc = locations_list->at(writing_sequence[next_block_indexes[j]]).getFirstLocationInList();
                res->at(loc-base_index).second = blocks[i]; // we need the first location in the list!
            }

            updateLocationsList(locations_list,next_block_indexes);

            writes_in_block += next_block_indexes.size();
            updateBlockNumAndWritesCount(&i,&writes_in_block,blocks);
            next_block_indexes = getNextBlockIndexes(locations_list,PAGES_PER_BLOCK-writes_in_block);
        }

        /* assign all local valid pages. i.e pages that will remain valid in the end of this window. In order to do
         * this we sort the pages by page score function and then assign to the blocks that remain in blocks vector.
         * by doing this we match each page to the best block corresponding with the pages score.
         */

        vector<long long> indexes_to_sort;
        for (unsigned int j = 0 ; j < res->size() ; j++) {
            if (res->at(j).second != TBD) {
                continue;
            } else {
                indexes_to_sort.emplace_back(j);
            }
        }
        sortIndexes(&indexes_to_sort);
        for (unsigned int j = 0 ; j < indexes_to_sort.size(); j++){
            updateBlockNumAndWritesCount(&i,&writes_in_block,blocks);
            res->at(indexes_to_sort[j]).second = blocks[i];
            writes_in_block++;
        }

    }

    static vector<long long> getNextBlockIndexes(map<unsigned int,ListItem>* locations_list, int number_of_indexes){
        vector<long long> locations_of_writes;
        for (auto & it : *locations_list) {
            long long loc = it.second.getPageLocation();
            if (loc != NOT_EXIST){
                locations_of_writes.push_back(loc);
            }
        }
        std::sort(locations_of_writes.begin(),locations_of_writes.end());
        long long offset = min(locations_of_writes.size(),number_of_indexes);
        vector<long long> res(offset);
        std::copy(locations_of_writes.begin(),locations_of_writes.begin()+offset,res.begin());
        return res;
    }

    void updateLocationsList(map<unsigned int,ListItem>* locations_list, const vector<long long>& indexes_to_remove) const{
        for (int i : indexes_to_remove){
            locations_list->at(writing_sequence[i]).updateLocationList();
        }
    }

    void sortIndexes(vector<long long>* indexes_to_sort) {
        std::sort(indexes_to_sort->begin(),indexes_to_sort->end(),[this] (int l_val, int r_val) {
            return pageScore(l_val) < pageScore(r_val);
        });
    }

    unsigned long long pageScore(unsigned long long page_index) const{
        unsigned int lpn = writing_sequence[page_index];
        return logical_pages_location_map->at(lpn).getFirstLocationAfterIndex(page_index);
    }

    void runGenerationalSimulation(int num_of_gens, long long window_size) {
        if (reach_steady_state){
            reachSteadyState();
        }

        for (int j = 0; j < num_of_gens; ++j) {
            Block* new_gen_block = nullptr;
            ftl->gen_blocks.insert({j, new_gen_block});
        }

        for (unsigned long long i = 0; i < window_size; ++i) {
            int generation = getGeneration(i, num_of_gens);
            ftl->writeGenerational(data, writing_sequence[i], generation, writing_sequence, i);
        }
//        for(std::list<Block*>::iterator it = ftl->freeList.begin(); it != ftl->freeList.end(); it++);
        for (unsigned long long i = window_size; i < NUMBER_OF_PAGES; i++) {
            ftl->write(data,writing_sequence[i],GREEDY, writing_sequence, i);
//            for(std::list<Block*>::iterator it = ftl->freeList.begin(); it != ftl->freeList.end(); it++)
//                cout<<"Y"<<endl;
        }
    }

    int getGeneration(unsigned long long page_index, int num_of_gens) const{
        unsigned long long page_score = pageScore(page_index) - page_index;
        int interval = (PAGES_PER_BLOCK*LOGICAL_BLOCK_NUMBER)/num_of_gens; //TODO: adjust this
        unsigned long long bound = interval;
        for (int i = 0; i < num_of_gens-1; ++i) {
            if(page_score < bound)
                return i;
            bound += bound;
        }
        return num_of_gens-1;
    }

};



#endif //FLASHGC_ALGORUNNER_H
