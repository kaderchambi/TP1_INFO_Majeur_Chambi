#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	// Affichage du message d'accueil
	printf("Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n");

	// Boucle pour le prompt
	while (1) {
    	printf("enseash %% "); // Prompt

    	// Lire la commande de l'utilisateur
    	char command[100]; // Définissez la taille en fonction de vos besoins
    	fgets(command, sizeof(command), stdin);

    	// Supprimer le saut de ligne à la fin de la commande
    	command[strcspn(command, "\n")] = '\0';

    	// Vérifier si l'utilisateur a saisi 'exit'
    	if (strcmp(command, "exit") == 0) {
        	printf("Au revoir !\n");
        	break; // Quittez la boucle
    	}

    	// Ici, nous traiterons l'exécution de la commande 
	}

	return 0;
}

