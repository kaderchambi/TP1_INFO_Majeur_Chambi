
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
int status;
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
    	char command[100]; // Ajustez la taille en fonction de vos besoins
    	ssize_t bytesRead = read(STDIN_FILENO, command, sizeof(command));

    	// Vérification si l'utilisateur veut quitter
    	if (bytesRead == 0 || (bytesRead > 0 && strcmp(command, "exit") == 0)) {
        	write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
        	break; // Quittez la boucle
    	}

    	// Suppression du saut de ligne à la fin de la commande
    	command[strcspn(command, "\n")] = '\0';

    	

    	// Fork pour créer un nouveau processus
    	pid_t pid = fork();

    	if (pid == -1) {
        	perror("Erreur lors de la création du processus");
        	exit(EXIT_FAILURE);
    	} else if (pid == 0) {
        	// Code exécuté par le processus fils
        	execlp(command, command, NULL);
                // Enregistrement du temps de fin d'exécution
        	clock_gettime(CLOCK_MONOTONIC, &endTime);
        	// Si execlp échoue
        	perror("Erreur lors de l'exécution de la commande");
        	exit(EXIT_FAILURE);
    	} else {
        	// Code exécuté par le processus parent
        	//waitpid(pid, &previousExitCode, 0);
       	        //wait(NULL);
// Enregistrement du temps de début d'exécution
    	clock_gettime(CLOCK_MONOTONIC, &startTime);
        	wait(&status);
		

        	// Calcul du temps d'exécution
        	long elapsedMilliseconds = (endTime.tv_nsec - startTime.tv_nsec) / 1000000;
(endTime.tv_sec - startTime.tv_sec) * 1000 +
                                   	

        	// Affichage du temps d'exécution
        	dprintf(STDOUT_FILENO, "|");
        	printElapsedTime(elapsedMilliseconds);
        	dprintf(STDOUT_FILENO, "ms");
    	}
	}

	return 0;
}


