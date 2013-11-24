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

#pragma once
#include <vector>
#include <utility>

// Base Class
class PrunedLabelingBase {
public:
    virtual ~PrunedLabelingBase(){};
    virtual void createIndex(std::vector<std::vector<int> >&) = 0;
    virtual bool query(int, int) = 0;
    virtual long long indexSize() = 0;
};

// Pruned Landmark Labeling
class RQPrunedLandmarkLabeling : public PrunedLabelingBase {
private:
    std::vector<std::vector<int> > G, rG;
    long long V;
    std::vector<std::vector<int> > reachTo, reachFrom;
    std::vector<int> convTable;
    bool compressedQuery(int, int);

public:
    RQPrunedLandmarkLabeling();
    RQPrunedLandmarkLabeling(std::vector<std::vector<int> >);
    virtual ~RQPrunedLandmarkLabeling();
    virtual void createIndex(std::vector<std::vector<int> >&);
    virtual bool query(int, int);
    long long indexSize();
};

// Pruned Path Labeling
class RQPrunedPathLabeling : public PrunedLabelingBase {
private:
    std::vector<std::vector<int> > G, rG;
    long long V;
    std::vector<std::vector<std::pair<unsigned short, int> > > reachToPath, reachFromPath;
    std::vector<std::vector<int> > reachTo, reachFrom;
    std::vector<int> convTable;
    bool compressedQuery(int, int);
    void getOptimalPath(std::vector<bool>&, std::vector<int>*);

public:
    RQPrunedPathLabeling();
    RQPrunedPathLabeling(std::vector<std::vector<int> >);
    virtual ~RQPrunedPathLabeling();
    virtual void createIndex(std::vector<std::vector<int> >&);
    virtual bool query(int, int);
    long long indexSize();
};

