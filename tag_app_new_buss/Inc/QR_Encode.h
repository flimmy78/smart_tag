#include "bsp.h"
//#include "stm32f10x_type.h"
typedef unsigned char  BYTE;
typedef unsigned int  WORD;
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

#define QR_VRESION_S	0 
#define QR_VRESION_M	1 
#define QR_VRESION_L	2 

#define MAX_ALLCODEWORD	 26//3706 
#define MAX_DATACODEWORD 19//2956 
#define MAX_CODEBLOCK	  153 
#define MAX_MODULESIZE	  21	// 21:Version=1,最大字符=17(8.5个汉字)
								// 25:Version=2,最大字符=32(16个汉字)
								// 29:Version=3,最大字符=49(24.5个汉字)
								// 33:Version=4,最大字符=78(39个汉字)
								// 37:Version=5,最大字符=106(53个汉字) 
								// 41:Version=6,最大字符=134(67个汉字)
							 	// 45:Version=7,最大字符=154(77个汉字)
							 	// 49:Version=8,最大字符=192(96个汉字)
#define QR_MARGIN	4

/////////////////////////////////////////////////////////////////////////////
struct RS_BLOCKINFO
{
	int ncRSBlock;		
	int ncAllCodeWord;	
	int ncDataCodeWord;
};
struct QR_VERSIONINFO
{
	int nVersionNo;	   
	int ncAllCodeWord; 

	int ncDataCodeWord[4];	
	int ncAlignPoint;	
	int nAlignPoint[6];	

	struct RS_BLOCKINFO RS_BlockInfo1[4]; 
	struct RS_BLOCKINFO RS_BlockInfo2[4]; 
};

bool EncodeData(char *lpsSource);

int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength);
bool EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup);

int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

int SetBitStream(int nIndex, WORD wData, int ncData);

bool IsNumeralData(unsigned char c);
bool IsAlphabetData(unsigned char c);
bool IsKanjiData(unsigned char c1, unsigned char c2);

BYTE AlphabetToBinaly(unsigned char c);
WORD KanjiToBinaly(WORD wc);

void GetRSCodeWord(BYTE *lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

void FormatModule(void);

void SetFunctionModule(void);
void SetFinderPattern(int x, int y);
void SetAlignmentPattern(int x, int y);
void SetVersionPattern(void);
void SetCodeWordPattern(void);
void SetMaskingPattern(int nPatternNo);
void SetFormatInfoPattern(int nPatternNo);
int CountPenalty(void);
void Print_2DCode(void);
