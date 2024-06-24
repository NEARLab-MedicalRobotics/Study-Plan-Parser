// PianiChecker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <iostream>


///////////////////////////////////////////////////////////////////////////

#define DM_270

///////////////////////////////////////////////////////////////////////////
// structs ...

struct ExamParam {

	short AA;
	short AC;
	short semester;
	char  Pos[2];
	short cfu;
	char  codice[8];
	char  name[256];
	char  stato[2];

};


struct TableElem {

	char codice[8];
	char name[256];
	bool fFound;
	char ordin[256];	

};


/////////////////////////////////////////////////////////////////////////////
// globals ...

int			nExam;
int			nOrd;
ExamParam	ExamList[256];

int			nTableElem;
TableElem	Table[256];


/////////////////////////////////////////////////////////////////////////////
// macors ...

#define IsNotAssigned(n)	(ExamList[n].stato[0] == 'N')
#define IsAssigned(n)		(ExamList[n].stato[0] == 'A')
#define IsValid(n)			(ExamList[n].Pos[0]=='E' || ExamList[n].Pos[0] == 'A' || ExamList[n].Pos[0] == 'O')

#define MarkAsAssigned(n)	(ExamList[n].stato[0] = 'A')
#define GetCredits(n)		(ExamList[n].cfu)

#define	IsInOrdinamento(n,o)	(Table[n].ordin[o] == '1')

#ifdef DM_270

#define	LABS_TABLE			"LABS_270.TXT"
#define	SPEC_TABLE			"SPEC_270.TXT"
#define	BASE_TABLE			"BASE_270.TXT"
#define	SISF_TABLE			"SISF_270.TXT"
#define	MATH_TABLE			"CHFM_270.TXT"
#define FINA_TABLE			"FINA_270.TXT"
#define FINA_TABLE_CLI      "FINA_CLI.TXT"
#define AMBASSADOR  		"AMBASSADOR.TXT"

#define	LABS_TABLE_ENG		"LABS_270_ENG.TXT"
#define	SPEC_TABLE_ENG		"SPEC_270_ENG.TXT"
#define	BASE_TABLE_ENG		"BASE_270_ENG.TXT"
#define	SISF_TABLE_ENG		"MOD_270_ENG.TXT"
#define	MATH_TABLE_ENG		"CHFM_270_ENG.TXT"
#define	MATH_TABLE_CLI		"CHFM_270_CLI.TXT"
#define	LABS_TABLE_CLI		"LABS_270_CLI.TXT"
#define	INGLM0_TABLE_ENG	"INGLM-BCI_270_ENG.TXT"
#define	INGLM1_TABLE_ENG	"INGLM-BTE_270_ENG.TXT"
#define	INGLM2_TABLE_ENG	"INGLM-BBB_270_ENG.TXT"
#define	INGLM3_TABLE_ENG	"INGLM-BCT_270_ENG.TXT"
#define	INGLM4_TABLE_ENG	"INGLM-BIF_270_ENG.TXT"
//#define FINA_TABLE_ENG		"FINA_270_ENG.TXT"


#define	MIN_LABS_CREDITS		5
#define	MIN_LABS_CREDITS_CLI	15
#define	MIN_SPEC_CREDITS		20
#define	MIN_BASE_CREDITS		30
#define	MIN_SISF_CREDITS		10
#define	MIN_SISF_CREDITS_CLI	5
#define	MIN_CHFM_CREDITS		12
#define	MIN_CHFM_CREDITS_CLI	10
#define	MIN_FINA_CREDITS		18

#else 

#define	LABS_TABLE			"LABS.TXT"
#define	SPEC_TABLE			"SPEC.TXT"
#define	BASE_TABLE			"BASE.TXT"
#define	SISF_TABLE			"SISF.TXT"
#define	MATH_TABLE			"CHFM.TXT"
#define FINA_TABLE			"FINA.TXT"

#define	MIN_LABS_CREDITS		10
#define	MIN_SPEC_CREDITS		15
#define	MIN_BASE_CREDITS		30
#define	MIN_SISF_CREDITS		10
#define	MIN_FINA_CREDITS		20

#endif

/////////////////////////////////////////////////////////////////////////////
// functions ...

bool	CheckBase(int nOrd, int nLang);
bool	CheckLabs(int nOrd, int nLang);
bool	CheckSpec(int nOrd,	int nLang);
bool	CheckGest(int nOrd);
bool	CheckSisF(int nOrd, int nLang);
bool	CheckBiom(int nOrd);
bool	CheckFina(int nOrd);
bool	CheckInge(int nOrd);
bool	CheckMatChf(int nOrd, int nLang);

short	LoadTable(char *strName,TableElem* pTable);
short	SearchCorrespondence(short nExamTable, short NeededCredits);

bool CheckPlanIta(FILE *f);
bool CheckPlanEng(FILE *f);


/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

	
	if(!argv[2]  || ((argv[2][1]) != 'E' && argv[2][1] != 'I') )
	{
		printf("\n usage: PianiChecker filename -opt");
		printf("\n opt= I for Italian or E for English");
		return -1;
	}


	FILE *f=fopen(argv[1],"rt");
	if(f == NULL)
	{
		printf("\n file not found!");
		printf("\n usage: PianiChecker filename -opt");
		printf("\n opt= I for Italian or E for English");
		return -2;
	}		



	if(argv[2][1] == 'I')
		CheckPlanIta(f);
	else if(argv[2][1] == 'E')
		CheckPlanEng(f);
	
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
bool CheckPlanIta(FILE *f)
{
char buffLine[2048];	
char tmp[2048];	
int	 nOrdinamento = 0;	
	
	while(!feof(f))
	{
		fgets(buffLine,2048,f);
	
		// 
		if( strncmp(buffLine, "Nome", 4) == 0)
		{
			// Process new piano ....
			printf("\n**********************************************************");
			sscanf(&buffLine[5],"%s", tmp);  // Nome ...
			printf("\n%s",tmp);

			fgets(buffLine,2048,f);
			sscanf(&buffLine[8],"%s", tmp);  // Cognome ...
			printf(" %s",tmp);

			fgets(buffLine,2048,f);
			sscanf(&buffLine[10],"%s", tmp);  // Matricola ...
			printf(" %s",tmp);

			// Skip lines till Ordinamento ...
			nOrdinamento = -1;		
			while(!feof(f) && (strncmp(buffLine, "Indirizzo", 9) != 0))
					fgets(buffLine,2048,f);
			//fgets(buffLine,2048,f); 
			for(int k=0; k<2048; k++)
			{
				if(buffLine[k]=='(')
				{
					switch(buffLine[k+3])
					{
					case 'C': nOrdinamento=0; break;
					case 'E': nOrdinamento=1; break;
					case 'B': nOrdinamento=2; break;
					case 'T': nOrdinamento=3; break;
					case 'F': nOrdinamento=4; break;
					}

				break;
				}
				
			}


			// Skip lines till exams list ...
			while(!feof(f) && (strncmp(buffLine, "AA", 2) != 0))
					fgets(buffLine,2048,f);
		
			// Next line start exam list ....
			nExam = 0;
			do {
				fgets(buffLine,2048,f);
				
				if(buffLine[0] == '2' || buffLine[0] == '0')
				{
					//"%d  %d  %d  %c  %s  %*[^0-9]  %d"
					//%*[^0-9]  %d
					/*sscanf(buffLine,"%d  %d  %d  %c  %s", &ExamList[nExam].AA, 
																&ExamList[nExam].AC,
																&ExamList[nExam].semester,
																&ExamList[nExam].Pos[0],
																ExamList[nExam].codice,
																&ExamList[nExam].cfu);*/
					sscanf(buffLine,"%d %d %d %c %d %*[^0-9] %s", 
																&ExamList[nExam].AA, 
																&ExamList[nExam].AC,
																&ExamList[nExam].semester,
																&ExamList[nExam].Pos[0],
																&ExamList[nExam].cfu,
																ExamList[nExam].codice);

					strncpy(ExamList[nExam].codice,&buffLine[15], 6);
					strcpy(ExamList[nExam].name,&buffLine[20]);
					ExamList[nExam].stato[0] = 'N';

					/*printf("\nAA %d AC %d SEM %d POS %c CFU %d COD %s NAME %s STATO %c", ExamList[nExam].AA, 
													ExamList[nExam].AC,
													ExamList[nExam].semester,
													ExamList[nExam].Pos[0],
													ExamList[nExam].cfu,
													ExamList[nExam].codice,
													ExamList[nExam].name,
													ExamList[nExam].stato[0]);
					*/
					nExam++;


			
				}




			} while((strncmp(buffLine, "Colonna", 7) != 0) || (strncmp(buffLine, "Media", 5) != 0));

			// Fine piano ...
			int totalCfu = 0;
			for (int k=0; k<nExam; k++)
			{
			/*	printf("\n%d %d %d %c %d %s %s %c", ExamList[k].AA, 
													ExamList[k].AC,
													ExamList[k].semester,
													ExamList[k].Pos[0],
													ExamList[k].cfu,
													ExamList[k].codice,
													ExamList[k].name,
													ExamList[k].stato[0]);
			*/	
				if(IsValid(k))
					totalCfu += ExamList[k].cfu;
			}
				
			printf("\nTotal CFU: %d", totalCfu);
			printf("\n**********************************************************\n");
			
		
			// Check Piano ...
			
			short esit = false;
			esit  = CheckLabs(nOrdinamento, 1);
			esit += CheckSpec(nOrdinamento, 1);
			esit += CheckGest(nOrdinamento);
			esit += CheckBase(nOrdinamento, 1); 
			esit += CheckSisF(nOrdinamento, 1); 
			esit += CheckBiom(nOrdinamento);
			esit += CheckInge(nOrdinamento);
			esit += CheckMatChf(nOrdinamento,1);
			esit += CheckFina(nOrdinamento);

			// OutputReports ...
			printf("\n OTHERS \n");
			for (int k=0; k<nExam; k++)
			{
				if(IsNotAssigned(k) && IsValid(k))
					printf("\n   %s (%c) %s", ExamList[k].codice, ExamList[k].Pos[0], ExamList[k].name);
			}
			printf("\n\n\n");

			//getch();		
		
		}

	}

	return 0;

}


///////////////////////////////////////////////////////////////////////////////
bool CheckPlanEng(FILE *f)
{
char buffLine[2048];	
char tmp[2048];	
int	 nOrdinamento = 0;	
	
	while(!feof(f))
	{
		fgets(buffLine,2048,f);
	
		// 
		if( strncmp(buffLine, "Nome", 4) == 0)
		{
			// Process new piano ....
			printf("\n**********************************************************");
			sscanf(&buffLine[5],"%s", tmp);  // Nome ...
			printf("\n%s",tmp);

			fgets(buffLine,2048,f);
			sscanf(&buffLine[8],"%s", tmp);  // Cognome ...
			printf(" %s",tmp);

			fgets(buffLine,2048,f);
			sscanf(&buffLine[10],"%s", tmp);  // Matricola ...
			printf(" %s",tmp);

			// Skip lines till Ordinamento ...
			nOrdinamento = -1;		
			while(!feof(f) && (strncmp(buffLine, "Indirizzo", 9) != 0))
					fgets(buffLine,4096,f);
			//fgets(buffLine,2048,f); 
			for(int k=0; k<4096; k++)
			{
				if(buffLine[k]=='(')
				{	
					printf("\n");
					printf(buffLine);
					printf("\n");
					switch(buffLine[k+3])
					{
					case 'I': nOrdinamento=0; printf("\n ING. CLINICA"); break;
					case 'E': nOrdinamento=1; printf("\n TECNOLOGIE ELETTR"); break;
					case 'B': nOrdinamento=2; printf("\n BIOMECCANICA"); break;
					case 'T': nOrdinamento=3; printf("\n CELLULE E TESSUTI"); break;
					case 'F': nOrdinamento=4; printf("\n BIONG. INFORMAZIONE"); break;
					case 'N': nOrdinamento=5; printf("\n DOUBLE DEGREE NANOTEC"); break;
					case 'M': nOrdinamento=6; printf("\n DOUBLE DEGREE MEC"); break;
					}

				break;
				}
				
			}


			// Skip lines till exams list ...
			while(!feof(f) && (strncmp(buffLine, "AA", 2) != 0))
					fgets(buffLine,2048,f);
		
			// Next line start exam list ....
			nExam = 0;
			do {
				fgets(buffLine,2048,f);
				
				if(buffLine[0] == '2' || buffLine[0] == '0')
				{
					ExamList[nExam].Pos[0] = 0;
					ExamList[nExam].cfu = -1;
					//%[^','],%[^','],%[^','],%s
					//%[^'  '],%[^'  '],%[^'  '],%[^'  '],%[^'  '],%[^'  '],%[^'  ']
					//%d  %d  %d  %c  %s  %s  %d
					sscanf(buffLine,"%d  %d  %d  %c  %d  %*[^0-9]}  %s", 
																&ExamList[nExam].AA, 
																&ExamList[nExam].AC,
																&ExamList[nExam].semester,
																&ExamList[nExam].Pos[0],
																&ExamList[nExam].cfu,
																ExamList[nExam].codice,
																ExamList[nExam].name);

					strncpy(ExamList[nExam].codice,&buffLine[14], 6);
					strcpy(ExamList[nExam].name,&buffLine[20]);
					ExamList[nExam].stato[0] = 'N';
					/*printf("\nAA %d AC %d SEM %d POS %c CFU %d COD %s NAME %s STATO %c", ExamList[nExam].AA, 
													ExamList[nExam].AC,
													ExamList[nExam].semester,
													ExamList[nExam].Pos[0],
													ExamList[nExam].cfu,
													ExamList[nExam].codice,
													ExamList[nExam].name,
													ExamList[nExam].stato[0]);*/
					nExam++;
				}




			} while((strncmp(buffLine, "Media voti", 10) != 0));

			// Fine piano ...
			int totalCfu = 0;
			for (int k=0; k<nExam; k++)
			{
				if(ExamList[k].cfu == -1)
				{
					//totalCfu = -11111111;
					continue;
				}

				
				if(IsValid(k))
					totalCfu += ExamList[k].cfu;
			}
				
			printf("\nTotal CFU: %d", totalCfu);
			printf("\n**********************************************************\n");
			
		
			// Check Piano ...
			
			short esit = false;
			esit  = CheckLabs(nOrdinamento,1);
			esit += CheckSpec(nOrdinamento,1);
			esit += CheckGest(nOrdinamento);
			esit += CheckBase(nOrdinamento,1); 
			esit += CheckSisF(nOrdinamento,1); 
			esit += CheckBiom(nOrdinamento);
			esit += CheckInge(nOrdinamento);
			esit += CheckMatChf(nOrdinamento,1);
			esit += CheckFina(nOrdinamento);

			// OutputReports ...
			printf("\n OTHERS \n");
			for (int k=0; k<nExam; k++)
			{
				if(IsNotAssigned(k) && IsValid(k))
					printf("\n   %s (%c) %s", ExamList[k].codice, ExamList[k].Pos[0], ExamList[k].name);
			}
			printf("\n\n\n");

			//getch();		
		
		}

	}

	return 0;

}





///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
bool CheckBase(int nOrd, int nLang)
{
int Credits = 0;
int NeededCredits = MIN_BASE_CREDITS;
int nExamTable;

// NEW exams ...
	if(nLang==0)
		nExamTable = LoadTable(BASE_TABLE,Table);
	if(nLang==1)
		nExamTable = LoadTable(BASE_TABLE_ENG,Table);

	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n BASE \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}


// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}
///////////////////////////////////////////////////////////////////////////////////////////
bool CheckMatChf(int nOrd, int nLang)
{
int Credits = 0;
int NeededCredits = MIN_CHFM_CREDITS;
int nExamTable;

// CHF exams ...
	if(nLang==0)
		nExamTable = LoadTable(MATH_TABLE,Table);
	if(nLang==1) {
		if (nOrd == 0){
			nExamTable = LoadTable(MATH_TABLE_CLI,Table);
			NeededCredits = MIN_CHFM_CREDITS_CLI;
		}
		else
			nExamTable = LoadTable(MATH_TABLE_ENG,Table);
	}

	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n MAT-CHF \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}

/*
// MAT exams ...
	nExamTable = LoadTable("MAT.TXT",Table);
	Credits += SearchCorrespondence(nExamTable, NeededCredits-Credits);

// OutputReports ...
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}
*/

// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}

///////////////////////////////////////////////////////////////////////////////////////////
bool CheckSisF(int nOrd, int nLang)
{
int Credits = 0;
int NeededCredits = MIN_SISF_CREDITS;
int nExamTable;

// NEW exams ...
	if(nLang==0)
		nExamTable = LoadTable(SISF_TABLE,Table);
	if(nLang==1){
		if(nOrd == 0)
			NeededCredits = MIN_SISF_CREDITS_CLI;

		nExamTable = LoadTable(SISF_TABLE_ENG,Table);
	}
	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n MOD \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}

// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}

///////////////////////////////////////////////////////////////////////////////////////////
bool CheckBiom(int nOrd)
{
int Credits = 0;
int NeededCredits = 5;


#ifdef DM_270
	return true;
#endif


// NEW exams ...
	int nExamTable = LoadTable("BIOM.TXT",Table);
	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n BIOM \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}

// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}
///////////////////////////////////////////////////////////////////////////////////////////
bool CheckFina(int nOrd)
{
int Credits = 0;
int NeededCredits = MIN_FINA_CREDITS;
int nExamTable;


// NEW exams ...
	if(nOrd==0) 
		nExamTable = LoadTable(FINA_TABLE_CLI,Table);
	else
		nExamTable = LoadTable(FINA_TABLE,Table);

	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n FINAL \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}

// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}
///////////////////////////////////////////////////////////////////////////////////////////
bool CheckInge(int nOrd)
{
int Credits = 0;
int NeededCredits = 10;


#ifdef DM_270
	return true;
#endif


// NEW exams ...
	int nExamTable = LoadTable("INGE.TXT",Table);
	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n INGE \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}

// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;

}

///////////////////////////////////////////////////////////////////////////////////////////
bool CheckLabs(int nOrd, int nLang)
{
int Credits = 0;
int NeededCredits = MIN_LABS_CREDITS;
int nExamTable;

// NEW exams ...
	if( nLang==0 )
		nExamTable = LoadTable(LABS_TABLE,Table);
	if( nLang==1 ){
		if(nOrd == 0){
			nExamTable = LoadTable(LABS_TABLE_CLI,Table);
			NeededCredits = MIN_LABS_CREDITS_CLI;
		}
		else
			nExamTable = LoadTable(LABS_TABLE_ENG,Table);
	}



	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n LABS \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}


// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool CheckSpec(int nOrd, int nLang)
{
int Credits = 0;
int NeededCredits = MIN_SPEC_CREDITS;
int nExamTable;
	

// NEW exams ...
	if(nLang==0)
		nExamTable = LoadTable(SPEC_TABLE,Table);
	if(nLang==1)
		nExamTable = LoadTable(SPEC_TABLE_ENG,Table);

	
	
	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n SPEC \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}


// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
bool CheckGest(int nOrd)
{
int Credits = 0;
int NeededCredits = 5;


#ifdef DM_270
	return true;
#endif


// NEW exams ...
	int nExamTable = LoadTable("GEST.TXT",Table);
	Credits = SearchCorrespondence(nExamTable, NeededCredits);

// OutputReports ...
	printf("\n GEST \n");
	for (int k=0; k<nExamTable; k++)
	{
		if(Table[k].fFound) 
		{
			if(Table[k].ordin[nOrd]=='1') // exam belongs to the ordinamento ...
				printf("   %s %s", Table[k].codice, Table[k].name);
			else	
				printf("(***)%s %s ", Table[k].codice, Table[k].name);
		}
	}


// Missing Credits ...
	if(Credits < NeededCredits) 
	{
		printf("   ??\n");
		return false;
	}
	else
   		return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
short LoadTable(char *strName,TableElem* pTable)
{
char	bfLine[2048];	
short	nElem;
TableElem	*pT = pTable;


	FILE *f=fopen(strName,"rt");
	
	nElem = 0;
	while(!feof(f))
	{
		
		fgets(bfLine,2048,f);

#ifdef DM_270
		sscanf(bfLine,"%s\t%s\t", &pT->codice, &pT->ordin);
		strcpy(pT->name, &bfLine[12]);
#else
		sscanf(bfLine,"%s ", &pT->codice);
		strcpy(pT->name, &bfLine[9]);
#endif
		pT->fFound = false;
		
		nElem++;
		pT++;

	}


	fclose(f);

	return nElem;
}
///////////////////////////////////////////////////////////////////////////
short SearchCorrespondence(short nExamTable, short NeededCredits)
{
short Credits=0;

	for (int k=0; k<nExamTable; k++)
	{

		// Search Exam of the Table in that of the student ...
		for (int n=0; n<nExam; n++)
		{
			if (IsValid(n) && IsNotAssigned(n) 
				&& (strcmp(ExamList[n].codice,Table[k].codice) == 0))
			{
				Table[k].fFound = true;
				MarkAsAssigned(n);
				Credits += GetCredits(n);

				break;
			}
		}

		if( Credits >= NeededCredits)
			break;

	}

	return Credits;

}
