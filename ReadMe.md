##  PARTIE 1 : OBSERVATION DU FONCTIONNEMENT SANS OS

1. Parcourir le code et repérer :
   + Les endroits où les données sont écrites dans le FIFO :
     + ça se trouvent aux lignes 329 et 352 du fichier app.c avec l'appel de fonction PutCharInFifo(&fifoLcd, frame[i]).
   + Quel est le format de données utilisé (codage des trames) :
     + Les trames sont codées sur 9 cases (frames) :
        - Frame 0 : Vaut toujours 2, c'est le début de trame
        - Frame 1 : Indique à l'aide d'un caractère si c'est une trame de température ou un caractère reçu par UART ('2' si par UART, '1' si c'est une trame de température),
        - Frame 2 : Sert à stocker le caractère UART OU le caractère LSB de la température,
        - Frame 3 : Commence à remplir le checksum pour le caractère UART OU stock le caractère suivant de la température,
        - Frame 4 : Fini de remplire le checksum pour le caractère UART OU stock le caractère suivant de la température,
        - Frame 5 : Si c'est pour un caractère UART alors vaut 3 pour fin de trame, sinon vaut le dernier caractère de la température,
        - Frame 6 : Remplissage avec 0 pour carractère UART, sinon commence à remplir le checksum pour les caractères de température,
        - Frame 7 : Remplissage avec 0 pour carractère UART, sinon fini de remplir le checksum pour les caractères de température,
        - Frame 8 : Remplissage avec 0 pour carractère UART, sinon vaut 3 pour fin de trame de température.
   + Où elles sont relues,  :
     + blablabla
   + Où et comment les données sont décodées en température ou caractère reçu, et les erreurs décelées,  :
     + blablabla
   + Ce qui est affiché  :
     + "EMSY3 TP5 FreeRTOS" est affiché sur la première colonne de la première ligne du LCD avec l'appel de la fonction printf_lcd("EMSY3 TP5 FreeRTOS") à la ligne 152 du fichier app.c.
     + "Demo sans OS" est affiché sur la première colonne de la deuxième ligne du LCD avec l'appel de la fonction printf_lcd("Demo sans OS") à la ligne 154 du fichier app.c.
     + Le déplacemment de la première à la deuxième ligne a été fait à la ligne 153 du fichier app.c à l'aide de la fonction lcd_gotoxy(1,2).
     + "/ car: -" est affiché sur la dixième colonne de la troisième ligne du LCD avec l'appel de la fonction printf_lcd("/ car: -") à la ligne 156 du fichier app.c.
     + Le déplacemment de la deuxième à la troisième ligne et de la première à la dixième colonne a été fait à la ligne 155 du fichier app.c à l'aide de la fonction lcd_gotoxy(10,3).
     + A FINIR
    

2. Faire fonctionner le programme en envoyant des caractères au clavier depuis PuTTY.  :
