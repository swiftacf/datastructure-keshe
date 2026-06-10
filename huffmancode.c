#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_CODE_LEN 100
#define MAX_LINE 1000

// 哈夫曼树节点结构
typedef struct {
    char data;
    int freq;
    int parent, lchild, rchild;
} HTNode, *HuffmanTree;

// 哈夫曼编码表结构
typedef struct {
    char data;
    char code[MAX_CODE_LEN];
} CodeItem, *HuffmanCode;

HuffmanTree huffman_tree = NULL;
HuffmanCode huffman_code = NULL;
int node_cnt = 0;

// 函数声明
void init();
void encode();
void decode();
void print_codefile();
void print_tree_cmd();
void build_huffman_tree();
void generate_huffman_code();
void save_tree_to_file();
void load_tree_from_file();
void free_memory();

// ========== 主菜单 ==========
int main() {
    printf("=== 哈夫曼编码/解码系统 ===\n");
    printf("命令说明:\n");
    printf(" I - 初始化 (构建哈夫曼树)\n");
    printf(" C - 编码 (将 tobetrans.dat 编码为二进制串)\n");
    printf(" D - 解码 (将 codefile 解码回文本)\n");
    printf(" P - 打印编码文件 (终端显示及 codeprint 文件)\n");
    printf(" T - 打印哈夫曼树 (终端显示及 treeprint 文件)\n");
    printf(" Q - 退出程序\n");

    char cmd;
    while (1) {
        printf("\n请输入命令: ");
        if (scanf(" %c", &cmd) != 1) {
            break;
        }
        switch (cmd) {
            case 'I': case 'i': init(); break;
            case 'C': case 'c': encode(); break;
            case 'D': case 'd': decode(); break;
            case 'P': case 'p': print_codefile(); break;
            case 'T': case 't': print_tree_cmd(); break;
            case 'Q': case 'q': free_memory(); return 0;
            default: printf("无效命令，请重新输入。\n");
        }
    }
    return 0;
}

// ========== 初始化 ==========
void init() {
    free_memory(); // 重置内存

    printf("请输入字符集大小 n: ");
    if (scanf("%d", &node_cnt) != 1) return;

    printf("请输入有频率的字符数 m (通常 m=n): ");
    int m;
    if (scanf("%d", &m) != 1) return;

    // 分配内存 (2n-1 个节点)
    huffman_tree = (HuffmanTree)malloc(sizeof(HTNode) * (2 * node_cnt));
    huffman_code = (HuffmanCode)malloc(sizeof(CodeItem) * node_cnt);

    // 初始化叶子节点
    for (int i = 1; i <= node_cnt; i++) {
        huffman_tree[i].parent = huffman_tree[i].lchild = huffman_tree[i].rchild = 0;
        printf("请输入第 %d 个字符及其频率 (例如 A 10): ", i);
        scanf(" %c %d", &huffman_tree[i].data, &huffman_tree[i].freq);
        huffman_code[i-1].data = huffman_tree[i].data;
    }

    build_huffman_tree();
    generate_huffman_code();
    save_tree_to_file();

    printf("初始化完成，哈夫曼树已构建并保存至 hfmtree 文件。\n");
}

// 构建树
void build_huffman_tree() {
    if (node_cnt <= 1) return; // 单节点特殊情况处理留给逻辑判断

    for (int i = node_cnt + 1; i <= 2 * node_cnt - 1; i++) {
        huffman_tree[i].parent = huffman_tree[i].lchild = huffman_tree[i].rchild = 0;
        huffman_tree[i].freq = 99999; // 哨兵值
    }

    // 选择两个最小权值的节点
    for (int i = node_cnt + 1; i <= 2 * node_cnt - 1; i++) {
        int min1 = -1, min2 = -1;
        for (int j = 1; j <= i - 1; j++) {
            if (huffman_tree[j].parent == 0) {
                if (min1 == -1 || huffman_tree[j].freq < huffman_tree[min1].freq) {
                    min2 = min1;
                    min1 = j;
                } else if (min2 == -1 || huffman_tree[j].freq < huffman_tree[min2].freq) {
                    min2 = j;
                }
            }
        }
        huffman_tree[min1].parent = i;
        huffman_tree[min2].parent = i;
        huffman_tree[i].lchild = min1;
        huffman_tree[i].rchild = min2;
        huffman_tree[i].freq = huffman_tree[min1].freq + huffman_tree[min2].freq;
    }
}

// 生成编码
void generate_huffman_code() {
    char temp_code[MAX_CODE_LEN];
    for (int i = 1; i <= node_cnt; i++) {
        int start = 0;
        // 特殊情况：如果只有一个节点
        if (node_cnt == 1) {
            huffman_code[i-1].code[0] = '0';
            huffman_code[i-1].code[1] = '\0';
            continue;
        }

        // 从叶子到根逆向求编码
        for (int c = i, p = huffman_tree[i].parent; p != 0; c = p, p = huffman_tree[p].parent) {
            if (huffman_tree[p].lchild == c) {
                temp_code[start++] = '0';
            } else {
                temp_code[start++] = '1';
            }
        }
        temp_code[start] = '\0';

        // 逆序存入 code 数组
        int len = strlen(temp_code);
        for (int j = 0; j < len; j++) {
            huffman_code[i-1].code[j] = temp_code[len - 1 - j];
        }
        huffman_code[i-1].code[len] = '\0';
    }
}

// 保存树结构到文件 (用于解码)
void save_tree_to_file() {
    FILE *fp = fopen("hfmtree", "w");
    if (!fp) { printf("错误：无法创建 hfmtree 文件。\n"); return; }
    // 保存节点数和树结构
    fprintf(fp, "%d\n", node_cnt);
    for (int i = 1; i <= 2 * node_cnt - 1; i++) {
        fprintf(fp, "%c %d %d %d %d\n",
                huffman_tree[i].data,
                huffman_tree[i].freq,
                huffman_tree[i].parent,
                huffman_tree[i].lchild,
                huffman_tree[i].rchild);
    }
    fclose(fp);
}

// 从文件加载树结构
void load_tree_from_file() {
    FILE *fp = fopen("hfmtree", "r");
    if (!fp) {
        printf("错误：未找到 hfmtree 文件，请先执行初始化(I)。\n");
        exit(1);
    }
    free_memory();
    fscanf(fp, "%d", &node_cnt);
    huffman_tree = (HuffmanTree)malloc(sizeof(HTNode) * (2 * node_cnt));
    huffman_code = (HuffmanCode)malloc(sizeof(CodeItem) * node_cnt);

    for (int i = 1; i <= 2 * node_cnt - 1; i++) {
        fscanf(fp, "%c %d %d %d %d",
               &huffman_tree[i].data, // 注意：这里读取字符可能会遇到换行符问题，但在保存时是一一对应的
               &huffman_tree[i].freq,
               &huffman_tree[i].parent,
               &huffman_tree[i].lchild,
               &huffman_tree[i].rchild);
        // 修正 scanf 读取字符时的换行符问题
        if (huffman_tree[i].data == '\n') {
            i--; // 重新读取这一行
            continue;
        }
        if (i <= node_cnt) {
            huffman_code[i-1].data = huffman_tree[i].data;
        }
    }
    fclose(fp);
}

// ========== 编码 ==========
void encode() {
    if (!huffman_tree) load_tree_from_file();

    FILE *fin = fopen("tobetrans.dat", "r");
    FILE *fout = fopen("codefile", "w");
    if (!fin) { printf("错误：未找到 tobetrans.dat 文件。\n"); return; }
    if (!fout) { printf("错误：无法创建 codefile 文件。\n"); fclose(fin); return; }

    char buffer[MAX_LINE];
    // 读取文件内容
    while (fgets(buffer, MAX_LINE, fin)) {
        for (int i = 0; buffer[i] != '\0'; i++) {
            // 查找字符对应的编码
            for (int j = 0; j < node_cnt; j++) {
                if (huffman_code[j].data == buffer[i]) {
                    fprintf(fout, "%s", huffman_code[j].code);
                    break;
                }
            }
        }
    }

    // --- 修复代码开始 ---
    // 如果是单字符文件，编码全是0，解码会死循环。
    // 这里添加一个换行符作为解码结束标记。
    if (node_cnt == 1) {
        fprintf(fout, "\n");
    }
    // --- 修复代码结束 ---

    fclose(fin);
    fclose(fout);
    printf("编码完成，结果已保存至 codefile。\n");
}

// ========== 解码 ==========
void decode() {
    if (!huffman_tree) load_tree_from_file();

    FILE *fin = fopen("codefile", "r");
    FILE *fout = fopen("textfile", "w");
    if (!fin) { printf("错误：未找到 codefile 文件。\n"); return; }
    if (!fout) { printf("错误：无法创建 textfile 文件。\n"); fclose(fin); return; }

    int root = 2 * node_cnt - 1; // 根节点下标
    int curr = root;
    char bit;

    while ((bit = fgetc(fin)) != EOF) {
        if (bit == '\n') break; // 遇到换行符结束（用于单字符情况的修复）
        if (bit == '0') {
            curr = huffman_tree[curr].lchild;
        } else if (bit == '1') {
            curr = huffman_tree[curr].rchild;
        }

        // 如果是叶子节点
        if (huffman_tree[curr].lchild == 0 && huffman_tree[curr].rchild == 0) {
            fputc(huffman_tree[curr].data, fout);
            curr = root; // 回到根节点
        }
    }

    fclose(fin);
    fclose(fout);
    printf("解码完成，结果已保存至 textfile。\n");
}

// ========== 打印编码文件 ==========
void print_codefile() {
    FILE *fin = fopen("codefile", "r");
    FILE *fout = fopen("codeprint", "w");
    if (!fin) { printf("错误：未找到 codefile 文件。\n"); return; }
    if (!fout) { printf("错误：无法创建 codeprint 文件。\n"); fclose(fin); return; }

    printf("\n--- codefile 内容 (每行50字符) ---\n");
    char c;
    int count = 0;
    while ((c = fgetc(fin)) != EOF) {
        putchar(c);
        fputc(c, fout);
        count++;
        if (count % 50 == 0) {
            putchar('\n');
            fputc('\n', fout);
        }
    }
    printf("\n------------------------------------\n");
    fclose(fin);
    fclose(fout);
}

// ========== 打印哈夫曼树 ==========
// 辅助递归函数
void print_tree_recursive(int node_idx, int depth, FILE *fout) {
    if (node_idx == 0) return;

    // 打印右子树
    print_tree_recursive(huffman_tree[node_idx].rchild, depth + 1, fout);

    // 打印当前节点
    for (int i = 0; i < depth; i++) printf("    ");
    for (int i = 0; i < depth; i++) fprintf(fout, "    ");

    if (huffman_tree[node_idx].lchild == 0 && huffman_tree[node_idx].rchild == 0) {
        // 叶子节点
        printf("%c(%d)\n", huffman_tree[node_idx].data, huffman_tree[node_idx].freq);
        fprintf(fout, "%c(%d)\n", huffman_tree[node_idx].data, huffman_tree[node_idx].freq);
    } else {
        // 非叶子节点
        printf("<internal>(%d)\n", huffman_tree[node_idx].freq);
        fprintf(fout, "<internal>(%d)\n", huffman_tree[node_idx].freq);
    }

    // 打印左子树
    print_tree_recursive(huffman_tree[node_idx].lchild, depth + 1, fout);
}

void print_tree_cmd() {
    if (!huffman_tree) load_tree_from_file();

    FILE *fout = fopen("treeprint", "w");
    if (!fout) { printf("错误：无法创建 treeprint 文件。\n"); return; }

    printf("\n--- 哈夫曼树结构 (横向显示) ---\n");
    int root = 2 * node_cnt - 1;
    print_tree_recursive(root, 0, fout);
    printf("------------------------------------\n");

    fclose(fout);
}

// ========== 内存释放 ==========
void free_memory() {
    if (huffman_tree) free(huffman_tree);
    if (huffman_code) free(huffman_code);
    huffman_tree = NULL;
    huffman_code = NULL;
    node_cnt = 0;
}