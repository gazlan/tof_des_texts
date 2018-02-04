/* ******************************************************************** **
** @@ tof_des_texts
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : All necessary include files (tci.h) and import library (tci.lib)
** @  Notes  : are installed with TCI SDK package into corresponding subdirectories.
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>

#include "tci.h"
#include "tberror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#define TD_DB_NAME            "TECDOC_CD_3_2015@localhost:2024"
#define TD_TB_LOGIN           "tecdoc"
#define TD_TB_PASSWORD        "tcd_error_0"

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD      dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */
                           
static TCIEnvironment*        pEnv  = NULL;
static TCIError*              pErr  = NULL;
static TCIConnection*         pConn = NULL;
static TCIStatement*          pStmt = NULL;
static TCIResultSet*          pRes  = NULL;
static TCITransaction*        pTa   = NULL;

static const char*   _pszTable = "tof_des_texts.csv";

static FILE*         _pOut = NULL;

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ AllocationError()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void AllocationError(char* what)
{
   printf("Can't allocate %s\n",what);

   if (pEnv)
   {
      TCIFreeEnvironment(pEnv);
   }

   exit(1);
}

/* ******************************************************************** **
** @@ Init()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Init()
{
   if (TCIAllocEnvironment(&pEnv))
   {
      AllocationError("environment handle");
   }

   if (TCIAllocError(pEnv,&pErr))
   {
      AllocationError("error handle");
   }

   if (TCIAllocTransaction(pEnv,pErr,&pTa))
   {
      AllocationError("transaction handle");
   }

   if (TCIAllocConnection(pEnv,pErr,&pConn))
   {
      AllocationError("connection handle");
   }

   if (TCIAllocStatement(pConn,pErr,&pStmt))
   {
      AllocationError("statement handle");
   }

   if (TCIAllocResultSet(pStmt,pErr,&pRes))
   {
      AllocationError("resultset handle");
   }
}

/* ******************************************************************** **
** @@ Cleanup()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Cleanup()
{
   TCIFreeResultSet(pRes);
   TCIFreeStatement(pStmt);
   TCIFreeConnection(pConn);
   TCIFreeTransaction(pTa);
   TCIFreeError(pErr);
   TCIFreeEnvironment(pEnv);
}

/* ******************************************************************** **
** @@ TBEerror()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static TCIState TBEerror(TCIState rc)
{
   if (rc && (TCI_NO_DATA_FOUND != rc))
   {
      TCIState    erc = TCI_SUCCESS;

      char     errmsg[1024];
      char     sqlcode[6];

      Error    tberr = E_NO_ERROR;

      sqlcode[5] = 0;

      erc = TCIGetError(pErr,1,1,errmsg,sizeof(errmsg),&tberr,sqlcode);

      if (erc)
      {
         erc = TCIGetEnvironmentError(pEnv,1,errmsg,sizeof(errmsg),&tberr,sqlcode);

         if (erc)
         {
            // Error !
            ASSERT(0);
            printf("Can't get error info for error %d (reason: error %d)\n",rc,erc);
            exit(rc);
         }
      }

      // Error !
      ASSERT(0);
      printf("Transbase Error %d (SQLCode %s):\n%s\n",tberr,sqlcode,errmsg);

      exit(rc);
   }

   return rc;
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ShowHelp()
{
   const char pszCopyright[] = "-*-   tof_des_texts 1.0  *   Copyright (c) Gazlan, 2015   -*-";
   const char pszDescript [] = "TECDOC_CD_3_2015 DB TOF_DES_TEXTS dumper";
   const char pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: tof_des_texts.com\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{           
   if (argc > 1)
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2)
   {
      if ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h")))
      {
         ShowHelp();
         return 0;
      }
   }

   struct TOF_DES_TEXTS
   {
      Int4           _iTEX_ID;      // INTEGER
      char*          _pszTEX_TEXT;  // VARCHAR(1200)
   };
   
   _pOut = fopen(_pszTable,"wt");

   if (!_pOut)
   {
      // Error !
      ASSERT(0);
      printf("Err: Can't open [%s] for write.\n",_pszTable);
      return 0;
   }

   TOF_DES_TEXTS    tofDesTexts;

   memset(&tofDesTexts,0,sizeof(TOF_DES_TEXTS));

   Init();

   TBEerror(TCIConnect(pConn,TD_DB_NAME));
   TBEerror(TCILogin(pConn,TD_TB_LOGIN,TD_TB_PASSWORD));
   
   TBEerror(TCIExecuteDirectA(pRes,"SELECT * FROM TOF_DES_TEXTS ORDER BY TEX_ID",1,0));

   int      iRow  = 0;

   Int2     Indicator = 0;

   TCIState    Err;

   // 1. TEX_ID
   TBEerror(TCIBindColumnA(pRes,1,&tofDesTexts._iTEX_ID,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));
   // 2. TEX_TEXT
   TBEerror(TCIBindColumnA(pRes,2,&tofDesTexts._pszTEX_TEXT,sizeof(Int4),NULL,TCI_C_SCHARPTR,&Indicator));

   fprintf(_pOut,"\"##\", \"TEX_ID\", \"TEX_TEXT\"\n");

   while ((Err = TCIFetchA(pRes,1,TCI_FETCH_NEXT,0)) == TCI_SUCCESS) 
   { 
      fprintf(_pOut,"%d",++iRow);

      // 1. TEX_ID
      fprintf(_pOut,", %ld",tofDesTexts._iTEX_ID);
      // 2. TEX_TEXT
      fprintf(_pOut,", \"%s\"",tofDesTexts._pszTEX_TEXT);

      fprintf(_pOut,"\n");
   } 

   if (Err != TCI_NO_DATA_FOUND) 
   {
     TBEerror(Err); 
   }
   
   TBEerror(TCICloseA(pRes));

   TBEerror(TCICloseA(pRes));
   TBEerror(TCILogout(pConn));
   TBEerror(TCIDisconnect(pConn));

   Cleanup();

   fclose(_pOut);
   _pOut = NULL;

   return 0;
}
