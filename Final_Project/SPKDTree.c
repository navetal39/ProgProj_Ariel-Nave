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

void spKDTreeDestroy(KDTree tree) {
    if (tree != NULL) {
        spPointDestroy(tree->data);
        spKDTreeDestroy(tree->left);
        spKDTreeDestroy(tree->right);
        free(tree);
    }
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
            return -1;
        }
        return split_dim;
    }
    // means we failed....
    return -1;
}



KDTree spKDTreeCreateRecursive(SPKDArray* array , SP_KDT_SPLIT what_method, int inc_index) {

    KDTree new_subtree;
    SPKDArray* left_array;
    SPKDArray* right_array;

    int error_flag = 0;
    int split_dim = -1;
    int median_index = -1;
    double median_value;

    new_subtree = (KDTree)malloc(sizeof(KDTreeNode));
    if (new_subtree == NULL) {
        printf("malloc fail :(\n");
        spKDArrayDestroy(array, array->num_points, array->dim);
        return NULL;
    }

    /// if the subtree is a leaf
    if (array->num_points == 1) {
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

    left_array = (SPKDArray*)malloc(sizeof(SPKDArray));
    if (left_array == NULL) {
        printf("malloc fail :(\n");
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }
    right_array = (SPKDArray*)malloc(sizeof(SPKDArray));
    if (right_array == NULL) {
        printf("malloc fail :(\n");
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        free(left_array);
        return NULL;
    }

    if ((error_flag = spKDArraySplit(array, left_array, right_array, split_dim)) != 0) {
        // in this case no need to free left_array and tight_array because split frees them...
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }

    new_subtree->left = spKDTreeCreateRecursive(left_array, what_method, split_dim);
    if ((new_subtree->left) == NULL) {
        printf("malloc fail :(\n");
        spKDArrayDestroy(array, array->num_points, array->dim);
        free(new_subtree);
        return NULL;
    }
    new_subtree->right = spKDTreeCreateRecursive(right_array, what_method, split_dim);
    if ((new_subtree->right) == NULL) {
        printf("malloc fail :(\n");
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


    if ((queue == NULL) || (query == NULL)) {
        return -1;
    }
    if (NULL == curr_node) {
        //todo error
        return -1;
    }

    // if curr_node is a leaf
    if (curr_node->dim == -1) {
        curr_node_dist = spPointL2SquaredDistance(curr_node->data , query);
        if (spBPQueueEnqueue(queue, spPointGetIndex(curr_node->data), curr_node_dist) == SP_BPQUEUE_INVALID_ARGUMENT) {
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
    BPQueueElement* tempElement;

    tempElement = (BPQueueElement*)malloc(sizeof(BPQueueElement));
    if (tempElement == NULL) {
        return -1;
    }

    queue = spBPQueueCreate(k);
    error_flag = spKDTreeKNNRecursive(queue, tree , query);
    if (error_flag == -1) {
        free(tempElement);
        ERASE_QUEUE(queue);
        return -1;
    }
    for (i = 0; i < k; i++) {
        if (spBPQueuePeek(queue, tempElement) != SP_BPQUEUE_SUCCESS) {
            for (j = i; j <= k; j++) {
                bestIndices[j] = -1;
            }
            ERASE_QUEUE(queue);
            free(tempElement);
            return -1;
        }
        bestIndices[i] = tempElement->index;
        spBPQueueDequeue(queue);
    }
    free(tempElement);
    ERASE_QUEUE(queue);
    bestIndices[k] = -1;
    return 0;
}


int main() {

    KDTree kdtree;
    SPPoint* query_point;
    double temp_data[3];
    int* int_arr;

    int_arr = (int*)malloc(sizeof(int) * 3);
    SPPoint* points_array[5];
    int i, j;
    temp_data[0] = 1;
    temp_data[1] = 1;
    temp_data[2] = 1;
    points_array[0] = spPointCreate(temp_data, 3, 0);
    temp_data[0] = 2;
    temp_data[1] = 50;
    temp_data[2] = 2;
    points_array[1] = spPointCreate(temp_data, 3, 1);
    temp_data[0] = 3;
    temp_data[1] = 3;
    temp_data[2] = 100;
    points_array[2] = spPointCreate(temp_data, 3, 2);
    temp_data[0] = 4;
    temp_data[1] = 4;
    temp_data[2] = 4;
    points_array[3] = spPointCreate(temp_data, 3, 3);
    temp_data[0] = 80;
    temp_data[1] = 100;
    temp_data[2] = 120;
    points_array[4] = spPointCreate(temp_data, 3, 4);
    for (i = 0; i < 5; ++i) {
        if (points_array[i] == NULL) {
            for (j = 0; j < 5; ++j) {
                spPointDestroy(points_array[j]);
            }
            return -1;
        }
    }

    kdtree = spKDTreeCreate(points_array, 5, SP_KD_SPLIT_MAX_SPREAD);
    for (i = 0; i < 5; ++i) {
        spPointDestroy(points_array[i]);
    }
    for (i = 0; i < 3; ++i) {
        temp_data[i] = 1;
    }
    query_point = spPointCreate(temp_data, 3, 1000000);
    if (query_point == NULL) {
        //TODO error?
        return true;
    }
    if (NULL == kdtree) {
        //TODO error?
        spPointDestroy(query_point);
        return true;
    }
    i = spKDTreeKNN(kdtree, query_point, 2, int_arr);
    printf("~~~~~~~~~~~~~~~~~hello : int_arr[0] = %d , int_arr[1] = %d , int_arr[2] = %d ", int_arr[0], int_arr[1], int_arr[2]);
    free(int_arr);
    spPointDestroy(query_point);
    spKDTreeDestroy(kdtree);
    return true;
}


























