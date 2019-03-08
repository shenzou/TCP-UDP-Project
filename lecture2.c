#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
int main()
{
    FILE* fichier = NULL;
    int caractereActuel = 0; 
    fichier = fopen("test.txt", "r");
    if (fichier != NULL)
    {        // Boucle de lecture des caractères un à un

        do
        {
            caractereActuel = fgetc(fichier); // On lit le caractère
            printf("%c", caractereActuel); // On l'affiche

        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)
        fclose(fichier);
    }else{
	printf("\n probleme lors de la lecture du fichier");
}
 
   
 
   char buffer[50];
 printf("Enter first command : ");
   fgets(buffer, 49, stdin); //on lit maximum 49 caractère sur stdin (entrée standard)
    printf("chaine lue : %s\n", buffer);
 printf("\n comparaison %d " ,strcmp(buffer, "#Help")   );
 	if(strcmp(buffer, "#Quit")==0)
    	{
   	     printf("\n You want to quit the program");
    	}
	if(strcmp(buffer, "#Help")==0)
    	{   	    
 printf("\n #Help (list Command)");
printf("\n #ListU ( list user in a server)");
printf("\n #ListF (list files in a server)");
printf("\n #TrfU ( transfert Upload file in a server)");
printf("\n #TrfD ( transfert Download file in a server)");
printf("\n #Private <user> (commute to private )");
printf("\n #Public (commute to public )");
printf("\n #Ring <user> (notification if user is connect)");
printf("\n Your original Commands");
    	}  
 
    // signal to operating system program ran fine

    return 0;
}
