Résolution

On a deux façons d'appliquer une pose.

1. Rotation puis translation
$$ P_1 = R(P) + T $$
2. Translation puis rotation
$$ P_2 = R(P + T) $$

Ces deux opérations ne donnent généralement pas le même résultat.

Exemple

Prenons :

Translation : \(T=(10,0,0)\)
Quaternion : rotation de 90° autour de Z
Point : \(P=(1,0,0)\)

Rotation puis translation :

$$ (1,0,0)\rightarrow(0,1,0) $$

puis :

$$ (0,1,0)+(10,0,0)=(10,1,0) $$

Translation puis rotation :

$$ (1,0,0)+(10,0,0)=(11,0,0) $$

puis rotation :

$$ (11,0,0)\rightarrow(0,11,0) $$

Les résultats sont donc différents :

Rotation puis translation :  (10, 1, 0)
Translation puis rotation :  (0, 11, 0)
Quand les deux résultats coïncident-ils ?

Un cas simple est de prendre une translation nulle :

$$ T=(0,0,0) $$

Par exemple :

Quaternion : rotation de 90° autour de Z
Point : \(P=(1,0,0)\)
Translation : \(T=(0,0,0)\)

Dans les deux cas :

$$ P'=(0,1,0) $$

Pourquoi ? Parce qu'une translation nulle ne modifie pas le point. L'ordre des opérations n'a donc plus d'effet.