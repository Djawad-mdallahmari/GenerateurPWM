# Mise en place du système
## Prérequis
- Quartus Prime
- Carte Altera DE2-115
## Mise en place du hard
1. Créer un projet Quartus
2. Ouvrir le fichier .qsys grâce à Platform Designer
3. Générer le .qip en faisant "Génerer HDL"
4. Ajouter les fichiers .v ainsi que le .qip généré
5. Mettre le fichier GenerateurPWM.v en top level
6. Compiler puis flasher sur la carte
## Mise en place du soft
1. Ouvrir Ecplise
2. Créer un nouveau projet NIOS II Application and BSP from template
3. Sélectionner le fichier .sopcinfo
4. Générer un projet hello world
5. Remplacer le fihier principal par le fichier mainPWM.c
6. Génerer le BSP, builder puis lancer sur la carte
7. Enjoy !
