Résolution

Une pose est composée de :

une translation \(T\) ;
un quaternion de rotation \(q\).

Pour inverser une pose :

1. Inversion du quaternion

Comme le quaternion est normalisé, son inverse est simplement son conjugué :

$$ q^{-1}=(w,-x,-y,-z) $$
2. Inversion de la position

La position inverse est :

$$ T^{-1}=R^{-1}(-T) $$

Autrement dit, on prend d'abord la position opposée, puis on la fait tourner avec le quaternion conjugué.

3. Vérification

On choisit un point \(P\).

On applique d'abord la pose :

$$ P' = Pose(P) $$

Puis son inverse :

$$ P'' = Inverser(P') $$

On calcule ensuite l'erreur :

$$ Erreur=P''-P $$

Cette erreur doit être très proche de zéro, par exemple :

0.000000

ou une valeur extrêmement petite due aux erreurs d'arrondi.