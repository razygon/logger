#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


static char outputbuf[1024];
FILE *parentlogfile;
static int parentpid;
int findex = 0;

void open_logfiles()
{
	unsigned int i;
	char *logfilename;

	parentpid = getpid();
	logfilename = malloc(30);
	sprintf(logfilename, "trinity-%d.log-%d", parentpid,findex);
	unlink(logfilename);
	parentlogfile = fopen(logfilename, "a");
	if (!parentlogfile) {
		perror("couldn't open logfile in open_logfiles\n");
		exit(EXIT_FAILURE);
	}

//	for (i = 0; i < shm->nr_childs; i++) {
//		logfilename = malloc(25);
//		sprintf(logfilename, "trinity-%d-child%d.log", parentpid, i);
//		unlink(logfilename);
//		shm->logfiles[i] = fopen(logfilename, "a");
//		if (!shm->logfiles[i]) {
//			printf("couldn't open logfile %s\n", logfilename);
//			exit(EXIT_FAILURE);
//		}
//	}
	free(logfilename);
}

void close_logfiles()
{
//	unsigned int i;

//	for (i = 0; i < shm->nr_childs; i++)
//		fclose(shm->logfiles[i]);
    return;
}

static FILE * find_logfile_handle()
{
	pid_t pid;
//	unsigned int i;

	pid = getpid();
	if (pid == parentpid)
		return parentlogfile;

//	for (i = 0; i < shm->nr_childs; i++) {
//		if (shm->pids[i] == pid)
//			return shm->logfiles[i];
//	}
	return NULL;
}

void synclogs()
{
//	unsigned int i;

	if (logging == 0)
		return;

//	for (i = 0; i < shm->nr_childs; i++) {
//		(void)fflush(shm->logfiles[i]);
//		(void)fsync(fileno(shm->logfiles[i]));
//	}

	(void)fflush(parentlogfile);
	fsync(fileno(parentlogfile));
}

void output(const char *fmt, ...)
{
	va_list args;
	int n;
    long size;
    char *logfilename;
    int status;
    fpos_t position;
    
    fgetpos (parentlogfile, &position);
    
    fseek(parentlogfile, 0, SEEK_END); // seek to end of file
    size = ftell(parentlogfile);
    fsetpos (parentlogfile, &position);
    
    if (10000000<size) {
        parentpid = getpid();
        fclose(parentlogfile);
        
        if (findex >= 2) {
            logfilename = malloc(50);
            sprintf(logfilename, "./trinity-%d.log-%d", parentpid,findex-1);
            unlink(logfilename);
            status = remove(logfilename);
            printf("status: %d\n",status);
            if( 0 == status )
                printf("-----%s file deleted successfully.\n",logfilename);
            else
            {
                printf("-----Unable to delete the file %s\n",logfilename);
                perror("Error");
                sleep(8);
            }
            free(logfilename);
        }
        logfilename = malloc(50);
        findex++;
        sprintf(logfilename, "trinity-%d.log-%d", parentpid,findex);
        unlink(logfilename);
        parentlogfile = fopen(logfilename, "a");
        if (!parentlogfile) {
            printf("couldn't open logfile %s\n",logfilename);
            exit(EXIT_FAILURE);
        }
        printf("\nopen %s successfully",logfilename);
        free(logfilename);
    }
    
	va_start(args, fmt);
	n = vsnprintf(outputbuf, sizeof(outputbuf), fmt, args);
	va_end(args);

	if (n < 0) {
		printf("Something went wrong in output() [%d]\n", n);
		exit(EXIT_FAILURE);
	}

	if (!quiet)
		printf("%s", outputbuf);

	if (logging == 0)
		return;

	fprintf(parentlogfile, "%s", outputbuf);
    
    

}
