typedef struct {
	int up, down, left, right;
} Point;
void addLine(Point arr[5][6], int row, int row2, int col, int col2);
int checkSquare(Point arr[5][6], int row, int row2, int col, int col2);
void printGrid(Point arr[5][6], char a[100]);
