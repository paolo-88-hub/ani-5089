# Inversion d'une matrice de pose

Une pose 3D est représentée par une matrice homogène $4 \times 4$ :

$$
T = \begin{pmatrix} R & t \\ 0_{1 \times 3} & 1 \end{pmatrix}
$$

où $R \in SO(3)$ est la matrice de rotation et $t \in \mathbb{R}^3$ le vecteur de translation.

## Inversion générale

Pour une matrice carrée inversible $A$, l'inversion générale cherche $A^{-1}$ telle que $A A^{-1} = I$. L'élimination de Gauss-Jordan convient à toute matrice inversible, mais ne tire pas parti de la structure particulière d'une pose.

Une matrice singulière, pour laquelle le déterminant est nul, n'a pas d'inverse.

## Inversion directe d'une pose

Une matrice de rotation est orthogonale, donc $R^{-1} = R^T$. L'inverse d'une transformation rigide s'écrit alors :

$$
T^{-1} = \begin{pmatrix} R^T & -R^T t \\ 0_{1 \times 3} & 1 \end{pmatrix}
$$

La rotation inverse est la transposée $R^T$, et la translation inverse est $-R^Tt$.

## Pose dégénérée

Si la partie rotation est singulière, la matrice homogène n'est pas inversible. La méthode générale doit alors signaler son échec. La formule directe suppose une vraie rotation orthogonale ; appliquée à une matrice dégénérée, elle ne donne pas nécessairement l'inverse.
