# Notes

## Windows

Le script de configuration utilise des liens symboliques, qui demande soit de lancer le script en administrateur, soit d'activer le [mode "developer" de Windows](https://www.ghacks.net/2016/12/04/windows-10-creators-update-symlinks-without-elevation/).

## Notes pour la remise

La remise électronique se fait via la plateforme Moodle.

N'oubliez pas d'inscrire le nom de vos coéquipiers dans le fichier `simplege/examples/chickendodge/data/equipe.yaml`!

**IMPORTANT:** Utilisez le script `remise.ps1` (Windows) ou `remise.sh` (Unix) pour générer un fichier de remise standardisé.

## Exercice #1

À l’aide du projet *SimpleGE*, implémenter un mécanisme de *batching* pour les *sprites*.

Un rapport du nombre d'appels de rendu est affiché dans une fenêtre de l'interface. Vous constaterez qu’il y a un appel de rendu (*draw call*) pour chaque *sprite*. Puisqu’elles partagent des caractéristiques communes, il devrait être possible de réduire de façon importante le nombre d’appels.

La stratégie recommandée consiste à déplacer la création, la mise à jour et l’appel de rendu de la méthode `Display` du composant `SpriteComponent` vers la méthode `Display` du composant `LayerComponent`. Le code de ces composants est dans les fichiers correspondants dans `simplege/src/components`.

Vous aurez probablement à exposer des fonctions et des propriétés supplémentaires, n'hésitez pas à modifier les structures déjà en place pour atteindre l'objectif!

## Exercice #2

À l’aide du projet *SimpleGE*, implémenter un effet visuel avec un *fragment shader*.

Le code de la caméra permet d'utiliser le rendu en mémoire. On peut ainsi supporter des effets spéciaux en plein écran à l’aide de *shaders*. Un composant appliquant une déformation de l’écran a ainsi été créé, afin d’être activé lorsque le joueur est touché par un poulet. Le résultat désiré est [visible ici](https://youtu.be/tauWfnZkD-c).

Votre rôle est d’implémenter le *fragment shader* situé dans le fichier `simplege/examples/chickendodge/data/shaders/deformation.frag` afin de pouvoir reproduire un effet semblable. Les différentes textures et paramètres sont configurés correctement dans le composant `DeformationCompositorComponent`, afin de mettre les variables uniformes à jour dans le shader.

L’algorithme suggéré est le suivant:
* Calculer l’intensité de la déformation à appliquer selon le temps, par la recherche d’une valeur dans la texture `uIntensity`, aux coordonnées `(uTime, 0.5)`. Mettre cette intensité à l’échelle `uScale`.
* Chercher un vecteur de déformation dans la texture `uDeformation`, aux coordonnées `vTextureCoord` décalé d’une valeur tirée de `uTime` (par exemple, le sinus de `uTime`). Moduler ce vecteur de déformation par l’intensité précédente.
* Chercher la couleur finale dans `uSampler` aux coordonnées `vTextureCoord`, décalées du vecteur de déformation.

## Exercice #3

À l’aide du projet *SimpleGE*, améliorer la gestion de collisions rudimentaire des classes `ColliderComponent` et `PhysicSystem`.

Vous devriez implémenter les premières étapes de vérifications vues en classe:
- drapeaux et masques
- subdivision spatiales à l'aide d'un quad-tree (vous pouvez utilisez une implémentation existante)
- rectangles englobants alignés aux axes (implicite, devrait déjà être fonctionnel)

## Exercice #4

À l’aide du projet *SimpleGE* (version mise à jour), compléter les fonctionnalités de régionalisation afin de supporter les champs substitués.

Vous constaterez que les différents textes de présentation au lancement et le compte à rebours sont déjà traduits, à l’aide de fichiers de localisation et de la classe *Localisation*.

Vous constaterez également que, lors de la victoire d’un joueur, le message qui apparaît comprend des champs qui devraient contenir les identifiants des joueurs. Vous devez modifier la fonction `Localisation::GetImpl`, du fichier `simplege/src/localisation.cpp`, afin d’appliquer les substitutions nécessaires.

Il est possible de modifier la langue du jeu en paramétrant la variable d'environnement `LANG` (depuis `.vscode/launch.json` pour Visual Studio Code, ou autre manière équivalente pour votre environnement de développement préféré).

**IMPORTANT:** Il s'agit de code de moteur de jeu, donc vous **devez** utiliser une implémentation qui pourrait être générique. De nouveaux champs pourraient exister dans un autre contexte.

## Exercice #5

À l’aide du projet *SimpleGE* (version mise à jour), ajouter les fonctionnalités réseau pertinentes permettant d’afficher un tableau des meneurs (*leaderboard*).

Le jeu a été modifié afin de supporter des échanges client-serveur. Les fonctionnalités de base pour l’échange de messages vous sont fournies et il est déjà possible de jouer en réseau (sous certaines réserves, voir les objectifs optionnels).

La stratégie suggérée est la suivante:
- Créer un nouveau type de message pour l’envoi de score au serveur, en s'inspirant de ce qui est dans `simplege/examples/chickendodge/messages`
- Lors de la mise à jour du score du joueur, envoyer le message ci-dessus au serveur
- Sur le serveur, conserver la liste des scores des joueurs en modifiant le composant `NetworkPlayerServerComponent`, dans `simplege/examples/chickendodge/server/networkplayerserver.cpp`. Si le coeur vous en dit, vous pouvez également implémenter un nouveau composant à cet effet, en l'ajoutant à la scène du serveur.
- Sur réception d’un message de score par le serveur, mettre cette liste à jour. Puisqu’il s’agit d’un tableau des meneurs, on ne devrait pas mettre à jour la valeur si elle est inférieure à un score déjà connu
- Sur réception d’un message de score par le serveur, envoyer une mise à jour du tableau des meneurs à tous les clients actifs, en créant un nouveau type de message au besoin
- La méthode appropriée du composant client *NetworkLeaderboardComponent* (`simplege/examples/chickendodge/src/components/networkleaderboard.cpp`) est alors appelée afin de mettre à jour le tableau des meneurs.
- Lors de la connexion d’un nouveau client, lui envoyer le tableau des meneurs

Visual Studio Code permet de débogguer simultanément plusieurs cibles. Une cible spéciale ("Multi") lançant 2 clients et le serveur a été ajoutée pour simplifier la logistique de déboggage. Elle peut être choisie dans le [menu de déboggage officiel](https://code.visualstudio.com/docs/editor/debugging).

### Objectifs optionnels

Vous constaterez que la génération des poulets et des rubis est incohérente entre les joueurs. De même, on ne transmet que les actions des joueurs. On a rapidement un comportement différent entre les diverses instances.

Vos objectifs optionnels (ie.: sans points supplémentaires, uniquement pour le plaisir) consistent à:
- Transmettre la position des joueurs réseau et pas seulement leurs actions afin que les joueurs se trouvent au même endroit entre les instances
- Désigner un client comme "maître" afin de générer le comportement des poulets. Transmettre les informations concernant les poulets aux autres joueurs
- Ajuster les autres aspects potentiellement incohérents entre les différents joueurs

## Exercice #6

À l’aide du projet *SimpleGE* (version mise à jour), ajouter les événements audio pertinents, à l'aide du middleware FMOD.

Nous allons utiliser ici deux composants de FMOD:
- le middleware lui-même, intégré au moteur de jeu
- le logiciel d'édition audio pour générer la banque sonore

La première étape est d'installer ces logiciels, [FMOD Studio et FMOD Engine](https://www.fmod.com/download#fmodstudiosuite) pour votre plateforme. Il faut idéalement se créer un compte pour les téléchargement, mais voici des liens directs si vous voulez éviter cette étape:
- [FMOD Studio (Windows, 64bits)](https://drive.google.com/file/d/1M80ssnndsX_vcjnSRlYfDuGcLv-WRdcd/view?usp=sharing)
- [FMOD Studio (Linux, deb)](https://drive.google.com/file/d/1QCHsC3ug74JWUvn1jmBkoJph2Z8GcYvr/view?usp=sharing)
- [FMOD Engine API (Windows)](https://drive.google.com/file/d/1_R8J-gG-eON3IGcfau_0oKEy_WdOn73i/view?usp=sharing)
- [FMOD Engine API (Linux)](https://drive.google.com/file/d/1yZdB_AnBGqVaVzQ5qJHboGQOG6FnSaF9/view?usp=sharing)

À l'aide de FMOD Studio, ouvrir le projet audio situé dans `simplege/examples/chickendodge/fmod/chickendodge/chickendodge.fspro`. Je vous invite à jeter un oeil sur la structure des différents éléments, mais vous pouvez simplement construire la banque sonore depuis le menu File->Build. Cet exportation va générer le fichier de banque sonore `simplege/examples/chickendodge/data/audio/Desktop/Master.bank`.

Sous Windows, CMake devrait trouver automatiquement les fichiers pertinents de FMOD, si ils ont été installés à l'endroit par défaut. Si vous utilisez la version Linux, créer un lien (ou copier) du dossier `api` du FMOD Engine téléchargé vers `simplege/extern/fmod`.

La liste de sons à déclencher est la suivante, un exemple est fait dans la méthode "ShowImage" du composant *CountdownComponent*:
- Coup d’épée (“attack”)
- Décompte du début : à chaque étape (“countdown”)
- Ramasser un rupee (“rupee_pickup”)
- Ramasser un cœur (“heart_pickup”)
- Frapper un poulet (“chicken_hit”)
- Rupee qui tombe par terre (“rupee_drop”)
- Cœur qui tombe par terre (“heart_drop”)
- Joueur reçoit du dommage (“player_hit”)
- Fin du jeu : une fois la partie terminée (“end”)

De plus, il y a un événement sonore en boucle, "chicken_idle", dont les propriétés varient selon un paramètre "NbChicken". Mettre ce paramètre à jour lors de la création et destruction d'un poulet. Il est paramétré à zéro dans le constructeur du composant *ChickenSpawnerComponent*.