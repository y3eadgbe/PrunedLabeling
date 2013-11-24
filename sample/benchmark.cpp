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

std::vector<std::vector<int> > readGraph() {
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
    return G;
}

int main() {
    vector<vector<int> > G = readGraph();
    double startTime, endTime;

    cout << "Constructing Index ... " << flush;

    startTime = getTime();

    PrunedLabelingBase* rq = new RQPrunedPathLabeling(G);
    //PrunedLabelingBase* rq = new RQPrunedLandmarkLabeling(G);

    endTime = getTime();

    cout << "DONE" << endl;
    cout << "Indexing Time: " << endTime - startTime << " sec" << endl;
    cout<< "Index Size: " << rq->indexSize() << " byte" << endl;

    int num = 0;
    vector<int> ss, ts;
    for (int i = 0; i < numQueries; i++) {
        ss.push_back(rand() % G.size());
        ts.push_back(rand() % G.size());
    }

    cout << "Querying (" << numQueries << " times) ... " << flush;

    startTime = getTime();

    for (int i = 0; i < numQueries; i++) {
        if (rq->query(ss[i],ts[i])) {
            num++;
        }
    }
    
    endTime = getTime();

    cout << "DONE" << endl;
    cout << "Total Query Time (" << numQueries << " times): " << endTime - startTime << " sec" << endl;
    
    return 0;
}
