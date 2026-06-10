#include <stdio.h>
#define ROW 5
#define COL 5

int directions[4][2]={{-1,0},{1,0},{0,-1},{0,1}}; 
int maps[5][5]={{ 1, 2, 3, 4,5,},
                {16,17,18,19,6,},
                {15,24,25,20,7,},
                {14,23,22,21,8,},
                {13,12,11,10,9}};
int max_deepth[ROW][COL];
int next_i[ROW][COL];
int next_j[ROW][COL];

int dfs(int x,int y){
    if(max_deepth[x][y]>0){
        return max_deepth[x][y];
    }
    int max_len=1;
    int best_next_i=-1;
    int best_next_j=-1;
    for(int i=0;i<4;i++){
        int newx=x+directions[i][0];
        int newy=y+directions[i][1];
        if(newx>=0&&newx<ROW&&newy>=0&&newy<COL&&maps[newx][newy]<maps[x][y]){
            int len=dfs(newx,newy)+1;
            if(len>max_len){
                max_len=len;
                best_next_i=newx;
                best_next_j=newy;
            }
        }
    }
    max_deepth[x][y]=max_len;
    next_i[x][y]=best_next_i;
    next_j[x][y]=best_next_j;
    return max_len;
}

void print_path(int strat_x,int start_y){
    int curr_x=strat_x;
    int curr_y=start_y;
    printf("路线:");
    while(curr_x!=-1){
        printf("%d",maps[curr_x][curr_y]);
        if(next_i[curr_x][curr_y]!=-1){
            printf("->");
        }
        int temp_x=next_i[curr_x][curr_y];
        int temp_y=next_j[curr_x][curr_y];
        curr_x=temp_x;
        curr_y=temp_y;
    }
}
int main(){
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            max_deepth[i][j]=0;
            next_i[i][j]=-1;
            next_j[i][j]=-1;
        }
    }
    int max_len=0;
    int max_start_i=0;
    int max_start_j=0;
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            int len=dfs(i,j);
            if(len>max_len){
                max_len=len;
                max_start_i=i;
                max_start_j=j;
            }
        }
    }
    printf("最长滑道长度:%d\n",max_len);
    printf("起点:(%d,%d) 高度:%d\n",max_start_i,max_start_j,maps[max_start_i][max_start_j]);
    print_path(max_start_i,max_start_j);
    return 0;
}
