/*
//****************************************************************************80
//
//  Purpose:
//
//    MAIN is the main program for HEATED_PLATE.
//
//  Discussion:
//
//    This code solves the steady state heat equation on a rectangular region.
//
//    The sequential version of this program needs approximately
//    18/epsilon iterations to complete. 
//
//
//    The physical region, and the boundary conditions, are suggested
//    by this diagram;
//
//                   W = 0
//             +------------------+
//             |                  |
//    W = 100  |                  | W = 100
//             |                  |
//             +------------------+
//                   W = 100
//
//    The region is covered with a grid of M by N nodes, and an N by N
//    array W is used to record the temperature.  The correspondence between
//    array indices and locations in the region is suggested by giving the
//    indices of the four corners:
//
//                  I = 0
//          [0][0]-------------[0][N-1]
//             |                  |
//      J = 0  |                  |  J = N-1
//             |                  |
//        [M-1][0]-----------[M-1][N-1]
//                  I = M-1
//
//    The steady state solution to the discrete heat equation satisfies the
//    following condition at an interior grid point:
//
//      W[Central] = (1/4) * ( W[North] + W[South] + W[East] + W[West] )
//
//    where "Central" is the index of the grid point, "North" is the index
//    of its immediate neighbor to the "north", and so on.
//   
//    Given an approximate solution of the steady state heat equation, a
//    "better" solution is given by replacing each interior point by the
//    average of its 4 neighbors - in other words, by using the condition
//    as an ASSIGNMENT statement:
//
//      W[Central]  <=  (1/4) * ( W[North] + W[South] + W[East] + W[West] )
//
//    If this process is repeated often enough, the difference between successive 
//    estimates of the solution will go to zero.
//
//    This program carries out such an iteration, using a tolerance specified by
//    the user, and writes the final estimate of the solution to a file that can
//    be used for graphic processing.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    22 July 2008
//
//  Author:
//
//    Original C version by Michael Quinn.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Michael Quinn,
//    Parallel Programming in C with MPI and OpenMP,
//    McGraw-Hill, 2004,
//    ISBN13: 978-0071232654,
//    LC: QA76.73.C15.Q55.
//
//  Parameters:
//
//    Commandline argument 1, double EPSILON, the error tolerance.  
//
//    Commandline argument 2, char *OUTPUT_FILENAME, the name of the file into which
//    the steady state solution is written when the program has completed.
//
//  Local parameters:
//
//    Local, double DIFF, the norm of the change in the solution from one iteration
//    to the next.
//
//    Local, double MEAN, the average of the boundary values, used to initialize
//    the values of the solution in the interior.
//
//    Local, double U[M][N], the solution at the previous iteration.
//
//    Local, double W[M][N], the solution computed at the latest iteration.
//
//****************************************************************************80
*/
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <cmath>
# include <ctime>
# include <string>

using namespace std;
# define M 1000
# define N 1000

/*double ctime;
double ctime1;
double ctime2;*/
double diff;
double epsilon;
FILE *fp;
int i;
int iterations;
int iterations_print;
int j;
double mean;
ofstream output;
char output_filename[80];
int success;
double u[M][N];
double w[M][N];


void setBoundaryValue();
void setAverageBoundary();
void writeToFile();
void getHeat();
int main(int argc, char *argv[]);
double cpu_time();


void setBoundaryValue(){
for (i = 1; i < M - 1; i++)
	{
		w[i][0] = 100.0;
	}
	for (i = 1; i < M - 1; i++)
	{
		w[i][N - 1] = 100.0;
	}
	for (j = 0; j < N; j++)
	{
		w[M - 1][j] = 100.0;
	}
	for (j = 0; j < N; j++)
	{
		w[0][j] = 0.0;
	}
}
void setAverageBoundary(){
	mean = 0.0;
	for (i = 1; i < M - 1; i++)
	{
		mean = mean + w[i][0];
	}
	for (i = 1; i < M - 1; i++)
	{
		mean = mean + w[i][N - 1];
	}
	for (j = 0; j < N; j++)
	{
		mean = mean + w[M - 1][j];
	}
	for (j = 0; j < N; j++)
	{
		mean = mean + w[0][j];
	}
	mean = mean / (double)(2 * M + 2 * N - 4);
	// 
	//  Initialize the interior solution to the mean value.
	//
	for (i = 1; i < M - 1; i++)
	{
		for (j = 1; j < N - 1; j++)
		{
			w[i][j] = mean;
		}
	}
	
}
void writeToFile(){
	output.open(output_filename);

	output << M << "\n";
	output << N << "\n";

	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++)
		{
			output << "  " << w[i][j];
		}
		output << "\n";
	}
	output.close();

	cout << "\n";
	cout << "  Solution written to the output file \"" << output_filename << "?\"?\n";
}

void getHeat(){
	iterations = 0;
	iterations_print = 1;
	cout << "\n";
	cout << " Iteration  Change\n";
	cout << "\n";
	while (epsilon <= diff)
	{
		//
		//  Save the old solution in U.
		//
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < N; j++)
			{
				u[i][j] = w[i][j];
			}
		}
		//
		//  Determine the new estimate of the solution at the interior points.
		//  The new solution W is the average of north, south, east and west neighbors.
		//
		diff = 0.0;
		for (i = 1; i < M - 1; i++)
		{
			for (j = 1; j < N - 1; j++)
			{
				
				w[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4.0;

				if (diff < fabs(w[i][j] - u[i][j]))
				{
					diff = fabs(w[i][j] - u[i][j]);
				}
			}
		}
		iterations++;
		if (iterations == iterations_print)
		{
			cout << "  " << setw(8) << iterations
				<< "  " << diff << "\n";
			iterations_print = 2 * iterations_print;
		}
	}
}


int main(int argc, char *argv[]){
	cout << "\n";
	cout << "HEATED_PLATE\n";
	cout << "  C++ version\n";
	cout << "  A program to solve for the steady state temperature distribution\n";
	cout << "  over a rectangular plate.\n";
	cout << "\n";
	cout << "  Spatial grid of " << M << " by " << N << " points.\n";
	// 
	//  Read EPSILON from the command line or the user.
	//
	if (argc < 2)
	{
		cout << "\n";
		cout << "  Enter EPSILON, the error tolerance:\n";
		cin >> epsilon;
	}
	else
	{
		success = sscanf(argv[1], "%f", &epsilon);

		if (success != 1)
		{
			cout << "\n";
			cout << "HEATED_PLATE\n";
			cout << "  Error reading in the value of EPSILON.\n";
			return 1;
		}
	}

	cout << "\n";
	cout << "  The iteration will be repeated until the change is <= "
		<< epsilon << "\n";
	diff = epsilon;
	// 
	//  Read OUTPUT_FILE from the command line or the user.
	//
	if (argc < 3)
	{
		cout << "\n";
		cout << "  Enter OUTPUT_FILENAME, the name of the output file:\n";
		cin >> output_filename;
	}
	else
	{
		success = sscanf(argv[2], "%s", output_filename);

		if (success != 1)
		{
			cout << "\n";
			cout << "HEATED_PLATE\n";
			cout << "  Error reading in the value of OUTPUT_FILENAME.\n";
			return 1;
		}
	}

	cout << "\n";
	cout << "  The steady state solution will be written to \"?"
		<< output_filename << "\".\n";
	// 
	//  Set the boundary values, which don't change. 
	//
	setBoundaryValue();
	//
	//  Average the boundary values, to come up with a reasonable
	//  initial value for the interior.
	// 
	setAverageBoundary();
	//
	//  iterate until the  new solution W differs from the old solution U
	//  by no more than EPSILON.
	//
	//ctime1 = cpu_time();

	getHeat();

	//ctime2 = cpu_time();
	//ctime = ctime2 - ctime1;

	cout << "\n";
	cout << "  " << setw(8) << iterations
		<< "  " << diff << "\n";
	cout << "\n";
	cout << "  Error tolerance achieved.\n";
//	cout << "  CPU time = " << ctime << "\n";
	// 
	//  Write the solution to the output file.
	//
	writeToFile();
	// 
	//  Terminate.
	//
	cout << "\n";
	cout << "HEATED_PLATE:\n";
	cout << "  Normal end of execution.\n";

	return 0;

# undef M
# undef N
}
//****************************************************************************80

double cpu_time()

//****************************************************************************80
//
//  Purpose:
//
//    CPU_TIME returns the current reading on the CPU clock.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    06 June 2005
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double CPU_TIME, the current reading of the CPU clock, in seconds.
//
{
	double value;

	value = (double)clock() / (double)CLOCKS_PER_SEC;

	return value;
}