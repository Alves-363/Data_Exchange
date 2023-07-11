// des加密算法
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<openssl/md5.h>
using namespace std;
 
int main()
{ //声明变量
	char MingWen[104]; //存放原始的明文
	char target[8]; //将明文断成8个字符的一个分组
	char InputKey[10]; //存放字符型的八位密钥
	int text[64]; //存放一个分组转成二进制后的数据
	int text_ip[64]; //存放第一次初始换位的结果
	int L0[32], Li[32]; //将64位分成左右各32位进行迭代
	int R0[32], Ri[32];
	int RE0[48]; //存放右半部分经过E表扩展换位后的48位数据
	int key[64]; //存放密钥的二进制形式
	int keyPC1[56]; //存放密钥key经过PC1换位表后变成的56位二进制
	int A[28]; //将keyPC1分成左右两部分，左部A，右部B，各28位，以便进行循环左移
	int B[28];
	int keyAB[56]; //将循环左移后两部分的结果合并起来
	int K[16][48]; //存放16次循环左移产生的子密钥
	int RK[48]; //存放RE和K异或运算后的结果
	int RKS[8]; //存放经过查找8个S表后得到的8个十进制结果
	int SP[32]; //将RKS表中的十进制数化成二进制
	int RKSP[32]; //存放SP表经过P盒换位后的结果
	int text_end[64]; //存放经过左右32位换位后的结果
	int text_out[14][64]; //存放初始化向量和所有经过DES的分组的二进制
	char init[9] = { "HTmadeit" }; //设置初始化向量为“HTmadeit”
	int CBC[64];
	int result[13][64];
	int H[208];
	char MiWen[208];
	int C[832];
	int M[13][8];
 
	char choice;
	int t;
	int i, j;
	int k, l, m, n;
	int r[8], c[8];
	int flag = 1;
 
	int IP[64] = { //初始换位表
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17, 9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7
	};
 
	int E[48] = { //扩展换位表
		32, 1, 2, 3, 4, 5,
		4, 5, 6, 7, 8, 9,
		8, 9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32, 1 };
 
	int PC1[56] = { //PC1换位表（64—>56）
		57, 49, 41, 33, 25, 17, 9,
		1, 58, 50, 42, 34, 26, 18,
		10, 2, 59, 51, 43, 35, 27,
		19, 11, 3, 60, 52, 44, 36,
		63, 55, 47, 39, 31, 23, 15,
		7, 62, 54, 46, 38, 30, 22,
		14, 6, 61, 53, 45, 37, 29,
		21, 13, 5, 28, 20, 12, 4 };
 
	int move[16] = { //循环移位表
		1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
 
	int PC2[48] = { //PC2换位表（56—>48）
		14, 17, 11, 24, 1, 5,
		3, 28, 15, 6, 21, 10,
		23, 19, 12, 4, 26, 8,
		16, 7, 27, 20, 13, 2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32 };
 
	int S1[4][16] = { //S换位表
		14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
	};
	int S2[4][16] = {
		15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
		3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
		0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
		13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
	};
	int S3[4][16] = {
		10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
		13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
		13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
		1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
	};
	int S4[4][16] = {
		7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
		13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
		10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
		3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
	};
	int S5[4][16] = {
		2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
		14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
		4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
		11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
	};
	int S6[4][16] = {
		12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
		10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
		9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
		4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
	};
	int S7[4][16] = {
		4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
		13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
		1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
		6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
	};
	int S8[4][16] = {
		13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
		1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
		7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
		2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
	};
	int P[32] = { //P换位表
		16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
		2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25
	};
 
	printf("HYs-des加密/解密\n\n");
	while (flag)
	{
		printf("A加密，B解密，请选择：\n");
		scanf("%c", &choice);
 
		while (choice != 'A' && choice != 'B' && choice != 'a' && choice != 'b')
		{
			printf("对不起，您的输入不合法。请选择A或B，A表示加密，B表示解密。\n");
			scanf("%c", &choice);
		}
		getchar();
 
		//生成子密钥
		cout<<"请输入8位密钥："<<endl;

		fgets(InputKey,10,stdin);
		// 检验输入内容
		// cout<<InputKey<<endl;
		// cout<<InputKey[0]<<","<<InputKey[1]<<","<<InputKey[7]<<","<<InputKey[8]<<endl;

		while ( InputKey[7] == '\n' || InputKey[8] != '\n') 
		{
			printf("您输入的密钥位数有误，请重新输入8位密钥：");
			fgets(InputKey,10,stdin);
		}
 
		for (i = 0; i < 8; i++) //将密钥转化成64位二进制数放到一维数组key中
		{
			int a[8] = { 0,0,0,0,0,0,0,0 };
			m = InputKey[i];
			for (j = 0; m != 0; j++)
			{
				a[j] = m % 2;
				m = m / 2;
			}
			for (j = 0; j < 8; j++)
				key[(i * 8) + j] = a[7 - j];
		}
 
		//for(i=0;i<64;i++)
		//printf("%d,",key[i]);
 
		for (i = 0; i < 56; i++) //通过PC1换位表变成56位密钥放在keyPC1中
			keyPC1[i] = key[PC1[i] - 1];
 
		for (i = 0; i < 28; i++) //分成A和B两部分，各28位
		{
			A[i] = keyPC1[i];
			B[i] = keyPC1[i + 28];
		}
 
		for (t = 0; t < 16; t++)
		{
			if (move[t] == 1) //按照循环移位表将Ai和Bi分别左移move[t]位
			{
				n = A[0];
				for (i = 0; i < 27; i++)
					A[i] = A[i + 1];
				A[27] = n;
				n = B[0];
				for (i = 0; i < 28; i++)
					B[i] = B[i + 1];
				B[27] = n;
			}
			else
			{
				n = A[0];
				m = A[1];
				for (i = 0; i < 26; i++)
					A[i] = A[i + 2];
				A[26] = n;
				A[27] = m;
				n = B[0];
				m = B[1];
				for (i = 0; i < 26; i++)
					B[i] = B[i + 2];
				B[26] = n;
				B[27] = m;
			}
 
			for (i = 0; i < 28; i++) //将A和B合并成56位
			{
				keyAB[i] = A[i];
				keyAB[i + 28] = B[i];
			}
 
			for (i = 0; i < 48; i++) //通过PC2换位表变成48位密钥
				K[t][i] = keyAB[PC2[i] - 1];
		}
 
		//for(t=0;t<16;t++) 
		//for(i=0;i<48;i++)
		// printf("%d,",K[t][i]);
 
		for (i = 0; i < 8; i++) //将初始化向量转化成二进制数储存到数组text_out的第一行中
		{
			int a[8] = { 0,0,0,0,0,0,0,0 };
			m = init[i];
			for (j = 0; m != 0; j++)
			{
				a[j] = m % 2;
				m = m / 2;
			}
			for (j = 0; j < 8; j++)
				text_out[0][(i * 8) + j] = a[7 - j];
		}
 
 
		//加密程序
 
		if (choice == 'A' || choice == 'a')
		{
			printf("请输入您想加密的内容："); //输入明文
			fgets(MingWen,104,stdin);
			cout<<MingWen<<endl;
			while (MingWen[0] == '\0')
			{
				printf("对不起，明文不可为空，请您输入正确的明文。\n");
				fgets(MingWen,104,stdin);
			}
 
			//CBC模式下的加密
			i = 0; //将明文每8个字符作为一个分组，共有n个分组
			n = 0;
			while (MingWen[i] != '\0')
			{
				n++;
				i++;
			}
			k = n % 8;
			n = (n - 1) / 8 + 1;
 
			for (l = 0; l < n; l++)
			{
				if (l == (n - 1) && k != 0)
				{
					for (i = 0; i < k; i++) //将每个分组的8个字符放到数组target中，不够的用空格补充
						target[i] = MingWen[i + (8 * l)];
					for (i = k; i < 8; i++)
						target[i] = ' ';
				}
				else
					for (i = 0; i < 8; i++)
						target[i] = MingWen[i + (8 * l)];
 
				for (i = 0; i < 8; i++) //将得到的明文转化成二进制数储存到数组text中
				{
					int a[8] = { 0,0,0,0,0,0,0,0 };
					m = target[i];
					for (j = 0; m != 0; j++)
					{
						a[j] = m % 2;
						m = m / 2;
					}
					for (j = 0; j < 8; j++)
						text[(i * 8) + j] = a[7 - j];
				}
 
				//for(i=0;i<64;i++)
				//printf("%d,",text[i]);
				//printf("\n");
 
				//for(i=0;i<64;i++)
				//printf("%d,",text_out[l][i]);
				//printf("\n");
 
				for (i = 0; i < 64; i++) //CBC模式下前一分组的密文异或当前分组
					text[i] = text_out[l][i] ^ text[i];
 
				//for(i=0;i<64;i++)
				//printf("%d,",text[i]);
				//printf("\n");
 
				//对每个text进行DES加密
 
				for (i = 0; i < 64; i++) //进行初始换位
					text_ip[i] = text[IP[i] - 1];
 
				for (i = 0; i < 32; i++) //分成左右两部分，各32位
				{
					L0[i] = text_ip[i];
					R0[i] = text_ip[i + 32];
				}
 
				//for(i=0;i<32;i++)
				// printf("%d,",L0[i]);
				//for(i=0;i<32;i++)
				// printf("%d,",R0[i]);
 
 
				//十六次迭代
 
				for (t = 0; t < 16; t++)
				{
					for (i = 0; i < 48; i++) //将右半部分通过扩展换位表E从32位扩展成48位
						RE0[i] = R0[E[i] - 1];
 
					//printf("RE0\n");
					//for(i=0;i<48;i++)
					//printf("%d,",RE0[i]);
 
					for (i = 0; i < 48; i++) //RE与K异或运算
						RK[i] = RE0[i] ^ K[t][i];
 
 
					//printf("\n");
					//for(i=0;i<48;i++)
					//printf("%d,",RK[i]);
 
					for (i = 0; i < 8; i++) //将R和K异或运算的结果通过S位移表
					{
						r[i] = RK[(i * 6) + 0] * 2 + RK[(i * 6) + 5];
						c[i] = RK[(i * 6) + 1] * 8 + RK[(i * 6) + 2] * 4 + RK[(i * 6) + 3] * 2 + RK[(i * 6) + 4];
					}
					RKS[0] = S1[r[0]][c[0]];
					RKS[1] = S2[r[1]][c[1]];
					RKS[2] = S3[r[2]][c[2]];
					RKS[3] = S4[r[3]][c[3]];
					RKS[4] = S5[r[4]][c[4]];
					RKS[5] = S6[r[5]][c[5]];
					RKS[6] = S7[r[6]][c[6]];
					RKS[7] = S8[r[7]][c[7]];
 
					for (i = 0; i < 8; i++) //把结果转成32位二进制储存在数组SP中
					{
						int b[4] = { 0,0,0,0 };
						m = RKS[i];
						for (j = 3; m != 0; j--)
						{
							b[j] = m % 2;
							m = m / 2;
						}
						for (j = 0; j < 4; j++)
							SP[j + (i * 4)] = b[j];
					}
 
					for (i = 0; i < 32; i++) //将二进制结果再经过一个P盒换位
						RKSP[i] = SP[P[i] - 1];
 
					for (i = 0; i < 32; i++) //与前一次的左部异或运算，得到本次迭代的右部
						Ri[i] = L0[i] ^ RKSP[i];
 
					for (i = 0; i < 32; i++)
					{
						L0[i] = R0[i];
						R0[i] = Ri[i];
					}
				}
 
				//一个左右32位交换
 
				for (i = 0; i < 32; i++)
					Li[i] = R0[i];
				for (i = 0; i < 32; i++)
					R0[i] = L0[i];
				for (i = 0; i < 32; i++)
					L0[i] = Li[i];
 
 
				//初始换位的逆过程
 
				for (i = 0; i < 32; i++) //把左右两部分合起来存到text_end中
					text_end[i] = L0[i];
				for (i = 32; i < 64; i++)
					text_end[i] = R0[i - 32];
 
				for (i = 0; i < 64; i++) //进行初始换位的逆过程
					text_out[l + 1][IP[i] - 1] = text_end[i];
 
				for (i = 0; i < 64; i++)
					result[l][i] = text_out[l + 1][i];
 
				//for(i=0;i<64;i++)
				//printf("%d,",result[l][i]);
				//printf("\n");
			}
 
			for (j = 0; j < n; j++) //把result中的二进制密文转成十进制存到数组H中
				for (i = 0; i < 16; i++)
					H[i + (j * 16)] = result[j][0 + (i * 4)] * 8 + result[j][1 + (i * 4)] * 4 + result[j][2 + (i * 4)] * 2 + result[j][3 + (i * 4)];
 
			//for(i=0;i<l*16;i++)
			//printf("%d,",H[i]);
 
			for (i = 0; i < n * 16; i++)
			{
				if (H[i] < 10)
					MiWen[i] = H[i] + 48;
				else if (H[i] == 10)
					MiWen[i] = 'A';
				else if (H[i] == 11)
					MiWen[i] = 'B';
				else if (H[i] == 12)
					MiWen[i] = 'C';
				else if (H[i] == 13)
					MiWen[i] = 'D';
				else if (H[i] == 14)
					MiWen[i] = 'E';
				else if (H[i] == 15)
					MiWen[i] = 'F';
				//else MiWen[i]='\0';
			}
			for (i = l * 16; i < 208; i++)
				MiWen[i] = '\0';//注意数组越界
 
			printf("您的明文经过DES加密后的密文是：\n");
			printf("%s\n", MiWen);
			printf("\n\n");
		}
 
 
		//解密程序
		else if (choice == 'B' || choice == 'b')
		{
			printf("请输入密文内容：\n");
			fgets(MiWen,208,stdin);

			n = 0;
			do
			{
				if (n % 16 != 0)
				{
					printf("对不起，您输入的密文不正确，请确认密文的内容，密文的字符数应是16的倍数。\n");
					printf("请输入密文内容：\n");
					fgets(MiWen,208,stdin);
				}

				// for (i = 0; i < 208; i++)
				H[208] = {0};
				cout<<H<<endl;
				for (i = 0; MiWen[i] != '\0'; i++) //将十六进制密文转化成十进制存放在数组H中
				{
					if (MiWen[i] >= '0' && MiWen[i] <= '9')
						H[i] = MiWen[i] - '0';
					else if (MiWen[i] >= 'A' && MiWen[i] <= 'F')
						H[i] = MiWen[i] - 'A' + 10;
					else if (MiWen[i] >= 'a' && MiWen[i] <= 'f')
						H[i] = MiWen[i] - 'a' + 10;
					else if (MiWen[i] == '\n')
						break;
					else
					{
						printf("注意:请输入用十六进制表示的密文内容：\n");
						fgets(MiWen,208,stdin);
						i = 0;
					}
				}

				n = i; //密文中共有n个字符
			}
			while (n%16 != 0);



 
			for (i = 0; i < n; i++) //将十进制密文转化成二进制存放在数组C中
			{
				int he[4] = { 0,0,0,0 };
				for (j = 3; H[i] != 0; j--)
				{
					he[j] = H[i] % 2;
					H[i] = H[i] / 2;
				}
				for (j = 0; j < 4; j++)
					C[j + (i * 4)] = he[j];
			}
 
			//for(i=0;i<130;i++)
			// printf("%d,",C[i]);
			//printf("\n");
 
			k = n / 16;
			for (l = 0; l < k; l++)
			{
				for (i = 0; i < 64; i++) //将每个分组对应的64位二进制密文放到text_out中
					text_out[l + 1][i] = C[i + (l * 64)];
 
				//for(i=0;i<64;i++)
				// printf("%d,",text_out[l][i]);
				//printf("\n");
 
				//对每个text进行DES解密
 
				for (i = 0; i < 64; i++) //进行初始换位
					text_ip[i] = text_out[l + 1][IP[i] - 1];
 
				//for(i=0;i<64;i++)
				//printf("%d,",text_ip[i]);
				//printf("\n");
 
				for (i = 0; i < 32; i++) //分成左右两部分，各32位
				{
					L0[i] = text_ip[i];
					R0[i] = text_ip[i + 32];
				}
				//for(i=0;i<32;i++)
				// printf("%d,",L0[i]);
				//for(i=0;i<32;i++)
				// printf("%d,",R0[i]);
 
 
				//十六次迭代
 
				for (t = 0; t < 16; t++)
				{
					for (i = 0; i < 48; i++) //将右半部分通过扩展换位表E从32位扩展成48位
						RE0[i] = R0[E[i] - 1];
 
					//printf("RE0\n");
					//for(i=0;i<48;i++)
					//printf("%d,",RE0[i]);
 
					for (i = 0; i < 48; i++) //RE与K异或运算
						RK[i] = RE0[i] ^ K[15 - t][i];
 
					//printf("\n");
					//for(i=0;i<48;i++)
					//printf("%d,",RK[i]);
 
					for (i = 0; i < 8; i++) //将R和K异或运算的结果通过S位移表
					{
						r[i] = RK[(i * 6) + 0] * 2 + RK[(i * 6) + 5];
						c[i] = RK[(i * 6) + 1] * 8 + RK[(i * 6) + 2] * 4 + RK[(i * 6) + 3] * 2 + RK[(i * 6) + 4];
					}
 
					RKS[0] = S1[r[0]][c[0]];
					RKS[1] = S2[r[1]][c[1]];
					RKS[2] = S3[r[2]][c[2]];
					RKS[3] = S4[r[3]][c[3]];
					RKS[4] = S5[r[4]][c[4]];
					RKS[5] = S6[r[5]][c[5]];
					RKS[6] = S7[r[6]][c[6]];
					RKS[7] = S8[r[7]][c[7]];
 
					for (i = 0; i < 8; i++) //把结果转成32位二进制储存在数组SP中
					{
						int b[4] = { 0,0,0,0 };
						m = RKS[i];
						for (j = 3; m != 0; j--)
						{
							b[j] = m % 2;
							m = m / 2;
						}
						for (j = 0; j < 4; j++)
							SP[j + (i * 4)] = b[j];
					}
 
					for (i = 0; i < 32; i++) //将二进制结果再经过一个P盒换位
						RKSP[i] = SP[P[i] - 1];
 
					for (i = 0; i < 32; i++) //与前一次的左部异或运算，得到本次迭代的右部
						Ri[i] = L0[i] ^ RKSP[i];
 
					for (i = 0; i < 32; i++)
					{
						L0[i] = R0[i];
						R0[i] = Ri[i];
					}
				}
 
				//一个左右32位交换
 
				for (i = 0; i < 32; i++)
					Li[i] = R0[i];
				for (i = 0; i < 32; i++)
					R0[i] = L0[i];
				for (i = 0; i < 32; i++)
					L0[i] = Li[i];
 
				//初始换位的逆过程
 
				for (i = 0; i < 32; i++) //把左右两部分合起来存到text_end中
					text_end[i] = L0[i];
				for (i = 32; i < 64; i++)
					text_end[i] = R0[i - 32];
 
				for (i = 0; i < 64; i++) //进行初始换位的逆过程 
					text[IP[i] - 1] = text_end[i];
 
 
				//CBC模式下的解密
 
				for (i = 0; i < 64; i++) //前一分组的密文异或当前分组所得明文的二进制放到result中
					result[l][i] = text_out[l][i] ^ text[i];
 
			}
 
			for (i = 0; i < (n / 16); i++) //将二进制转成十进制
				for (j = 0; j < 8; j++)
					M[i][j] = result[i][(j * 8) + 0] * 128 + result[i][(j * 8) + 1] * 64 + result[i][(j * 8) + 2] * 32 + result[i][(j * 8) + 3] * 16 + result[i][(j * 8) + 4] * 8 + result[i][(j * 8) + 5] * 4 + result[i][(j * 8) + 6] * 2 + result[i][(j * 8) + 7];
 
			printf("您的密文经过DES解密后的明文是：\n");
			for (i = 0; i < (n / 16); i++)
				for (j = 0; j < 8; j++)
					printf("%c", M[i][j]);
			printf("\n\n\n");
		}
		flag = 0;
		printf("是否继续？\n");
		printf("Y继续，N退出，请选择：\n");
		scanf("%c", &choice);
 
		while (choice != 'Y' && choice != 'N' && choice != 'y' && choice != 'n')
		{
			printf("对不起，您的输入不合法。请选择Y或N，Y表示继续使用本程序，N表示退出。\n");
			scanf("%c", &choice);
		}
		getchar();
		if (choice == 'Y' || choice == 'y')
			flag = 1;
	}

	return 0;
}
