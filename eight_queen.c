#include <stdio.h>
#include <stdlib.h>
#define N 8
int	queen[N];
int count=0;
int is_safe(int row,int col){
	for(int i=0;i<row;i++){
		if(queen[i]==col) return 0;
		if(abs(i-row)==abs(queen[i]-col)) return 0;
	}
	return 1;
}
void print_queen(){
	printf("方案%d\n",count);
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			if(queen[i]==j){
				printf("Q");
			}
			else{
				printf("+");
			}
		}
		printf("\n");
	}
}
void solve(int row){
	if(row==N){
		count++;
		print_queen();
		return;
	}
	for(int col=0;col<N;col++){
		if(is_safe(row,col)){
			queen[row]=col;
			solve(row+1);
		}
	}
}
int main(){
	printf("---八皇后问题---\n");
	solve(0);
	printf("方案总数:%d",count);
	return 0;
}