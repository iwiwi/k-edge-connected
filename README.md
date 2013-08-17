# MkECSs by Random Contraction

This software library efficiently decomposes a graph into maximal *k*-edge-connected subgraphs (MkECSs) by our new randomized algorithm based on *random contraction*.

Capturing sets of closely related vertices from large networks
is an essential task in many applications such as social network analysis, bioinformatics, and web link research.
The idea of using MkECS as a model of cohesive clusters
was recently proposed to overcome drawbacks of classic models such as *k*-core.

Please see [our paper](http://www-imai.is.s.u-tokyo.ac.jp/~takiba/papers/mkecs_cikm13.pdf) for the algorithm specification, theoretical analysis and experimental evaluation.


## Usage

### From CUI Interface

    $ make
    $ bin/mkecs 4 < sample.txt
    0 1 2 3 4
    5 6 7 8 9

### From Your Program

    vector<vector<int> > mkecs;
    mkecs::Decompose(edge_list, mkecs, K);
    cout << "Number of MkECSs: " << mkecs.size() << endl;

### Details

* MkECSs with size one are discarded.
* In a graph file, each line should contain two integers describing an edge (see `sample.txt`). Vertices should be described by integers starting from zero.
* Execute `make test` to run tests (*google-gtest* is required).


## References

* Takuya Akiba, Yoichi Iwata, and Yuichi Yoshida, **[Linear-Time Enumeration of Maximal k-Edge-Connected Subgraphs in Large Networks by Random Contraction](http://www-imai.is.s.u-tokyo.ac.jp/~takiba/papers/mkecs_cikm13.pdf)**. In *CIKM'13*.
