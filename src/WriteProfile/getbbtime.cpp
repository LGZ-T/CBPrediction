#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

//#define CPUT 0.37037
#define MAXBB 20000
typedef unsigned long long ulonglong;

extern "C"
{
    extern ulonglong BlockPredCount[MAXBB];
    extern ulonglong BlockPredCycle[MAXBB];
    void outinfo_cbcycle();
    void outinfo_cbcount();
}
void outinfo_cbcycle()
{
    char outstring[150];
    char hostname[100];
    int pid = getpid(), i;
    long double t = 1.0;
    gethostname(hostname,99);
    sprintf(outstring,"%s.%d.bbtimeout",hostname,pid);
    FILE *fp = fopen(outstring,"w");
    if(fp==NULL) { printf("open file wrong\n"); return; }
    for(i=0;i<MAXBB;++i)
    {
        //if(BlockPredCount[i]==0) BlockPredCount[i] = BlockPredCount[i-1];
        //if(BlockPredCycle[i]==0) continue;
        fprintf(fp,"%d\t%Lf\t%llu\n",i,BlockPredCycle[i],BlockPredCount[i]);    
    }
    fclose(fp);
}
void outinfo_cbcount()
{
    char outstring[150];
    char hostname[100];
    int pid = getpid(), i;
    gethostname(hostname,99);
    sprintf(outstring,"%s.%d.bbcountout",hostname,pid);
    FILE *fp = fopen(outstring,"w");
    if(fp==NULL) { printf("open file wrong\n"); return; }
    for(i=0;i<MAXBB;++i)
    {
        //if(BlockPredCount[i]==0) continue;
        fprintf(fp,"%d\t%llu\n",i,BlockPredCount[i]);    
    }
    fclose(fp);
}

