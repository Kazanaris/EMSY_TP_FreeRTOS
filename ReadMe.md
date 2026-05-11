##  PARTIE 1 : OBSERVATION DU FONCTIONNEMENT SANS OS

1. Parcourir le code et repérer :
   + Les endroits où les données sont écrites dans le FIFO :
     + Ils se trouvent aux lignes 329 et 352 du fichier app.c avec l'appel de fonction PutCharInFifo(&fifoLcd, frame[i]).
   + Quel est le format de données utilisé (codage des trames) :
     + Les trames sont codées sur 9 cases (frames) :
        - Frame 0 : Vaut toujours 2, c'est le début de trame
        - Frame 1 : Indique à l'aide d'un caractère si c'est une trame de température ou un caractère reçu par UART ('2' si par UART, '1' si c'est une trame de température),
        - Frame 2 : Sert à stocker le caractère UART OU le caractère LSB de la température,
        - Frame 3 : Commence à remplir le checksum pour le caractère UART OU stocke le caractère suivant de la température,
        - Frame 4 : Fini de remplire le checksum pour le caractère UART OU stocke le caractère suivant de la température,
        - Frame 5 : Si c'est pour un caractère UART alors vaut 3 pour fin de trame, sinon vaut le dernier caractère de la température,
        - Frame 6 : Remplissage avec 0 pour caractère UART, sinon commence à remplir le checksum pour les caractères de température,
        - Frame 7 : Remplissage avec 0 pour caractère UART, sinon fini de remplir le checksum pour les caractères de température,
        - Frame 8 : Remplissage avec 0 pour caractère UART, sinon vaut 3 pour fin de trame de température.
   + Où elles sont relues,  :
     + Ils se trouvent dans le fichier app.c à la ligne 245 avec GetReadSize(&fifoLcd)qui nous indique combien de caractères il y a dans la fifo pour pouvoir faire un test s'il y a un truc à lire.
     + Et toujours dans le même fichier à la ligne 251 avec l'appel de fonction GetCharFromFifo(&fifoLcd, (int8_t*)&car) on lit les caractères stockés dans la fifo.
   + Où et comment les données sont décodées en température ou caractère reçu, et les erreurs décelées,  :
     + Dans app.c à partir de la ligne 254, tous les caractères qui valent "0" sont jettés (c'est le padding). les autres qui sont différents de "0" sont mis dans un buffer jusqu'à ce que le dernier caractère enregistré             dans le buffer soit la fin de trame 0x03.
     + Ensuite, pour une trame de température on contrôle que le début du buffer vaut bien "2", que le buffer fait bien une taille de 9 caractères et que le caractère suivant du début de trame soit bien l'identifiant d'une             trame de température ('1' dans ce cas-ci).
     + Pour une trame de caractère, ce sont les même contrôles, mais ici on contrôle que la taille du buffer fait 6 caractères et que l'identifiant de trame soit celui d'un caractère UART ('2' ici).
     + Si ces 2 tests du buffer échouent, alors on incrémente de 1 le compteur d'erreur de appData, on vide le buffer, et au prochain tour on va dans le case de retVal = 3 donc affichage message d'erreur et nombre d'erreurs.
     + Une fois ces contrôles réussis on passe au contrôle de checksum du buffer (un genre de CRC).
     + Si le contrôle de checksum rate, alors on incrémente de 1 le compteur d'erreur de appData, on vide le buffer, et au prochain tour on va dans le case de retVal = 3 donc affichage message d'erreur et nombre d'erreurs.
     + Quand le contrôle de checksum réussi on enregistre les datas du buffer dans appData (appData.newtemp[0 à 3] pour la température dont appData.newtemp[4] pour la fin de chaîne, et appData.newChar pour caractère                   UART), ensuite on vide le buffer, et si c'était une trame de température, alors au prochain tour on va dans le case de retVal = 1 donc affichage de la température et nombre d'erreur, sinon si c'était une                   trame de caractère UART, alors au prochain tour on va dans le case de retVal = 2 donc affichage du caractère et nombre d'erreurs.
   + Ce qui est affiché  :
     + "EMSY3 TP5 FreeRTOS" est affiché sur la première colonne de la première ligne du LCD avec l'appel de la fonction printf_lcd("EMSY3 TP5 FreeRTOS") à la ligne 152 du fichier app.c.
     + "Demo sans OS" est affiché sur la première colonne de la deuxième ligne du LCD avec l'appel de la fonction printf_lcd("Demo sans OS") à la ligne 154 du fichier app.c.
     + Le déplacement de la première à la deuxième ligne a été fait à la ligne 153 du fichier app.c à l'aide de la fonction lcd_gotoxy(1,2).
     + "/ car: -" est affiché sur la dixième colonne de la troisième ligne du LCD avec l'appel de la fonction printf_lcd("/ car: -") à la ligne 156 du fichier app.c.
     + Le déplacement de la deuxième à la troisième ligne et de la première à la dixième colonne a été fait à la ligne 155 du fichier app.c à l'aide de la fonction lcd_gotoxy(10,3).
     + Lorsque l'on reçoit une température "t: ", est affichée grâce à l'appel de la fonction printf_lcd("t: ") à la ligne 189 du fichier app.c.
     + La valeur de la température est affichée grâce à l'appel de la fonction printf_lcd("%s", appData.newTemp).
     + Ces deux informations sont affichées à partir de la première colonne de la troisième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,3) à la ligne 188 du fichier app.c.
     + L'information de si nous avons des erreurs ainsi que de leurs nombres est affichée grâce à l'appel de la fonction printf_lcd(" OK / %4d erreurs", appData.nbErrors) à la ligne 192 de l'app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 191 du fichier app.c.
     + Lorsque l'on reçoit un caractère ce dernier est affiché grâce à l'appel de la fonction printf_lcd("/ car: %c", appData.newChar) à la ligne 198 du fichier app.c.
     + Cette information est affichée sur le LCD à partir de la dixième colonne de la troisième ligne avec l'appel de la fonction lcd_gotoxy(10,3) à la ligne 197 du fichier app.c.
     + L'information de si nous avons des erreurs ainsi que de leurs nombres est affichée grâce à l'appel de la fonction printf_lcd(" OK / %4d erreurs", appData.nbErrors) à la ligne 200 de l'app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 199 du fichier app.c.
     + S'il y a une erreur, l'information que les données ne sont pas correctes (NOK) ainsi que le nombre d'erreurs sont affichés grâce à l'appel de
     + la fonction printf_lcd("NOK / %4d erreurs", appData.nbErrors) à la ligne 206 du fichier app.c.
     + Ces deux informations sont affichées à partir de la première colonne de la quatrième ligne du LCD avec l'appel de la fonction lcd_gotoxy(1,4) à la ligne 205 du fichier app.c.
    

2. Faire fonctionner le programme en envoyant des caractères au clavier depuis PuTTY.  :
   
   Premier envoi de 1 caractère :
   ![Premier envoie de 1 caractère](/Partie_1_screen_edit/envoie_un_seul_caractère_1(edit).png)

   Deuxième envoi de 1 caractère
   ![Deuxième envoie de 1 caractère](/Partie_1_screen_edit/envoie_un_seul_caractère_2(edit).png)

   + Nous pouvons voir que :
        + La LED0 toggle toutes les 100ms (execution de l'APP_TASK), sauf lorsque l'on envoi un caractère, En effet, l'envoi de caractère "force" le toggle de la LED. Le prochain toggle de la LED se fera en même temps que celui de la LED2.
        + La LED1 (interruption de l'UART 1) reste dans son état jusqu'à l'envoi d'un caractère.
        + La LED2 (interruption du Timer 1) toggle toutes les 100ms et n'est pas affectée par l'envoi de caractère.
   
3. Idem mais copier plusieurs dizaines de caractères d'un seul coup :
   + Est-ce systématique ? :
     + Dépend du nombre de caractères mais si à chaque fois on copie-colle plusieurs dizaines de caractères alors oui ça sera systématique (dès qu’on écrit plus de 6 caractères simultanément).
   + Comme précédemment, inspectez le déroulement temporel à l’aide des 3 signaux. :
     + Envoie de beaucoup de caractères pour mettre le programme en galère, version dézoomée :
       ![bombarde de caractères dézoomer](/Partie_1_screen_edit/bombarde_de_caractères_dézoomer(edit).png)
     + Version zoomée :
       ![bombarde de caractères zoomer](/Partie_1_screen_edit/bombarde_de_caractères_zoomer(edit_1).png)
   + Mettez en évidence le dysfonctionnement et expliquez sa cause :
     + Pour résumer, la fifo est pleine et le programme perd des caractères volontairement.
     + L’app_task s’exécute seulement quand on se trouve dans les carrés rouges comme montré ci-dessous, cependant chaque flanc sur le signal rose correspond à un caractère reçu et donc une frame de 6 espaces occupés dans la fifo (de 32 cases). Donc après 5 caractères, la fifo est pleine. Les seuls moments où on sort un caractère de la fifo ( donc 6 cases de libres) c’est quand on rentre dans l’app_task avec le getMessage(), donc le problème est qu’on ne consomme pas assez vite les caractères reçus. (Dans les carrés verts tous les caractères qu’on garde en fifo, les autres sont perdus).
       ![bombarde de caractères zoomer (edit)](/Partie_1_screen_edit/bombarde_de_caractères_zoomer(edit_2).png)
   + Quelle(s) solution(s) proposez-vous ? :
     + Utiliser un free RTOS.
     + Ou pas envoyer plus de 5 caractères à la fois.
