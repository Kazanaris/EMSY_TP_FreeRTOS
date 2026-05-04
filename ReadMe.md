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
     + ça se trouvent dans le fichier app.c à la ligne 245 avec GetReadSize(&fifoLcd)qui nous idique combien de caractères il y'a dans la fifo pour pouvoir faire un teste si il y'a un truc à lire.
     + et toujours dans le même fichier à la ligne 251 avec l'appel de fonction GetCharFromFifo(&fifoLcd, (int8_t*)&car) on lis les caractères stockée en fifo.
   + Où et comment les données sont décodées en température ou caractère reçu, et les erreurs décelées,  :
     + dans app.c à partire de la ligne 254, tout les caractères qui valent "0" sont jettée (c'est le padding). les autres qui sont différent de "0" sont mis dans un buffer jusqu'a ce que le dernier caractère enregistrer             dans le buffer soit la fin de trame 0x03.
     + Ensuite pour une trame de température on contrôle que le début du buffer vaut bien "2", que le buffer fait bien une taille de 9 caractères et que le caractère suivant du début de trame soit bien l'identifiant d'une             trame de température ('1' dans ce cas-ci).
     + Pour une trame de caractère c'est les même contrôles mais ici on contrôle que la taille du buffer fait 6 caractères et que l'identifiant de trame soit celui d'un caractère UART ('2' ici).
     + si ces 2 tests du buffer échouent lors on incrémente de 1 le compteur d'erreur de appData, on vide le buffer, et au prochain tour on vas dans le case de retVal = 3 donc affichage message d'erreur et nombre d'erreur.
     + une fois ces contrôles réussi on passe au contrôle de checksum du buffer(un genre de CRC).
     + si le contrôle de checksum rate alors on incrémente de 1 le compteur d'erreur de appData, on vide le buffer, te au prochain tour on vas dans le case de retVal = 3 donc affichage message d'erreur et nombre d'erreur.
     + quand le contrôle de checksum réussi on enregistre les datas du buffer dans appData (appData.newtemp[0 à 3] pour la température dont appData.newtemp[4] pour la fin de chaîne , et appData.newChar pour caractère                   UART), ensuite on vide le buffer, et si c'était une trame de température alors au prochain tour on vas dans le case de retVal = 1 donc affichage de la température et nombre d'erreur, sinon si c'était une                   trame de caractère UART alors au prochain tour on vas dans le case de retVal = 2 donc affichage du caractère et nombre d'erreur.
   + Ce qui est affiché  :
     + "EMSY3 TP5 FreeRTOS" est affiché sur la première colonne de la première ligne du LCD avec l'appel de la fonction printf_lcd("EMSY3 TP5 FreeRTOS") à la ligne 152 du fichier app.c.
     + "Demo sans OS" est affiché sur la première colonne de la deuxième ligne du LCD avec l'appel de la fonction printf_lcd("Demo sans OS") à la ligne 154 du fichier app.c.
     + Le déplacemment de la première à la deuxième ligne a été fait à la ligne 153 du fichier app.c à l'aide de la fonction lcd_gotoxy(1,2).
     + "/ car: -" est affiché sur la dixième colonne de la troisième ligne du LCD avec l'appel de la fonction printf_lcd("/ car: -") à la ligne 156 du fichier app.c.
     + Le déplacemment de la deuxième à la troisième ligne et de la première à la dixième colonne a été fait à la ligne 155 du fichier app.c à l'aide de la fonction lcd_gotoxy(10,3).
     + Lorsque l'on reçois une température "t: " est affiché grâce à l'appel de la fonction printf_lcd("t: ") à la ligne 189 du fichier app.c.
     + La valeur de la température est affichée grâce à l'appel de la fonction printf_lcd("%s", appData.newTemp).
     + Ces deux informations sont affichées à partir de la première colonne de la troisième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,3) à la ligne 188 du fichier app.c.
     + L'information de si nous avons des erreurs ainsi que de leurs nombres est affiché grâce à l'appel de la fonction printf_lcd(" OK / %4d erreurs", appData.nbErrors) à la ligne 192 de l'app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 191 du fichier app.c.
     + Lorsque l'on reçois un caractère ce dernier est afficher grâce à l'appel de la fonction printf_lcd("/ car: %c", appData.newChar) à la ligne 198 du fichier app.c.
     + Cette information est affichée sur le LCD à partir de la dixième colonne de la troisème ligne avec l'appel de la fonction lcd_gotoxy(10,3) à la ligne 197 du fichier app.c.
     + L'information de si nous avons des erreurs ainsi que de leurs nombres est affiché grâce à l'appel de la fonction printf_lcd(" OK / %4d erreurs", appData.nbErrors) à la ligne 200 de l'app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 199 du fichier app.c.
     + Si il y a une erreur, l'information que les données ne sont pas correctes (NOK) ainsi que le nombre d'erreurs sont afficher grâce à l'appel de
     + la fonction printf_lcd("NOK / %4d erreurs", appData.nbErrors) à la ligne 206 du fichier app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 205 du fichier app.c.
    

2. Faire fonctionner le programme en envoyant des caractères au clavier depuis PuTTY.  :
