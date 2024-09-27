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
