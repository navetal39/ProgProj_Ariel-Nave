


/**
 * Reads a series of seperated floating point numbers from user
 * and returns a pointer to SPPoint with matching coordinates with index of "index".
 * Additionally, incriments "index" by 1
 *
 * Given the dimension and a pointer to the wanted index for the point the functions returns a
 * new point P = (P_1,...,P_{dim-1}) such that:
 * - P_i = source_i (The ith coordinate the user typed)
 * - dim(P) = dim
 * - index(P) = *index
 *
 * @param dim - the number of coordinates
 * @assert (dim > 0)
 * @return
 * NULL if an error occures while reading or making the point
 * Others a pointer to a point
 */
SPPoint* readPoint (int dim, int* index);
/**
 * Reads a series of points from user as a series of seperated
 * floating point numbers for each points, and returns a pointer
 * to a dinamically allocated array containing pointers to the
 * created points.
 *
 * Given the ammount pf points, dimension of the points and the wanted index
 * for the 1st point, the functions returns a pointer to an array of points
 * [P(1),P(2),...,P(ammount)], such that:
 * - P(i)_j = source(i)_j (The jth coordinate the user typed in the ith row)
 * - dim(P(i)) = dim for every i
 * - index(P(1)) = *index, and the index(P(i)) = index(P(i-1))+1 for the rest of them.
 *
 * @param ammount - the ammount of points to be read
 * @param dim - the number of coordinates
 * @assert (dim > 0)
 * @return
 * NULL if an error occures while reading or making the point
 * Others a pointer to a point
 */
SPPoint** readPoints(int ammount, int dim, int* index);
find_KNN(SPPoint** points, SPPoint* target, int ammount, int dim, int k);
void cleanPointsArray(SPPoint** array, int arrayLength);