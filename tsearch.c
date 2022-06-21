#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Simple program to experiment between normal character-by-character search
// And rolling checksum search

int hashmode=0;  // 0=normal, 1= rolling checksum

char iline[4097];  // input line (original)
char line[4098];   // processed line (space) line (space) -- all lower case with only letters and spaces
char fn[257];  // file name

char sstring[1025];  // search string (will add spaces and convert to lower case

int compares=0;  // instrumentation
int matches=0;
int hashcalcs=0; 

int printflag=0;  // options (print, start at 0 or 1, end at 1 or 2)
int starthash=0;
int maxhash=2;
int single=0;  // single match per line

// Calculate a hash over a length; could use different algorithms here
unsigned int hash(const char *s, unsigned int len)
{
  unsigned int rv=0;
  if (len==0) len=strlen(s);  // if len=0 compute real length
  while (len-- && *s)
      rv+=(unsigned int)*s++;
  return rv;
}

// Update a hash based on removing old and adding new
unsigned int hashupdate(unsigned int hash,unsigned int old, unsigned int new)
{
  hashcalcs+=2;
  return hash-old+new;
}


// Convert line to have leading and trailing space, all lower case. Any non-letter converted to space
// Leave result in line
void process(char *out, const char *in)
{
  const char *p=in;
  char *q=out+1;
  out[0]=' ';  // word seperator
  while (*p)
    {
      *q=tolower(*p);
      if (!isdigit(*q) && (*q<'a'||*q>'z')) *q=' ';
      p++;
      q++;
    }
  *q='\0';
}

int main(int argc, char *argv[])
{
  FILE *in;
  unsigned int target;
  unsigned int sum=0;
  unsigned int len;
  // process options:
  // -h hash test only
  // -p print matched lines (may print same line more than once if matched)
  // -n normal test only (note, -h -n makes no sense).
  // Note you can not stack options. That is use -h -p not -hp (too lazy to use getopt for this)
  while (argc>1 && *argv[1]=='-')
    {
      int cmd=argv[1][1];
      argc--;
      argv++;
      if (cmd=='h'||cmd=='H') starthash=1;  // only do hash test
      if (cmd=='p'||cmd=='P') printflag=1;  // print matching lines (will print multiples if more than one match on a line)
      if (cmd=='n'||cmd=='N') maxhash=1;    // only do normal test (note -h and -n together make no sense!)
      if (cmd=='1') single=1;  // single match per line
    }

  if (argc==1)  // if argument, use as search string, otherwise use convict
    {
      printf("Using default search string: convict\n");
      strcpy(sstring," convict ");
    }
  else
    {
      process(sstring,argv[1]);  // same processing as text
      strcat(sstring," ");  // add word boundry
      argc--;
      argv++;
    }
  // Pick up file name if supplied
  strcpy(fn,"warpeace.txt");
  if (argc>1)
    {
      // we only do one file name
      strcpy(fn,argv[1]);
    }
  else
    printf("Using warpeace.txt as file input\n");
      
  hashmode=starthash;
  while (hashmode!=maxhash) // with no options we do mode 0 then mode 1
    {
      in=fopen(fn,"r");  // get file (could be argv[2])
      if (!in)
	{
	  perror(fn);
	  return 1;
	}
      if (hashmode)
	target=hash(sstring,0);  // compute search hash and length once (we don't count this computation)
      
      len=strlen(sstring);
      // for each line
      while (!feof(in))
	{
	  if (fgets(iline,sizeof(iline),in))  // get line
	    {
	      int found=0;

	  // remove punct and otherwise process
	      process(line,iline);
	      if (hashmode)
		{
		  // seed hash:
		  unsigned lhash=hash(line,len);  // get first hash
		  unsigned index=0;
		  unsigned llen=strlen(line);  
		  if (llen<len) continue;    // if the line is too short, skip it
	      
		  while (index<(llen-len))
		    {
		      compares++;
		      if (lhash==target)     // if these numbers don't match, no way is it a match
			{
			  // might be a match, let's make sure
			  const char *p=sstring;
			  const char *t=line+index;
			  while (*p)
			    {
			      compares++;
			      if (*p++!=*t++) break;
			    }
			  if (*p=='\0')
			    {
			      if (printflag) printf("Match: %s",iline);
			      matches++;
			      found=single;  // if single =1 then abort this line
			    }
			}
		      if (found) break; // don't look in lines that lready matched
		      // move to next letter
		      index++;
		      // Update hash for next substring
		      lhash=hashupdate(lhash,line[index-1],line[index+len-1]);
		    }
		}
	      else
		{
		  int i=0,j=0;
		  while (line[i+j])
		    {
		      if (found) break;  // found in one line so skip
		      compares++;
		      if (!sstring[j])  // end of search string so we found it!
			{
			  matches++;
			  j=0;  // reset
			  i+=j+1;  
			  if (printflag) printf("Match: %s",iline);
			  // if single is 1, stop processing this line
			  found=single;
			  continue;
			}
		      if (line[i+j]!=sstring[j])  // if no match, reset search string and keep going
			{
			  j=0;
			  i++;
			  continue;
			}
		      j++;  // match but not complete, so move on
		    }
		}
	    }
	}
      fclose(in);
      // print instrumentation
      printf("%s done (%d,%d,%d,%d)\n",hashmode?"Hashed":"Normal",matches,compares,hashcalcs,compares+hashcalcs);
      hashmode++;  // next mode
      compares=matches=hashcalcs=0;  // ready for next run
    }
  return 0;
}

