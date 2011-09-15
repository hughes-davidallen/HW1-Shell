#ifndef _SHELL_H_
#define _SHELL_H_

void setup(void);
void cleanup(void);
int cd(char *path);
int pushd(char *path);
int popd(void);
void dirs(void);
void pathmanager(char *symbol, char *path);
void execute(char *args[]);

#endif
