#include "SPPoint.h"
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"

/**
 * kd_tree summary
 *
 * the kd_tree contains inforamtion of points
 * the leaves in the tree represents points and contains their data
 * the internal nodes are for splitting the tree by certain value
 *
 *
 * The following functions are supported:
 *
 * spKDTreeDestroy               - frees all memory connected to a given kd_tree
 * spKDTreeGetSplitDimension        - get the splitting dimension according to the splitting method
 * spKDTreeCreateRecursive        - creates new kd_tree recursively from a given kd_array
 * spKDTreeCreate                - creates new kd_tree from given array of (pointers to) points 
 * spKDTreeKNNRecursive                  - filling a given queue with the closest points in a tree to a given query
 * KNN                      - filling integer array with the indexes of the k closest to query
 *
 */

/** the struct used for kd_tree node**/
typedef struct kd_tree_node_t {
    int dim;
    double val;
    struct kd_tree_node_t* left;
    struct kd_tree_node_t* right;
    SPPoint* data;
} KDTreeNode;

/** type used to define a kd_tree **/
typedef KDTreeNode* KDTree;

/** type for splitting method of a kd_tree **/
typedef enum sp_kdt_split_method {
	SP_KD_SPLIT_RANDOM,
	SP_KD_SPLIT_MAX_SPREAD,
	SP_KD_SPLIT_INCREMENTAL,
	SP_KD_SPLIT_UNKNOWN
} SP_KDT_SPLIT;
/**
 * Free all memory related to a kd_array
 * @param tree  - the kd_tree to destroy
 	if an object need to be freed is already NULL - nothing happens...
 */
void spKDTreeDestroy(KDTree tree);

/**
 * get the new splitting dimension accordin to the splitting method and the previous splitting dimension
 * @param array        - a (pointer to) kd_array - needed for checking MAX_SPREAD dimension in case : method = MAX_SPREAD
 * 		  what method  - the splitting method (3 cases : INCREMENTAL/RANDOM/MAX_SPREAD)
 * 		  inc_index    - the previous splitting dimension - needed only in case: method = INCREMENTAL
 * @return
 * an integer:
 * positive value - means success, and this is the asked dimension
 * -1             - means fail
 */
int spKDTreeGetSplitDimension(SPKDArray* array, KDTREE_WHAT_METHOD what_method, int inc_index);

/**
 * creates the tree recursively from a given kd_array (splitting the kd_array and calling the itself for left and for right)
 * @param array        - a (pointer to) kd_array - contains all the needed information for the kd_tree (/subtree/leaf)
 * 		  what method  - the splitting method (3 cases : INCREMENTAL/RANDOM/MAX_SPREAD)
 * 		  inc_index    - the previous splitting dimension - needed only in case: method = INCREMENTAL
 * @return
 * a kd_tree - the new kd_tree that was created and initialized
 * in case of failure - returns NULL
 */
KDTree spKDTreeCreateRecursive(SPKDArray* array , KDTREE_WHAT_METHOD what_method, int inc_index);

/**
 * creates the tree given the data - practicly - creating kd_array from the data and calling to KDTree_Create_Rec
 * @param points        - an array of (pointer to) points - which will be in the leaves of the new kd_tree
 * 		  length        - the number of (pointers to) points in the given array
 * 		  what method  - the splitting method (3 cases : INCREMENTAL/RANDOM/MAX_SPREAD)
 * @return
 * a kd_tree - the new kd_tree that was created and initialized
 * in case of failure - returns NULL
 */
KDTree spKDTreeCreate(SPPoint** points, int length,  KDTREE_WHAT_METHOD what_method)

/**
 * filling a given queue with the closest points to a query point in a given kd_tree
 * @param queue        - bounded priority queuem - bounded by k (which is a parameter to the no recursive KNN)
 * 		  curr_node    - the kd_tree in which we are searching in
 * 		  query        - the query point
 * @return
 * an integer:
 * 0  - means success
 * -1 - means fail
 */
int spKDTreeKNNRecursive(SPBPQueue* queue, KDTree curr_node, SPPoint* query);

/**
 * finding the k closest points to a query point in a given kd_tree and returns their indexes in -1 terminated array
 * @param tree         - the kd_tree in which we are searching in
 * 		  query        - the query point
 		  k            - number of points to return (k closest)
 		  bestIndices  - allocated integer array (allocated in size (K+1)*(sizeof(int)))
 * @return
 * an integer:
 * 0  - means success
 * -1 - means fail
 */
int spKDTreeKNN(KDTree tree, SPPoint* query, int k, int* bestIndices);





















