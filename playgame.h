typedef struct {
	int up, down, left, right;
} Point;
int addLine(Point arr[5][6], int row, int row2, int col, int col2);
int checkSquare(Point arr[5][6], int row, int row2, int col, int col2, int* p);
void printGrid(Point arr[5][6], char a[100]);
