/********************************************************
 * NMF.h: header file of NMF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/7
********************************************************/


/* Non-negative matrix factorization */
void NMF_core(double *removedData, int numUser, int numService, 
    int dim, double lmda, int maxIter, double *UData, double *SData);

/* Compute the loss value of NMF */
double loss(double **U, double **S, double **removedMatrix, double **predMatrix, 
	double lmda, int numUser, int numService, int dim);

/* Compute predMatrix */
void U_dot_S(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double **predMatrix);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr); 

/* Transpose a matrix */ 
void transpose(double **matrix, double **matrix_T, int row, int col);



