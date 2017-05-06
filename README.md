# dynamic_kcore
k-core decomposition in dynamic graphs.

The core number is a fundamental index reflecting the cohesiveness of a graph, which are widely used in large-scale graph analytics. The core maintenance problem requires to update the core numbers of vertices after a set of edges and vertices are inserted into or deleted from the graph. We investigate the parallelism in the core update process when multiple edges and vertices are inserted or deleted. Specifically, we discover a structure called superior edge set, the insertion or deletion of edges in which can be processed in parallel. 

# How To Use

make

./mykcore -p(parallel)/-c(centralized) graph_filename edge_filename

# example

./mykcore -p AP AP_1K 
//run our parallel algorithm, the new core numbers are saved in AP_core_del.txt and AP_core_ins.txt for deletion and insertion case respectively, the time cost is saved in AP_time_p.txt

./mykcore -c AP AP_1K 
//run the centralized algorithm, the time cost is saved in AP_time_c.txt
