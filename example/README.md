Librairies Robopoly pour Linux
==============================

Cette archive contient les fichiers nécessaires à la programmation
du PRisme sous Linux.
Attention : ceci est une version préliminaire qui n'a été que très
peu testée. Ce README fait office de documentation (sommaire) pour
l'instant, pendant que je travaille sur un document plus complet.
Ces fichiers devraient cependant permettre à la plupart d'entre
vous de démarrer sans trop de soucis.

Installation des outils
-----------------------

Avant de commencer, il vous faut installer les outils de
développement AVR pour votre distribution. Installez donc
avr-libc, gcc-avr et avr-binutils. Les exemples suivants seront
donnés pour Ubuntu puisque c'est ce que vous utilisez pour la
plupart, mais tous les tests ont été effectués avec succès sous
Arch Linux. Installation des programmes de base pour Ubuntu:

	sudo apt-get install avr-libc gcc-avr avr-binutils

Pour charger les programmes sur le microcontrôleur avec Pygaload,
installez python si nécessaire (bien qu'il devrait être installé
par défaut avec Gnome):

	sudo apt-get install python2.6

Vous avez maintenant tout ce qu'il vous faut pour commencer à
programmer le PRisme depuis Linux!


Utilisation de la librairie
---------------------------

Avant d'écrire votre premier programme, vérifiez tout d'abord que
tout fonctionne. L'archive que vous avez téléchargée contient un
exemple de projet qui compile correctement si les outils adéquats
sont installés. En supposant que vous avez extrait l'archive dans
`~/robopoly/robopolibs`, tapez les commandes suivantes dans un
terminal:

	cd ~/robopoly/robopolibs/
	make hex

Si la compilation s'effectue sans erreurs, vous pouvez maintenant
essayer de charger le programme sur le microcontrôleur en appelant

	make pygaload

Si vous arrivez jusqu'ici sans erreurs, vous pouvez commencer
votre propre projet! Pour ce faire, faites une copie de ce dossier
et commencez à modifier la copie. Si vous décidez de changer le
nom du fichier .c, n'oubliez pas de mettre à jour les références
dans le Makefile!

En cas de problèmes
-------------------

Ceci est une ébauche en attendant d'avoir le temps d'écrire une
documentation plus complèẗe. La dernière version se trouve en tout
temps sur [GitHub](http://github.com/tunebird/robopoly-linux).
Vous pouvez télécharger automatiquement la dernière version avec
la commande

	git clone http://github.com/tunebird/robopoly-linux.git

Si vous avez des questions, envoyez un e-mail à [Andrew
Watson](http://robopoly.epfl.ch)
