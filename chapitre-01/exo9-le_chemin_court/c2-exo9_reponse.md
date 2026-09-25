# Chemin court et vitesse angulaire

## Formulation de la vitesse angulaire moyenne

L'orientation 3D est représentée par deux quaternions unitaires $q_1$ et $q_2$. Sur un intervalle $dt$, la rotation relative est :

$$
\Delta q = q_2 \cdot q_1^{-1}
$$

On peut écrire le quaternion relatif sous la forme $\Delta q = [\cos(\theta/2), \mathbf{u}\sin(\theta/2)]$, où $\theta$ est l'angle de rotation autour de l'axe unitaire $\mathbf{u}$. La vitesse angulaire moyenne est alors :

$$
\mathbf{\omega} = \frac{\theta}{dt}\mathbf{u}
$$

## Avec le chemin court

Les quaternions $q$ et $-q$ représentent la même rotation. Pour sélectionner la rotation la plus courte, on vérifie le produit scalaire $q_1 \cdot q_2$. Si celui-ci est négatif, on remplace $q_2$ par $-q_2$ avant de calculer $\Delta q$. L'angle retenu est alors au plus $\pi$.

## Sans le chemin court

Si le signe n'est pas corrigé, le calcul peut sélectionner le grand arc de la sphère $S^3$. L'angle obtenu peut alors être $\theta' = 2\pi - \theta$.

### Exemple

Prenons deux orientations presque identiques séparées par $dt = 0.1\text{ s}$ :

$$
q_1 = (1, 0, 0, 0), \qquad q_2 = (-0.9999, 0, 0, 0.0141) \approx -q_1
$$

Avec le chemin court, l'angle est environ $0.0282\text{ rad}$, d'où :

$$
\|\mathbf{\omega}_{\text{court}}\| = \frac{0.0282}{0.1} = 0.282\text{ rad/s}
$$

Sans correction du signe, l'angle du grand arc est environ $6.255\text{ rad}$ :

$$
\|\mathbf{\omega}_{\text{long}}\| = \frac{6.255}{0.1} = 62.55\text{ rad/s}
$$

Pour une rotation physique presque nulle, cette valeur élevée est absurde. La sélection du chemin court évite cette discontinuité.
