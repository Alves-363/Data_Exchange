//base16
#include<iostream>
using namespace std;
//加密映射表
static const char BASE16_ENC_TAB[] = "0123456789ABCDEF";
//解密映射表 0~9 > 48~57  A`F > 65~70
static const char BASE16_DEC_TAB[128] = {
	-1,								    //0
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,      //1~10
	- 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,     //11~20
	- 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,     //21~30
	- 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,     //31~40
	- 1,-1,-1,-1,-1,-1,-1,0, 1, 2,      //41~50
	 3, 4, 5, 6, 7, 8, 9, -1,-1,-1,     //51~60
	- 1,-1,-1,-1, 10, 11, 12, 13, 14, 15 ,    //61~70
};

//加密函数encode
int encode(unsigned const char* in, int size, char* out)
{
	//分别取出一个字符的高4位和低4位
	for (int i = 0; i < size; i++)
	{
		char high = in[i] >> 4;		//右移四位来取得高4位
		char low = in[i] & 0x0f;	//与ffff并得到低4位
		//将高4位和低4位对应的字符放入out中
		out[i * 2] = BASE16_ENC_TAB[high];
		out[i * 2 + 1] = BASE16_ENC_TAB[low];
	}
	//加密后，扩大为两倍；
	return size * 2;
}
//解密函数decode
int decode(const string &in, unsigned char* out) 
{
	if (in.size() % 2 != 0)
	{
		cout << "解密数据格式错误" << endl;
		return -1;
	}
	for (int i = 0; i < in.size(); i += 2)
	{
		unsigned const char ch = in[i];
		unsigned const char cl = in[i+1];
		//转换成原来的值
		unsigned char h = BASE16_DEC_TAB[ch];
		unsigned char l = BASE16_DEC_TAB[cl];
		//两个4位合并为一个
		//1000 >>4 1000 0000
		//0001 >>  0000 0001
		//  |  >>  1000 0001
		out[i / 2] = h << 4 | l;
	}
	return in.size() / 2;
}

//用于测试
/*int main()
{
	cout << "Base16 test :" << endl;
	//测试数据内容
	unsigned const char data[] = "测试数据abc123";
	int len = sizeof(data);
	char out1[1024] = { 0 };
	//原版内容
	cout << data << endl;
	//加密
	int re = encode(data, len, out1); 
	cout << "加密后"<<re << ":" << out1 << endl;
	//解密
	unsigned char out2[1024] = { 0 };
	re = decode(out1, out2);
	cout << "解密后" << re << ":" << out2 << endl;
	return 0;
}*/