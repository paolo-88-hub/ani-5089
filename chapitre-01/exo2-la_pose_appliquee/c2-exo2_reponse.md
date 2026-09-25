# Application d'une pose

## Résolution

Une pose contient :

- une position (tx, ty, tz) ;
- un quaternion (w, x, y, z).

Pour un point \(P\), on applique :

$$ P' = R(P) + T $$

où \(R(P)\) est le point après rotation et \(T\) est la translation.

Pour un quaternion normalisé \(q=(w,x,y,z)\), la rotation d'un vecteur peut être calculée avec :

$$ t = 2(q_{xyz} \times P) $$

puis :

$$ P'_{rotation}=P+w\,t+(q_{xyz}\times t) $$

Enfin :

$$ P_{final}=P'_{rotation}+T $$

## Exemple

Prenons :

Position : 10 0 0
Quaternion : 1 0 0 0
Point : 1 2 3

Le quaternion (1, 0, 0, 0) représente une rotation nulle.

Donc :

$$ P_{rotation}=(1,2,3) $$

Puis la translation :

$$ P_{final}=(1,2,3)+(10,0,0) $$

On obtient :

11 2 3
