#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <sys/time.h>

#include "../src/PrunedLabeling.h"

using namespace std;

const int numQueries = 1000000;

double getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.;
}

// returns (Adjacency List, (|V|, |E|))
pair<vector<vector<int> >, pair<int, int> > readGraph() {
    string line;
    int V, E;
    stringstream ss;
    getline(cin, line);
    ss << line;
    ss >> V >> E;

    vector<vector<int> > G(V);

    for (int i = 0; i < V; i++) {
        stringstream ss;
        int n;
        getline(cin, line);
        ss << line;

        while(ss >> n) {
            n--;
            G[i].push_back(n);
        }
    }
    return make_pair(G, make_pair(V, E));
}

int main() {
    pair<vector<vector<int> >, pair<int, int> > G = readGraph();
    vector<vector<int> > adj = G.first;
    int V = G.second.first, E = G.second.second;
    double startTime, endTime;

    cout << "|V| = " << V << ", |E| = " << E << endl << endl; 
    cout << "Constructing Index ... " << flush;

    startTime = getTime();

    PrunedLabelingBase* rq = new RQPrunedPathLabeling(adj);
    //PrunedLabelingBase* rq = new RQPrunedLandmarkLabeling(adj);

    endTime = getTime();

    cout << "DONE" << endl << endl;
    cout << "Indexing Time: " << endTime - startTime << " sec" << endl;
    cout<< "Index Size: " << rq->indexSize() << " byte" << endl << endl;

    vector<int> ss, ts;
    for (int i = 0; i < numQueries; i++) {
        ss.push_back(rand() % V);
        ts.push_back(rand() % V);
    }

    cout << "Querying (" << numQueries << " times) ... " << flush;

    startTime = getTime();

    int positiveQuery = 0;
    for (int i = 0; i < numQueries; i++) {
        if (rq->query(ss[i],ts[i])) {
            positiveQuery++;
        }
    }
    
    endTime = getTime();

    cout << "DONE" << endl << endl;
    cout << "Total Query Time (" << numQueries << " times): " << endTime - startTime << " sec" << endl;
    cout << "#PositiveQuery: " << positiveQuery << endl;
    
    return 0;
}
