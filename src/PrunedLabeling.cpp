/*
 The MIT License (MIT)

 Copyright (c) 2013 Yosuke Yano

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <stack>
#include "PrunedLabeling.h"

using namespace std;

/*******************
 * Utility functions
 *******************/

void getReversedGraph(vector<vector<int> >& G, vector<vector<int> >* rG) {
    *rG = vector<vector<int> >(G.size());
    for (int i = 0; i < (int)G.size(); i++) {
        for (int j = 0; j < (int)G[i].size(); j++) {
            (*rG)[G[i][j]].push_back(i);
        }
    }
}

void getSCCGraph(vector<vector<int> >& G, vector<vector<int> >* SCCGraph, vector<int>* table) {
    const int V = G.size();
    vector<vector<int> > rG;
    vector<vector<int> > SCCList;
    vector<int> postOrder;
    bool *used = new bool[V];
    int *nextIndex = new int[V];
    *table = vector<int>(V);
    getReversedGraph(G, &rG);

    // DFS    
    for (int i = 0; i < V; i++) {
        used[i] = false;
        nextIndex[i] = 0;
    }

    for (int i = 0; i < V; i++) {
        if (used[i]) {
            continue;
        }
        used[i] = true;
        stack<int> st;
        st.push(i);
        while (!st.empty()) {
            int v = st.top();
            if (nextIndex[v] == (int)G[v].size()) {
                postOrder.push_back(v);
                st.pop();
            } else {
                int newv = G[v][nextIndex[v]];
                if (!used[newv]) {
                    st.push(newv);
                    used[newv] = true;
                }
                nextIndex[v]++;
            }
        }
    }
    // Reversed DFS
    for (int i = 0; i < V; i++) {
        used[i] = false;
        nextIndex[i] = 0;
    }

    int it = 0;
    for (int i = V - 1; i >= 0; i--) {
        int index = postOrder[i];
        if (used[index]) {
            continue;
        }
        used[index] = true;
        (*table)[index] = it;
        stack<int> st;
        vector<int> list;
        st.push(index);
        list.push_back(index);
        while (!st.empty()) {
            int v = st.top();
            if (nextIndex[v] == (int)rG[v].size()) {
                st.pop();
            } else {
                int newv = rG[v][nextIndex[v]];
                if (!used[newv]) {
                    st.push(newv);
                    list.push_back(newv);
                    used[newv] = true;
                    (*table)[newv] = it;
                }
                nextIndex[v]++;
            }
        }
        SCCList.push_back(list);
        it++;
    }

    // creating new graph;
    const int compressedV = it;
    *SCCGraph = vector<vector<int> >(compressedV);
        
    vector<int> edgeUsed(compressedV, -1);
    for (int i = 0; i < compressedV; i++) {
        for (int j = 0; j < (int)SCCList[i].size(); j++) {
            int v = SCCList[i][j];
            for (int k = 0; k < (int)G[v].size(); k++) {
                int w = G[v][k];
                // v -> w
                if (edgeUsed[(*table)[w]] != i) {
                    edgeUsed[(*table)[w]] = i;
                    if (i != (*table)[w]) (*SCCGraph)[i].push_back((*table)[w]);
                }
            }
        }
    }
    
    delete[] used;
    delete[] nextIndex;
}

void getMultipliedDegreeValue(vector<vector<int> >& G, vector<vector<int> >& rG, vector<long long>* value) {
    const int V = G.size();
    *value = vector<long long>(V);
    
    for (int i = 0; i < V; i++) {
        (*value)[i] = (long long)(G[i].size() + 1) * (long long)(rG[i].size() + 1);
    }
}

void getMultipliedDegreeOrder(vector<vector<int> >& G, vector<vector<int> >& rG, vector<int>* order) {
    const int V = G.size();
    vector<pair<long long, int> > o(V); // (indeg. * outdeg., vertexNumber)
    vector<long long> val;
    
    *order = vector<int>(V);
    
    getMultipliedDegreeValue(G, rG, &val);
    
    for (int i = 0; i < V; i++) {
        o[i] = make_pair(val[i], i);
    }
    sort(o.begin(), o.end(), greater<pair<long long, int> >());
    
    for (int i = 0; i < V; i++) {
        (*order)[i] = o[i].second;
    }
}

/**************************
 * RQPrunedLandmarkLabeling
 **************************/

RQPrunedLandmarkLabeling::RQPrunedLandmarkLabeling() {
    RQPrunedLandmarkLabeling(vector<vector<int> >());
}

RQPrunedLandmarkLabeling::RQPrunedLandmarkLabeling(vector<vector<int> > G) {
    createIndex(G);
}

RQPrunedLandmarkLabeling::~RQPrunedLandmarkLabeling() {
}

void RQPrunedLandmarkLabeling::createIndex(vector<vector<int> >& G) {
    getSCCGraph(G, &(this->G), &(this->convTable));
    getReversedGraph(this->G, &(this->rG));
    this->V = this->G.size();
    this->reachTo = vector<vector<int> >(this->V);
    this->reachFrom = vector<vector<int> >(this->V);
    vector<bool> visited(this->V);

    vector<int> order;
    getMultipliedDegreeOrder(this->G, this->rG, &order);
    
    vector<int> q(this->V);
    int qs, qt;
    for (int i = 0; i < this->V; i++) {
        const int start = order[i];

        // BFS
        qs = qt = 0;
        q[qt++] = start;
        while(qs != qt) {
            int v = q[qs++];

            if (this->compressedQuery(start, v)) continue; //prune!
            this->reachFrom[v].push_back(i);
            
            for (int j = 0; j < (int)this->G[v].size(); j++) {
                if (!visited[this->G[v][j]]) {
                    q[qt++] = this->G[v][j];
                    visited[this->G[v][j]] = true;
                }
            }
        }

        for (int j = 0; j < qt; j++) {
            visited[q[j]] = false;
        }

        // reversed BFS
        qs = qt = 0;
        q[qt++] = start;
        while(qs != qt) {
            int v = q[qs++];

            if (this->compressedQuery(v, start)) continue; //prune!
            this->reachTo[v].push_back(i);
            
            for (int j = 0; j < (int)this->rG[v].size(); j++) {
                if (!visited[this->rG[v][j]]) {
                    q[qt++] = this->rG[v][j];
                    visited[this->rG[v][j]] = true;
                }
            }
        }

        for (int j = 0; j < qt; j++) {
            visited[q[j]] = false;
        }
        visited[start] = true;
    }
}

inline bool RQPrunedLandmarkLabeling::compressedQuery(int s, int t) {
    if (s > t) return false;

    const int sn = this->reachTo[s].size(), tn = this->reachFrom[t].size();
    int si = 0, ti = 0;
    
    while(si < sn && ti < tn) {
        const int sp = reachTo[s][si], tp = reachFrom[t][ti];
        if (sp == tp) {
            return true;
        }
        if (sp <= tp) {
            si++;
        } else {
            ti++;
        }
    }
    return false;
}

bool RQPrunedLandmarkLabeling::query(int s, int t) {
    const int source = this->convTable[s - 1], target = this->convTable[t - 1];
    return this->compressedQuery(source, target);
}

long long RQPrunedLandmarkLabeling::indexSize() {
    long long size = 0;
    for (int i = 0; i < this->V; i++) {
        size += reachTo[i].size();
        size += reachFrom[i].size();
    }
    return size * 4;
}

/**************************
 * RQPrunedPathLabeling
 **************************/

RQPrunedPathLabeling::RQPrunedPathLabeling() {
    RQPrunedPathLabeling(vector<vector<int> >());
}

RQPrunedPathLabeling::RQPrunedPathLabeling(vector<vector<int> > G) {
    createIndex(G);
}

RQPrunedPathLabeling::~RQPrunedPathLabeling() {
}

void RQPrunedPathLabeling::getOptimalPath(vector<bool>& used, vector<int>* path) {
    vector<long long> dp(this->V);
    vector<long long> value;
    *path = vector<int>();
    
    getMultipliedDegreeValue(this->G, this->rG, &value);

    for (int i = 0; i < this->V; i++) {
        dp[i] = 0;
    }

    int maxi = -1;
    long long maxValue = -(this->V + 1);
    for (int i = 0; i < this->V; i++) {
        for (int j = 0; j < (int)this->rG[i].size(); j++) {
            dp[i] = max(dp[i], dp[this->rG[i][j]]);
        }
        if (used[i]) {
            value[i] = 0;
        }

        dp[i] += value[i];

        if (maxValue < dp[i]) {
            maxi = i;
            maxValue = dp[i];
        }
    }
 
    if (maxi == -1)return;

    // back tracing
    int v = maxi;
    path->push_back(v);
    
    while(dp[v] != value[v]) {
        long long nextVal = dp[v] - value[v];
        for (int i = 0; i < (int)this->rG[v].size() && path->size() < (1 << 16); i++) {
            if (dp[this->rG[v][i]] == nextVal) {
                v = this->rG[v][i];
                path->push_back(v);
                break;
            }
        }
        if (path->size() == (1 << 16) - 1) break;
    }

    reverse(path->begin(), path->end());
}

void RQPrunedPathLabeling::createIndex(vector<vector<int> >& G) {
    int optimalPathNumber = 50;
    const int minimumPathLength = 10;
    getSCCGraph(G, &(this->G), &(this->convTable));
    getReversedGraph(this->G, &(this->rG));
    this->V = this->G.size();
    this->reachToPath = vector<vector<pair<unsigned short, int> > >(this->V);
    this->reachFromPath = vector<vector<pair<unsigned short, int> > >(this->V);
    this->reachTo = vector<vector<int> >(this->V);
    this->reachFrom = vector<vector<int> >(this->V);
    vector<bool> visited(this->V);
    vector<bool> used(this->V);

    vector<int> order;
    getMultipliedDegreeOrder(this->G, this->rG, &order);    

    for (int i = 0; i < this->V; i++) {
        visited[i] = false;
        used[i] = false;
    }

    vector<int> path;
    vector<int> visitedVertex(this->V);
    int visitedNum;

    int vertexIndex = 0;
    int pathNum = 0;

    for (int i = 0; i < this->V; i++) {
        if (optimalPathNumber) {
            this->getOptimalPath(used, &path);
            optimalPathNumber--;
            if ((int)path.size() < minimumPathLength) {
                optimalPathNumber = 0;
                while(vertexIndex < this->V && used[order[vertexIndex]]) {
                    vertexIndex++;
                }
                if (vertexIndex == this->V)break;
                path = vector<int>(1, order[vertexIndex]);
            }
            if (path.size() == 0)continue;
        } else {
            while(vertexIndex < this->V && used[order[vertexIndex]]) {
                vertexIndex++;
            }
            if (vertexIndex == this->V)break;
            path = vector<int>(1, order[vertexIndex]);
        }
        
        int pathSize = path.size();
        queue<int> q;
        
        // BFS
        visitedNum = 0;
        for (int j = pathSize - 1; j >= 0; j--) {
            q.push(path[j]);

            while(!q.empty()) {
                int v = q.front();
                q.pop();
                if (visited[v]) continue;
                visited[v] = true;
                visitedVertex[visitedNum] = v;
                visitedNum++;

                if (used[v]) continue; //prune!
                if (this->compressedQuery(path[j], v)) continue; //prune!
                if (pathSize > 1) {
                    this->reachFromPath[v].push_back(make_pair(pathNum, j));
                } else {
                    this->reachFrom[v].push_back(pathNum);
                }

                for (int k = 0; k < (int) this->G[v].size(); k++) {
                    q.push(this->G[v][k]);
                }
            }
        }
        for (int j = 0; j < visitedNum; j++) {
            visited[visitedVertex[j]] = false;
        }

        // reversed BFS
        visitedNum = 0;        
        for (int j = 0; j < (int)pathSize; j++) {
            q.push(path[j]);

            while(!q.empty()) {
                int v = q.front();
                q.pop();
                if (visited[v]) continue;
                visited[v] = true;
                visitedVertex[visitedNum] = v;
                visitedNum++;

                if (used[v]) continue;//prune!
                if (this->compressedQuery(v, path[j])) continue; //prune!
                if (pathSize > 1) {
                    this->reachToPath[v].push_back(make_pair(pathNum, j));
                } else {
                    this->reachTo[v].push_back(pathNum);
                }

                for (int k = 0; k < (int) this->rG[v].size(); k++) {
                    q.push(this->rG[v][k]);
                }
            }
            used[path[j]] = true;
        }
        for (int j = 0; j < visitedNum; j++) {
            visited[visitedVertex[j]] = false;
        }
        
        pathNum++;
    }
}

bool RQPrunedPathLabeling::compressedQuery(int s, int t) {
    if (s > t) return false;

    const int snp = this->reachToPath[s].size(), tnp = this->reachFromPath[t].size();
    const int sn = this->reachTo[s].size(), tn = this->reachFrom[t].size();
    int si = 0, ti = 0;
    //Path
    while(si < snp && ti < tnp) {
        const int sp = reachToPath[s][si].first, tp = reachFromPath[t][ti].first;
        const int sv = reachToPath[s][si].second, tv = reachFromPath[t][ti].second;
        if (sp == tp && sv <= tv) {
            return true;
        }
        if (sp <= tp) {
            si++;
        } else {
            ti++;
        }
    }

    si = 0, ti = 0;
    //Vertex
    while(si < sn && ti < tn) {
        const int sp = reachTo[s][si], tp = reachFrom[t][ti];
        if (sp == tp) {
            return true;
        }
        if (sp <= tp) {
            si++;
        } else {
            ti++;
        }
    }
    return false;
}

bool RQPrunedPathLabeling::query(int s, int t) {
    const int source = this->convTable[s - 1], target = this->convTable[t - 1];
    return this->compressedQuery(source, target);
}

long long RQPrunedPathLabeling::indexSize() {
    long long size = 0;
    for (int i = 0; i < this->V; i++) {
        size += reachToPath[i].size() * 6;
        size += reachFromPath[i].size() * 6;
        size += reachTo[i].size() * 4;
        size += reachFrom[i].size() * 4;
    }
    return size;
}
