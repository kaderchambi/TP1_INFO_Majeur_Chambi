
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

int main() {
	char input[MAX_COMMAND_LENGTH];
	int status;

	while (1) {
    	// Affichage du prompt
    	printf("enseash %% ");

    	// Lecture de la commande de l'utilisateur
    	fgets(input, sizeof(input), stdin);

    	// Suppression du saut de ligne à la fin de la commande
    	input[strcspn(input, "\n")] = '\0';

    	// Vérification si l'utilisateur veut quitter
    	if (strcmp(input, "exit") == 0) {
        	printf("Bye bye...\n");
        	break; // Quittez la boucle
    	}

    	// Fork pour créer un nouveau processus
    	pid_t pid = fork();

    	if (pid == -1) {
        	perror("Erreur lors de la création du processus");
        	exit(EXIT_FAILURE);
    	} else if (pid == 0) {
        	// Code exécuté par le processus fils

        	// Recherche de la redirection de sortie '>'
        	char *output_redirect = strchr(input, '>');
        	if (output_redirect != NULL) {
            	// Rediriger la sortie vers un fichier
            	*output_redirect = '\0';  // Truncate la commande à '>'
            	output_redirect++;    	// Pointe vers le nom du fichier

            	// Fermer stdout et réaffecter à un fichier
            	int file_descriptor = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            	if (file_descriptor == -1) {
                	perror("Erreur lors de la redirection de sortie");
                	exit(EXIT_FAILURE);
            	}
            	dup2(file_descriptor, STDOUT_FILENO);
            	close(file_descriptor);
        	}

        	// Recherche de la redirection d'entrée '<'
        	char *input_redirect = strchr(input, '<');
        	if (input_redirect != NULL) {
            	// Rediriger l'entrée à partir d'un fichier
            	*input_redirect = '\0';  // Truncate la commande à '<'
            	input_redirect++;    	// Pointe vers le nom du fichier

            	// Fermer stdin et réaffecter à un fichier
            	int file_descriptor = open(input_redirect, O_RDONLY);
            	if (file_descriptor == -1) {
                	perror("Erreur lors de la redirection d'entrée");
                	exit(EXIT_FAILURE);
            	}
            	dup2(file_descriptor, STDIN_FILENO);
            	close(file_descriptor);
        	}

        	// Exécuter la commande
        	execlp("sh", "sh", "-c", input, (char *)NULL);

        	// Si execlp échoue
        	perror("Erreur lors de l'exécution de la commande");
        	exit(EXIT_FAILURE);
    	} else {
        	// Code exécuté par le processus parent
        	waitpid(pid, &status, 0);

        	// Afficher le code de sortie
        	printf("enseash [exit:%d] %%\n", WEXITSTATUS(status));
    	}
	}

	return 0;
}

