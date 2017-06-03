#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <map>
#define PSEUDO_EOF 256
#define MAXNODE 256
#define BUFFERSIZE 1024
typedef struct Hnode {
	int weight=0;
	int name;
	Hnode * parent=NULL; 
	Hnode * lchild = NULL;
	Hnode * rchild = NULL;
}HnodeTree, * HNode;


std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

using namespace std;


void encode();
void decode();
void findmin(vector <HNode>);
HNode combine(HNode & ,HNode &);
HNode encodebuildtree(vector <HNode>&);
void travel(HNode &root,string);
void order(HNode &root);
HNode min1, min2;

map <int,string> dic;
int main(void) {
	char flag;
	cout << "0:压缩文件  1:解压文件" << endl;
	cin >> flag;
	switch (flag) {
	case '0':{
		encode();
		break;
		}
	case '1': {
		decode();
		break;
	}
	}
	return 0;
}

void encode(){
	ifstream infile;
	vector <int> weightlist;
	vector <int> namelist;
	map<int, string>::iterator mit;
	unsigned char out_c, tmp_c;
	char inchar;
	int size, w[MAXNODE] = { 0 }, i,j,length=0,codelength=0;  //length为字符种类数量
	string filename;
	cout << "Please input the file you want to compress:"  ;
	cin >> filename;
	infile.open(filename,ios::binary);
	//string buffer((std::istreambuf_iterator<char>(infile)),
		//std::istreambuf_iterator<char>());

	//size = buffer.size();
	//infile.get(inchar);
	while (!infile.eof()){
		
		infile.get(inchar);
		//if (inchar==EOF)
		//	break;
		w[(int)(unsigned char)inchar]++;
		//if ((int)(unsigned char)inchar == 49) {
		//	cout << "found" << endl;
		//}
	}
	//for (i = 0; i < size; i++) {
		//cout << (int)(unsigned char)buffer.at(i) << endl;
		//w[(int)(unsigned char)buffer.at(i)]++;
	//}
	for (i = 0; i <MAXNODE; i++) {
		if (w[i] != 0) {
			length++;
			weightlist.push_back(w[i]);
			namelist.push_back(i);
		}
			
	}
	weightlist.push_back(1);
	namelist.push_back(PSEUDO_EOF);
	length++;

	vector <HNode> HT(length);
	for (i = 0; i < length;i++) {
		HT[i] = new HnodeTree;
		HT[i]->lchild = HT[i]->rchild = HT[i]->parent = NULL;
		HT[i]->weight = weightlist[i];
		HT[i]->name = namelist[i];
	}
	HNode root=encodebuildtree(HT);
	order(root);
	ofstream outfile;
	filename = filename + ".huf";
	outfile.open(filename,ios::binary);
	outfile << length << endl;
	for ( mit= dic.begin(); mit != dic.end(); mit++) {
		outfile << mit->first << " " << mit->second << endl;
	}
	string code,out_string;
	//infile.close();
	infile.clear();
	infile.seekg(ios::beg);
	//infile.open(filename, ios::binary);
	while (!infile.eof()) {
		infile.get(inchar);
		//if (infile.eof())
		//	break;
		//for (string::iterator sit = buffer.begin(); sit != buffer.end(); sit++) {
		mit = dic.find((int)(unsigned char)inchar);
		if (mit == dic.end()) {
			//cout << (int)(unsigned char)inchar;
			//code += mit->second;
		}
		code += mit->second;
		//}
		codelength = code.length();
		if (codelength > BUFFERSIZE) {

			out_string.clear();
		for (i = 0; i + 7 < codelength; i += 8)
		{
			// 每八位01转化成一个unsigned char输出
			// 不使用char，如果使用char，在移位操作的时候符号位会影响结果
			// 另外char和unsigned char相互转化二进制位并不变
			out_c = 0;
			for (j = 0; j < 8; j++)
			{
				if ('0' == code[i + j])
					tmp_c = 0;
				else
					tmp_c = 1;
				out_c += tmp_c << (7 - j);
			}
			out_string += out_c;
		}
		outfile << out_string;
		code = code.substr(i, codelength - i);
		}
	}
	// 已读完所有文件，先插入pseudo-EOF
	mit = dic.find(PSEUDO_EOF);
	if (mit != dic.end())
		code += mit->second;
	else
	{
		printf("Can't find the huffman code of pseudo-EOF\n");
		exit(1);
	}
	out_c = 0;
	for (i = 0; i < code.length(); i++)
	{
		if ('0' == code[i])
			tmp_c = 0;
		else
			tmp_c = 1;
		out_c += tmp_c << (7 - (i % 8));
		if (0 == (i + 1) % 8 || i == code.length() - 1)
		{
			// 每8位写入一次文件
			outfile << out_c;
			out_c = 0;
		}
	}
	infile.close();
	outfile.close();
}

void decode(){
	map <int, string> dic;
	ifstream in_file;
	int i ;
	string filename, nodenum, line, sequence,outstring;
	int chr;
	string::iterator sit;
	char in_char; 
	unsigned u_char;
	unsigned int flag;
	cout << "Please input the filename you want to depress:";
	cin >> filename;
	//int sublength = (int)filename.size - 4;
	in_file.open(filename,ios::binary);
	HNode root = new HnodeTree;
	HNode tmpnode = root;
	safeGetline(in_file, nodenum);
	for (i = 0; i < stoi(nodenum); i++) {
		tmpnode = root;
		in_file >> chr;
		in_file >> sequence;
		dic.insert(pair<int, string>(chr, sequence));
		for (sit = sequence.begin(); sit != sequence.end(); sit++) {
			if ((*sit) == '0') {
				if (tmpnode->lchild == NULL) {
					tmpnode->lchild = new HnodeTree;
				}
				tmpnode = tmpnode->lchild;
			}
			if ((*sit) == '1') {
				if (tmpnode->rchild == NULL) {
					tmpnode->rchild = new HnodeTree;
				}
				tmpnode = tmpnode->rchild;
			}
		}
		tmpnode->name = chr;
	}
	
	in_file.ignore(1024,'\n');
	tmpnode = root;

	ofstream out_file(filename+".unhuf", ios::binary);
	bool pseudo_eof = false;
	while (!in_file.eof()) {
		
		//tmpnode = root;
		in_file.get(in_char);
		u_char = (unsigned char)in_char;
		flag = 0x80;
		//cout << (int)(unsigned char)flag << endl;
		for (i = 0; i < 8; ++i) {
			if (int(u_char & flag)!=0) {
				tmpnode = tmpnode->rchild;
			}
			else {
				tmpnode = tmpnode->lchild;
			}
			//cout << (u_char & flag) << endl;
			//char test = 0x80;
			//cout << (int)test << endl;
			//cout << (int)u_char << " " << (int)flag << endl;
			if (tmpnode->lchild == NULL && tmpnode->rchild == NULL) {
				chr = tmpnode->name;
				if (chr == PSEUDO_EOF) {
					pseudo_eof = true;
					break;
					
				}
					
				else {
					outstring += (char)chr;
					tmpnode = root;
				}
			}

			flag = flag >> 1;
		}
		if (pseudo_eof)
			break;
		if (outstring.size() > BUFFERSIZE) {
			out_file << outstring;
			outstring.clear();
		}
	}
	out_file << outstring;
	outstring.clear();
	out_file.close();
	in_file.close();
}

void findmin(vector <HNode> nodelist){
	min1 = nodelist[1]; min2 = nodelist[0];
	if (min1->weight > min2->weight) {
		min1 = nodelist[0];
		min2 = nodelist[1];
	}
		
	for (vector <HNode> ::iterator it = nodelist.begin()+2; it != nodelist.end();it++) {
		if ((*it)->weight <= min1->weight) {
			min2 = min1;
			min1 = (*it);
		}
	}
}

HNode encodebuildtree(vector <HNode> &nodelist){

	if (nodelist.size() <= 1)
		return nodelist[0];
	else {
		findmin(nodelist);
		HNode newnode = combine(min1, min2);
		nodelist.push_back(newnode);
		for (vector <HNode> ::iterator it = nodelist.begin(); it != nodelist.end();it++) {
			if (*it == min1) {
				nodelist.erase(it);
				break;
			}
		}
		for (vector <HNode> ::iterator it = nodelist.begin(); it != nodelist.end();it++) {
			if (*it == min2) {
				nodelist.erase(it);
				break;
			}
		}
		return encodebuildtree(nodelist);
	}

}
void travel(HNode & root,string pos){
	//Stack s;
	if (root!=NULL) {
		if (root->lchild != NULL) {
			pos +='0' ;
			travel(root->lchild,pos);
		}
		if (root->rchild != NULL) {
			pos.erase(pos.end() - 1);
			pos += '1';
			travel(root->rchild,pos);
		}
		if (root->lchild == NULL && root->rchild == NULL) {
			if (root->weight > 0 && root->name>=0) {
				dic.insert(pair<int, string>(root->name, pos));

			}
		}
	}
}
void order(HNode & root)
{
	string pos = "";
	travel(root,pos);
}
HNode combine(HNode &min1, HNode &min2) {
	HNode result = new HnodeTree;
	result->lchild = min1;
	result->rchild = min2;
	result->weight = min1->weight + min2->weight;
	min1->parent = min2->parent = result;
	return result;
}