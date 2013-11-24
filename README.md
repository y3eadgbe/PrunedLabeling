Pruned Labeling for Reachability Queries
--------------

### What is this?
This is an implementation of Pruned Labeling methods for reachability queries on directed graphs.
A reachability query (s, t) asks if there is a path between two nodes s and t.
Our program quickly answers reachability queries on a given graph by precomputing an index.

### Usage
    $ make
    $ ./benchmark < sample.graph
An input (directed) graph file should be given in the following format:
* The first line contains #vertices and #edges separated by a space.
* The (i+1)-th line (0 <= i < |V|) contains indexes of vertices which are adjacent to the i-th vertex, separated by a space. (0-indexed)
* The input graph can contain cycles.

### References
* Yosuke Yano, Takuya Akiba, Yoichi Iwata, Yuichi Yoshida. [Fast and scalable reachability queries on graphs by pruned labeling with landmarks and paths](http://dl.acm.org/citation.cfm?doid=2505515.2505724). In CIKM 2013. (short paper)
* Takuya Akiba, Yoichi Iwata, Yuichi Yoshida. [Fast Exact Shortest-Path Distance Queries on Large Networks by Pruned Landmark Labeling](http://www-imai.is.s.u-tokyo.ac.jp/~takiba/papers/sigmod13_pll.pdf). In SIGMOD 2013.
