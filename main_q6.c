

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Constantes pour le message de bienvenue et le prompt
#define MESSAGE_BIENVENUE "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit' ou utilisez <Ctrl>+d.\n"
#define PROMPT_TEMPLATE "enseash [%s:%d|%ldms] %% "

void printElapsedTime(long elapsedMilliseconds) {
	dprintf(STDOUT_FILENO, "%ld", elapsedMilliseconds);
}

int main() {
	// Affichage du message d'accueil
	write(STDOUT_FILENO, MESSAGE_BIENVENUE, strlen(MESSAGE_BIENVENUE));

	// Code de sortie de la commande précédente
	int previousExitCode = 0;
	// Temps de début et fin d'exécution
	struct timespec startTime, endTime;

	// Boucle pour le prompt
	while (1) {
    	// Affichage du prompt avec le code de sortie et le temps d'exécution
    	if (WIFEXITED(previousExitCode)) {
        	dprintf(STDOUT_FILENO, PROMPT_TEMPLATE, "exit", WEXITSTATUS(previousExitCode), 0);
    	} else if (WIFSIGNALED(previousExitCode)) {
        	dprintf(STDOUT_FILENO, PROMPT_TEMPLATE, "sign", WTERMSIG(previousExitCode), 0);
    	} else {
        	write(STDOUT_FILENO, "enseash % ", strlen("enseash % "));
    	}

    	// Lecture de la commande de l'utilisateur
    	char input[100]; // Ajustez la taille en fonction de vos besoins
    	ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input));

    	// Vérification si l'utilisateur veut quitter
    	if (bytesRead == 0 || (bytesRead > 0 && strcmp(input, "exit") == 0)) {
        	write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
        	break; // Quittez la boucle
    	}

    	// Suppression du saut de ligne à la fin de la commande
    	input[strcspn(input, "\n")] = '\0';

    	// Enregistrement du temps de début d'exécution
    	clock_gettime(CLOCK_MONOTONIC, &startTime);

    	// Utilisation de strtok pour découper l'entrée en commandes et arguments
    	char *token;
    	char *args[10]; // Ajustez la taille en fonction de vos besoins

    	int i = 0;
    	token = strtok(input, " ");
    	while (token != NULL) {
        	args[i++] = token;
        	token = strtok(NULL, " ");
    	}
    	args[i] = NULL; // Ajout d'un pointeur NULL à la fin pour execvp

    	// Fork pour créer un nouveau processus
    	pid_t pid = fork();

    	if (pid == -1) {
        	perror("Erreur lors de la création du processus");
        	exit(EXIT_FAILURE);
    	} else if (pid == 0) {
        	// Code exécuté par le processus fils
        	execvp(args[0], args);
        	// Si execvp échoue
        	perror("Erreur lors de l'exécution de la commande");
        	exit(EXIT_FAILURE);
    	} else {
        	// Code exécuté par le processus parent
        	waitpid(pid, &previousExitCode, 0);
       	 
        	// Enregistrement du temps de fin d'exécution
        	clock_gettime(CLOCK_MONOTONIC, &endTime);

        	// Calcul du temps d'exécution
        	long elapsedMilliseconds = (endTime.tv_sec - startTime.tv_sec) * 1000 +
                                   	(endTime.tv_nsec - startTime.tv_nsec) / 1000000;

        	// Affichage du temps d'exécution
        	dprintf(STDOUT_FILENO, "|");
        	printElapsedTime(elapsedMilliseconds);
        	dprintf(STDOUT_FILENO, "ms");
    	}
	}

	return 0;
}

