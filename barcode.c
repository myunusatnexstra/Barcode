/************************************************************************
**                                                                      *
**  CIVP - 'C' Installation Verification Program to call                *
**          U.S. Postal Service Intelligent Mail Barcode Encoder V1R2M0 *
**          TPG Technology Consulting Limited                           *
**          Source code compiles on Windows/XP with:                    *
**          MinGW gcc Version 3.4.2                                     * 
**          Microsoft Visual C/C++ 6.0                                  *  
**          March 3, 2008                                               *
**                                                                      *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ivptab_size = 11;     /* num of entries in test case table */
      int j,success_count,fail_count ;

const
struct testcase_list
       {
       char      TestNum[6];
       char      TrackStr[20];
       char      RouteStr[11];
       char      ExpectBar[65];
       int       ExpectRC;
       } testcase[11] =
{"000001",               /* Test Case 1: Valid Track & Route RC=0 */
 "53379777234994544928",
 "51135759461",
 "DAFDTDAFFDFTDADTDDFTTFDTATATFFFDFTTFFFTFDDTDAAFATDFTFDFDTTTDTTFDA",
 0,
 "000002",               /* Test Case 2: Valid Track & Route RC=0 */
 "53055494689272602879",
 "13765583689",
 "AFDADAFTAFDTDFTFTFDAAFDDTAFDFDTTFADATAATAAAADDDFAAAAATDADAFADFTTT",
 0,
 "000003",               /* Test Case 3: Valid Track & Route RC=0 */
 "40120111574675115924",
 "62176609110",
 "DDAFFFDAFTFDFFAAATTTDDFFTFADDFAFTTDAAAAAADFTTFDTAFDDTDDADATDAFAFF",
 0,
 "000004",               /* Test Case 4: Valid Track & Route RC=0 */
 "82205455868913559972",
 "54765515722",
 "TTTADTFFADAFDTTFDTAADATFFFADFFTDDFFFATDADAATAAADDATFTAADFADTADADD",
 0,
 "000005",              /* Test Case 5: 2nd byte Track inv. RC =11*/
 "57379777234994544928",
 "51135759461",
 "                                                                 ",
 11,
 "000006",             /* Test Case 6: invalid char Route  RC=13  */
 "53055494689272602879",
 "137655B3689",
 "                                                                 ",
 13,
 "000007",             /* Test Case 7: invalid char Track  RC=10  */
 "4012X111574675115924",
 "62176609110",
 "                                                                 ",
 10,
 "000008",             /* Test Case 8: Valid Route 0 bytes  RC=0  */
 "01234567094987654321",
 "",
 "ATTFATTDTTADTAATTDTDTATTDAFDDFADFDFTFFFFFTATFAAAATDFFTDAADFTFDTDT",
 0,
 "000009",             /* Test Case 9: Valid Route 5 bytes  RC=0  */
 "01234567094987654321",
 "01234",
 "DTTAFADDTTFTDTFTFDTDDADADAFADFATDDFTAAAFDTTADFAAATDFDTDFADDDTDFFT",
 0,
 "000010",             /* Test Case 10: Valid Route 9 bytes RC=0  */
 "01234567094987654321",
 "012345678",
 "ADFTTAFDTTTTFATTADTAAATFTFTATDAAAFDDADATATDTDTTDFDTDATADADTDFFTFA",
 0,
 "000011",             /* Test Case 11: Valid Route 11 bytes RC=0 */
 "01234567094987654321",
 "01234567891",
 "AADTFFDFTDADTAADAATFDTDDAAADDTDTTDAFADADDDTFFFDDTTTADFAAADFTDAADA",
 0 };



char TrackString[21];     /* Input parameter Track String + 1 null*/
char RouteString[12];     /* Input parameter Route String + 1 null*/
char BarString[66];       /* Output parameter Bar String + 1 null */


int RetCode;              /* Return code from the usps4cb encoder */

__declspec (dllimport) int USPS4CB( char *TrackPtr, char *RoutePtr, char *BarPtr);
char *unescstring(char *, int, char *, int);
static int xctod(int);
static int xctod(int c)
{
if(isdigit(c))
	return c - '0';
else if(isupper(c))
	return c - 'A' + 10;
else if(islower(c))
	return c - 'a' + 10;
else	return 0;
}
char *unescstring(char *src, int srclen, char *dest, int destsize)
{
char *endp = src + srclen;
char *srcp;
char *destp = dest;
int nwrote = 0;

for(srcp = src; srcp < endp; srcp++)
	{
	if(nwrote > destsize)
		return NULL;
	if(*srcp == '+')
		*destp++ = ' ';
	else if(*srcp == '%')
		{
		*destp++ = 16 * xctod(*(srcp+1)) + xctod(*(srcp+2));
		srcp += 2;
		}
	else	*destp++ = *srcp;
	nwrote++;
	}

*destp = '\0';

return dest;
}


//unsued function
char *cgigetval(char *fieldname)
{
int fnamelen;
char *p, *p2, *p3;
int len1, len2;
static char *querystring = NULL;

querystring = getenv("QUERY_STRING");

if(querystring == NULL)
	{
	querystring = getenv("QUERY_STRING");
	printf("querystring=");
	printf(querystring);
	printf("\n");
	if(querystring == NULL)
		return NULL;
	}

if(fieldname == NULL)
	return NULL;

fnamelen = strlen(fieldname);

for(p = querystring; *p != '\0';)
	{
	p2 = strchr(p, '=');
	p3 = strchr(p, '&');
	if(p3 != NULL)
		len2 = p3 - p;
	else	len2 = strlen(p);

	if(p2 == NULL || p3 != NULL && p2 > p3)
		{
		/* no = present in this field */
		p3 += len2;
		continue;
		}
	len1 = p2 - p;

	if(len1 == fnamelen && strncmp(fieldname, p, len1) == 0)
		{
		/* found it */
		int retlen = len2 - len1 - 1;
		char *retbuf = malloc(retlen + 1);
		if(retbuf == NULL)
			return NULL;
		unescstring(p2 + 1, retlen, retbuf, retlen+1);
		return retbuf;
		}

	p += len2;
	if(*p == '&')
		p++;
	}

/* never found it */

return NULL;
}

void SetValue(char *str,char* keypair)
{
	
	char pairstr[500];
	//copy string into news variable
	sprintf(pairstr,"%s",keypair);
	char *tok = pairstr;
	tok = strtok(tok, "=");
	tok = strtok(NULL, "=");
	if(tok!=NULL)
		sprintf(str,"%s",tok);
		
}
void SetKeyPair(char *value,char* querystr, char* varname)
{
	char querystring[1024];
	char token[255];
	char name[255];
	char keypair[500];
	sprintf(keypair,"");
	//copy string into news variable
	sprintf(querystring,"%s",querystr);
	char *tok = querystring;
	if (varname!=NULL) 
	{
		sprintf(name,"%s",varname);
		strupr(name);
	}
    while ((tok = strtok(tok, "&")) != NULL)
    {
			sprintf(token,"%s",tok);
			strupr(token);
			if (strstr(token, name))
			{
				sprintf(keypair,"%s",tok);
				break;
			}
					
            tok = NULL;
    }
	if (keypair!=NULL && strlen(keypair)>1)
	{
		
		SetValue(value,keypair);
		//printf(value);
	}
		
}

int main(void)
{
	printf("Content-Type: text/plain;charset=us-ascii\n\n");
	char *qrrstring;
	qrrstring	= getenv("QUERY_STRING");
	//"tcode=53379777234994544928&zipcode=51135759461";

	char RouteString[12];//= "51135759461";
	char TrackString[21];//="53379777234994544928";
	char zipcode[255];
	char tcode[255];

	sprintf(zipcode,"");
	sprintf(tcode,"");
	
	if(qrrstring!=NULL && strlen(qrrstring)>1)
	{
		SetKeyPair(zipcode,qrrstring,"zipcode");
		SetKeyPair(tcode,qrrstring,"tcode");
	}
 
	if ((zipcode!=NULL) && (tcode!=NULL) && strlen(zipcode)>1 && strlen(tcode)>1 )
	{
		 sprintf(RouteString,"%s",zipcode);
		 sprintf(TrackString,"%s",tcode);
		 RetCode =  USPS4CB(TrackString,RouteString,BarString); 
		 	  
	     if (BarString!=NULL)
			printf(BarString);
		 else
			printf("Bar Code Generation Failed"); 

	}
	else
	{
	    
		printf("Error");
	}

 }

