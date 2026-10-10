#include <stdio.h>
int main()
{
    FILE *fp;
    fp = fopen("data.txt", "w");
    if (fp == NULL)
    {
        printf("File creation failed.\n");
        return 1;
    }
    fprintf(fp, "C CodeCraft File Test\n");
    fclose(fp);
    printf("File created successfully.\n");
    return 0;
}