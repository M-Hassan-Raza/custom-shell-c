//Muhammad Hassan Raza - Do not plagiarize, take help and then write your own implementations!
//Copying teh code for assignments or project will hurt you a lot later down the line!

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool isExitCommand(char* tokens[])
{
    return !strcmp(tokens[0], "exit");
}

bool isChangeDirectoryCommand(char** tokens)
{
    return !strcmp(tokens[0], "cd");
}

void executeChangeDirectory(char* tokens[])
{
    char changeDir[] = {"chdir"};
    strcpy(tokens[0], changeDir);
}

char **tokenizeCommand(char *completeCommand, char *seperators, int *count)
{
  int length = strlen(completeCommand);
  char buffer[50];
  int tokenIndex = 0;
  
  *count = 0;
  
  //this whole block of code counts the number of substrings that we need to allocate memory for
  int i = 0;
  for(; i < length;)
  {
    while (i < length)
    {
      if (strchr(seperators, completeCommand[i]) == NULL)
      {
        break;
      }
      i++;
    }
    
    int previous_counter = i;
    while (i < length)
    {
        
      if (strchr(seperators, completeCommand[i]) != NULL)
      {
        break;
      }
      i++;
    }

    //substring has been found, increment the counter
    if (i > previous_counter) 
    {
        *count = *count + 1;
    }
  }
  
  //dynamically allocate memory to store the split tokens
  char **splitTokens = malloc(sizeof(char *) * *count);

  i = 0;
  for(;i < length;)
  { 
    //skip all the delimiters and other special characters to we can get the actual commands
    while (i < length)
    {
      if (strchr(seperators, completeCommand[i]) == NULL)
      {
        break;
      }
      i++;
    }

    //substirng has been found, copy its contents to a temporary buffer so that it can be sent to execvp
    int j = 0;
    for(;i < length; j++, i++)
    {
      if (strchr(seperators, completeCommand[i]) != NULL)
      {
        break;
      }
      buffer[j] = completeCommand[i];
    }
    
    if (j > 0)
    {  
      buffer[j] = '\0';
      int to_allocate = sizeof(char) * (strlen(buffer) + 1);
      splitTokens[tokenIndex] = malloc(to_allocate);
      strcpy(splitTokens[tokenIndex++], buffer);
    }
  }

  return splitTokens;
}

void executeCommand(char* tokens[])
{
    int p_id;
    int status;
    p_id = fork();

    if(p_id == 0)
    {
        //child code for execvp
        int execvpStatus;

        //check if it's a cd command, executes chdir instead since cd is not a system executable file
        if(isChangeDirectoryCommand(tokens))
        {
            int status = chdir(tokens[1]);
            if(status != 0)
            {
                printf("change directory failed command failed\n");
            }
        }
        else
        {
            //execute using execvp command (passing the array of tokens as well)
            execvpStatus = execvp(*tokens, tokens);
            if (execvpStatus < 0) 
            {
                //failure running execvp command
                perror("exec");
                exit(1);
            }
        }
    }
    else if(p_id > 0)
    {
        //wait for teh child to complete its execution
        wait(NULL);
    }
    else
    {
        //fork error
        perror("fork");
    }
    
}



int main(int argc, char *argv[])
{
    char completeCommand[200];
    char** argumentList;
    char* delimiterCharacters = " \n\t\0";

    //continue the loop until user exists
    while(true)
    {
        //grab user input
        printf("Enter Command: \n");
        fgets(completeCommand, 200, stdin);

        //boundary value check for command length
        if(strlen(completeCommand) >= 1)
        {
            int numberOfTokens;

            //split the command into tokens that can be sent to execvp
            argumentList = tokenizeCommand(completeCommand, delimiterCharacters, &numberOfTokens);

            //terminate shell
            if(isExitCommand(argumentList))
            {
                exit(0);
            }

            //execute commands using execvp
            executeCommand(argumentList);

            //De-Alocate all the memory dynamically allocated on the heap
            for(int i = 0; i < numberOfTokens; i++)
            {
                free(argumentList[i]);
            }
            free(argumentList);
        }
    }

    return 0;
}
