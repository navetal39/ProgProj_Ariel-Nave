


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
 * @param index - a pointer to the wanted index for the point
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
 * @param index - a pointer to the wanted index for the first point to be made
 * @assert (dim > 0)
 * @return
 * NULL if an error occures while reading or making the point
 * Others a pointer to a point
 */
SPPoint** readPoints(int ammount, int dim, int* index);
/**
 * Finds the indexes of the k nearest points from "points" to "target", with all of
 * them being points of dimension dim and len(points)=ammount. 
 *
 * Given an array of pointers to the points, a pointer to the target point, the
 * ammount pf points, dimension of the points, wanted ammount of points to be found
 * the functions returns an array of indexes (integers) [i_1,...,i_n]
 * [P(1),P(2),...,P(ammount)], such that:
 * - for every two indexes i and j, such that i is in [i_1,...,i_n] and j isn't, 
 * 	 the point with index i is closer to the target point than the point with index j
 *
 * @param points - an array of pointers to points form which the k nearest will be found
 * @param target - a pointer to the point from which the distances will be calculated
 * @param ammount - the ammount of points to be read
 * @param dim - the number of coordinates
 * @param k - the ammount of points to be found
 * @return
 * an array of K integers
 */
find_KNN(SPPoint** points, SPPoint* target, int ammount, int dim, int k);
void cleanPointsArray(SPPoint** array, int arrayLength);