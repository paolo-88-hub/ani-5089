# Exercice 1 — Les trois cadences

## Énoncé

Calculez la durée d'une image à 72, 90 et 120 hertz, au dixième de milliseconde.

Pour chacune, soustrayez les huit millisecondes que prennent les capteurs, la transmission, la composition et l'affichage, et dites ce qu'il reste à votre code.

## Résolution

La durée d'une image est calculée avec :

Durée d'une image = 1000 / fréquence

### À 72 Hz

1000 / 72 = **13,9 ms**

Temps restant pour le code :

13,9 − 8 = **5,9 ms**

### À 90 Hz

1000 / 90 = **11,1 ms**

Temps restant pour le code :

11,1 − 8 = **3,1 ms**

### À 120 Hz

1000 / 120 = **8,3 ms**

Temps restant pour le code :

8,3 − 8 = **0,3 ms**
