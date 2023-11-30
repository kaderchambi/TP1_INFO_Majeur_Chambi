#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Constantes pour le message de bienvenue et le prompt
#define MESSAGE_BIENVENUE "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "

int main() {
	// Initialisation du Shell
	write(STDOUT_FILENO, MESSAGE_BIENVENUE, strlen(MESSAGE_BIENVENUE));

	// Boucle principale
	while (1) {
    	// Affichage du prompt
    	write(STDOUT_FILENO, PROMPT, strlen(PROMPT));

    	// Lecture de la commande de l'utilisateur
    	char command[100];
    	read(STDIN_FILENO, command, sizeof(command));
    	command[strcspn(command, "\n")] = '\0';

    	// Vérification de la commande "exit"
    	if (strcmp(command, "exit") == 0) {
        	write(STDOUT_FILENO, "Au revoir !\n", strlen("Au revoir !\n"));
        	break;
    	}

    	// Fork pour créer un nouveau processus
    	pid_t pid = fork();

    	if (pid == -1) {
        	perror("Erreur lors de la création du processus");
        	exit(EXIT_FAILURE);
    	} else if (pid == 0) {
        	// Code exécuté par le processus fils
        	execlp(command, command, NULL);
        	perror("Erreur lors de l'exécution de la commande");
        	exit(EXIT_FAILURE);
    	} else {
        	// Code exécuté par le processus parent
        	int status;
        	waitpid(pid, &status, 0);
        	dprintf(STDOUT_FILENO, "Code de sortie : %d\n", WEXITSTATUS(status));
    	}

    	// Retour au prompt enseash % et attente de la commande suivante
	}

	return 0;
}




