#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

#include "SPKDTree.h"

#define ERASE_QUEUE(queue) \
do{ \
    while(!spBPQueueIsEmpty((queue))) \
    { \
        spBPQueueDequeue((queue)); \
    } \
    spBPQueueDestroy((queue)); \
} while(0);

int spKDTreeGetDim(KDTree tree) {
    if (tree != NULL) {
        return tree->dim;
    }
    return -1;
}

double spKDTreeGetValue(KDTree tree) {
    if (tree != NULL) {
        return tree->val;
    }
    return -1.0;
}

KDTree spKDTreeGetLeft(KDTree tree) {
    if (tree != NULL) {
        return tree->left;
    }
    return NULL;
}

KDTree spKDTreeGetRight(KDTree tree) {
    if (tree != NULL) {
        return tree->right;
    }
    return NULL;
}

SPPoint* spKDTreeGetData(KDTree tree) {
    if (tree != NULL) {
        return tree->data;
    }
    return NULL;
}

void spKDTreeDestroy(KDTree tree) {
    if (tree != NULL) {
        spPointDestroy(tree->data);
        spKDTreeDestroy(tree->left);
        spKDTreeDestroy(tree->right);
        free(tree);
    }
    /* No need to print warning into log since it's normal to send NULL to this method (leaves) */
}

int spKDTreeGetSplitDimension(SPKDArray* array, SP_KDT_SPLIT what_method, int inc_index) {
    int split_dim;
    int i;
    double tmp__max , tmp__min, tmp__curr;

    if (what_method  == SP_KD_SPLIT_INCREMENTAL) {
        split_dim = (inc_index + 1) % array->dim;
        return split_dim;
    }
    else if (what_method  == SP_KD_SPLIT_RANDOM) {
        srand(time(NULL));
        split_dim =  rand() % array->dim;
        return split_dim;
    }
    tmp__curr = -1;
    if (what_method  == SP_KD_SPLIT_MAX_SPREAD) {
        for (i = 0; i < array->dim; ++i) {
            tmp__min = spPointGetAxisCoor((array->points_array[array->sort_mat[i][0]]), i);
            tmp__max = spPointGetAxisCoor((array->points_array[array->sort_mat[i][array->num_points - 1]]), i);

            if (tmp__curr < tmp__max - tmp__min) {
                tmp__curr = tmp__max - tmp__min;
                split_dim = i;
            }
        }
        if (tmp__curr == -1) {
            spLoggerPrintError(LOG_ERR_MSG_EMPTY_ARR, __FILE__,__func__,__LINE__);
            return -1;
        }
        return split_dim;
    }
    spLoggerPrintError(LOG_ERR_MSG_NO_METH, __FILE__,__func__,__LINE__);
    return -1;
}

int temp_tomer = 0;

KDTree spKDTreeCreateRecursive(SPKDArray* array , SP_KDT_SPLIT what_method, int inc_index) {

    KDTree new_subtree;
    SPKDArray* left_array;
    SPKDArray* right_array;
    int error_flag = 0, split_dim = -1, median_index = 0;
    double median_value;

    new_subtree = (KDTree)malloc(sizeof(KDTreeNode));
    if (new_subtree == NULL) {
        spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
        spKDArrayDestroy(array, array->num_points, array->dim);
        return NULL;
    }
    if ((array->num_points) == 1) { // if the subtree is a leaf
        new_subtree->dim = -1;
        new_subtree->val = -1;
        new_subtree->left = NULL;
        new_subtree->right = NULL;
        new_subtree->data = spPointCopy(array->points_array[0]);
        spKDArrayDestroy(array, array->num_points, array->dim);
        return new_subtree;
    }
     if ((split_dim = spKDTreeGetSplitDimension(array, what_method, inc_index)) == -1) {
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    } 
    median_index = array->sort_mat[split_dim][(array->num_points - 1) / 2];
    median_value = spPointGetAxisCoor(array->points_array[median_index], split_dim);
    new_subtree->dim = split_dim;
    new_subtree->val = median_value;
    new_subtree->data = NULL;

    left_array = (SPKDArray*)malloc(sizeof(SPKDArray));
    if (left_array == NULL) {
        spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }
    right_array = (SPKDArray*)malloc(sizeof(SPKDArray));
    if (right_array == NULL) {
        spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        free(left_array);
        return NULL;
    }
    if ((error_flag = spKDArraySplit(array, left_array, right_array, split_dim)) != 0) {
        // in this case no need to free left_array and right_array because split frees them...
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }
    new_subtree->left = spKDTreeCreateRecursive(left_array, what_method, split_dim);
    if ((new_subtree->left) == NULL) {
        spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }
    new_subtree->right = spKDTreeCreateRecursive(right_array, what_method, split_dim);
    if ((new_subtree->right) == NULL) {
        spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
        spKDArrayDestroy(array, array->num_points, array->dim);
        spKDTreeDestroy(new_subtree->left);
        free(new_subtree);
        return NULL;
    }
    spKDArrayDestroy(array, array->num_points, array->dim);
    return new_subtree;
}

KDTree spKDTreeCreate(SPPoint** points, int length,  SP_KDT_SPLIT what_method) {

    SPKDArray* array;
    array = spKDArrayCreate(points, length);
    if (array == NULL) {
        return NULL;
    }
    return spKDTreeCreateRecursive(array, what_method, -1);
}


// change query name
int spKDTreeKNNRecursive(SPBPQueue* queue, KDTree curr_node, SPPoint* query) {
    double curr_node_dist;
    int flag_other_subtree; // 0 means the recurtion went to left subtree , and 1 means to right subtree
    double query_relevant_dim_value;


    if ((queue == NULL) || (query == NULL) || (curr_node)==NULL) {
        spLoggerPrintError(LOG_ERR_MSG_BAD_ARGS, __FILE__,__func__,__LINE__);
        return -1;
    }
    // if curr_node is a leaf
    if (curr_node->dim == -1) {
        curr_node_dist = spPointL2SquaredDistance(curr_node->data , query);
        if (spBPQueueEnqueue(queue, spPointGetIndex(curr_node->data), curr_node_dist) == SP_BPQUEUE_INVALID_ARGUMENT) {
            spLoggerPrintError(LOG_ERR_MSG_BAD_ARGS, __FILE__,__func__,__LINE__);
            return -1;
        }
        return 0;
    }

    // means curr_node is not a leaf
    if (spPointGetAxisCoor(query, curr_node->dim) <= curr_node->val) {
        if (spKDTreeKNNRecursive( queue, curr_node->left, query) == -1) {
            return -1;
        }
        flag_other_subtree = 0;
    }
    else {
        if (spKDTreeKNNRecursive(queue, curr_node->right, query) == -1) {
            return -1;
        }
        flag_other_subtree = 1;
    }
    // Now maybe we should go on and search in the second subtree also...
    query_relevant_dim_value = spPointGetAxisCoor(query, curr_node->dim);
    if ((curr_node->val - query_relevant_dim_value) * (curr_node->val - query_relevant_dim_value) < spBPQueueMaxValue(queue)
            || (!spBPQueueIsFull(queue))) {
        if (flag_other_subtree) {
            if (spKDTreeKNNRecursive(queue, curr_node->left,  query) == -1)
                return -1;
        }
        else {
            if (spKDTreeKNNRecursive(queue, curr_node->right, query) == -1)
                return -1;
        }
    }
    return 0;
}

// need to free outside the function because it doesnt free the int array

int spKDTreeKNN(KDTree tree, SPPoint* query, int k, int* bestIndices) {

    SPBPQueue* queue;
    int i, j, error_flag;
    BPQueueElement tempElement;

    queue = spBPQueueCreate(k);
    error_flag = spKDTreeKNNRecursive(queue, tree , query);
    if (error_flag == -1) {
        ERASE_QUEUE(queue);
        return -1;
    }
    for (i = 0; i < k; i++) {
        if (spBPQueuePeek(queue, &tempElement) != SP_BPQUEUE_SUCCESS) {
            spLoggerPrintError(LOG_ERR_MSG_BAD_ARGS, __FILE__,__func__,__LINE__);
            for (j = i; j <= k; j++) {
                bestIndices[j] = -1;
            }
            ERASE_QUEUE(queue);
            return -1;
        }
        bestIndices[i] = tempElement.index;
        spBPQueueDequeue(queue);
    }
    ERASE_QUEUE(queue);
    bestIndices[k] = -1;
    return 0;
}



























