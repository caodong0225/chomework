#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DICFNAME dicfname /*默认词典文件名*/
#define MAX(x,y) (x)>(y) ? (x) : (y) /*给x和y加括号目的是防止出现算数优先级问题*/

/*词语结构体*/
typedef struct _words{
    int total; /*词频*/
    char word[64]; /*词语*/

    struct _words *next; /*结点指针*/
}Words;

/*词典全局变量，为了不重复加载词典(效率高)*/
char dicfname[128] = "out1.txt"; /*默认词典文件名*/
int maxLen_word; /*词典中单个词语最大的长度*/
int total_words; /*词语的总个数*/
Words * words_list = NULL; /*词典链表*/

/*初始化一条链表*/
Words* initList();

/* 插入链表数据（头插法）
   参数 1.链表头结点 2.词语  3.词频 */
void headInsert(Words* head, char* textLine, int total);

/* 插入链表数据（尾插法）*/
void tailInsert(Words* head, char* word, int total);

/*统计链表长度*/
int linked_list_len(Words* head);

/*查看词典*/
void viewDictionary(char* fileName);

/*  加载词典函数
    参数 词典文件名称
    返回 词典的链表*/
Words* readDictionary(char* fileName);

/*处理字符函数，用于去除非中文字符，以避免字符处理出现乱码*/
void remove_zh_ch(char* str);

/* 比较是否存在该词语
   参数 1.词典链表 2.比较的词语
   返回 存在:词条数据的指针  不存在:NULL */
Words* wordSame(Words* head,char* ch);

/*正向最大匹配法*/
Words* forwardMax(char* str);

/*逆向最大匹配法*/
Words* reverseMax(char* str);

/*双向最大匹配法*/
Words* bidirectionalMax(char* str);

/*显示分词结果*/
void displayResults(char* str);

/*分析分词结果详情数据,没有对数据进行封装*/
void statisticalWord(Words* head);

/*菜单主页*/
void homeMenu();

/*分词页面*/
void participle();

/*输入任意长(不限)度字符串*/
char* getstr();

/*生成标题*/
void titlecreate();

int main(int argc,char * argv[])
{
	if(argc == 1) /*一个参数默认不是在命令行操作*/
	{
		homeMenu();
	}
	if(strcmp("-p",argv[1]) == 0 && argc > 2) /*进行命令行分词*/
	{
		readDictionary(DICFNAME);/*加载默认词典*/
		displayResults(argv[2]);
	}
	else if(strcmp("-s",argv[1]) == 0 && argc > 2) /*进行命令行分词*/
	{
		readDictionary(DICFNAME);/*加载默认词典*/
	}
	else
	{
		printf("你的命令输入有误！");
	}
    return 0;
}

Words* initList()
{
    Words* node = (Words*)malloc(sizeof(Words));
    node->next=NULL;
    return node;
}

void headInsert(Words* head, char* word, int total)
{
    Words* node = (Words*)malloc(sizeof(Words));
    node->total = total;
    strcpy(node->word,word);
    node->next = head->next;
    head->next = node;
}

void tailInsert(Words* head, char* word, int total)
{
    Words* n = head;
    Words* node = (Words*)malloc(sizeof(Words));
    node->total = total;
    strcpy(node->word,word);
    while(n->next != NULL)
    {
        n = n->next;
    }
    node->next = NULL;
    n->next = node;
}

int linked_list_len(Words* head)
{
	Words* node = head;
	int count = 0;
	node = node->next;
	while(node != NULL)
	{
		count++;
		node = node->next;
	}
	return count;
}

void viewDictionary(char* fileName)
{
    FILE* pf = fopen(fileName,"r");
    Words* node;//用于循环链表
    char choice;

    node = words_list->next;
    if(!pf)
	{
		printf("未找到该词典,词典文件打开失败！\n");
	}
	else
    {
        while(node!=NULL)
        {
            printf("词语：%s   词频：%d\n",node->word,node->total);
            node=node->next;
        }
    }
    while(1)
        {
    printf("是否返回上级菜单?( y or n)\n");
    scanf("%c",&choice);
    if(choice=='y'||choice=='Y')
    {
        system("cls");//清屏函数
        homeMenu();
        break;
    }
    else if(choice=='n'||choice=='N')
    {
        exit(1);
        break;
    }
    else
    {
        printf("输入错误！");
        getchar();}
    }

}

Words* readDictionary(char* fileName)
{
    FILE* pf = fopen(fileName,"r");
    char str[64];
    char *strtemp;
    char *ret;
    int index = 0;
    int count = 0;
    Words* head = initList(); //创建一个链表
	Words* node; //用于循环链表

	if(!pf)
	{
		printf("未找到该词典,词典文件打开失败！\n");
		return NULL;
	}
	else
	{

		while(fgets(str,sizeof(str),pf)!=NULL)
		{
			ret = strchr(str,9);/*找tab*/
			index = ret-str; /*计算长度*/
			strtemp = (char*)malloc(index+1);/*分配空间*/
			strncpy(strtemp,str,index+1); /*把 str 所指向的字符串复制到 strtemp，最多复制 index 个字符*/
			strtemp[index] = '\0';
			/*提取字典里的词频,后移3位，从词频数字开始，
			  有些词性占两个字符，但是使用atoi()函数转换成int后会自动去除掉，不影响。*/
			strcpy(str,ret+3);
			headInsert(head,strtemp,atoi(str)); //插入链表
			count++;
		}
		/*词语总个数赋值给全局变量*/
		total_words = count;
		node = head->next;
		/*找出词典中单个词语的最大长度*/
		while(node != NULL)
		{
			maxLen_word = MAX((int)strlen(node->word),maxLen_word);
			node = node->next;
		}
		fclose(pf);
		words_list = head; /*赋值给全局变量*/
		return head;
	}
}

void remove_zh_ch(char* str)
{
    int i=0;
    char* strtemp = (char*)malloc(strlen(str));
    strcpy(strtemp,"\0"); //防止乱码，置空
    while(i != (int)strlen(str)){
        /*将p字节进行移位运算，右移8位，这样,
        如果移位后是0，则说明原来的字节最高位为0，不是0那么也就不是汉字的一个字节。*/
        if(~(str[i] >> 8) == 0)
        {   //代表是汉字
            strncat(strtemp, &str[i], 1);
        }
        i++;
    }
    memmove(str,strtemp,strlen(str));
}

Words* wordSame(Words* head,char* ch)
{
    int count=0;
    head = head->next;
    while(head!=NULL)
    {
        if(strcmp(ch,head->word) == 0)
        {
            return head;
            break;
        }
        head = head->next;
    }
    return NULL;
}

Words* forwardMax(char* str)
{
    char* strtemp = (char*)malloc(strlen(str));  //提取要切分最长词语的中间变量
    char* terms = (char*)malloc(strlen(str));  //要切分的词
    int i=0,j=0;
    int count=0;  //用于计算切分位置的长度
    Words* head = words_list; //赋值词典链表
    Words* words = initList(); //词语链表用于存放切分好的词
    Words* word; //接收比较相同后的词语结构指针
    head = head->next;
    strcpy(strtemp,"\0");
    remove_zh_ch(str); //过滤掉英文字符，避免乱码

    /*循环到字符串的末尾结束*/
    while(count != (int)strlen(str))
    {
        /*将str字符拷贝到strtemp，长度为maxLen+1*/
        memmove(strtemp,str+count,maxLen_word+1);
        /* i从词语maxLen+1个(词典里的最长词语)长度开始循环，循环缩减到剩一个字符
           如果字符串(尾部)最后不足maxLen+1的长度，则等于str的长度-count(计数读到的长度)*/
        for(i=(int)strlen(str)-count<maxLen_word+1?(int)strlen(str)-count-1:maxLen_word-1;i>=0;i-=2)
        {
            strcpy(terms,"\0"); /*将terems置空*/
            for(j=0;j<=i;j++)
            {
                strncat(terms, &strtemp[j], 1);  /*将循环缩减截取的词语赋值给terms*/
            }
            /*判断字典是否存在这个词*/
            if( (word = wordSame(head,terms)) != NULL )
            {
                /*正向匹配的使用尾插法把数据存入到链表，避免遍历的时候顺序颠倒*/
                tailInsert(words,word->word,word->total);
                count += (int)strlen(terms); /*计算字符截取的位置*/
                break;
            }
            /*不存在,且词长度只剩一个中文字符*/
            else if(strlen(terms) == 2)
            {
                tailInsert(words,terms,0);
                count += (int)strlen(terms); /*计算字符截取的位置*/
                break;
            }
        }
    }
	return words;
}

Words* reverseMax(char* str)
{
    char* strtemp = (char*)malloc(strlen(str));  //提取要切分最长词语的中间变量
    char* terms = (char*)malloc(strlen(str));  //要切分的词
    int count=0;  //用于计算切分位置的长度
    int reduce=0;
    int i=0,j=0;

    Words* head = words_list; //赋值词典链表
    Words* words = initList(); //词语链表用于存放切分好的词
    Words* word; //接收比较相同后的词语结构指针
    head = head->next;
    strcpy(strtemp,"\0");
    remove_zh_ch(str); //过滤掉英文字符，避免乱码

    /*循环到字符串的末尾结束*/
    while(count != (int)strlen(str))
    {
        /*逆向分匹配，从字符串的尾部开始切割词语*/
        if((int)strlen(str)-maxLen_word-count>0)
        {
            /*将str字符拷贝到strtemp，长度为maxLen+1
              逆向最大匹配法，先从字符串最后边开始，每循环结束除去已匹配到的字符,然后往前移。*/
            memmove(strtemp,str+(int)strlen(str)-maxLen_word-count,maxLen_word+1);
        }
        /*词语切割到前头后不在往后移*/
        else if((int)strlen(str)-maxLen_word-count<=0)
        {
            memmove(strtemp,str,maxLen_word+1);
        }
        /*如果字符串(尾部)最后不足maxLen+1的长度，则等于str的长度-count(计数读到的长度)*/
        reduce = (int)strlen(str)-count<maxLen_word+1?(int)strlen(str)-count-1:maxLen_word-1;
        /* i从词语maxLen+1个(词典里的最长词语)长度开始循环，循环缩减到剩一个字符*/
        for(i=reduce ;i>=0; i-=2)
        {
            strcpy(terms,"\0"); /*将terems置空*/
            for(j=reduce-i; j<=reduce; j++)
            {
                strncat(terms, &strtemp[j], 1);  /*将循环缩减截取的词语赋值给terms*/
            }
            /*判断字典是否存在这个词*/
            if( (word = wordSame(head,terms)) != NULL )
            {
                /*逆向匹配的使用头插法把数据存入到链表，避免遍历的时候顺序颠倒*/
                headInsert(words,word->word,word->total);
                count += (int)strlen(terms); /*计算字符截取的位置*/
                break;
            }
            /*不存在,且词长度只剩一个中文字符*/
            else if(strlen(terms) == 2)
            {
                headInsert(words,terms,0);
                count += (int)strlen(terms); /*计算字符截取的位置*/
                break;
            }
        }
    }
    return words;
}

Words* bidirectionalMax(char * str)
{
	Words * fnode;
	Words * rnode;
	Words * fwords = forwardMax(str);
	Words * rwords = reverseMax(str);
	int fsum = 0;
	int rsum = 0;
	int r;
	srand((unsigned)time(NULL)); // 播种
	r = rand() % 2 + 1; //产生随机数

	/*正反向分词结果词数不同，则取分词数量较少的那个*/
	if(linked_list_len(fwords) > linked_list_len(rwords))
	{
		return rwords;
	}
	/*正反向分词结果词数不同，则取分词数量较少的那个*/
	else if(linked_list_len(fwords) < linked_list_len(rwords))
	{
		return fwords;
	}
	/*分词结果词数相同*/
	else
	{
		fnode = fwords->next;
		rnode = rwords->next;
		while(fnode != NULL || rnode != NULL)
		{
			/*分词结果不同，返回单字较少的那个*/
			if(strcmp(fnode->word, rnode->word) != 1)
			{
				fnode = fwords->next;
				rnode = rwords->next;
				/*统计单个字的数量*/
				while(fnode != NULL || rnode != NULL)
				{
					if(strlen(fnode->word) == 2)
						fsum++;
					if(strlen(rnode->word) == 2)
						rsum++;
					fnode = fnode->next;
					rnode = rnode->next;
				}
				/*单个字的数量相同，任意(随机)返回一个*/
				if(fsum == rsum)
				{
					return r==1 ? fwords : rwords;
					break;
				}
				/*单个字的数量不相同，返回单字较少*/
				return fsum>rsum ? rwords : fwords;
				break;
			}
			fnode = fnode->next;
			rnode = rnode->next;
		}
		/*分词结果相同，任意(随机)返回一个*/
		return r==1 ? fwords : rwords;
	}
}

void displayResults(char* str)
{
	Words * words;
	Words * head;
	double duration;
	char ch;

	words = forwardMax(str);/*导入正向分词结果*/
	words = words->next;
	printf("\n正向分词结果：");
	while(words != NULL)
	{
		printf("%s/ ",words->word);
		words = words->next;
	}/*输出正向分词结果*/
	printf("\n");

	words = reverseMax(str);/*导入逆向分词结果*/
	words = words->next;
	printf("逆向分词结果：");
	while(words != NULL)
	{
		printf("%s/ ",words->word);
		words = words->next;
	}/*输出逆向分词结果*/
	printf("\n");

	words = bidirectionalMax(str);/*导入双向分词结果*/
	head = words;
	words = words->next;
	printf("双向分词结果：");
	while(words != NULL)
	{
		printf("%s/ ",words->word);
		words = words->next;
	}/*输出双向分词结果*/
    statisticalWord(head);/*判断不同分词结果来输出最优分词结果及分词出现频率*/
}

void statisticalWord(Words* head)
{
	Words *inode = head->next;
	Words *jnode = head->next;
	int *records = (int *) malloc(linked_list_len(head) * sizeof(int)); /*动态申请内存，长度分词结果相同*/
	double sum = linked_list_len(words_list),hot=1; /*获取词典中词语的总数*/
	int i,j;

	/*遍历两条链表，统计词语出现次数*/
	for(i=0; i<linked_list_len(head); i++)
	{
		jnode = head->next;
		records[i] = 0; /*统计结果存入数组*/
		for(j=0; j<linked_list_len(head); j++)
		{
			if(strcmp(inode->word,jnode->word) == 0) /*相同则出现次数+1*/
			{
				records[i]++;
			}
			jnode = jnode->next;
		}
		inode = inode->next; /*链表移动到下一个结点*/
	}
	inode = head->next;

	for(i=0; i<linked_list_len(head); i++)
	{
		jnode = head->next;
		records[i] = 0; /*统计结果存入数组*/
		for(j=0; j<linked_list_len(head); j++)
		{
			if(strcmp(inode->word,jnode->word) == 0) /*相同则出现次数+1*/
			{
				records[i]++;
			}
			jnode = jnode->next;
		}
		inode = inode->next; /*链表移动到下一个结点*/
	}
	printf("\n");
	inode = head->next;;

	printf("词语\t\t出现次数\t词典中出现概率\n");
	for(i=0;i<linked_list_len(head);i++)
	{
		printf("%s\t\t  %d次\t\t  %lf\n",inode->word,records[i],inode->total/sum);/*格式化输出分词结果以及出现频率*/
        hot=hot*(1/(inode->total/sum));
		inode = inode->next;
	}
	printf("标题热度:%lf\n",pow(hot,(double)1/i));
	printf("\n");
}

void homeMenu()
{
	FILE *pf;
	int choice = -1;
	char textLine[64];
	if(words_list == NULL) /*判断字典是否加载*/
	{
		readDictionary(DICFNAME);/*加载词典*/
	}
    printf ("********************************************************\n");
    printf ("-------                文本分词小工具            -------\n");
	printf ("********************************************************\n");
    printf ("*                                                      *\n");
    printf ("*\t1.文本分词                                     *\n");
    printf ("*\t2.查看词典                                     *\n");
    printf ("*\t3.生成标题                                     *\n");
	printf ("*\t0.退出程序                                     *\n");
    printf ("*                                                      *\n");
    printf ("*------------------------------------------------------*\n");
    printf ("------Press key(1/2/3/0) to continue the program------\n");

	while(choice != 0)
	{
		printf("请选择：");
		scanf("%d",&choice);
		fflush(stdin);  /*清空缓冲区，也可以使用rewind(stdin);*/
		switch(choice)
		{
		case 0:
			exit(1);
			break;
		case 1:
			system("cls");
			participle();
			break;
        case 2:
            viewDictionary(DICFNAME);
            break;
        case 3:
            titlecreate();
            break;
		default:
			printf("输入错误！");
		}
	}

}

void participle()
{
	FILE *pf;
	char *fText,*fText1; //存放文件里的文本
	char *str; //用户输入字符
	int choice = -1;
	int i=0,len;
	char ch;

	printf ("********************************************************\n");
	printf ("------------------首页—>文本分词---------------------\n");
	printf ("********************************************************\n");
	printf ("*                                                      *\n");
	printf ("*\t1.输入文本                                     *\n");
	printf ("*\t2.导入文本                                     *\n");
	printf ("*\t0.返回上一级菜单                               *\n");
	printf ("*                                                      *\n");
	printf ("*------------------------------------------------------*\n");
	printf ("--------Press key(1/2/0) to continue the program--------\n");

	while(1)
	{
		printf("请选择：");
		scanf("%d",&choice);
		fflush(stdin);   /*清空缓冲区*/
		switch(choice)
		{
		case 0:
			system("cls"); /*清屏函数*/
			homeMenu();/*返回上一级菜单*/
			break;
		case 1:
			printf("请输入文本：");
			str = getstr(); /*输入文本，不限长度*/
			displayResults(str);/*显示分词结果*/
			system("pause");
			participle();/*递归调用来实现重复显示*/
			break;
		case 2:
			printf("注：确保文件存放在项目目录中，否则要输入绝对路径(例：D:\\\\xxxxx.xxx)\n");
			printf("请输入文件名称(路径)：");
			str = getstr();
			pf = fopen(str,"rb");
			if(!pf)
			{
				printf("打开失败！\n");
			}
			else
			{
				fseek(pf, 0, SEEK_END);/*文件指针位置移到最后*/
				fText1 = (char*)malloc(ftell(pf)+1);
				fText1[ftell(pf)] = '\0';
				len=ftell(pf);/*用len储存文件总长度*/
				fseek(pf, 0, SEEK_SET); /*文件指针移到最前头*/
				while(fread(&ch,sizeof(char),1,pf) == 1)
                {
                    printf("%c",ch);
                    if(ch=='\n')/*用换行符作为一行结束的标识符*/
                    {
                        fText= (char*)malloc(i+1);/*给fText分配对应读入行的内存实现动态分配长度*/
                        memcpy(fText,fText1,i);
                        fText[i]='\0';
                        displayResults(fText);/*按行显示分词结果*/
                        i=0;/*重置i的值*/
                        free(fText1);/*释放内存*/
                        free(fText);
                        fText1 = (char*)malloc(len+1);/*重新给指针fText1分配内存*/
                        continue;
                    }
					fText1[i]=ch;
					i++;
				}
				if(ch!='\n')/*对尾行不以换行符结尾的文本进行单独判断*/
                {
                    fText= (char*)malloc(i+1);
                    memcpy(fText,fText1,i);
                    fText[i]='\0';
                    displayResults(fText);
                }
				free(fText1);
				free(fText);/*读取结束释放内存*/
				fclose(pf);
			}
			system("pause");
			participle();/*递归调用来实现重复显示*/
			break;
		default:
			printf("输入错误！");
		}
	}
}

char* getstr()
{
	char* str;
	char* _str;
	int i = 1;
	str = (char*)malloc(sizeof(char) * (i + 1));
	while('\n' != (str[i - 1] = getchar()))
	{
		i ++;
		_str = (char*)malloc(strlen(str) + 1);
		str[i - 1] = '\0';
		strcpy(_str, str);
		free(str);
		str = (char*)malloc(sizeof(char) * (i + 1));
		if(NULL == str)
		{
			free(_str);
			printf("No enough memory!");
			return NULL;
		}
		strcpy(str, _str);
		free(_str);
	}
	str[i - 1] = '\0';
	return str;
}
void titlecreate()
{
    FILE *fp;
    char ch;
    char choice;
    system("标题生成.exe");
    fp=fopen("title.txt","r");
    while(fread(&ch,sizeof(char),1,fp) == 1)
    {
        printf("%c",ch);
    }
    while(1)
    {
    printf("\n是否返回上级菜单?( y or n)\n");
    scanf("%c",&choice);
    if(choice=='y'||choice=='Y')
    {
        system("cls");//清屏函数
        homeMenu();
        break;
    }
    else if(choice=='n'||choice=='N')
    {
        exit(1);
        break;
    }
    else
    {
        printf("输入错误！");
        getchar();}
    }

}
