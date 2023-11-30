#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Constantes pour le message de bienvenue et le prompt
#define MESSAGE_BIENVENUE "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit' ou utilisez <Ctrl>+d.\n"
#define PROMPT_TEMPLATE "enseash [exit:%d] %% "
#define PROMPT_SIGNAL_TEMPLATE "enseash [sign:%d] %% "

int main() {
	// Affichage du message d'accueil
	write(STDOUT_FILENO, MESSAGE_BIENVENUE, strlen(MESSAGE_BIENVENUE));

	// Code de sortie de la commande précédente
	int previousExitCode = 0;
	// Signal de la commande précédente
	int previousSignal = 0;

	// Boucle pour le prompt
	while (1) {
    	// Affichage du prompt avec le code de sortie ou le signal
    	if (WIFEXITED(previousExitCode)) {
        	dprintf(STDOUT_FILENO, PROMPT_TEMPLATE, WEXITSTATUS(previousExitCode));
    	} else if (WIFSIGNALED(previousExitCode)) {
        	dprintf(STDOUT_FILENO, PROMPT_SIGNAL_TEMPLATE, WTERMSIG(previousExitCode));
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
        	// Si execlp échoue
        	perror("Erreur lors de l'exécution de la commande");
        	exit(EXIT_FAILURE);
    	} else {
        	// Code exécuté par le processus parent
        	waitpid(pid, &previousExitCode, 0);
    	}
	}

	return 0;
}

